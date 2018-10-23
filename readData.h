// Interface to readData.c functions
// Written by Helena Ling on 07/10/18

#ifndef READDATA_H
#define READDATA_H

typedef struct IndexList *indexList;
typedef struct IndexNode *indexNode;

typedef struct IndexNode{
	char *url;
    int index;
    indexNode next;
} IndexNode;

typedef struct IndexList{
	indexNode head;
	indexNode last;
} IndexList;

// Create a linked list to store the name of urls and corresponding index
indexList newIndexList();
// Insert an index node to the given index list
void insertIndex(indexList list, char *url, int index);
// Set up an index list using urls in collection.txt
indexList setUpIndex();
// Free up the given index list
void freeIndexList(indexList list);
// Creates set of urls from collection.txt
Set urlList();
// Convert name of an url to the unique index 
int NameToNum(indexList list, char *urlName);
// Creates graph of urls from collection.txt
Graph urlGraph(Set list);
// Returns pointer to opened url file
FILE *openUrl(char *url);

#endif
