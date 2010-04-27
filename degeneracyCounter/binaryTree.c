/**
	A binary tree, from http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/tree.html
*/



#include <stdlib.h>
#include <stdio.h>
#include "binaryTree.h"



//typedef struct tree_el node;

void insert(node ** tree, node * item) {
   if(!(*tree)) {
      *tree = item;
      return;
   }
   if(item->val<(*tree)->val)
      insert(&(*tree)->left, item);
   else if(item->val>(*tree)->val)
      insert(&(*tree)->right, item);
}

void printout(node * tree) {
   if(tree->left) printout(tree->left);
   printf("%d\n",tree->val);
   if(tree->right) printout(tree->right);
}

void addValue( node **tree, long long num )
{
	node *curr = malloc( sizeof(node) );
	curr->val = num;
	curr->left = curr->right = 0;
	insert(tree, curr);
}

int hasNum( node *tree, long long num )
{
	if (tree==0)
		return 0;
	if (tree->val > num && tree->left != 0)
		return hasNum( tree->left, num );
	if (tree->val < num && tree->right != 0)
		return hasNum( tree->right, num );
	if (tree->val == num)
		return 1;

	return 0;
}

void deleteTree( node *tree )
{
	if (tree==0)
		return;

	if (tree->left != 0)
	{
		deleteTree( tree->left );
	}
	if (tree->right != 0)
	{
		deleteTree( tree->right );
//		free( tree->right );
//		tree->right = 0;
	}
	free( tree );
	return;
}

/*
void main() {
   node * curr, * root;
   int i;

   root = NULL;

   for(i=1;i<=10;i++) {
      curr = (node *)malloc(sizeof(node));
      curr->left = curr->right = NULL;
      curr->val = rand();
      insert(&root, curr);
   }

   printout(root);
}*/
