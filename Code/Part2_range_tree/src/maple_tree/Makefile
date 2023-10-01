EXECUTABLE_NAME = main
CC = gcc
#CFLAGS = -std=c11
#CFLAGS += -O3
CFLAGS += -O0
CFLAGS += -g
#CFLAGS += -DCONFIG_DEBUG_MAPLE_TREE
INC = -I.

all: main

maple_tree:
	$(CC) $(CFLAGS) -c maple_tree.c -o maple_tree.o

main: maple_tree
	$(CC) $(CFLAGS) tests.c maple_tree.o -o $(EXECUTABLE_NAME)
