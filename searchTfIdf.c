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
#include <unistd.h>
#include "set.h"
#include "graph.h"
#include "readData.h"

#define TRUE            1
#define FALSE           0
#define MAXLINE         1000
#define MAXPRINTED      30
#define MAXQUERYTERMS   50
#define MAXNAMELEN      50

typedef struct tfidfValue{
    char *url;
    int nterms;
    double tfidf;
} tfidfValue;

int compare (const void * a, const void * b);
double calculateTf(char *toCalculate, char *url);
char *normaliseWord (char *word);

int compare (const void * a, const void * b) {

    const tfidfValue *tfidfValueA = (const tfidfValue *)a;
    const tfidfValue *tfidfValueB = (const tfidfValue *)b;

    // First sort in descending order of number of matching terms
    if (tfidfValueA->nterms < tfidfValueB->nterms) {
        return 1;
    } else if (tfidfValueA->nterms > tfidfValueB->nterms) {
        return -1;
    } else {    // Number of terms found is equal, then order according to tfidf values
        if (tfidfValueA->tfidf < tfidfValueB->tfidf) {
            return 1;
        } else if (tfidfValueA->tfidf > tfidfValueB->tfidf) {
            return -1;
        } else {
            return 0;
        }
    }
}

int main(int argc, char *argv[]) {
    char **queryTerm = calloc(MAXQUERYTERMS, sizeof(char *));
    
    if (argc < 2) {
        fprintf(stderr, "Please enter query terms.");
        exit(1);
    }
    int count = 1;
    int numWords = 0;
    int sizeup = MAXQUERYTERMS;
    // Read all query terms supplied in command lines and put them into
    // an array of strings
    for (count = 1; count < argc; count++) {
        if (numWords >= MAXQUERYTERMS) {
            sizeup *= 2;
            queryTerm = (char **) realloc(queryTerm, sizeup * sizeof(char *));
        }
        queryTerm[numWords] = strdup(argv[count]);
        numWords++;
    }

    // Set up the collection of url list and index list
    Set list = urlList();
    indexList urlIndex = setUpIndex();
    // A table of tf values for each URL in the collection
    double **tfTable = calloc(list->nelems, sizeof(double *));
    int row_count = 0;
    for (row_count = 0; row_count < (list->nelems); row_count++) {
        tfTable[row_count] = calloc(numWords + 1, sizeof(double));
    }
    // A table of idf values for each URL in the collection
    double **idfTable = calloc(list->nelems, sizeof(double *));
    row_count = 0;
    for (row_count = 0; row_count < list->nelems; row_count++) {
        idfTable[row_count] = calloc(numWords + 1, sizeof(double));
    }

    // An array of structs storing tfidf value and number of matching terms
    // of corresponding url
    tfidfValue *tfidfList = calloc(list->nelems, sizeof(struct tfidfValue));
    int count_url = 0;
    Link url = NULL;
    for (url = list->elems; url != NULL; url = url->next) {
        tfidfList[count_url].url = strdup(url->val);
        tfidfList[count_url].nterms = 0;
        tfidfList[count_url].tfidf = 0;
        count_url++;
    }
    
    // Start calculating tfidf value for each url
    FILE *fp = fopen("invertedIndex.txt", "r");
    int queryCount = 0;
    // Calculate for each query term, the tf and idf values of urls in the given collection
    for (queryCount = 0; queryCount < numWords; queryCount++) {
        char *toCalculate = strdup(queryTerm[queryCount]);
        char line[MAXLINE] = "";
        int found = FALSE;
        // Find the line starting with the query term in invertedIndex.txt
        while (found == FALSE && fgets(line, MAXLINE, fp)) {
            char wordName[MAXNAMELEN] = "";
            int findWord = sscanf(line, "%s", wordName);
            if (findWord == 1 && strcmp(wordName, toCalculate) == 0) {
                found = TRUE;
            }
        }
        if (found == TRUE) {
            int numURLs = 0;
            char urlName[MAXNAMELEN] = "";
            char *data = line;
            int offset = 0;
            // Calculate number of urls that contain current query term
            while (sscanf(data, "%s%n", urlName, &offset) == 1) {
                data += offset;
                if (strcmp(urlName, toCalculate) != 0) {
                    numURLs++;
                }
            }
            // Calculate tf and idf values for each url based on current query term
            char url[MAXNAMELEN] = "";
            int offsetTwo = 0;
            char *dataTwo = line;
            while (sscanf(dataTwo, "%s%n", url, &offsetTwo) == 1) {
                dataTwo += offsetTwo;
                if (strcmp(url, toCalculate) != 0) {
                    int index = NameToNum(urlIndex, url);
                    tfTable[index][queryCount] = calculateTf(toCalculate, url);
                    idfTable[index][queryCount] = log10( (double) list->nelems / (double) numURLs);
                    tfidfList[index].nterms += 1;
                    tfidfList[index].tfidf += tfTable[index][queryCount] * idfTable[index][queryCount];
                }
            }
        }
        free(toCalculate);
        rewind(fp);
    }

    fclose(fp);

    // Sort the array of struct based on the number of matching terms found and
    // tfidf value in descending order
    qsort(tfidfList, list->nelems, sizeof(tfidfValue), &compare);

    // Print out the top 30 urls with at least one matching term
    int count_urlName= 0;
    int printed = 0;
    for (count_urlName = 0; count_urlName < list->nelems && printed < MAXPRINTED; count_urlName++) {
        if (tfidfList[count_urlName].nterms != 0) {
            fprintf(stdout, "%s  %.6f\n", tfidfList[count_urlName].url, tfidfList[count_urlName].tfidf);
            printed++;
        }
    }

    // Free all calloced memory
    int free_count = 0;
    for (free_count = 0; free_count < numWords; free_count++) {
        free(queryTerm[free_count]);
    }
    free(queryTerm);

    free_count = 0;
    for (free_count = 0; free_count < list->nelems; free_count++) {
        free(tfTable[free_count]);
    }
    free(tfTable);

    free_count = 0;
    for (free_count = 0; free_count < list->nelems; free_count++) {
        free(idfTable[free_count]);
    }
    free(idfTable);

    free_count = 0;
    for (free_count = 0; free_count < count_url; free_count++) {
        free(tfidfList[free_count].url);
    }
    free(tfidfList);

    disposeSet(list);
    freeIndexList(urlIndex);

    return 0;
}

