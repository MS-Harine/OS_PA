#include <stdlib.h>
#include "data.h"

int compare_data(data_t *d1, data_t *d2) {
	return (d1->mutex == d2->mutex);
}

int compare_owner(data_t *d1, data_t *d2) {
	return *(d1->owner) == *(d2->owner);
}

data_t * make_data(pthread_mutex_t *m, pthread_t o) {
	data_t *new_data = (data_t *)malloc(sizeof(data_t));
	new_data->mutex = m;

	struct _owner_node *dummy = (struct _owner_node *)malloc(sizeof(struct _owner_node));
	dummy->next = NULL;
	dummy->owner = 0;

	struct _owner_node *new_node = (struct _owner_node *)malloc(sizeof(struct _owner_node));
	new_node->next = NULL;
	new_node->owner = o;

	new_data->owner_list = (struct _owner_list *)malloc(sizeof(struct _owner_list));
	new_data->owner_list->head = dummy;
	new_data->owner_list->head->next = new_node;
	
	new_data->owner = &(new_node->owner);
	return new_data;
}

void delete_data(data_t *data) {
	struct _owner_node *cur = data->owner_list->head, *prev = NULL;
	while (cur != NULL) {
		prev = cur;
		cur = cur->next;
		free(prev);
	}

	free(data);
}

pthread_mutex_t * get_data(data_t *data) {
	return data->mutex;
}

pthread_t get_owner(data_t *data) {
	if (data->owner == NULL)
		return 0;
	return *(data->owner);
}

#include <stdio.h>
void add_owner(data_t *data, pthread_t owner) {
	if (data == NULL)
		return;

	struct _owner_node *cur = data->owner_list->head;
	while (cur->next != NULL)
		cur = cur->next;

	cur->next = (struct _owner_node *)malloc(sizeof(struct _owner_node));
	cur = cur->next;
	cur->next = NULL;
	cur->owner = owner;
}

void delete_owner(data_t *data, pthread_t target) {
	if (data == NULL)
		return;

	struct _owner_node *cur = data->owner_list->head->next;
	struct _owner_node *prev = data->owner_list->head;

	
	cur = data->owner_list->head->next;
	while (cur != NULL) {
		if (cur->owner == target)
			break;
		prev = cur;
		cur = cur->next;
	}

	if (cur == NULL)
		return;

	prev->next = cur->next;
	free(cur);
	
	data->owner = &(data->owner_list->head->next->owner);
}
