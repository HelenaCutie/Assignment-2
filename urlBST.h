// Interface for urlBST ADT
// Written by Michael Darmanian 8/10/18
// Acknowledgement: This ADT is based on the BSTree ADT implementation from week 10 lab

#ifndef URLBST_H
#define URLBST_H

typedef struct BSTNode *BST;

// Create new url BST
BST newBST();

// Free memory allocated to url BST
void freeBST(BST tree);

// Inserts url "u" into BST at node containing the word "k" where k is a word that exists in the url u
// Creates new node for k if it doesn't exist and inserts u
BST BSTInsert(BST t, char *k, char *u);

// Prints BST in infix order to file
void fprintBST(BST t, FILE *);

#endif
