#ifndef __SEG_DATA_H__
#define __SEG_DATA_H__

#include <pthread.h>

#define TRUE 1
#define FALSE 0

struct _seg_data_type {
	pthread_t owner;
	int segment;
};

typedef struct _seg_data_type seg_data_t;

seg_data_t * make_seg_data(pthread_t, int);
void delete_seg_data(seg_data_t *);

int get_segment(seg_data_t *);
pthread_t get_seg_owner(seg_data_t *);

int compare_seg_data(seg_data_t *, seg_data_t *);
int compare_seg_owner(seg_data_t *, seg_data_t *);

#endif
