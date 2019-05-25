#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "data.h"
#include "linked_list.h"

typedef struct _adj_node {
	data_t *data;
	LinkedList *link;
	struct _adj_node *next;
	struct _adj_node *prev;

	int visited;
	int visiting;
} AdjNode;

typedef struct {
	AdjNode *head;
	AdjNode *tail;
} AdjList;

AdjList * make_adj_list();
void delete_adj_list(AdjList *);

void adj_push_node(AdjList *, data_t *);
data_t * adj_pop_node(AdjList *);
void adj_delete_node(AdjList *, data_t *);

int is_empty_adj(AdjList *);

#endif
