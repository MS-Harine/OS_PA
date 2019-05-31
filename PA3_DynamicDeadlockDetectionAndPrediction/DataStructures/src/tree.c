#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

Tree * make_tree(seg_data_t *data) {
	Tree *new_tree = (Tree *)malloc(sizeof(Tree));
	new_tree->root = (TreeNode *)malloc(sizeof(TreeNode));
	new_tree->root->data = data;
	new_tree->root->left = NULL;
	new_tree->root->right = NULL;
	return new_tree;
}

void _delete_tree(TreeNode *node) {
	if (node == NULL)
		return;
	else if (node->left == NULL && node->right == NULL) {
		delete_seg_data(node->data);
		free(node);
	}
	else if (node->left == NULL)
		_delete_tree(node->right);
	else if (node->right == NULL)
		_delete_tree(node->left);
}

void delete_tree(Tree *ptree) {
	if (ptree == NULL)
		return;

	_delete_tree(ptree->root);
	free(ptree);
}

void connect_left(TreeNode *pnode, seg_data_t *data) {
	if (pnode == NULL)
		return;

	TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
	new_node->data = data;
	new_node->left = NULL;
	new_node->right = NULL;

	pnode->left = new_node;
}

void connect_right(TreeNode *pnode, seg_data_t *data) {
	if (pnode == NULL)
		return;

	TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
	new_node->data = data;
	new_node->left = NULL;
	new_node->right = NULL;

	pnode->right = new_node;
}

TreeNode * get_left_child(TreeNode *pnode) {
	if (pnode == NULL)
		return NULL;

	return pnode->left;
}

TreeNode * get_right_child(TreeNode *pnode) {
	if (pnode == NULL)
		return NULL;

	return pnode->right;
}

TreeNode * find_tree_node(TreeNode *pnode, seg_data_t *d) {
	if (pnode == NULL)
		return NULL;
	else if (compare_seg_owner(pnode->data, d))
		return pnode;

	TreeNode *result = NULL;
	if ((result = find_tree_node(pnode->left, d)) != NULL)
		return result;
	else if ((result = find_tree_node(pnode->right, d)) != NULL)
		return result;
	else
		return NULL;
}
