// Interface to readData.c functions
// Written by Helena Ling on 07/10/18

#ifndef READDATA_H
#define READDATA_H

typedef unsigned char Num;

typedef struct GraphRep *Graph;

typedef struct SetRep *Set;

// Creates set of urls from collection.txt
Set urlList();
// Creates graph of urls from collection.txt
Graph urlGraph(Set list);
// Returns pointer to opened url file
FILE *openUrl(char *url);

#endif
