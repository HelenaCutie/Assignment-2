// Interface for urlBST ADT.
// Written by Michael Darmanian 8/10/18

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

// Prints BST in infix order
void printBST(BST t);

#endif
