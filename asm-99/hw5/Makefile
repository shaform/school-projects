CC=g++
CFLAGS=-g -c -Wall

all: asm

asm: sicxeasm.o optab.o
	$(CC) sicxeasm.o optab.o -o asm

sicxeasm.o: sicxeasm.cpp sicxeasm.h textrecord.h
	$(CC) $(CFLAGS) sicxeasm.cpp

optab.o: optab.cpp sicxeasm.h
	$(CC) $(CFLAGS) optab.cpp

clean:
	rm -rf *.o asm
