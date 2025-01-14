#ifndef TREE_H
#define TREE_H

#define MAX_TREENODES 128
#define NULL (void *)0

typedef struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
} TreeNode;

TreeNode* treeCreateNode(int val);
TreeNode* treeInsertNode(TreeNode *root, int val);
TreeNode* treeDeleteNode(TreeNode *root, int val);
TreeNode* treeSearch(TreeNode *root, int val);
TreeNode* treeFindMin(TreeNode *root);
void treePrintInOrder(TreeNode *root);

#endif
