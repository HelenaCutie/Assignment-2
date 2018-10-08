// Binary search tree ADT implementation for urls based on keyword using linked lists
// Written by Michael Darmanian 8/10/18

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "urlBST.h"

typedef struct URLNode *URL;

typedef struct URLNode 
{
	char *url;
	URL next;
} URLNode;

typedef struct BSTNode 
{
	char *key;
	URL urlList;
	BST left, right;
} BSTNode;

// Function prototypes:

static BSTNode newBSTNode(char *k);
static URL newURLNode(char *u);
static void freeURLList(URL l);
static URL URLListInsert(URL l, char *u);
static void printURLList(URL l);

// Create new node in BST with the key "k"
static BSTNode newBSTNode(char *k)
{
    BST new = malloc(sizeof(BSTNode));
    assert(new != NULL);
    new->key = k;
    new->urlList = newSet();
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
    freeURLList(t->urlList);
    free(t);
}

// Inserts url "u" into BST at node containing the word "k" where k is a word that exists in the url u
// Creates new node for k if it doesn't exist
BST BSTInsert(BST t, char *k, char *u)
{
    if (t == NULL) 
    {
        BST new = newBSTNode(k);
        new->url = URLListInsert(new->url, u);
        return new;
    }
    else if (strcmp(k, t->key) < 0) t->left = BSTInsert(t->left, k, u);
    else if (strcmp(k, t->key) > 0) t->right = BSTInsert(t->right, k, u);
    else // if (strcmp(k, t->key) == 0)
    {
        t->url = URLListInsert(t->url, u);
        return t;
    }
}

// Prints BST in infix order
void printBST(BST t)
{
    if (t == NULL) return;
    printBST(t->left);
    printf("%s ", t->key);
    printURLList(t->urlList);
    printf("\n");
    printBST(t->right);
}

// Helper functions:

// Creates a new URL node
static URL newURLNode(char *u)
{
    URL new = malloc(sizeof(URLNode));
    assert(new != NULL);
    new->url = u;
    new->next = NULL;
    return new;
}

// Frees memory allocated for list
static void freeURLList(URL l)
{
    if (l == NULL) return;
    free(l->next);
    free(l);
}

// Inserts the url "u" into the list in alphabetical order if it doesn't already exist and returns the head of the new list
// Returns l if u is NULL
static URL URLListInsert(URL l, char *u)
{
    if (u == NULL) return l;
    else if (l == NULL) return newURLNode(u);
    for (URL curr = l; curr->next != NULL; curr = curr->next)
    {
        // duplicate
        if (strcmp(u, curr->url) == 0) break; 
        // start of the list
        else if (curr == l && strcmp(u, curr->url) < 0)
        {
            URL new = newURLNode(u); 
            new->next = l;
            return new;
        }
        // middle of the list
        else if (strcmp(u, curr->url) > 0 && strcmp(u, curr->next->url) < 0)
        {
            URL temp = curr->next;
            curr->next = newURLNode(u); 
            curr->next->next = temp;
            break;
        }
        // end of the list
        else if (curr->next == NULL && strcmp(u, curr->url) > 0)
        {
            curr->next = newURLNode(u); 
            break;
        }
    }
    return l;
}

// Prints the URL list
static void printURLList(URL l)
{
    if (l == NULL) return;
    printf("%s ", l->url);
    printURLList(l->next);
}
