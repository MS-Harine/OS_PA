#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "data.h"
#include "graph.h"
#include "linked_list.h"
#include "detector_func.h"

AdjNode * find_node_by_data(AdjList *plist, data_t *target) {
	if (plist == NULL)
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
	if (plist == NULL || is_empty_adj(plist))
		return TRUE;

	int result = TRUE;
	int is_leaf = TRUE;
	AdjNode *start = find_node_by_data(plist, linked_pop_node(stack));
	Node *cur = start->link->tail->prev;
	while (cur != start->link->head) {
		AdjNode *temp = find_node_by_data(plist, cur->data);
		if (temp->visiting)
			return FALSE;

		if (temp->visited == FALSE) {
			is_leaf = FALSE;
			linked_push_node(stack, cur->data);
		}
		cur = cur->prev;
	}

	start->visited = TRUE;
	start->visiting = TRUE;

	if (is_leaf) {
		start->visiting = FALSE;
		return TRUE;
	}

	while (!is_empty_linked(stack)) {
		result &= dfs(plist, stack);
		if (result == FALSE)
			return FALSE;

		is_leaf = TRUE;
		cur = start->link->head;
		while (cur != start->link->tail) {
			if (find_node_by_data(plist, cur->data)->visited == FALSE) {
				is_leaf = FALSE;
				break;
			}
			cur = cur->next;
		}
		
		if (is_leaf)
			start->visiting = FALSE;
	}

	return result;
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
	
	LinkedList *stack = make_linked_list();
	linked_push_node(stack, plist->head->next->data);
	int result = dfs(plist, stack);
	delete_linked_list(stack);

	return !result;
}
