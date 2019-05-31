#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "data.h"
#include "linked_list.h"
#include "graph.h"

AdjNode * make_adj_node(data_t *data) {
	AdjNode *new_node = (AdjNode *)malloc(sizeof(AdjNode));
	new_node->next = NULL;
	new_node->prev = NULL;
	
	new_node->data = data;
	new_node->link = make_linked_list();

	return new_node;
}

AdjList * make_adj_list() {
	AdjList *new_list = (AdjList *)malloc(sizeof(AdjList));
	new_list->head = make_adj_node(make_data(NULL, pthread_self()));
	new_list->tail = make_adj_node(make_data(NULL, pthread_self()));

	new_list->head->next = new_list->tail;
	new_list->tail->prev = new_list->head;
	return new_list;
}

void delete_adj_list(AdjList *plist) {
	if (plist == NULL)
		return;
	
	while (!is_empty_adj(plist))
		adj_pop_node(plist);

	delete_data(plist->head->data);
	free(plist->head);
	delete_data(plist->tail->data);
	free(plist->tail);
	free(plist);
}

void adj_push_node(AdjList *plist, data_t *data) {
	if (plist == NULL)
		return;

	AdjNode *new_node = make_adj_node(data);
	
	new_node->prev = plist->tail->prev;
	new_node->next = plist->tail;
	plist->tail->prev->next = new_node;
	plist->tail->prev = new_node;
}

data_t * adj_pop_node(AdjList *plist) {
	if (is_empty_adj(plist))
		return NULL;

	data_t *ret_data = plist->tail->prev->data;
	AdjNode *del_node = plist->tail->prev;

	del_node->prev->next = del_node->next;
	del_node->next->prev = del_node->prev;
	delete_linked_list(del_node->link);
	free(del_node);
	return ret_data;
}

void adj_delete_node(AdjList *plist, data_t *target) {
	if (is_empty_adj(plist))
		return;

	AdjNode *del_node = plist->head->next;
	while((del_node != plist->tail) && !compare_data(del_node->data, target))
		del_node = del_node->next;

	if (del_node == plist->tail)
		return;

	del_node->prev->next = del_node->next;
	del_node->next->prev = del_node->prev;

	delete_linked_list(del_node->link);
//	delete_data(del_node->data);
	free(del_node);
}

int is_empty_adj(AdjList *plist) {
	if (plist == NULL)
		return TRUE;
	
	return (plist->head->next == plist->tail);
}
