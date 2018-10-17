CC=gcc
CFLAGS=-Wall -Werror -g

OBJS=graph.o set.o readData.o

pagerank : pagerank.o $(OBJS)
	$(CC) -o pagerank $(OBJS)

readData.o: readData.c readData.h graph.o set.o
graph.o : graph.c graph.h
set.o : set.c set.h

clean :
	rm -f pagerank $(OBJS) core *.dSYM