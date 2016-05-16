CC = g++

CFLAGS = -Wall -Werror -ansi -pedantic

all: rshell.o

rshell.o: src/main.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.cpp -o bin/rshell
