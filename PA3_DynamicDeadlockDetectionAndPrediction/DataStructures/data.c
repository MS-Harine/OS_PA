#include "data.h"

int compare_data(data_t *d1, data_t *d2) {
	return (d1->mutex == d2->mutex);
}

int compare_owner(data_t *d1, data_t *d2) {
	return (d1->owner == d2->owner);
}

data_t * make_data(pthread_mutex_t *m, pthread_t o) {
	data_t *new_data = (data_t *)malloc(sizeof(data_t));
	new_data->mutex = m;
	new_data->owner = o;
	return new_data;
}
