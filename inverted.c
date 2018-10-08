// Reads collection.txt and outputs the words contained in which urls for every word across all given urls
// Written by Michael Darmanian 8/10/18
// Acknowledgement: set ADT written by John Shepherd

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "set.h"
#include "urlBST.h"
#include "inverted.h"

#define EXTENSIONLENGTH 5
#define TRUE            1
#define FALSE           0
#define MAXLINE         1000

static Set getWordsNormalise(Set urls);
static int wordUrlCmp(char *word, char *url);

// Outputs the inverted index to file invertedIndex.txt
void createInvertedIndex(Set urls)
{
    BST t = newBST();
    Set words = getWordsNormalise(urls);
    Link currW, currU;
    for (currW = words->elems; currW != NULL; currW = currW->next)
    {
        for (currU = urls->elems; currU != NULL; currU = currU->next)
        {
            if (wordUrlCmp(currW->val, currU->val)) 
                BSTInsert(BST t, currW->val, currU->val);
        }
    }    
    FILE *output = fopen("invertedIndex.txt", "w");
    fprintBST(t, output);
    
    fclose(output);
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
        char *url = curr->val;
        char filename[strlen(url) + EXTENSIONLENGTH] = "";
        strcpy(filename, url);
        strcat(filename, ".txt");
        FILE *fp = fopen(filename, "r");
        char *word;
        int reached = FALSE;
        while (fscanf(fp, "%s", word))
        {
            if (reached)
            {
                if (strcmp(word, "Section-2") == 0) break; // end of Section-2
                char *normWord;
                int i;
                for (i = 0; i < strlen(word); i++) // normalise
                {
                    word[i] = tolower(word[i]);
                    if (i == 1) strcpy(normWord, word[i]);
                    else if (i == strlen(word) - 2 &&
                             !(word[i] >= 'a' && word[i] <= 'z')) break;
                    else strcat(normWord, word[i]);
                }
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
    char line[MAXLINE], filename[strlen(url) - 1 + EXTENSIONLENGTH] = "";
    strcpy(filename, url);
    if (strstr(url, ".txt") == NULL) strcat(filename, ".txt");
    FILE *fp = fopen(filename, "r");
    int found = FALSE;
    while (fgets(line, MAXLINE, fp))
    {
        if (strstr(line, word))
        {
            found = TRUE;
            break;
        }
    }    
    fclose(fp);
    return found;
}

