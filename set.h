// set.h ... interface to simple Set of Strings
// Written by John Shepherd, September 2015

#ifndef SET_H
#define SET_H

typedef struct SetRep *Set;
typedef struct Node *Link;

typedef struct Node {
	char *val;
	Link  next;
} Node;
	
typedef struct SetRep {
	int   nelems;
	int   max;
	Link  elems;
} SetRep;

// Function signatures

#define strEQ(s,t) (strcmp((s),(t)) == 0)
#define strLT(s,t) (strcmp((s),(t)) < 0)

Set newSet();
void disposeSet(Set);
void insertInto(Set,char *);
void dropFrom(Set,char *);
int  isElem(Set,char *);
int  nElems(Set);
void showSet(Set);

#endif
