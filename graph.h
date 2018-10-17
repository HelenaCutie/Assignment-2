// graph.h ... Interface to Graph of strings
// Written by John Shepherd, September 2015

#ifndef GRAPH_H
#define GRAPH_H

// #define strEQ(g,t) (strcmp((g),(t)) == 0)

typedef unsigned char Num;

typedef struct GraphRep *Graph;

typedef struct GraphRep {
	int   nV;
	int   maxV;
	char  **vertex;
	Num   **edges;
} GraphRep;

// Function signatures

Graph newGraph(int);
void  disposeGraph(Graph);
int   addEdge(Graph,char *,char *);
int   nVertices(Graph);
int   isConnected(Graph, char *, char *);
void  showGraph(Graph,int);

#endif
