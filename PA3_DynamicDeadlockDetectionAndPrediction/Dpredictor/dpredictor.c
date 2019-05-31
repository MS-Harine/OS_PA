#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "data.h"
#include "linked_list.h"
#include "graph.h"
#include "seg_data.h"
#include "tree.h"

#define BUF_SIZE 1024

void create(char *[3]);
void lock(char *[3]);
void unlock(char *[3]);
void print_fileline(const char *);

Tree *seg_tree = NULL;
AdjList *graph = NULL;
AdjList *thread_lock_stack = NULL;
char *binary_file = NULL;

#ifdef DEBUG
void print_seg_tree(TreeNode *seg) {
	if (seg == NULL)
		return;

	print_seg_tree(get_left_child(seg));
	printf("%d ", get_segment(seg->data));
	print_seg_tree(get_right_child(seg));
}

void print_graph(AdjList *plist) {
	printf("-------------------------\n");
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		printf("%p | %ld : ", get_data(cur->data), get_owner(cur->data));
		Node *c = cur->link->head->next;
		while (c != cur->link->tail) {
			printf("%p -> ", get_data(c->data));
			c = c->next;
		}
		printf("END\n");
		cur = cur->next;
	}
	printf("-------------------------\n");
}
#endif

AdjNode * find_adj_node(AdjList *plist, data_t *data, int (*comp)(data_t *, data_t *)) {
	if (plist == NULL || data == NULL)
		return NULL;
	
	AdjNode *cur = plist->head->next;
	while (cur != plist->tail) {
		if (comp(cur->data, data))
			break;
		cur = cur->next;
	}
	
	if (cur == plist->tail)
		return NULL;
	return cur;
}

int main(int argc, char *argv[]) {
	const char *filename = NULL;
	char buffer[BUF_SIZE] = { 0, };
	char *str = NULL, *data[3] = { 0, };
	FILE *fp = NULL;
	int i = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage %s <filepath>\n", argv[0]);
		return 0;
	}

	filename = argv[1];
	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Failed to open %s\n", filename);
		return 0;
	}

	graph = make_adj_list();
	thread_lock_stack = make_adj_list();

	fgets(buffer, BUF_SIZE - 1, fp);
	binary_file = strdup(buffer);
	binary_file = strtok(binary_file, "\n");
	while (fgets(buffer, BUF_SIZE - 1, fp) != NULL) {
		str = strtok(buffer, ",");
		for (i = 0; i < 3; i++)
			data[i] = strtok(NULL, ",\n");
		
		switch(str[0]) {
		case 'c':
			create(data);
			break;
		case 'l':
			lock(data);
			break;
		case 'u':
			unlock(data);
			break;
		}
#ifdef DEBUG
	print_graph(graph);
	printf("=== Segment inorder ===\n");
	print_seg_tree(seg_tree->root);
	printf("\n=======================\n\n");
#endif
	}

	fclose(fp);


	return 0;
}

void create(char *data[3]) {
	static int segment = 0;
	pthread_t parent = atoi(data[0]), child = atoi(data[1]);
	TreeNode *cur = NULL;

	if (seg_tree == NULL)
		seg_tree = make_tree(make_seg_data(parent, segment));
	
	seg_data_t *temp = make_seg_data(parent, -1);
	cur = find_tree_node(seg_tree->root, temp);
	delete_seg_data(temp);
	
	if (cur == NULL) {
		fprintf(stderr, "Algorithm is wrong...\n");
		exit(-1);
	}

	while (cur->left != NULL)
		cur = cur->left;

	connect_left(cur, make_seg_data(parent, segment + 1));
	connect_right(cur, make_seg_data(parent, segment + 2));
	segment += 2;

	AdjNode *node = graph->head->next;
	while (node != graph->tail)
		node = node->next;

	if (node != graph->tail)
		update_segment_data(node->data, get_segment(cur->data));
}

void lock(char *data[3]) {
	pthread_t tid = atoi(data[0]);
	char *mutex = data[1];
	
	if (seg_tree == NULL)
		seg_tree = make_tree(make_seg_data(tid, 0));

	seg_data_t *temp = make_seg_data(tid, -1);
	TreeNode * node = find_tree_node(seg_tree->root, temp);
	while (node->left != NULL)
		node = node->left;
	delete_seg_data(temp);
/*
	printf("%ld %d\n", get_seg_owner(node->data), get_segment(node->data));

	// Connect nodes with locked mutexes current
	data_t *tdata = make_data(mutex, tid, -1, -1, NULL);
	AdjNode *cur = find_adj_node(thread_lock_stack, tdata, compare_owner), *cur_node = NULL;
	if (cur == NULL) 
		adj_push_node(thread_lock_stack, make_data(mutex, tid, -1, -1, NULL));
	else {
		Node *lnode = cur->link->head->next;
		while (lnode != cur->link->tail) {
			cur_node = find_adj_node(graph, lnode->data, compare_data);
			linked_push_node(cur_node->link, make_data(mutex, tid, get_psegment(cur_node->data), get_segment(node->data), NULL));
			lnode = lnode->next;
		}
	}

	// Make nodes
	cur_node = find_adj_node(graph, tdata, compare_data);
	if (cur_node == NULL)
		adj_push_node(graph, make_data(mutex, tid, get_segment(node->data), -1, NULL));
	else if (get_owner(cur_node->data) == tid)
		linked_push_node(cur_node->link, make_data(mutex, tid, get_psegment(cur_node->data), get_segment(node->data), (void *)cur->link));
	else
		// TODO: Owner list?
		;
	
	linked_push_node(cur->link, make_data(mutex, tid, get_segment(node->data), -1, NULL));
	delete_data(tdata);
	*/
}

void unlock(char *data[3]) {
	pthread_t tid = atoi(data[0]);
	char *mutex = data[1];

	/*
	data_t *tdata = make_data(mutex, tid, -1, -1, NULL);
	AdjNode *cur = find_adj_node(thread_lock_stack, tdata, compare_owner);
	linked_delete_node(cur->link, cur->data);
	*/
}

void print_fileline(const char *address) {
	char buf[1024] = { 0, };
	sprintf(buf, "addr2line %s -e %s", address, binary_file);
	system(buf);
}
