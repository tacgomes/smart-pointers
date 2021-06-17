CC=g++
CFLAGS=-std=c++17 -O0 -g -Wall -Wextra

main: main.cpp unique_ptr.h shared_ptr.h weak_ptr.h cblock.h
	$(CC) $(CFLAGS) $< -o $@

test: main
	valgrind --leak-check=full ./main

clean:
	rm -f main
