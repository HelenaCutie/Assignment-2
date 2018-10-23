// Calculate weighted pagerank for every url in the file
// Written by Helena Ling on 07/10/18
// Acknowledgement: Graph and set ADTs written by John Shepherd, BSTree ADT from week10 lab

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "set.h"
#include "graph.h"
#include "readData.h"

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

inLink newInlinkRep();
outLink newOutlinkRep();
int compare (const void * a, const void * b);
double calculatePageRank(Set list, Graph g, double d, double diffPR, int maxIterations, double **pagerank, inLink *inLinkTable, outLink *outLinkTable, indexList urlIndex);
double sumPageRank(inLink *inLinkTable, outLink *outLinkTable, int index, int iteration, double **pagerank, indexList urlIndex);
int inLinkSumOfReferencePages(inLink *inLinkTable, outLink *outLinkTable, int src, indexList urlIndex);
double outLinkSumOfReferencePages(inLink *inLinkTable, outLink *outLinkTable, int src, indexList urlIndex);
double differenceAllURL(double **pagerank, int iteration, int numNode);

inLink newInlinkRep() {

	inLink new = malloc(sizeof(inLinkRep));
	assert(new != NULL);
    new->ninlinks = 0;
    Set inlinkURL = newSet();
    new->inlinkURL = inlinkURL;
	return new;
}

outLink newOutlinkRep() {

	outLink new = malloc(sizeof(outLinkRep));
	assert(new != NULL);
    new->noutlinks = 0;
    Set outlinkURL = newSet();
    new->outlinkURL = outlinkURL;
	return new;
}

void freeInlink(inLink i) {
    disposeSet(i->inlinkURL);
    free(i);
}

void freeOutlink(outLink i) {
    disposeSet(i->outlinkURL);
    free(i);
}

int compare (const void * a, const void * b) {

    const prValue *prValueA = (const prValue *)a;
    const prValue *prValueB = (const prValue *)b;
    if (prValueA->pageRankValue < prValueB->pageRankValue) {
        return 1;
    } else {
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: pagerank DampingFactor DifferenceInPageRankSum maxIterations\n");
        exit(1);
    }
    if (atof(argv[1]) >= 1 || atof(argv[1]) < 0) {
        fprintf(stderr, "0 < damping factor < 1\n");
        exit(1);
    }
    if (atof(argv[2]) < 0) {
        fprintf(stderr, "difference in PageRank Sum > 0\n");
        exit(1);
    }
    if (atof(argv[3]) <= 0) {
        fprintf(stderr, "maxIterations > 0\n");
        exit(1);
    }
    Set list = urlList();
    Graph g = urlGraph(list);
    indexList urlIndex = setUpIndex();
    int maxIterations = atof(argv[3]);
    double **pagerank = calloc(list->nelems, sizeof(double *));
    int row_count = 0;
    for (row_count = 0; row_count < (list->nelems); row_count++) {
        pagerank[row_count] = calloc(maxIterations + 1, sizeof(double));
    }
    // A table of inLink counts and inLink URLs for each URL in the collection
    inLink *inLinkTable = calloc(list->nelems, sizeof(inLink));
    // A table of outLink counts and outLink URLs for each URL in the collection
    outLink *outLinkTable = calloc(list->nelems, sizeof(outLink));

    int numIterations = calculatePageRank(list, g, atof(argv[1]), atof(argv[2]), maxIterations, pagerank, inLinkTable, outLinkTable, urlIndex);
    Link url = NULL;
    prValue *PRList = calloc(list->nelems, sizeof(struct prValue));

    int count_url = 0;
    for (url = list->elems; url != NULL; url = url->next) {
        int index = NameToNum(urlIndex, url->val);
        PRList[count_url].url = strdup(url->val);
        PRList[count_url].noutlinks = (outLinkTable[index])->noutlinks;
        PRList[count_url].pageRankValue = pagerank[index][numIterations];
        count_url++;
    }
    qsort(PRList, list->nelems, sizeof(prValue), &compare);
    FILE *fp = fopen("pagerankList.txt", "w");
    int count = 0;
    for (count = 0; count < count_url; count++) {
        fprintf(fp, "%s, %d, %.7f\n", PRList[count].url, (int) PRList[count].noutlinks, PRList[count].pageRankValue);
    }

    fclose(fp);

    int free_count = 0;
    for (free_count = 0; free_count < count_url; free_count++) {
        free(PRList[free_count].url);
    }
    free(PRList);

    Link to = NULL;
    for (to = list->elems; to != NULL; to = to->next) {
        // Initialise the pagerank value for each URL
        int index = NameToNum(urlIndex, to->val);
        freeOutlink(outLinkTable[index]);
    }
    free(outLinkTable);

    to = NULL;
    for (to = list->elems; to != NULL; to = to->next) {
        // Initialise the pagerank value for each URL
        int index = NameToNum(urlIndex, to->val);
        freeInlink(inLinkTable[index]);
    }
    free(inLinkTable);

    free_count = 0;
    for (free_count = 0; free_count < list->nelems; free_count++) {
        free(pagerank[free_count]);
    }
    free(pagerank);

    disposeGraph(g);
    disposeSet(list);
    freeIndexList(urlIndex);
    return 0;
}

