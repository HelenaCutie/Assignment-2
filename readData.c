
// Read data from collection.txt and build a graph representation using adjacency matrix
// Written by Helena Ling on 07/10/18
// Acknowledgement: Graph, stack, queue, set ADTs written by John Shepherd, BSTree ADT from week10 lab

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include "set.h"
#include "graph.h"
#include "readData.h"
#include "urlBST.h"

#define MAXURL 1000
#define TRUE   1
#define FALSE  0

Set urlList() {
    FILE *fp = fopen("collection.txt", "r");
    char currURL[MAXURL];
    Set list = newSet();
    while(fscanf(fp, "%s", currURL) == 1) {
        insertInto(list, currURL);
    }
    fclose(fp);
    return list;
}

Graph urlGraph(Set list) {
    Graph g = newGraph(list->nelems);
    Link curr = list->elems;
    while (curr != NULL) {
        FILE *fp = openUrl(curr->val);
        char dest[MAXURL];
        while(fscanf(fp, "%s", dest) == 1) {
            if (strcmp(dest, "#end") == 0) {
                break;
            }
            char *search = strstr(dest, "url");
            if (strcmp(curr->val, dest) != 0 && search != NULL) {
                addEdge(g, curr->val, dest);
            }
        }
        fclose(fp);
        curr = curr->next;
    }
    return g;
}

FILE *openUrl(char *url)
{
    FILE *fp;
    char filename[MAXURL];
    strcpy(filename, url);
    if (strstr(url, ".txt") == NULL) strcat(filename, ".txt");
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("ERROR: No such file %s in current directory\n", filename);
        abort();
    }
    return fp;
}

indexList newIndexList() {
    indexList new = calloc(1, sizeof(IndexList));
    new->head = NULL;
    new->last = NULL;
    return new;
}

void insertIndex(indexList list, char *url, int index) {
    indexNode new = calloc(1, sizeof(IndexNode));
    new->url = strdup(url);
    new->index = index;
    new->next = NULL;
    if (list->head == NULL) {
        list->head = new;
    } else {
        list->last->next = new;
    }
    list->last = new;
}

indexList setUpIndex() {
    indexList list = newIndexList();
    FILE *fp = fopen("collection.txt", "r");
    char currURL[MAXURL];
    int index = 0;
    while(fscanf(fp, "%s", currURL) == 1) {
        insertIndex(list, currURL, index);
        index++;
    }
    fclose(fp);
    return list;
}

// Change the name of the url into an integer to act as an index
int NameToNum(indexList list, char *urlName) {
    indexNode curr = list->head;
    for (curr = list->head; curr != NULL; curr = curr->next) {
        if (strcmp(curr->url, urlName) == 0) {
            return curr->index;
        }
    }
    return 0;
}
