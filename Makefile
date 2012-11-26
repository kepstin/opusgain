CFLAGS=-Wall -ggdb
CC=gcc

default: opustag

opustag: opustag.o ogg.o oggopus.o
opustag.o: ogg.h oggopus.h
ogg.o: ogg.h bits.h
oggopus.o: ogg.h oggopus.h


clean:
	rm *.o
	rm opustag
