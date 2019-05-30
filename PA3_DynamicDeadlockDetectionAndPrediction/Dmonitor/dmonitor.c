#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static const char *filename = "dmonitor.trace";
static __thread int n_count = 0;

static FILE *fp = NULL;

char * parsing_address(const char *str) {
	int i = 0, j = 0, status = 0;
	char *result = (char *)malloc(sizeof(char) * 12);
	while (str[i] != '\0') {
		if (status == 1)
			result[j++] = str[i];
		
		if (str[i] == '[')
			status = 1;
		else if (str[i] == ']')
			break;
		
		i++;
	}
	result[j - 1] = '\0';
	result = realloc(result, sizeof(char) * (strlen(result) + 1));
	return result;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
	int (*orig_create)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) = NULL;
	int (*orig_lock)(pthread_mutex_t *) = NULL;
	int (*orig_unlock)(pthread_mutex_t *) = NULL;
	char *error = NULL, **stack = NULL, *address = NULL;
	void *log[2];
	size_t sz = 0;
	int i = 0;

	orig_create = dlsym(RTLD_NEXT, "pthread_create");
	if ((error = dlerror()) != NULL)
		exit(-1);
	
	orig_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != NULL)
		exit(-1);

	orig_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if ((error = dlerror()) != NULL)
		exit(-1);

	n_count++;
	if (n_count == 1) {
		orig_lock(&m);
		
		orig_create(thread, attr, start_routine, arg);
		
		sz = backtrace(log, 2);
		stack = backtrace_symbols(log, sz);
		address = parsing_address(stack[1]);
		
		if (fp != NULL) {
			if ((fp = fopen(filename, "a")) == NULL)
				exit(-2);
		}
		else {
			if ((fp = fopen(filename, "w")) == NULL)
				exit(-2);
		}

#ifdef DEBUG
		printf("c,%ld,%ld,%s\n", pthread_self(), *thread, address);
#endif
		fprintf(fp, "c,%ld,%ld,%s\n", pthread_self(), *thread, address);
		fclose(fp);
		
		free(stack);
		free(address);
		orig_unlock(&m);
	}
	n_count--;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	int (*orig_lock)(pthread_mutex_t *) = NULL;
	int (*orig_unlock)(pthread_mutex_t *) = NULL;
	char *error = NULL, **stack = NULL, *address = NULL;
	void *log[2];
	size_t sz = 0;
	int i = 0;

	orig_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != NULL)
		exit(-1);

	orig_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if ((error = dlerror()) != NULL)
		exit(-1);

	n_count++;
	if (n_count == 1) {
		orig_lock(&m);
		
		sz = backtrace(log, 2);
		stack = backtrace_symbols(log, sz);
		address = parsing_address(stack[1]);

		if (fp != NULL) {
			if ((fp = fopen(filename, "a")) == NULL)
				exit(-2);
		}
		else {
			if ((fp = fopen(filename, "w")) == NULL)
				exit(-2);
		}

#ifdef DEBUG
		printf("l,%ld,%p,%s\n", pthread_self(), mutex, address);
#endif
		fprintf(fp, "l,%ld,%p,%s\n", pthread_self(), mutex, address);
		fclose(fp);
		
		free(stack);
		free(address);
		orig_unlock(&m);
	}
	n_count--;

	return orig_lock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	int (*orig_lock)(pthread_mutex_t *) = NULL;
	int (*orig_unlock)(pthread_mutex_t *) = NULL;
	char *error = NULL, **stack = NULL, *address = NULL;
	void *log[10];
	size_t sz = 0;
	int i = 0;

	orig_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != NULL)
		exit(-1);

	orig_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if ((error = dlerror()) != NULL)
		exit(-1);

	n_count++;
	if (n_count == 1) {
		orig_lock(&m);

		sz = backtrace(log, 10);
		stack = backtrace_symbols(log, sz);
		address = parsing_address(stack[1]);

		if (fp != NULL) {
			if ((fp = fopen(filename, "a")) == NULL)
				exit(-2);
		}
		else {
			if ((fp = fopen(filename, "w")) == NULL)
				exit(-2);
		}

#ifdef DEBUG
		printf("u,%ld,%p,%s\n", pthread_self(), mutex, address);
#endif
		fprintf(fp, "u,%ld,%p,%s\n", pthread_self(), mutex, address);
		fclose(fp);
	
		free(stack);
		free(address);
		orig_unlock(&m);
	}
	n_count--;

	return orig_unlock(mutex);
}
