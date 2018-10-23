// Reads collection.txt and outputs the words contained in which urls for every word across all given urls
// Written by Michael Darmanian 8/10/18
// Acknowledgement: set and graph ADTs written by John Shepherd

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "set.h"
#include "graph.h"
#include "urlBST.h"
#include "readData.h"

#define TRUE            1
#define FALSE           0
#define MAXLINE         1000

static Set getWordsNormalise(Set urls);
static int wordUrlCmp(char *word, char *url);
static void normaliseWord(char *word, char *buffer);

int main(void)
{
    BST t = newBST();
    Set urls = urlList();
    Set words = getWordsNormalise(urls);
    for (Link currW = words->elems; currW != NULL; currW = currW->next)
    {
        for (Link currU = urls->elems; currU != NULL; currU = currU->next)
        {
            if (wordUrlCmp(currW->val, currU->val)) 
                t = BSTInsert(t, currW->val, currU->val);                
        }
    }    
    FILE *output = fopen("invertedIndex.txt", "w");
    fprintBST(t, output);
    
    fclose(output);
    disposeSet(urls);
    disposeSet(words);
    freeBST(t);
}

// Puts all unique words from the contents of each url in the set of urls into a single large set and returns it
// Words are all set to lowercase and do not include 
static Set getWordsNormalise(Set urls)
{
    Set words = newSet();
    for (Link curr = urls->elems; curr != NULL; curr = curr->next)
    {
        FILE *fp = openUrl(curr->val);
        if (fp == NULL) abort();
        char word[MAXLINE];
        int reached = FALSE;
        while (fscanf(fp, "%s", word))
        {
            if (reached)
            {
                if (strcmp(word, "#end") == 0) break; // end of Section-2
                char normWord[strlen(word) + 1];
                normaliseWord(word, normWord);
                insertInto(words, normWord);
            }
            // start of Section-2
            else if (strcmp(word, "Section-2") == 0) reached = TRUE;
        }
        fclose(fp);
    } 
    return words;  
}

// Returns TRUE if word exists in the url and 0 otherwise
static int wordUrlCmp(char *word, char *url)
{
    FILE *fp = openUrl(url);
    char findWord[MAXLINE];
    int found = FALSE;
    while (fscanf(fp, "%s", findWord) == 1)
    {
        char normWord[strlen(findWord) + 1];
        normaliseWord(findWord, normWord);
        if (strcmp(normWord, word) == 0)
        {
            found = TRUE;
            break;
        }
    }    
    fclose(fp);
    return found;
}

static void normaliseWord(char *word, char *buffer)
{
    char normWord[strlen(word) + 1];
    int i, last = strlen(word) - 1;
    for (i = 0; i < strlen(word); i++) word[i] = tolower(word[i]);
    strcpy(normWord, word);
    if (word[last] == '.' || word[last] == ',' || word[last] == ';' || 
        word[last] == '?') normWord[last] = '\0';
    strcpy(buffer, normWord);
}
