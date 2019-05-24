#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <pthread.h>

typedef struct {
	pthread_mutex_t *mutex;
	pthread_t tid;
} data_t;

typedef struct _node {
	data_t *data;
	struct _node *next;
	struct _node *prev;
} Node;

typedef struct {
	Node *head;
	Node *tail;
} LinkedList;

typedef struct _adj_node {
	data_t *node;
	LinkedList *link;
	struct _adj_node *next;
	struct _adj_node *prev;
} AdjNode;

typedef struct _adjList {
	AdjNode *head;
	AdjNode *tail;
} AdjList;

void linked_list_init(LinkedList *);
void delete_list(LinkedList *);

void adj_list_init(AdjList *);
void add_node(AdjList *, pthread_mutex_t *);
void del_node(AdjList *, pthread_mutex_t *);
void delete_adj_list(AdjList *);

#endif
