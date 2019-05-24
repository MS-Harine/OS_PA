#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

#define get_data(x)	((x)->node)

void linked_list_init(LinkedList *plist) {
	if (plist == NULL)
		return;
	
	plist->head = NULL;
	plist->tail = NULL;
}

void adj_list_init(AdjList *plist) {
	if (plist == NULL)
		return;

	plist->head = NULL;
}

void _add_node(LinkedList *plist, data_t *data) {
	if (plist == NULL || data == NULL)
		return;
	
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = NULL;
	newNode->prev = NULL;

	Node *cur = plist->head;
	if (cur == NULL) {
		plist->head = newNode;
		plist->tail = newNode;
	}
	else {
		newNode->next = plist->tail;
		newNode->prev = plist->tail->prev;
		plist->tail->prev->next = newNode;
		plist->tail = newNode;
	}
}

void add_node(AdjList *plist, pthread_mutex_t *mutex) {
	if (plist == NULL || mutex == NULL)
		return;

	pthread_t tid = pthread_self();
	AdjNode *cur = plist->head;
	if (cur == NULL) {
		data_t *newData = (data_t *)malloc(sizeof(data_t));
		newData->tid = tid;
		newData->mutex = mutex;

		AdjNode *newNode = (AdjNode *)malloc(sizeof(AdjNode));
		newNode->node = newData;
		newNode->next = NULL;
		newNode->prev = NULL;
		
		LinkedList *link = (LinkedList *)malloc(sizeof(LinkedList));
		linked_list_init(link);
		newNode->link = link;

		plist->head = newNode;
		plist->tail = newNode;
	}
	else {
		while (cur != NULL) {
			if (get_data(cur)->mutex == mutex)
				break;
			cur = cur->next;
		}

		if (cur != NULL) {
			if (get_data(cur)->tid == tid) {
				data_t *newData = (data_t *)malloc(sizeof(data_t));
				newData->tid = tid;
				newData->mutex = mutex;
				_add_node(cur->link, newData);
			}
		}
		else {
			data_t *newData = (data_t *)malloc(sizeof(data_t));
			newData->tid = tid;
			newData->mutex = mutex;

			AdjNode *newNode = (AdjNode *)malloc(sizeof(AdjNode));
			newNode->node = newData;
			newNode->next = NULL;
			newNode->prev = NULL;
			
			LinkedList *link = (LinkedList *)malloc(sizeof(LinkedList));
			linked_list_init(link);
			newNode->link = link;

			newNode->next = plist->tail;
			newNode->prev = plist->tail->prev;
			plist->tail = newNode;
		}

		// TODO: Connect edges with stacked nodes
	}
}

void _del_node(LinkedList *plist, data_t *data) {
	if (plist == NULL || plist->head == NULL || data == NULL)
		return;
	
	pthread_t tid = pthread_self();
	Node *cur = plist->head;
	while ((cur != NULL) && (cur->data != data))
		cur = cur->next;

	if (cur == NULL)
		return;

	if (cur->prev != NULL) cur->prev->next = cur->next;
	else plist->head = cur->next;

	if (cur->next != NULL) cur->next->prev = cur->prev;
	else plist->tail = cur->prev;

	free(cur->data);
	free(cur);

}

void del_node(AdjList *plist, pthread_mutex_t *target) {
	if (plist == NULL || plist->head == NULL || target == NULL)
		return;

	pthread_t tid = pthread_self();
	AdjNode *cur_adj = plist->head;
	while (cur_adj != NULL) {
		Node *cur = NULL, *del = NULL;
		if (get_data(cur_adj)->mutex == target) {
			cur = cur_adj->link->head;
			while (cur != NULL) {
				del = cur;
				cur = cur->next;
				if (del->data->tid == tid)
					_del_node(cur_adj->link, del->data);
			}

			if (cur != NULL && cur_adj->link->head == NULL) {
				AdjNode *del_adj = cur_adj;
				if (cur_adj->prev != NULL) cur_adj->prev->next = cur_adj->next;
				else plist->head = cur_adj->next;

				if (cur_adj->next != NULL) cur_adj->next->prev = cur_adj->prev;
				else plist->tail = cur_adj->prev;

				delete_list(cur_adj->link);
				del_adj = cur_adj;
				cur_adj = cur_adj->prev;
				free(del_adj);
			}
		}
		else {
			cur = cur_adj->link->head;
			while (cur != NULL) {
				del = cur;
				cur = cur->next;
				if ((del->data->mutex == target) && (del->data->tid == tid))
					_del_node(cur_adj->link, del->data);
			}
		}
		cur_adj = cur_adj->next;
	}
}

void delete_list(LinkedList *plist) {
	if (plist == NULL)
		return;

	if (plist->head == NULL)
		free(plist);

	while (plist->head != NULL)
		_del_node(plist, plist->head->data);

	free(plist);
}

void delAdjList(AdjList *plist) {
	if (plist == NULL)
		return;

	AdjNode *cur = plist->head, *del = NULL;
	while (cur != NULL) {
		del = cur;
		cur = cur->next;
		while (del->link->head != NULL)
			_del_node(del->link, del->link->head->data);
		free(del->link);
		free(del->node);
		free(del);
	}
	free(plist);
}
