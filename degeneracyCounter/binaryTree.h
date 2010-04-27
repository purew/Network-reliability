/**
	A binary tree, from http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/tree.html
*/


#ifndef BINARYTREE_H_
#define BINARYTREE_H_



#include<stdlib.h>
#include<stdio.h>


typedef struct {
   long long val;
   struct node * right, * left;
} node;

void insert(node ** tree, node * item);

void printout(node * tree);

/** Add a value to the tree. If the tree doesn't exist yet,
	send a null pointer:
		node *root;
		addValue( &root, value ) */
void addValue( node **tree, long long num );

/** Return true if num exists in the tree. */
int hasNum( node *tree, long long num );

/** Delete all memory allocated by the tree, except for the top node. */
void deleteTree( node *tree );

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

#endif
