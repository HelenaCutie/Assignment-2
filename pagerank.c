// Calculate weighted pagerank for every url in the file
// Written by Helena Ling on 07/10/18
// Acknowledgement: Graph, stack, queue, set ADTs written by John Shepherd, BSTree ADT from week10 lab

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
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
	double ninlinks;
    Set inlinkURL;
} inLinkRep;

typedef struct outLinkRep{
	double noutlinks;
    Set outlinkURL;
} outLinkRep;

typedef struct prValue{
    char *url;
    double noutlinks;
    double pageRankValue;
} prValue;

typedef struct inLinkRep *inLink;

typedef struct outLinkRep *outLink;

inLink *newInlinkRep();
outLink *newOutlinkRep();
double compare (const void * a, const void * b);
int calculatePageRank(Set list, Graph g, int d, int diffPR, int maxIterations, double **pagerank, inLink *inLinkTable, outLink *outLinkTable);
int sumPageRank(inLink inLinkTable, outLink outLinkTable, int index, int iteration, double **pagerank);
int inLinkSumOfReferencePages(inLink inLinkTable, outLink outLinkTable, int src);
int outLinkSumOfReferencePages(inLink inLinkTable, outLink outLinkTable, int src);
int differenceAllURL(double **pagerank, int iteration, int numNode);

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

// PRValue newPRValue(char *urlName, int links, int pr) {
//     PRValue new = calloc(1, sizeof(prValue));
//     new->url = strdup(urlName);
//     new->noutlinks = links;
//     new->pageRankValue = pr;
//     return new;
// }

inLink *newInlinkRep() {

	inLink new = malloc(sizeof(inLinkRep));
	assert(new != NULL);
    new->ninlinks = 0;
    Set inlinkURL = newSet();
	return new;
}

outLink *newOutlinkRep() {

	inLink new = malloc(sizeof(outLinkRep));
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


double compare (const void * a, const void * b) {

    prValue *prValueA = (prValue *)a;
    prValue *prValueB = (prValue *)b;

    return (prValueA->pageRankValue - prValueB->pageRankValue);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: pagerank DampingFactor DifferenceInPageRankSum maxIterations\n");
    }
    exit(1);
    if (argv[1] >= 1 || argv[1] < 0) {
        fprintf(stderr, "0 < damping factor < 1\n");
    }
    exit(1);
    if (argv[2] < 0) {
        fprintf(stderr, "difference in PageRank Sum > 0\n");
    }
    exit(1);
    if (argv[3] <= 0) {
        fprintf(stderr, "maxIterations > 0\n");
    }
    exit(1);

    Set list = urlList();
    Graph g = urlGraph(list);
    int maxIterations = argv[3];
    double pagerank[list->max + 1][maxIterations + 1] = {0};
    // A table of inLink counts and inLink URLs for each URL in the collection
    inLink *inLinkTable = calloc(list->max + 1, sizeof(inLink));
    // A table of outLink counts and outLink URLs for each URL in the collection
    outLink *outLinkTable = calloc(list->max + 1, sizeof(outLink));
    numIterations = calculatePageRank(list, g, argv[1], argv[2], maxIterations, pagerank, inLinkTable, outLinkTable);
    Link url = NULL;
    prValue *PRList = calloc(list->max + 1, sizeof(struct prValue));
    int index = 0;
    for (url = list->elems; url != NULL; url = url->next) {
        PRList[index].url = strdup(url->val);
        PRList[index].noutlinks = (outLinkTable[index])->noutlinks;
        PRList[index].pageRankValue = pagerank[index][numIterations];
        index++;
    }
    qsort(PRList, list->nelems, sizeof(prValue), compare);
    FILE *fp = fopen("pagerankList.txt", "w");
    int count = 0;
    for (count = 0; count < index; count++) {
        fprintf(fp, "%s, %d, %.7f", PRList[count].url, PRList[count].noutlinks, PRList[count].pageRankValue);
    }
    fclose(fp);
    return 0;
}

int calculatePageRank(Set list, Graph g, int d, int diffPR, int maxIterations, double **pagerank, inLink *inLinkTable, outLink *outLinkTable) {

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
            if ((outLinkTable[index])->noutlinks == 0) {
                (outLinkTable[index])->noutlinks = 0.5;
            }
        }
    }
    
    int iteration = 0;
    int diff = diffPR;
    while (iteration < maxIterations && diff >= diffPR) {
        Link curr = NULL;
        for (curr = list->elems; curr != NULL; curr = curr->next) {
            int sum = 0;
            int index = NameToNum(curr->val);
            sum = sumPageRank(inLinkTable, outLinkTable, index, iteration, pagerank);
            pagerank[index][iteration + 1] = (1 - d) / numNode + d * sum;
            diff = differenceAllURL(pagerank, iteration, numNode);
        }
        iteration++;
    }
    return iteration;
}

int sumPageRank(inLink inLinkTable, outLink outLinkTable, int index, int iteration, double **pagerank) {
    Link src = NULL;
    int sum = 0;
    for (src = (inLinkTable[index])->inlinkURL; src != NULL; src = src->next) {
        int src = NameToNum(src->val);
        int weightIn = (inLinkTable[index])->ninlinks / inLinkSumOfReferencePages(inLinkTable, outLinkTable, src);
        int weightOut = (outLinkTable[index])->noutlinks / outLinkSumOfReferencePages(inLinkTable, outLinkTable, src);
        sum += pagerank[index][iteration] * weightIn * weightOut;
    }
    return sum;
}

int inLinkSumOfReferencePages(inLink inLinkTable, outLink outLinkTable, int src) {
    int sum = 0;
    Link destUrl = NULL;
    for (destUrl = (outLinkTable[src])->outlinkURL; destUrl != NULL; destUrl = destUrl->next) {
        int dest = NameToNum(destUrl->val);
        sum += (inLinkTable[dest])->ninlinks;
    }
    return sum;
}

int outLinkSumOfReferencePages(inLink inLinkTable, outLink outLinkTable, int src) {
    int sum = 0;
    Link destUrl = NULL;
    for (destUrl = (outLinkTable[src])->outlinkURL; destUrl != NULL; destUrl = destUrl->next) {
        int dest = NameToNum(destUrl->val);
        sum += (outLinkTable[dest])->noutlinks;
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


int differenceAllURL(double **pagerank, int iteration, int numNode) {
    int diff = 0;
    int count = 0;
    for (count = 0; count < numNode; count++) {
        int diffOneURL = abs(pagerank[count][iteration + 1] - pagerank[count][iteration]);
        diff += diffOneURL;
    }
    return diff;
}