// Applies scaled footrule rank aggregation method to rank urls in given rank files
// and prints the smallest scaled-footrule value with the corresponding order of
// urls to achieve this
// Written by Michael Darmanian 18/10/18

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAXURL    1000
#define TRUE      1
#define FALSE     0
#define RESERVED -1

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
double weightUrl(int urlPos, int listSize, int assignPos, int maxPos);
void sumWeightsForUrls(RankList *lists, int numLists, int size, double sums[size][size]);
void scaledFootrule(RankList *lists, int numLists, int size, double sums[size][size], int *output);
double totalSum(RankList *lists, int *positions, int numLists);
void findBestCombination(RankList *lists, int numLists, int *taken, int size, double *lowestSum, int *comb, int index, int skip, int *bestComb);
void printList(RankList list, int *positions);
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
        exit(1);
    }
    
    int i, numLists = argc; // number of given files + 1 for union list
    RankList lists[numLists]; 
    for (i = 0; i < numLists; i++)
    {
        if (i == 0) lists[i] = newRankList("union"); // element 0 is union list
        else lists[i] = newRankList(argv[i]);
    }
    fillLists(argv, numLists, lists);
    
    int numPositions = lists[0]->size;
    double urlSums[numPositions][numPositions];
    sumWeightsForUrls(lists, numLists, numPositions, urlSums);
    
    // Each element of positions is the P value given to the corresponding
    // alphabetically ordered url in the union list
    int positions[numPositions];
    scaledFootrule(lists, numLists, numPositions, urlSums, positions);

    printList(lists[0], positions);
    
    for (i = 0; i < numLists; i++) freeRankList(lists[i]);
    
    return 0;
}

void fillLists(char **files, int numLists, RankList *lists)
{
    for (int i = 1; i < numLists; i++)
    {
        FILE *fp;
        if ((fp = fopen(files[i], "r")) == NULL)
        {
            printf("ERROR: No such file %s\n", files[i]);
            abort();
        }
        
        char url[MAXURL];
        int pos = 1;
        while (fscanf(fp, "%s", url) == 1)
        {
            insertRankList(lists[i], url, pos++);
            insertRankList(lists[0], url, 0); // pos for union irrelevant
        }        
        fclose(fp);
    }
}

// Applies algorithm 
double weightUrl(int urlPos, int listSize, int assignPos, int maxPos)
{
    return fabs(((double)(urlPos) / (double)(listSize)) - 
                ((double)(assignPos) / (double)(maxPos)));
}

// Puts the total weighting of each url across every given list and saves values
// into given 2D double array
void sumWeightsForUrls(RankList *lists, int numLists, int size, double sums[size][size])
{
    UrlNode search = lists[0]->start;
    for (int i = 0; i < size; i++)
    {
        char *searchUrl = search->url;
        for (int j = 0; j < size; j++)
        {
            double sum = 0;
            int assign = j + 1;
            for (int k = 1; k < numLists; k++)
            {
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
                    sum += weightUrl(curr->pos, lists[k]->size, assign, lists[0]->size);
                }
            }
            sums[i][j] = sum;
        }
        search = search->next;
    }
}

