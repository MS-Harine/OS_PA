#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "linked_list.h"

int compare_data(data_t *d1, data_t *d2) {
	return !strcmp(d1->mutex, d2->mutex);
}

int compare_owner(data_t *d1, data_t *d2) {
	return d1->owner == d2->owner;
}

struct _m_list * make_list(char *mutex) {
	struct _m_list *new = malloc(sizeof(struct _m_list));
	new->mutex = mutex;
	new->next = NULL;
	return new;
}

data_t * make_data(const char *m, pthread_t o, int p_segment, int c_segment, void *link) {
	data_t *new_data = (data_t *)malloc(sizeof(data_t));
	new_data->mutex = strdup(m);
	new_data->owner = o;
	new_data->p_segment = p_segment;
	new_data->c_segment = c_segment;
	new_data->gate_lock = NULL;

	struct _m_list *n = NULL;

	Node *cur = (Node *)link;
	while (cur != NULL) {
		if (cur == link) {
			new_data->gate_lock = make_list(get_data(cur->data));
			n = new_data->gate_lock;
		}
		else {
			n->next = make_list(get_data(cur->data));
			n = n->next;
		}
		cur = cur->next;
	}
	return new_data;
}

data_t * make_dummy_data() {
	data_t *new_data = malloc(sizeof(data_t));
	new_data->mutex = NULL;
	new_data->owner = 0;
	new_data->p_segment = -1;
	new_data->c_segment = -1;
	new_data->gate_lock = NULL;
	return new_data;
}

void delete_data(data_t *data) {
	struct _m_list *n = data->gate_lock, *m;
	while (n != NULL) {
		m = n;
		n = n->next;
		free(m);
	}

	free(data->mutex);
	free(data);
}

char * get_data(data_t *data) {
	return data->mutex;
}

pthread_t get_owner(data_t *data) {
	return data->owner;
}

int get_psegment(data_t *data) {
	return data->p_segment;
}

int get_csegment(data_t *data) {
	return data->c_segment;
}

void update_segment_data(data_t *data, int seg) {
	data->p_segment = seg;
}
