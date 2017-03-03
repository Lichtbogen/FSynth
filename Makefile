#
# Automatic generated Makefile by pymake
#
# Preset: Default GCC
#

## General variables
NAME   = fsynth

## Compiler and flags
CC     = g++
CFLAGS = -Wall
DFLAGS = -DDEBUG -g
OFLAGS = -O3 -s

## Object file list
OBJ = main.o samples.o 

all: $(NAME)

debug: CFLAGS += $(DFLAGS)
debug: $(NAME)

release: CFLAGS += $(OFLAGS)
release: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o fsynth

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

samples.o: samples.c
	$(CC) $(CFLAGS) -c samples.c

clean:
	rm $(OBJ)

.PHONY: clean
