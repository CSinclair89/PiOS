#include "tree.h"
#include "io.h"

TreeNode* treeCreateNode(int val) {
	static TreeNode nodePool[MAX_TREENODES];
	static int nodeIndex = 0;
	
	if (nodeIndex >= MAX_TREENODES) {
		printp("No more available nodes.\n");
		return NULL;
	}

	TreeNode *node = &nodePool[nodeIndex++];
	node->val = val;
	node->left = NULL;
	node->right = NULL;

	return node;
}

TreeNode* treeInsertNode(TreeNode *root, int val) { 
	// base case{
	if (root == NULL) return treeCreateNode(val);

	// binary search
	if (val < root->val) root->left = treeInsertNode(root->left, val);
	else root->right = treeInsertNode(root->right, val);
	return root;
}

TreeNode* treeDeleteNode(TreeNode *root, int val) {
	// base case
	if (root == NULL) return NULL;

	if (val < root->val) root->left = treeDeleteNode(root->left, val);
	else if (val > root->val) root->right = treeDeleteNode(root->right, val);
	else {
		// if there are no children
		if (root->left == NULL && root->right == NULL) return NULL;
		
		// if there is one child
		if (root->left == NULL) return root->right;
		else if (root->right == NULL) return root->left;
	
		// if there are two children
		TreeNode *minNode = treeFindMin(root->right);
		root->val = minNode->val;
		root->right = treeDeleteNode(root->right, minNode->val);
	}
	return root;
}

TreeNode* treeSearch(TreeNode *root, int val) {
	// base case
	if (root == NULL) return NULL;
	
	// binary search
	if (root->val == val) return root;
	else if (root->val > val) return treeSearch(root->left, val);
	else return treeSearch(root->right, val);
}

TreeNode* treeFindMin(TreeNode *root) {
	while (root->left != NULL) root = root->left;
	return root;
}

void treePrintInOrder(TreeNode *root) {
	if (root == NULL) return;
	treePrintInOrder(root->left);
	printp("%d ", root->val);
	treePrintInOrder(root->right);
}
