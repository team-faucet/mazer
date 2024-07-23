CFLAGS = -pedantic -Wall

.PHONY: all

all: mazer

mazer: main.o maze.o
	gcc $^ -o $@

main.o: main.c maze.h
	gcc -c $(CFLAGS) $<

maze.o: maze.c maze.h
	gcc -c $(CFLAGS) $<