// Uses Hungarian algorithm on given 2D array of sums i.e. subtracting smallest
// value in each "row" from each value in that row then does the same thing but
// for each column. Finds positional values for each url that will result in the
// smallest sum of all applications of the algorithm (corresponding to zeros)
// and outputs those positions to the given integer array.
void scaledFootrule(RankList *lists, int numLists, int size, double sums[size][size], int *output)
{
    // taken: each element refers to the given position P value i.e. element 0
    // is value 1 where last element is value size (max position value)
    // combination: array keeping every possible test combination
    // size: max number of unique elements in union list i.e. lists[0]->size
    int i, j, k, taken[size], combination[size];  
    
    // Subtract smallest value of each row from all values in each row and initialise arrays
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
            if (sums[i][j] < 1e-7) sums[i][j] = 0; // effectively zero
        }
        taken[i] = FALSE;
        combination[i] = 0;
        output[i] = 0;
    } 
    
    // Subtract smallest value of each column from all values in each column
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
            if (sums[j][i] < 1e-7) sums[j][i] = 0; // effectively zero
        }
    } 
    
    // Assign positions in output based on position of zero in corresponding column
    // Unique values that are RESERVED are saved as negative in the output array
    // which are used for finding possible combinations but will never change
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            if (sums[i][j] == 0)
            {       
                if (taken[j] == FALSE)
                {         
                    taken[j] = RESERVED;
                    output[i] = -1 * (j + 1);
                    combination[i] = j + 1;
                }
                else if (taken[j] == RESERVED)
                {
                    taken[j] = TRUE;
                    for (k = 0; k < size; k++)
                    {
                        if (output[k] == -1 * (j + 1))
                        {
                            output[k] = abs(output[k]);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }  
    
    // Reset taken array for finding combinations
    for (i = 0; i < size; i++)
    {
        if (taken[i] != RESERVED) taken[i] = FALSE;
    }
    
    double lowestSum = INFINITY;
    findBestCombination(lists, numLists, taken, size, &lowestSum, combination, 0, FALSE, output);
    printf("%.6f\n", lowestSum);
}

double totalSum(RankList *lists, int *positions, int numLists)
{
    double total = 0;
    UrlNode search = lists[0]->start;
    for (int i = 0; i < lists[0]->size; i++)
    {
        char *searchUrl = search->url;
        for (int j = 1; j < numLists; j++)
        {
            UrlNode curr;
            for (curr = lists[j]->start; curr != NULL; curr = curr->next)
            {
                char *listUrl = curr->url;
                // break loop early since in alphabetical order
                if (strcmp(searchUrl, listUrl) < 0) break;
                else if (strcmp(searchUrl, listUrl) == 0)
                {
                    total += weightUrl(curr->pos, lists[j]->size, 
                                       abs(positions[i]), lists[0]->size);
                    break;
                }
            }
        }
        search = search->next;
    }
    return total;   
}

void findBestCombination(RankList *lists, int numLists, int *taken, int size, double *lowestSum, int *comb, int index, int skip, int *bestComb)
{
    // taken: same array from scaledFootrule function
    // size: same value from scaledFootrule function
    // comb: current test combination
    // index: position of next combination array element to find a value for
    // skip: boolean to prevent unnecessary checking of totalSum
    // bestComb: same array as output from scaledFootrule function
    
    if (index == size && !skip)
    {       
        double sum = totalSum(lists, comb, numLists);
        if (sum < *lowestSum)
        {
            *lowestSum = sum;
            for (int i = 0; i < size; i++) 
            {
                if (bestComb[i] >= 0) bestComb[i] = comb[i];
            }
        }
        return;
    }
    
    if (bestComb[index] >= 0) // prevent set values from being altered
    {
        for (int i = 0; i < size; i++)
        {
            if (taken[i] == FALSE)
            {
                taken[i] = TRUE; // so further iterations do not use same value
                comb[index] = i + 1;
                findBestCombination(lists, numLists, taken, size, lowestSum,
                                    comb, index + 1, FALSE, bestComb);
                taken[i] = FALSE;
            }
        }
    }
    else if (index + 1 == size) 
    findBestCombination(lists, numLists, taken, size, lowestSum, 
                        comb, index + 1, FALSE, bestComb);
    else 
    findBestCombination(lists, numLists, taken, size, lowestSum, 
                        comb, index + 1, TRUE, bestComb);
}

void printList(RankList list, int *positions)
{
    int i, j, count;
    for (i = 1; i <= list->size; i++)
    {
        count = 0;
        for (j = 0; j < list->size; j++)
        {
            if (abs(positions[j]) == i) break;
        }
        for (UrlNode curr = list->start; curr != NULL; curr = curr->next)
        {
            if (count == j)
            {
                printf("%s\n", curr->url);
                break;
            }
            count++;
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
    free(list->name);
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
