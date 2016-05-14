CC = g++

CFLAGS = -Wall -Werror -ansi -pedantic

all: rshell.o

rshell.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o rshell
