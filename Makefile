CC = g++

CFLAGS = -Wall -Werror -ansi -pedantic

all: rshell.o

rshell.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o rshell
