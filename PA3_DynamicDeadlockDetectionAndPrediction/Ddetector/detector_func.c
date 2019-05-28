#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "data.h"
#include "graph.h"
#include "linked_list.h"
#include "detector_func.h"

AdjNode * find_node_by_data(AdjList *plist, data_t *target) {
	if (plist == NULL || target == NULL)
		return NULL;
	
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		if (get_data(cur->data) == get_data(target))
			break;
		cur = cur->next;
	}
	
	if (cur == plist->tail)
		return NULL;
	return cur;
}

int is_exist_linked_node(LinkedList *plist, data_t *target) {
	if (plist == NULL || target == NULL)
		return FALSE;

	Node *cur = plist->head->next;
	while (cur != plist->tail) {
		if (get_data(cur->data) == get_data(target))
			break;
		cur = cur->next;
	}

	if (cur == plist->tail)
		return FALSE;
	return TRUE;
}

void assign_mutex(AdjList *plist, data_t *data, LinkedList *stack) {
	if (plist == NULL)
		return;

	AdjNode *cur = NULL;

	// Connect nodes with locked mutexes current
	Node *locked_mutex = stack->head->next;
	while (locked_mutex != stack->tail) {
		cur = find_node_by_data(plist, locked_mutex->data);
		if (cur == NULL) {
			fputs("Algorithm is wrong...", stderr);
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
	else
		add_owner(cur->data, pthread_self());

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
				if (compare_owner(cur_node->data, data)) {
					Node *del_node = cur_node;
					cur_node = cur_node->prev;
					linked_delete_node(cur->link, del_node->data);
				}
				cur_node = cur_node->next;
			}

			
			if (is_empty_linked(cur->link)) {
				AdjNode *del = cur;

				delete_owner(del->data, get_owner(data));
				if (get_owner(del->data) == 0) {
					cur = cur->prev;
					adj_delete_node(plist, del->data);
				}
			}
		}
		else {
			Node *cur_node = cur->link->head->next;
			while (cur_node != cur->link->tail) {
				if (compare_data(cur_node->data, data) && compare_owner(cur_node->data, data)) {
					Node *del_node = cur_node;
					cur_node = cur_node->prev;
					linked_delete_node(cur->link, del_node->data);
					break;
				}
				cur_node = cur_node->next;
			}
		}
		cur = cur->next;
	}
}

int is_cycle(AdjList *plist, AdjNode *start) {
	if (plist == NULL || is_empty_adj(plist) || start == NULL)
		return FALSE;
	
	AdjNode *temp = NULL;
	Node *cur = NULL;

	if (start->visited == FALSE) {
		start->visited = TRUE;
		start->visiting = TRUE;

		cur = start->link->head->next;
		while (cur != start->link->tail) {
			temp = find_node_by_data(plist, cur->data);
			if (temp->visiting)
				return TRUE;
			else if ((temp->visited == FALSE) && is_cycle(plist, temp))
				return TRUE;
			cur = cur->next;
		}
	}
	start->visiting = FALSE;
	return FALSE;
}

int find_cycle(AdjList *plist) {
	if (is_empty_adj(plist))
		return FALSE;

	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		cur->visited = FALSE;
		cur->visiting = FALSE;
		cur = cur->next;
	}
	
	cur = plist->head->next;
	while (cur != plist->tail) {
		if (is_cycle(plist, cur))
			return TRUE;
		cur = cur->next;
	}

	return FALSE;
}
