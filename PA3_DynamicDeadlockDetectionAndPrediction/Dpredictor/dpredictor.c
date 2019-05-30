#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "tree.h"

#define BUF_SIZE 1024

void create(char *[3]);
void lock(char *[3]);
void unlock(char *[3]);
char * get_filename(const char *);

Tree *seg_tree = NULL;
//AdjList *graph = NULL;
char *binary_file = NULL;

#ifdef DEBUG
void print_seg_tree(TreeNode *seg) {
	if (seg == NULL)
		return;

	print_seg_tree(get_left_child(seg));
	printf("%d ", get_segment(seg->data));
	print_seg_tree(get_right_child(seg));
}
#endif

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

	fgets(buffer, BUF_SIZE - 1, fp);
	binary_file = strdup(buffer);
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

		printf("%s\n", get_filename(data[2]));
	}

	fclose(fp);

#ifdef DEBUG
	printf("=== Segment inorder ===\n");
	print_seg_tree(seg_tree->root);
	printf("\n=======================\n");
#endif

	return 0;
}

void create(char *data[3]) {
	static int segment = 0;
	pthread_t parent = atoi(data[0]), child = atoi(data[1]);
	TreeNode *cur = NULL;

	if (seg_tree == NULL)
		seg_tree = make_tree(make_seg_data(parent, segment));
	
	seg_data_t *temp = make_seg_data(parent, -1);
	cur = find_node(seg_tree->root, temp);
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
}

void lock(char *data[3]) {
	pthread_t tid = atoi(data[0]);
	
}

void unlock(char *data[3]) {

}

char * get_filename(const char *address) {
	char *line = NULL, buf[1024] = { 0, };
	int pipes[2], pid = 0;
	pipe(pipes);

	pid = fork();
	if (pid == 0) {
		// Child process
		close(pipes[0]);
		dup2(pipes[1], 1);
		execl("addr2line", "addr2line", address, "-e", binary_file, NULL);
	}
	
	// Parent process
	close(pipes[1]);
	read(pipes[0], buf, BUF_SIZE - 1);
	line = strdup(buf);
	return line;
}
