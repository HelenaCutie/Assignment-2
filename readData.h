// Interface to readData.c functions
// Written by Helena Ling on 07/10/18

#ifndef READDATA_H
#define READDATA_H

// Creates set of urls from collection.txt
Set urlList();
// Convert name of an url to the number included in the name
int NameToNum(char *urlName);
// Creates graph of urls from collection.txt
Graph urlGraph(Set list);
// Returns pointer to opened url file
FILE *openUrl(char *url);

#endif