// Calculate the tf value for a given url
double calculateTf(char *toCalculate, char *url) {

    FILE *fp = openUrl(url);
    char search[MAXNAMELEN] = "";
    int reached = FALSE;
    int numWords = 0;
    int match = 0;
    // Calculate the number of words inside the corresponding url file
    // and number of occurence of the given search term
    // final tf value given by (frequencySearchTerm / numWords)
    while (fscanf(fp, "%s", search) == 1) {
        if (reached) {
            if (strcmp(search, "#end") == 0) {
                break;
            }
            numWords++;
            // Normalise the content inside the url file to compare
            // with the given search term
            char *normSearch = normaliseWord(search);
            if (strcmp(normSearch, toCalculate) == 0) {
                match++;
            }
            free(normSearch);
        }
        if (strcmp(search, "Section-2") == 0) {
            reached = TRUE;
        }
    }
    double tf = (double) match / (double) numWords;
    fclose(fp);
    return tf;
}

// Normalise a given word (turn all letters into lower case and remove the last
// special character if there's any)
char *normaliseWord (char *word) {
    int i;
    char *normWord = calloc(strlen(word) + 1, sizeof(char));
    for (i = 0; i < strlen(word); i++)
    {
        word[i] = tolower(word[i]);
        if (i == 0) {
            normWord[i] = word[i];
        } else if (i == strlen(word) - 1 && !(word[i] >= 'a' && word[i] <= 'z')) {
            break;
        } else {
            normWord[i] = word[i];
        }
    }
    char *normalWord = strdup(normWord);
    free(normWord);
    return normalWord;
}
