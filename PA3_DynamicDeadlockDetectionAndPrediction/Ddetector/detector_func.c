#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "data.h"
#include "graph.h"
#include "linked_list.h"
#include "detector_func.h"

AdjNode * find_node_by_id(AdjList *plist, cid_t target) {
	if (plist == NULL)
		return NULL;
	
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		if (cur->tid == target)
			break;
		cur = cur->next;
	}
	
	if (cur == plist->tail)
		return NULL;
	return cur;
}

AdjNode * find_node_by_data(AdjList *plist, data_t *target) {
	if (plist == NULL)
		return NULL;
	
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		if (cur->mutex == target)
			break;
		cur = cur->next;
	}
	
	if (cur == plist->tail)
		return NULL;
	return cur;
}

void assign_mutex(AdjList *plist, data_t *data, LinkedList *stack) {
	if (plist == NULL)
		return;

	AdjNode *cur = NULL;

	// Connect nodes with locked mutexes current
	Node *locked_mutex = stack->head->next;
	while (stacked_mutex != stack->tail) {
		cur = find_node_by_data(plist, get_data(locked_mutex->data));
		if (cur == NULL) {
			fputs(stderr, "Algorithm is wrong...");
			exit(-1);
		}

		linked_push_node(cur->link, data);
		locked_mutex = locked_mutex->next;
	}
	
	// Make nodes
	cur = find_node_by_data(plist, data);
	if (cur == NULL)
		adj_push_node(plist, data);
	else if (get_owner(cur->data) == pthread_self())
		linked_push_node(cur->link, data);

	linked_push_node(stack, data);
}

void expire_mutex(AdjList *plist, data_t *data) {
	if (plist == NULL)
		return;

	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		if (compare_data(cur->data, data)) {
			Node *cur_node = cur->link->head->next;
			while (cur_node != cur->link->tail) {
				if (compare_owner(cur_node->data, data))
					linked_delete_node(cur->link, cur_node->data);
				cur_node = cur_node->next;
			}

			if (is_empty_linked(cur->link))
				adj_delete_node(plist, cur->data);
		}
		else {
			Node *cur_node = cur->link->head->next;
			while (cur_node != cur->link->tail) {
				if (compare_data(cur_node->data, data) && compare_owner(cur_node->data, data)) {
					linked_delete_node(cur->link, cur_node->data);
					break;
				}
				cur_node = cur_node->next;
			}
		}
	}
}

int dfs(AdjList *plist, LinkedList *stack) {
}

int find_cycle(AdjList *plist) {
}