double calculatePageRank(Set list, Graph g, double d, double diffPR, int maxIterations, double **pagerank, inLink *inLinkTable, outLink *outLinkTable, indexList urlIndex) {

    int numNode = list->nelems;
    Link to = NULL;
    // Fill in the table of inLink values for each URL in the collection
    for (to = list->elems; to != NULL; to = to->next) {
        // Initialise the pagerank value for each URL
        int index = NameToNum(urlIndex, to->val);
        pagerank[index][0] = (double) 1 / numNode;
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
        int index = NameToNum(urlIndex, src->val);
        outLink collection = newOutlinkRep();
        outLinkTable[index] = collection;
        Link dest = NULL;
        for (dest = list->elems; dest != NULL; dest = dest->next) {
            if (isConnected(g, src->val, dest->val)) {
                (outLinkTable[index])->noutlinks += 1;
                insertInto((outLinkTable[index])->outlinkURL, dest->val);
            }
        }
        if ((outLinkTable[index])->noutlinks == 0) {
            (outLinkTable[index])->noutlinks = 0.5;
        }
    }

    int iteration = 0;
    double diff = diffPR;
    while (iteration < maxIterations && diff >= diffPR) {
        Link curr = NULL;
        for (curr = list->elems; curr != NULL; curr = curr->next) {
            double sum = 0;
            int index = NameToNum(urlIndex, curr->val);
            sum = sumPageRank(inLinkTable, outLinkTable, index, iteration, pagerank, urlIndex);
            double damping = (double) (1 - d) / (double) numNode;
            pagerank[index][iteration + 1] = damping + d * sum;
        }
        diff = differenceAllURL(pagerank, iteration, list->nelems);
        iteration++;
    }
    return iteration;
}

double sumPageRank(inLink *inLinkTable, outLink *outLinkTable, int index, int iteration, double **pagerank, indexList urlIndex) {
    Link srcURL = NULL;
    double sum = 0;
    for (srcURL = (inLinkTable[index])->inlinkURL->elems; srcURL != NULL; srcURL = srcURL->next) {
        int src = NameToNum(urlIndex, srcURL->val);
        double weightIn = (double) (inLinkTable[index])->ninlinks / inLinkSumOfReferencePages(inLinkTable, outLinkTable, src, urlIndex);
        double weightOut = (double) (outLinkTable[index])->noutlinks / outLinkSumOfReferencePages(inLinkTable, outLinkTable, src, urlIndex);
        sum += (double) pagerank[src][iteration] * weightIn * weightOut;
    }
    return sum;
}

int inLinkSumOfReferencePages(inLink *inLinkTable, outLink *outLinkTable, int src, indexList urlIndex) {
    int sum = 0;
    Link destUrl = NULL;
    for (destUrl = (outLinkTable[src])->outlinkURL->elems; destUrl != NULL; destUrl = destUrl->next) {
        int dest = NameToNum(urlIndex, destUrl->val);
        sum += (inLinkTable[dest])->ninlinks;
    }
    return sum;
}

double outLinkSumOfReferencePages(inLink *inLinkTable, outLink *outLinkTable, int src, indexList urlIndex) {
    double sum = 0;
    Link destUrl = NULL;
    for (destUrl = (outLinkTable[src])->outlinkURL->elems; destUrl != NULL; destUrl = destUrl->next) {
        int dest = NameToNum(urlIndex, destUrl->val);
        sum += (outLinkTable[dest])->noutlinks;
    }
    return sum;
}

double differenceAllURL(double **pagerank, int iteration, int numNode) {
    double diff = 0;
    int count = 0;
    for (count = 0; count < numNode; count++) {
        double diffOneURL = fabs(pagerank[count][iteration + 1] - pagerank[count][iteration]);
        diff += diffOneURL;
    }
    return diff;
}
