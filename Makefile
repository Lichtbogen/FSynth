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
OFLAGS = -O2

## Object file list
OBJ = cshell.o errors.o hull.o list.o main.o samples.o sequencer.o wavefmt.o logging.o \
	

all: $(NAME)

debug: CFLAGS += $(DFLAGS)
debug: $(NAME)

release: CFLAGS += $(OFLAGS) -DNDEBUG
release: LFLAGS += -s
release: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LFLAGS) $(CFLAGS) $(OBJ) -o $(NAME)

cshell.o: cshell.c
	$(CC) $(CFLAGS) -c cshell.c

errors.o: errors.c
	$(CC) $(CFLAGS) -c errors.c

hull.o: hull.c
	$(CC) $(CFLAGS) -c hull.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

samples.o: samples.c
	$(CC) $(CFLAGS) -c samples.c

sequencer.o: sequencer.c
	$(CC) $(CFLAGS) -c sequencer.c

wavefmt.o: wavefmt.c
	$(CC) $(CFLAGS) -c wavefmt.c

logging.o: logging.c
	$(CC) $(CFLAGS) -c logging.c

clean:
	rm $(OBJ)

.PHONY: clean
