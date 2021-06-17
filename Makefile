CC=g++
CFLAGS=-std=c++17 -O0 -g -Wall -Wextra

main: main.cpp
	$(CC) $(CFLAGS) $^ -o $@

test: main
	valgrind --leak-check=full ./main
