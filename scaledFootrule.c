// Applies scaled footrule rank aggregation method to rank urls in given rank files
// and prints the smallest scaled-footrule value with the corresponding order of
// urls to achieve this
// Written by Michael Darmanian 18/10/18
// // Acknowledgement: set ADT written by John Shepherd

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "set.h"

#define TRUE  1
#define FALSE 0

typedef struct List *RankList;

typedef struct Node *UrlNode;

typedef struct Node
{
    char *url;
    int pos;
    UrlNode next;
} Node;

typedef struct List
{
    char *name;
    int size;
    UrlNode start;
} List;

void fillLists(char **files, int numLists, RankList *lists);
void sumWeightsForUrls(RankList *lists, int numLists, double **sums, int size);
void applyHungarianAlgorithm(double **sums, int *output);
RankList newRankList(char *name);
UrlNode newUrlNode(char *url, int pos);
void freeRankList(RankList list);
void freeAllNodes(UrlNode node);
void insertRankList(RankList list, char *url, int pos);

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("USAGE: ./scaledFootrule rankA.txt rankB.txt [rankC.txt] ...\n");
        exit();
    }
    
    int numLists = argc; // number of given files + 1 for union list
    RankList lists[numLists]; // element 0 is union list
    fillLists(argv, numLists, lists);
    
    int numPositions = lists[0]->size;
    double urlSums[numPositions][numPositions];
    sumWeightsForUrls(lists, numLists, urlSums, numPositions);
    
    int positions[numPositions];
    
    
    for (int i = 0; i < numLists; i++)
    {
        freeRankList(lists[i]);
    }
    
    return 0;
}

void fillLists(char **files, int numLists, RankList *lists)
{
    for (int i = 1; i < numLists; i++)
    {
        FILE *fp;
        if ((fp = fopen(*files[i], "r")) == NULL)
        {
            printf("ERROR: No such file %s\n", *files[i]);
            abort();
        }
        
        char *url;
        int pos = 1;
        while (fscanf(fp, "%s", url) == 1)
        {
            insertRankList(lists[i], url, pos++);
            insertRankList(lists[0], url, 0); // pos for union irrelevant
        }        
        fclose(fp);
    }
}

// Puts the total weighting of each url across every given list and saves values
// into given 2D double array
void sumWeightsForUrls(RankList *lists, int numLists, double **sums, int size)
{
    double unionSize = (double)(lists[0]->size);
    UrlNode search = lists[0]->start;
    for (int i = 0; i < size; i++)
    {
        char *searchUrl = search->url;
        for (int j = 0; j < size; j++)
        {
            double sum = 0, assign = (double)(j + 1);
            for (int k = 1; k < numLists; k++)
            {
                double listSize = lists[k]->size;
                UrlNode curr;
                int found = FALSE;
                for (curr = lists[k]->start; curr != NULL; curr = curr->next)
                {
                    char *listUrl = curr->url;
                    // break loop early since in alphabetical order
                    if (strcmp(searchUrl, listUrl) < 0) break;
                    else if (strcmp(searchUrl, listUrl) == 0)
                    {
                        found = TRUE;
                        break;
                    }
                }
                
                if (found)
                {
                    double urlPos = (double)(curr->pos);
                    // applying formula
                    sum += fabs((urlPos / listSize) - (assign / unionSize));
                }
            }
            sums[i][j] = sum;
        }
        search->next;
    }
}

// Uses Hungarian algorithm on given 2D array of sums i.e. subtracting smallest
// value in each "row" from each value in that row then does the same thing but
// for each column. Finds positional values for each url that will result in the
// smallest sum of all applications of the algorithm and outputs those positions
// to the given integer array.
void applyHungarianAlgorithm(double **sums, int *output, int size)
{
    int i, j, k, taken[size] = {FALSE};
    for (i = 0; i < size; i++)
    {
        double minRow = INFINITY;
        for (j = 0; j < size; j++)
        {
            if (sums[i][j] < minRow) minRow = sums[i][j];
        }
        for (j = 0; j < size; j++)
        {
            sums[i][j] -= minRow;
        }
    }
    for (i = 0; i < size; i++)
    {
        double minColumn = INFINITY;
        for (j = 0; j < size; j++)
        {
            if (sums[j][i] < minColumn) minColumn = sums[j][i];
        }
        for (j = 0; j < size; j++)
        {
            sums[j][i] -= minColumn;
            if (sums[j][i] != 0) sums[j][i] = -1;
        }
    } 
    
    int clash = FALSE, postClash = FALSE;
    for (i = 0; i < size; i++)
    {
        int pass = FALSE:
        for (j = 0; j < size; j++)
        {   
            if (clash && !pass) 
            {
                if (sums[i][clash] != 1) break;
                j = clash + 1;
                sums[i][clash] = 0;
                clash = FALSE;
                postClash = TRUE;
            }
            if (sums[i][j] == 0)
            {
                if (taken[j])
                {
                    clash = j; 
                    pass = TRUE;
                }                
                else 
                {
                    sums[i][j] = 1;
                    taken[j] = 1;
                    if (clash) clash = FALSE;
                    else if (postClash)
                    {
                        i++;
                        postClash = FALSE:
                    }
                    break;
                }
            }
        }
        if (clash)
        {
            i -= 2;
            clash = FALSE;
        }  
    }
}

// Essential linked list functions:

RankList newRankList(char *name)
{
    RankList new = malloc(sizeof(RankList));
    new->name = strdup(name);
    new->size = 0;
    new->start = NULL;
    return new;
}

UrlNode newUrlNode(char *url, int pos)
{
    UrlNode new = malloc(sizeof(UrlNode));
    new->url = strdup(url);
    new->pos = pos;
    new->next = NULL;
    return new;
}

void freeRankList(RankList list)
{
    if (list == NULL) return;
    freeAllNodes(list->start);
    free(list);
}

void freeAllNodes(UrlNode node)
{
    if (node == NULL) return;
    freeAllNodes(node->next);
    free(node->url);
    free(node);
}

// Inserts alphabetically without duplicates
void insertRankList(RankList list, char *url, int pos)
{
    if (list == NULL || url == NULL) return;
    UrlNode curr, prev = NULL;
    for (curr = list->start; curr != NULL; curr = curr->next)
    {
        if (strcmp(url, curr->url) == 0) return;
        else if (strcmp(url, curr->url) < 0) break;
        prev = curr;
    }
    
    UrlNode new = newUrlNode(url, pos);
    if (curr == list->start)
    {
        new->next = list->start;
        list->start = new;
    }
    else
    {
        prev->next = new;
        new->next = curr;
    }
    list->size++;
} 
