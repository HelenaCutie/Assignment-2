// Searches for urls containing given words in command line arguments proritising
// higher pageranks first and outputting to stdout in descending order. 
// Uses pagerankList.txt and invertedIndex.txt files in the current directory.
// Written by Michael Darmanian 13/10/18
// Acknowledgement: set ADT written by John Shepherd

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "set.h"

#define MAXLINE 1000
#define TRUE   1
#define FALSE  0

void printSetReverse(Link list);
void getNextUrl(FILE *pRankList, char *buffer);
int foundWord(char *word, char **searchWords, int numWords);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("USAGE: ./searchPagerank WORD1 [[WORD2] [WORD3] ... ]\n");
        exit(1);
    }
    
    Set foundUrls = newSet(), urls = newSet();
    char currUrl[MAXLINE], word[MAXLINE];
    FILE *invIndex, *pRankList;
    if ((pRankList = fopen("pagerankList.txt", "r")) == NULL)
    {
        printf("ERROR: pagerankList.txt not in current directory\n");
        exit(1);
    }
    
    getNextUrl(pRankList, currUrl);
    while (strcmp(currUrl, "#end#") != 0)
    {
        insertInto(urls, currUrl);
        getNextUrl(pRankList, currUrl);
    }
    
    rewind(pRankList); 
    getNextUrl(pRankList, currUrl);
    while (strcmp(currUrl, "#end#") != 0)
    {
        if ((invIndex = fopen("invertedIndex.txt", "r")) == NULL)
        {
            printf("ERROR: invertedIndex.txt not in current directory\n");
            exit(1);
        }
        
        int found = FALSE, count = 0; // count of foundWord calls
        while (fscanf(invIndex, "%s", word) == 1 && count < argc)
        {
            if (nElems(foundUrls) == 30) break;
            
            if (found)
            {
                // reaches a word after urls for previous word
                if (!isElem(urls, word) && !foundWord(word, argv, argc)) found = FALSE;
                else if (strcmp(word, currUrl) == 0) insertInto(foundUrls, currUrl);
            }
            // reaches a word to be searched for
            else if (foundWord(word, argv, argc))
            {
                found = TRUE; 
                count++;
            }
        }
        
        if (nElems(foundUrls) == 30) break;
        fclose(invIndex);
        getNextUrl(pRankList, currUrl);
    }
    
    printSetReverse(foundUrls->elems);
    fclose(pRankList);
    disposeSet(urls);
    disposeSet(foundUrls);
    
    return 0;
}

// Prints sorted set in reverse
void printSetReverse(Link list)
{
    if (list == NULL) return;
    printSetReverse(list->next);
    printf("%s\n", list->val);
}

// Gets the next url from the pagerankList.txt file and stores it in the buffer
// Stores "#end#" in the buffer when there are no more urls to be read
void getNextUrl(FILE *pRankList, char *buffer)
{
    char line[MAXLINE];
    if (fgets(line, MAXLINE, pRankList) == NULL)
    {
        strcpy(buffer, "#end#");
        return;
    }
    char *word = strtok(line, ",");
    strcpy(buffer, word);
}

// Compares word to every string in searchWords and returns TRUE if there is a
// match otherwise returns FALSE 
int foundWord(char *word, char **searchWords, int numWords)
{
    for (int i = 1; i < numWords; i++)
    {
        if (strcmp(word, searchWords[i]) == 0) return TRUE;
    }
    return FALSE;
}
