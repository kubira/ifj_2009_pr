CFLAGS=-std=c99 -Wall -pedantic -g
BIN=project
CC=gcc
RM=rm -f

ALL: str.o scaner.o ilist.o main.o bvs.o stack.o sa_parser.o interpret.o parser.o
	$(CC) $(CFLAGS) -o $(BIN) str.o scaner.o ilist.o bvs.o stack.o sa_parser.o parser.o interpret.o main.o

clean:
	$(RM) *.o $(BIN)
