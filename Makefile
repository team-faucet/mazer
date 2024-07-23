CFLAGS = -pedantic -Wall

.PHONY: all debug sanitize clean test
all: mazer

test: CFLAGS += -g
test: test_linked_list
	./test_linked_list

debug: CFLAGS += -g
debug: all

sanitize: CFLAGS += -fsanitize=address
sanitize: LDFLAGS += -lasan
sanitize: all

mazer: main.o maze.o
	$(CC) $^ -o $@

main.o: main.c maze.h
	$(CC) -c $(CFLAGS) $<

maze.o: maze.c maze.h
	$(CC) -c $(CFLAGS) $<

test_linked_list: test_linked_list.o linked_list.o
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f *.o
	$(RM) -f mazer
	$(RM) -f test_linked_list