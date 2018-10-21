// Read data from collection.txt and build a graph representation using adjacency matrix
// Written by Helena Ling on 07/10/18
// Acknowledgement: Graph, stack, queue, set ADTs written by John Shepherd, BSTree ADT from week10 lab

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "stack.h"
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
            if (search != NULL) {
                addEdge(g, curr->val, dest);
            }
        }
        fclose(fp);
        curr = curr->next;
    }
    return g;
}

FILE *openUrl(char *url) {
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
