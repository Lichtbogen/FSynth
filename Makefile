#
# Automatic generated Makefile by pymake
#
# Preset: Default GCC
#

## General variables
NAME   = fsynth

## Compiler and flags
CC     = gcc
CFLAGS = -Wall
LFLAGS = -lm
DFLAGS = -DDEBUG -g
OFLAGS = -O3 -s

## Object file list
OBJ = main.o wavefmt.o samples.o errors.o hull.o sequencer.o 

all: $(NAME)

debug: CFLAGS += $(DFLAGS)
debug: $(NAME)

release: CFLAGS += $(OFLAGS)
release: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJ) -o fsynth

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

wavefmt.o: wavefmt.c
	$(CC) $(CFLAGS) -c wavefmt.c

samples.o: samples.c
	$(CC) $(CFLAGS) -c samples.c

errors.o: errors.c
	$(CC) $(CFLAGS) -c errors.c

hull.o: hull.c
	$(CC) $(CFLAGS) -c hull.c

sequencer.o: sequencer.c
	$(CC) $(CFLAGS) -c sequencer.c

clean:
	rm $(OBJ)

.PHONY: clean
