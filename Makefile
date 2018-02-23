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
DFLAGS = -DDEBUG
OFLAGS = -DNDEBUG -O2
LIBS   = -lm -lreadline

## Object file list
OBJ = cshell.o errors.o hull.o list.o logging.o main.o prompt.o samples.o sequencer.o \
	wavefmt.o

release: CF = $(CFLAGS) $(OFLAGS)
release: LF = -s
release: $(NAME)

debug: CF = -g $(DFLAGS) $(CFLAGS)
debug: LF = -g
debug: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LF) $(OBJ) -o $(NAME) $(LIBS)

cshell.o: ./src/cshell.c
	$(CC) $(CF) -c ./src/cshell.c
errors.o: ./src/errors.c
	$(CC) $(CF) -c ./src/errors.c
hull.o: ./src/hull.c
	$(CC) $(CF) -c ./src/hull.c
list.o: ./src/list.c
	$(CC) $(CF) -c ./src/list.c
logging.o: ./src/logging.c
	$(CC) $(CF) -c ./src/logging.c
main.o: ./src/main.c
	$(CC) $(CF) -c ./src/main.c
prompt.o: ./src/prompt.c
	$(CC) $(CF) -c ./src/prompt.c
samples.o: ./src/samples.c
	$(CC) $(CF) -c ./src/samples.c
sequencer.o: ./src/sequencer.c
	$(CC) $(CF) -c ./src/sequencer.c
wavefmt.o: ./src/wavefmt.c
	$(CC) $(CF) -c ./src/wavefmt.c

clean:
	rm $(OBJ)

.PHONY: clean