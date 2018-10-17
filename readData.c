// Read data from collection.txt and build a graph representation using adjacency matrix
// Written by Helena Ling on 07/10/18
// Acknowledgement: Graph, stack, queue, set ADTs written by John Shepherd, BSTree ADT from week10 lab

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "set.h"
#include "graph.h"
#include "readData.h"

#define EXTENSIONLENGTH 5

// Change the name of the url into an integer to act as an index
int NameToNum(char *urlName) {
    int num = 0;
    char *name = urlName + 3;
    num = atoi(name);
    return num;
}

Set urlList() {
    FILE *fp = fopen("collection.txt", "r");
    char *currURL = "";
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
        char *url = curr->val;
        char *filename = calloc(strlen(url) + EXTENSIONLENGTH + 1, sizeof(char));
        strcpy(filename, url);
        strcat(filename, ".txt");
        FILE *fp = fopen(filename, "r");
        char *dest = "";
        while(fscanf(fp, "%s", dest) == 1) {
            if (strcmp(dest, "#end") == 0) {
                break;
            }
            char *search = strstr(dest, "url");
            if (search != NULL) {
                addEdge(g, url, dest);
            }
        }
        fclose(fp);
        curr = curr->next;
    }
    return g;
}


