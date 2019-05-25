#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "data.h"
#include "linked_list.h"

Node * make_linked_node(data_t *data) {
	Node *new_node = (Node *)malloc(sizeof(Node));
	new_node->next = NULL;
	new_node->prev = NULL;
	new_node->data = data;
	return new_node;
}

LinkedList * make_linked_list() {
	LinkedList *new_list = (LinkedList *)malloc(sizeof(LinkedList));
	plist->head = make_linked_node(make_data(NULL, pthread_self()));
	plist->tail = make_linked_node(make_data(NULL, pthread_self()));

	plist->head->next = plist->tail;
	plist->tail->prev = plist->head;
	return new_list;
}

void delete_linked_list(LinkedList *plist) {
	if (plist == NULL)
		return;
	
	while (!is_empty_linked(plist))
		linked_pop_node(plist);
	delete_data(plist->head->data);
	free(plist->head);
	delete_data(plist->tail->data);
	free(plist->tail);
	free(plist);
}

void linked_push_node(LinkedList *plist, data_t *data) {
	if (plist == NULL)
		return;

	Node *new_node = make_linked_node(data);
	new_node->prev = plist->tail->prev;
	new_node->next = plist->tail;
	plist->tail->prev->next = new_node;
	plist->tail->prev = new_node;
}

data_t * linked_pop_node(LinkedList *plist) {
	if (is_empty_list(plist))
		return NULL;

	data_t *ret_data = plist->tail->prev->data;
	Node *del_node = plist->tail->prev;

	del_node->prev->next = del_node->next;
	del_node->next->prev = del_node->prev;
	free(del_node);
	return ret_data;
}

void linked_delete_node(LinkedList *plist, data_t *target) {
	if (is_empty_list(plist))
		return;

	Node *del_node = plist->head->next;
	while((del_node != plist->tail) && !compare_data(del_node->data, target))
		del_node = del_node->next;

	if (del_node == plist->tail)
		return;

	del_node->prev->next = del_node->next;
	del_node->next->prev = del_node->prev;
	delete_data(del_node->data);
	free(del_node);
}

int is_empty_linked(LinkedList *plist) {
	if (plist == NULL)
		return TRUE;
	
	return (plist->head->next == plist->tail);
}
