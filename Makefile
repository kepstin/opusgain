CFLAGS=-Wall -ggdb
CC=gcc

default: opustag

opustag: opustag.o ogg.o
opustag.o: ogg.h
ogg.o: ogg.h bits.h

clean:
	rm *.o
	rm opustag
