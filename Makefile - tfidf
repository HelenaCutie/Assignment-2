CC=gcc
CFLAGS=-Wall -lm -std=c11

OBJS=graph.o set.o readData.o

searchTfIdf : searchTfIdf.o $(OBJS)
	gcc -g -o searchTfIdf searchTfIdf.o $(OBJS) -lcurl

readData.o: readData.c readData.h graph.o set.o
graph.o : graph.c graph.h
set.o : set.c set.h

clean :
	rm -f searchTfIdf $(OBJS) core *.dSYM