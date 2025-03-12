CC = gcc
CFLAGS = -Wall -g

# Executable
proj1: utils.o main.o
				$(CC) $(CFLAGS) utils.o main.o -o proj1

# File compilation
main.o: main.c utils.h
				$(CC) $(CFLAGS) -c main.c

utils.o: utils.c utils.h
				$(CC) $(CFLAGS) -c utils.c

# Run program
run: proj1
	./proj1

# Valgrind to check memory
val: proj1
	valgrind ./proj1

# Clean files
clean:
	rm -f proj1 utils.o main.o
