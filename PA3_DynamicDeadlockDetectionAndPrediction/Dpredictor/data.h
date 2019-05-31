#ifndef __DATA_H__
#define __DATA_H__

#include <pthread.h>

#define TRUE 1
#define FALSE 0

struct _m_list {
	char *mutex;
	struct _m_list *next;
};

struct _data_type {
	pthread_t owner;
	char *mutex;
	int p_segment;
	int c_segment;
	struct _m_list *gate_lock;
};

typedef struct _data_type data_t;

data_t * make_data(const char *, pthread_t, int, int, void *);
data_t * make_dummy_data();
void delete_data(data_t *);

int compare_data(data_t *, data_t *);
int compare_owner(data_t *, data_t *);

char * get_data(data_t *);
pthread_t get_owner(data_t *);
int get_psegment(data_t *);
int get_csegment(data_t *);

void update_segment_data(data_t *, int);

#endif
