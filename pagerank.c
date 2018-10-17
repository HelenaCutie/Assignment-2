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

// typedef struct prNode {
// 	int index; // name of the url
//     int *prValue;
// 	struct prNode *prev;
// 	struct prNode *next;
// } prNode;

// typedef struct pageRank{
//     int nURLs;
// 	prNode *head;
//     prNode *last;
// } pageRank;

typedef struct inLinkRep{
	int ninlinks;
    Set inlinkURL;
} inLinkRep;

typedef struct outLinkRep{
	int noutlinks;
    Set outlinkURL;
} outLinkRep;

typedef struct inLinkRep *inLink;

typedef struct outLinkRep *outLink;

// pageRank *newPR() {

// 	// if (url == NULL) {
// 	// 	fprintf(stderr, "Error: Invalid input url.");
// 	// 	abort();
// 	// }
// 	pageRank new = malloc(sizeof(pageRank));
// 	assert(new != NULL);
// 	new->head = new->last = NULL;
//     new->nURLs = 0;
// 	return new;
// }

inLink *newInlinkRep() {

	inLink new = malloc(sizeof(inLink));
	assert(new != NULL);
    new->ninlinks = 0;
    Set inlinkURL = newSet();
	return new;
}

outLink *newOutlinkRep() {

	inLink new = malloc(sizeof(outLink));
	assert(new != NULL);
    new->noutlinks = 0;
    Set outlinkURL = newSet();
	return new;
}

// static prNode *newprNode(char *urlName，int maxIterations) {

// 	prNode *new = malloc(sizeof(prNode));
// 	if (new == NULL) {
// 		fprintf(stderr, "Error: Memory allocation failed.");
// 		abort();
// 	}
// 	new->url = strdup(urlName);
// 	new->prev = new->next = NULL;
//     new->prValue = calloc(maxIterations, sizeof(int));
// 	return new;
// }

// static prNode appendURL(pageRank pr, char *urlName, int maxIterations) {

// 	prNode *new = newprNode(urlName, maxIterations), maxIterations;
// 	if (pr->nURLs == 0) {
// 		pr->head = pr->last = new;
// 	} else {
// 		new->prev = pr->last;
// 		pr->last->next = new;
// 		pr->last = new;
// 	}
// 	pr->nURLs++;
//     return new;
// }

// static prNode findURL(pageRank pr, char *urlName) {
//     int found = 0;
//     prNode node = NULL;
//     for (prNode curr = pr->head; curr != NULL && found == 0; curr = curr->next) {
//         if (strcmp(curr->url, urlName) == 0) {
//             found = 1;
//             node = curr;
//         }
//     }
//     return node;
// }

void calculatePageRank(Graph g, int d, int diffPR, int maxIterations);

int main(int argc, char *argv[]) {
    
}

void calculatePageRank(int d, int diffPR, int maxIterations) {
    Set list = urlList();
    Graph g = urlGraph(list);
    // A table of inLink counts and inLink URLs for each URL in the collection
    inLink inLinkTable[list->max] = calloc(list->max, sizeof(inLink));
    // A table of outLink counts and outLink URLs for each URL in the collection
    outLink outLinkTable[list->max] = calloc(list->max, sizeof(outLink));

    int pagerank[list->max][maxIterations] = {0};
    int numNode = list->nelems;
    Link to = NULL;
    // Fill in the table of inLink values for each URL in the collection
    for (to = list->elems; to != NULL; to = to->next) {
        // Initialise the pagerank value for each URL
        int index = NameToNum(to->val);
        pagerank[index][0] = 1 / numNode;
        // Actually start filling in the table
        inLink collection = newInlinkRep();
        inLinkTable[index] = collection;
        Link from = NULL;
        for (from = list->elems; from != NULL; from = from->next) {
            if (isConnected(g, from->val, to->val)) {
                (inLinkTable[index])->ninlinks += 1;
                insertInto((inLinkTable[index])->inlinkURL, from->val);
            }
        }
    }

    Link src = NULL;
    // Fill in the table of outLink values for each URL in the collection
    for (src = list->elems; src != NULL; src = src->next) {
        int index = NameToNum(from->val);
        outLink collection = newOutlinkRep();
        outLinkTable[index] = collection;
        Link dest = NULL;
        for (dest = list->elems; dest != NULL; dest = dest->next) {
            if (isConnected(g, src->val, dest->val)) {
                (outLinkTable[index])->noutlinks += 1;
                insertInto((outLinkTable[index])->outlinkURL, dest->val);
            }
        }
    }
    
    int iteration = 0;
    int diff = diffPR;
    while (iteration < maxIterations && diff >= diffPR) {
        Link curr = NULL;
        for (curr = list->elems; curr != NULL; curr = curr->next) {
            int sum = 0;
            sum = sumPageRank(inLinkTable, outLinkTable, curr->val, iteration, pagerank);
            pagerank[index][iteration + 1] = (1 - d) / numNode + d * sum;
            diff = differenceAllURL(pr, iteration);
        }
        iteration++;
    }
    
}

int sumPageRank(inLink inLinkTable, outLink outLinkTable, char *urlName, int t, int **pagerank) {
    Link curr = NULL;
    int sum = 0;
    int index = NameToNum(urlName);
    for (curr = (inLinkTable[index])->inlinkURL; curr != NULL; curr = curr->next) {
        int weightIn = weightedIn(g, curr->val, urlName);
        int weightOut = weightedOut(g, curr->val, urlName);
        sum += parentURL->prValue[t] * weightIn * weightOut;
    }
    return sum;
}

// int *calculateInlinks(Set list, Graph g) {
//     int inLink[list->max] = {0};
//     Link to;
//     for (to = list->elems; to != NULL; to = to->next) {
//         int index = NameToNum(to);
//         Link from;
//         for (from = list->elems; from != NULL; from = from->next) {
//             if (isConnected(g, from, to)) {
//                 inLink[index]++;
//             }
//         }
//     }
//     return inLink;
// }


int differenceAllURL(pageRank pr, int iteration) {
    d
}

int weightedIn(Graph g, char *from, char *to) {

}

int weightedOut(Graph g, char *from, char *to) {
    
}
