#ifndef __DATA_H__
#define __DATA_H__

#include <pthread.h>

#define TRUE 1
#define FALSE 0

struct _owner_node {
	pthread_t owner;
	struct _owner_node *next;
};

struct _owner_list {
	struct _owner_node *head;
};

struct _data_type {
	pthread_mutex_t *mutex;
	pthread_t *owner;
	struct _owner_list * owner_list;
};

typedef struct _data_type data_t;

data_t * make_data(pthread_mutex_t *, pthread_t);
void delete_data(data_t *);

int compare_data(data_t *, data_t *);
int compare_owner(data_t *, data_t *);

pthread_mutex_t * get_data(data_t *);
pthread_t get_owner(data_t *);

void add_owner(data_t *, pthread_t);
void delete_owner(data_t *, pthread_t);

#endif
