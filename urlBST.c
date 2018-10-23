// Binary search tree ADT implementation for urls based on keyword using linked lists
// Written by Michael Darmanian 8/10/18
// Acknowledgement: This ADT is based on the BSTree ADT implementation from lectures and set ADT by John Shepherd

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "urlBST.h"
#include "set.h"

typedef struct BSTNode 
{
	char *key;
	Set list;
	BST left, right;
} BSTNode;

// Function prototypes:

static BST newBSTNode(char *k);
static void fShowSet(Link list, FILE *fp);

// Create new node in BST with the key "k"
static BST newBSTNode(char *k)
{
    BST new = malloc(sizeof(BSTNode));
    assert(new != NULL);
    new->key = strdup(k);
    new->list = newSet();
    new->left = new->right = NULL;
    return new;
}

// Return empty BST
BST newBST()
{
    return NULL;
}

// Frees allocated memory in BST
void freeBST(BST t)
{
    if (t == NULL) return;
    freeBST(t->right);
    freeBST(t->left);
    disposeSet(t->list);
    free(t->key);
    free(t);
}

// Inserts url "u" into BST at node containing the word "k" where k is a word that exists in the url u
// Creates new node for k if it doesn't exist
BST BSTInsert(BST t, char *k, char *u)
{
    if (t == NULL) 
    {
        BST new = newBSTNode(k);
        insertInto(new->list, u);
        return new;
    }
    else if (strcmp(k, t->key) < 0) t->left = BSTInsert(t->left, k, u);
    else if (strcmp(k, t->key) > 0) t->right = BSTInsert(t->right, k, u);
    else insertInto(t->list, u);
    return t;
}

// Prints BST in infix order to file
void fprintBST(BST t, FILE *fp)
{
    if (t == NULL) return;
    fprintBST(t->left, fp);
    fprintf(fp, "%s  ", t->key);
    fShowSet(t->list->elems, fp);
    fprintf(fp, "\n");
    fprintBST(t->right, fp);
}

// Helper functions:

// Prints set into file
static void fShowSet(Link list, FILE *fp)
{
    if (list == NULL) return;
    fprintf(fp, "%s ", list->val);
    fShowSet(list->next, fp);
}
