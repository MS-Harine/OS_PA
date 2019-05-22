#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <pthread.h>

typedef pthread_mutex_t * Data;

typedef struct _node {
	Data *data;
	struct _node *next;
	struct _node *prev;
} Node;

typedef struct _linkedList {
	Node *head;
	Node *tail;
} LinkedList;

LinkedList * makeLinkedList();
void addNode(LinkedList *, Data *);
void delNode(LinkedList *, Data *);
void delList(LinkedList *);


typedef struct _adjNode {
	LinkedList *link;
	struct _adjNode *next;
	struct _adjNode *prev;
} AdjNode;

typedef struct _adjList {
	AdjNode *head;
} AdjList;

AdjList * makeAdjList();
void addNodeToAdj(AdjList *, Data *, Data *);
void delNodeFromAdj(AdjList *, Data *);
void delAdjList(AdjList *);

#endif
