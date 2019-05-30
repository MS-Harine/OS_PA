#include <stdlib.h>
#include "seg_data.h"

seg_data_t * make_seg_data(pthread_t o, int s) {
	seg_data_t *new_data = (seg_data_t *)malloc(sizeof(seg_data_t));
	new_data->owner = o;
	new_data->segment = s;
	return new_data;
}

void delete_seg_data(seg_data_t *data) {
	free(data);
}

int get_segment(seg_data_t *data) {
	return data->segment;
}

pthread_t get_owner(seg_data_t *data) {
	return data->owner;
}

int compare_segment(seg_data_t *d1, seg_data_t *d2) {
	return d1->segment == d2->segment;
}

int compare_owner(seg_data_t *d1, seg_data_t *d2) {
	return d1->owner == d2->owner;
}
