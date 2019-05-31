#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include <execinfo.h>
#include "data.h"
#include "graph.h"
#include "detector_func.h"

static AdjList *graph = NULL;
static AdjList *threads_lock_stack = NULL;

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

#ifdef DEBUG
void print_graph(AdjList *plist) {
	printf("-------------------------\n");
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		printf("%p | %ld : ", get_data(cur->data), get_owner(cur->data));
		Node *c = cur->link->head->next;
		while (c != cur->link->tail) {
			printf("%p -> ", get_data(c->data));
			c = c->next;
		}
		printf("END\n");
		cur = cur->next;
	}
	printf("-------------------------\n\n");
}
#endif

AdjNode * find_node(AdjList *plist, pthread_t id) {
	if (is_empty_adj(plist))
		return NULL;
	
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		if (get_owner(cur->data) == id)
			return cur;
		cur = cur->next;
	}

	return NULL;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	int (*orig_lock)(pthread_mutex_t *) = NULL;
	int (*orig_unlock)(pthread_mutex_t *) = NULL;
	char *error = NULL;

	orig_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != NULL)
		exit(1);
	orig_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if ((error = dlerror()) != NULL)
		exit(1);
	
	orig_lock(&m);
	{
	#ifdef DEBUG
		printf("Lock Mutex %p by %ld\n", mutex, pthread_self());
	#endif
		
		if (graph == NULL)
			graph = make_adj_list();
		if (threads_lock_stack == NULL)
			threads_lock_stack = make_adj_list();

		AdjNode *self = NULL;
		if ((self = find_node(threads_lock_stack, pthread_self())) == NULL) {
			adj_push_node(threads_lock_stack, make_data(NULL, pthread_self()));
			assign_mutex(graph, make_data(mutex, pthread_self()), threads_lock_stack->tail->prev->link);
		}
		else
			assign_mutex(graph, make_data(mutex, pthread_self()), self->link);

	#ifdef DEBUG
		print_graph(graph);
		print_graph(threads_lock_stack);
	#endif

		if (find_cycle(graph))
			fprintf(stderr, "Deadlock detected!\n");
	}
	orig_unlock(&m);

	return orig_lock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	int (*orig_lock)(pthread_mutex_t *) = NULL;
	int (*orig_unlock)(pthread_mutex_t *) = NULL;
	char *error = NULL;

	orig_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != NULL)
		exit(1);
	orig_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if ((error = dlerror()) != NULL)
		exit(2);

	orig_lock(&m);
	{
	#ifdef DEBUG
		printf("Unlock Mutex %p by %ld\n", mutex, pthread_self());
	#endif
		
		if (graph == NULL || threads_lock_stack == NULL) {
			fprintf(stderr, "Invalid function invoke detected!\n");
			return orig_unlock(mutex);
		}

		data_t *data = make_data(mutex, pthread_self());
		expire_mutex(graph, data);
		AdjNode *self = find_node(threads_lock_stack, pthread_self());
		if (self == NULL) {
			fprintf(stderr, "Algorithm is wrong... :(\n");
			return orig_unlock(mutex);
		}
	
		linked_delete_node(self->link, data);
	#ifdef DEBUG
		print_graph(graph);
		print_graph(threads_lock_stack);
	#endif
	}
	orig_unlock(&m);

	return orig_unlock(mutex);
}
