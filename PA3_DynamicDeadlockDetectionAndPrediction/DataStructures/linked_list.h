#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "data.h"

typedef struct _node {
	data_t *data;
	struct _node *next;
	struct _node *prev;
} Node;

typedef struct {
	Node *head;
	Node *tail;
} LinkedList;

LinkedList * make_linked_list();
void delete_linked_list(LinkedList *);

void linked_push_node(LinkedList *, data_t *);
data_t * linked_pop_node(LinkedList *);
void linked_delete_node(LinkedList *, data_t *);

int is_empty_linked(LinkedList *);

#endif
