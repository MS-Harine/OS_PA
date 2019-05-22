#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

/*  Linked List Functions
 *  About 1-dimentional linked list
 */
LinkedList * makeLinkedList() {
	LinkedList *plist = (LinkedList *)malloc(sizeof(LinkedList));
	if (plist == NULL)
		perror("makeLinkedList() malloc on plist");
	
	plist->head = (Node *)malloc(sizeof(Node));
	plist->tail = (Node *)malloc(sizeof(Node));
	if (plist->head == NULL)
		perror("makeLinkedList() malloc on plist->head");
	if (plist->tail == NULL)
		perror("makeLinkedList() malloc on plist->tail");

	// Dummy nodes
	plist->head->next = plist->tail;
	plist->head->prev = NULL;
	plist->head->data = NULL;

	plist->tail->next = NULL;
	plist->tail->prev = plist->head;
	plist->tail->data = NULL;
	
	return plist;
}

void addNode(LinkedList *plist, Data *data) {
	if (plist == NULL || data == NULL)
		return;

	Node *newNode = (Node *)malloc(sizeof(Node));
	if (newNode == NULL)
		perror("addNode() malloc");

	newNode->data = data;

	newNode->prev = plist->tail->prev;
	newNode->next = plist->tail;
	plist->tail->prev->next = newNode;
	plist->tail->prev = newNode;
}

void delNode(LinkedList *plist, Data *data) {
	if (plist == NULL || data == NULL)
		return;

	Node *delNode = plist->head;
	while ((delNode->data != data) && (delNode != plist->tail))
		delNode = delNode->next;

	if (delNode == plist->tail)
		return NULL;
	
	delNode->prev->next = delNode->next;
	delNode->next->prev = delNode->prev;
	free(delNode);
}

void delList(LinkedList *plist) {
	if (plist == NULL)
		return;

	Node *cur = plist->head->next;
	while(cur != plist->tail) {
		cur = cur->next;
		delNode(plist, cur->prev->data);
	}

	delNode(plist, plist->head->data);
	delNode(plist, plist->tail->data);
	free(plist);
}

Data * getFirstDataFromList(List *plist) {
	if (plist == NULL)
		return NULL;
	
	if (plist->head->next == plist->tail)
		return NULL;

	return plist->head->next->data;
}


/*  Adjacency LinkedList Functions
 *  About 2-dimentional linked list
 */
AdjList * makeAdjList() {
	AdjList *plist = (AdjList *)malloc(sizeof(AdjList));
	if (plist == NULL)
		perror("makeAdjList() malloc on plist");

	// Dummy node
	plist->head = (AdjNode *)malloc(sizeof(AdjNode));
	plist->head->link = NULL;
	plist->head->next = NULL;
	plist->head->prev = NULL;

	return plist;
}

void addNodeToAdj(AdjList *plist, Data *from, Data *to) {
	if (plist == NULL || from == NULL)
		return;

	AdjNode *cur = plist->head;
	if (to == NULL) {
		while (cur->next != NULL)
			cur = cur->next;

		cur->next = (AdjNode *)malloc(sizeof(AdjNode));
		cur->next->next = NULL;
		cur->next->prev = cur;
		
		cur = cur->next;
		cur->link = makeList();
		addNode(cur->link, from);
	}
	else {
		cur = cur->next;
		while ((cur != NULL) &&
				(getFirstDataFromList(cur->link) != from))
			cur = cur->next;

		if (cur == NULL)
			return;
		addNode(cur->link, to);
	}
}

void delNodeFromAdj(AdjList *plist, Data *target) {
	if (plist == NULL || target == NULL)
		return;

	AdjNode *cur = plist->head->next;
	while ((cur != NULL) &&
			(getFirstDataFromList(cur->link) != target))
		cur = cur->next;

	if (cur == NULL)
		return NULL;
	
	cur->prev->next = cur->next;
	cur->next->prev = cur->prev;
	delList(cur->link);
	free(cur);
	
	cur = plist->head->next;
	while (cur != NULL) {
		delNode(cur->link, target);
		cur = cur->next;
	}
}

void delAdjList(AdjList *plist) {
	if (plist == NULL)
		return;

	AdjNode *cur = plist->head->next;
	AdjNode *prev = cur->prev;
	while (cur != NULL) {
		delList(cur->link);
		prev = cur;
		cur = cur->next;
		free(prev);
	}

	free(plist);
}
