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
void findBestCombination(RankList *lists, int numLists, int *taken, int start, int end, double *lowestSum, int *bestComb);
void printList(RankList list, int *positions);
RankList newRankList(char *name);
UrlNode newUrlNode(char *url, int pos);
void freeRankList(RankList list);
void freeAllNodes(UrlNode node);
void insertRankList(RankList list, char *url, int pos);
void printOut(RankList list);

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
    
    int positions[numPositions];
    scaledFootrule(lists, numLists, numPositions, urlSums, positions);
    
    for (i = 0; i < numPositions; i++) printf("%d ", positions[i]);
    printf("\n"); 
    
   // double sum = totalSum(lists, positions, numLists);
    printList(lists[0], positions);
    
    printf("\n");
    
    for (i = 0; i < numLists; i++)
    {
        printOut(lists[i]);
        freeRankList(lists[i]);
    }
    
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
// smallest sum of all applications of the algorithm and outputs those positions
// to the given integer array.
void scaledFootrule(RankList *lists, int numLists, int size, double sums[size][size], int *output)
{
    int i, j, taken[size], zerosRow[size], zerosCol[size];
    
    printf("Sums:\n");
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            printf("%.6f ", sums[i][j]);
        }
        printf("\n");
    }
    printf("\n");    
    
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
        zerosRow[i] = 0;
        zerosCol[i] = 0;
    }
    
    printf("After row reduction:\n");
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            printf("%.6f ", sums[i][j]);
        }
        printf("\n");
    }
    printf("\n"); 
    
    for (i = 0; i < size; i++)
    {
        int zeroCount = 0, zeroPos = 0;
        double minColumn = INFINITY;
        for (j = 0; j < size; j++)
        {
            if (sums[j][i] < minColumn) minColumn = sums[j][i];
        }
        for (j = 0; j < size; j++)
        {
            sums[j][i] -= minColumn;
            if (sums[j][i] < 1e-7) // effectively zero
            { 
                sums[j][i] = 0;
                zerosRow[j]++;
                zerosCol[i]++; 
            }
        }
    } 
    
    printf("After column reduction:\n");
    for (i = 0; i < size; i++)
    {
       // int zeroCount = 0, zeroPos = 0;
        for (j = 0; j < size; j++)
        {
            printf("%.6f ", sums[i][j]);
          /*  if (sums[i][j] == 0)
            {
                zeroCount++;
                zeroPos = j;
            } */
        }
        printf("\n");
      /*  if (zeroCount == 1)
        {
            taken[i] = TRUE;
            output[zeroPos] = i + 1;
        }
        else */ taken[i] = FALSE;
    }
    printf("\n"); 
    
    double lowestSum = INFINITY;
    findBestCombination(lists, numLists, taken, 0, size, &lowestSum, output);
    printf("%.6f\n", lowestSum);
    
  /*  int clash = FALSE, allow = FALSE;
    for (i = 0; i < size; i++)
    {
        //printf("i = %d\n", i);
        int assigned = FALSE;
        if (i < 0)
        {
           // printf("checked\n");
            i = 0;
            allow = TRUE;
            clash = FALSE;
        }
        for (j = 0; j < size; j++)
        {
            if (sums[i][j] == -1 && clash)
            {
                sums[i][j] = 0;
                taken[j] = FALSE;
                clash = FALSE;
            }
            else if (sums[i][j] == 0 && (!taken[j] || allow) && !clash)
            {
                taken[j] = TRUE;
                assigned = TRUE;
                sums[i][j] = -1;
                output[i] = j + 1;
                break;
            }
        }
        if (!assigned)
        {
            i -= 2;
            clash = TRUE;
        }
    }  */
    
  /*  int clash = FALSE, next = FALSE, k;
    for (i = 0; i < size; i++)
    {
        int pass = FALSE, pos, *reserve;
        double *hold;
        for (k = 0; k < size; k++)
        {
            if (sums[i][k] == -1 && !clash)
            {
                pass = TRUE;
                break;
            }
        }
        
        if (!pass)
        {
            for (j = 0; j < size; j++)
            {   
                if (clash && !pass) 
                {
                    if (sums[i][clash] != -1)
                    {
                        i -= 2;
                        next = TRUE;
                        break; 
                    }
                    j = clash + 1;
                    sums[i][clash] = 0;
                    clash = FALSE;
                    next = FALSE;
                }
                if (sums[i][j] == 0)
                {
                    if (taken[j])
                    {
                        hold = &sums[i][j];
                        reserve = &output[i];
                        pos = j + 1;
                        clash = j; 
                        pass = TRUE;
                    }                
                    else 
                    {
                        sums[i][j] = -1;
                        taken[j] = TRUE;
                        output[i] = j + 1;
                        if (clash) clash = FALSE;
                        break;
                    }
                }
            }
            if (clash && !next)
            {
                i -= 2;
                *hold = -1;
                *reserve = pos;
                if (pass) pass = FALSE;
                else clash = FALSE;
            } 
        } 
    } */
    
  /*  printf("Combination:\n");
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            printf("%.6f ", sums[i][j]);
        }
        printf("\n");
    }
    printf("\n"); */
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
                    total += weightUrl(curr->pos, lists[j]->size, positions[i],
                                       lists[0]->size);
                    break;
                }
            }
        }
        search = search->next;
    }
    return total;   
}

void findBestCombination(RankList *lists, int numLists, int *taken, int start, int end, double *lowestSum, int *bestComb)
{
    if (start == end)
    {
        printf("checked\n");
        double sum = totalSum(lists, bestComb, numLists);
        if (sum < *lowestSum) *lowestSum = sum;
        return;
    }
    
    for (int i = start; i < end; i++)
    {
        if (taken[i] == FALSE)
        {
            taken[i] = TRUE;
            bestComb[i] = i + 1;
            findBestCombination(lists, numLists, taken, i + 1, end, lowestSum, bestComb);
            taken[i] = FALSE;
        }
    }
}

void printList(RankList list, int *positions)
{
    int i, j, count;
    for (i = 1; i <= list->size; i++)
    {
        count = 0;
        for (j = 0; j < list->size; j++)
        {
            if (positions[j] == i) break;
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

void printOut(RankList list)
{
    printf("%s, size = %d\n", list->name, list->size);
    for (UrlNode curr = list->start; curr != NULL; curr = curr->next)
    {
        printf("%s, pos = %d\n", curr->url, curr->pos);
    }
} 
