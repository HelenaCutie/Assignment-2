// Calculate weighted pagerank for every url in the file
// Written by Helena Ling on 07/10/18
// Acknowledgement: Graph, stack, queue, set ADTs written by John Shepherd, BSTree ADT from week10 lab

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "stack.h"
#include "set.h"
#include "graph.h"
#include "readData.h"

typedef struct prNode {
	char *url; // string of this line
    int *prValue;
	struct prNode *prev;
	struct prNode *next;
} prNode;

typedef struct pageRank{
    int nURLs;
	prNode *head;
    prNode *last;
} pageRank;

pageRank *newPR () {

	if (url == NULL) {
		fprintf(stderr, "Error: Invalid input url.");
		abort();
	}
	pageRank new = malloc(sizeof(pageRank));
	assert(new != NULL);
	new->head = new->last = NULL;
    new->nURLs = 0;
	return new;
}

static prNode *newprNode(char *urlName，int maxIterations) {

	prNode *new = malloc(sizeof(prNode));
	if (new == NULL) {
		fprintf(stderr, "Error: Memory allocation failed.");
		abort();
	}
	new->url = strdup(urlName);
	new->prev = new->next = NULL;
    new->prValue = calloc(maxIterations, sizeof(int));
	return new;
}

static prNode appendURL(pageRank pr, char *urlName, int maxIterations) {

	prNode *new = newprNode(urlName, maxIterations), maxIterations;
	if (pr->nURLs == 0) {
		pr->head = pr->last = new;
	} else {
		new->prev = pr->last;
		pr->last->next = new;
		pr->last = new;
	}
	pr->nURLs++;
    return new;
}

static prNode findURL(pageRank pr, char *urlName) {
    int found = 0;
    prNode node = NULL;
    for (prNode curr = pr->head; curr != NULL && found == 0; curr = curr->next) {
        if (strcmp(curr->url, urlName) == 0) {
            found = 1;
            node = curr;
        }
    }
    return node;
}

void calculatePageRank(Graph g, int d, int diffPR, int maxIterations);

int main(int argc, char *argv[]) {
    
}

void calculatePageRank(int d, int diffPR, int maxIterations) {
    Set list = urlList();
    Graph g = urlGraph(list);
    int numNode = list->nelems;
    // Calculate page rank
    pageRank pr = newPR();
    Link currListNode = NULL;
    for (currListNode = list->elems; currListNode != NULL; currListNode = currListNode->next) {
        prNode newNode = appendURL(pr, currListNode->val, maxIterations);
        newNode->prValue[0] = 1/numNode;
    }
    int iteration = 0;
    int diff = diffPR;
    while (iteration < maxIterations && diff >= diffPR) {
        iteration++;
        Link curr = NULL;
        for (curr = list->elems; curr != NULL; curr = curr->next) {
            prNode currURL = findURL(pr, curr->val);
            int sum = 0;
            sum = sumPageRank(list, g, curr->val, iteration - 1, pr);
            currURL->prValue[itertion] = (1 - d) / numNode + d * sum;
            diff = differenceAllURL(pr, iteration);
        }
    }
    
}

int sumPageRank(Set list, Graph g, char *urlName, int t, pageRank pr) {
    Link curr = NULL;
    int sum = 0;
    for (curr = list->elems; curr != NULL; curr = curr->next) {
        if (strcmp(curr->val, urlName) != 0 && isConnected(g, curr->val, urlName)) {
            prNode parentURL = findURL(pr, curr->val);
            int weightIn = weightedIn(g, curr->val, urlName);
            int weightOut = weightedOut(g, curr->val, urlName);
            sum += parentURL->prValue[t] * weightIn * weightOut;
        }
    }
    return sum;
}

int differenceAllURL(pageRank pr, int iteration) {
    d
}

int weightedIn(Graph g, char *from, char *to) {

}

int weightedOut(Graph g, char *from, char *to) {
    
}