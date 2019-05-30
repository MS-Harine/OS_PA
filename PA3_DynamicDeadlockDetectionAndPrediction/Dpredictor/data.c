#include <stdlib.h>
#include <string.h>
#include "data.h"

int compare_data(data_t *d1, data_t *d2) {
	return !strcmp(d1->mutex, d2->mutex);
}

int compare_owner(data_t *d1, data_t *d2) {
	return d1->owner == d2->owner;
}

data_t * make_data(const char *m, pthread_t o) {
	data_t *new_data = (data_t *)malloc(sizeof(data_t));
	new_data->mutex = strdup(m);
	new_data->owner = o;
	return new_data;
}

void delete_data(data_t *data) {
	free(data->mutex);
	free(data);
}

char * get_data(data_t *data) {
	return data->mutex;
}

pthread_t get_owner(data_t *data) {
	return data->owner;
}
