#ifndef __DATA_H__
#define __DATA_H__

#include <pthread.h>

#define TRUE 1
#define FALSE 0

struct _data_type {
	pthread_t owner;
	char *mutex;
};

typedef struct _data_type data_t;

data_t * make_data(const char *, pthread_t);
void delete_data(data_t *);

int compare_data(data_t *, data_t *);
int compare_owner(data_t *, data_t *);

char * get_data(data_t *);
pthread_t get_owner(data_t *);

#endif
