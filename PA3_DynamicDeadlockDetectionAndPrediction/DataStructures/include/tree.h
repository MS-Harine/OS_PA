#ifndef __TREE_H__
#define __TREE_H__

#include "seg_data.h"

typedef struct _tree_node {
	seg_data_t *data;
	struct _tree_node *left;
	struct _tree_node *right;
} TreeNode;

typedef struct {
	TreeNode *root;
} Tree;

Tree * make_tree(seg_data_t *);
void delete_tree(Tree *);

void connect_left(TreeNode *, seg_data_t *);
void connect_right(TreeNode *, seg_data_t *);
TreeNode * get_left_child(TreeNode *);
TreeNode * get_right_child(TreeNode *);

TreeNode * find_node(TreeNode *, seg_data_t *);

#endif
