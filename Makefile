CFLAGS = -pedantic -Wall -Wextra

SOURCES = $(shell find -name '*.c')
OBJS    = $(SOURCES:%.c=%.o)

.PHONY: all debug clean
all: mazer

debug: CFLAGS += -g
debug: all

mazer: $(OBJS)
	$(CC) $^ -o $@

main.o: main.c maze.h
	$(CC) -c $(CFLAGS) $<

maze.o: maze.c maze.h
	$(CC) -c $(CFLAGS) $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f *.o
	$(RM) -f mazer