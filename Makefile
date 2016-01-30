CC=gcc
CFLAGS=-I. -lpthread -lncurses -lm
DEBUGFLAGS=$(CFLAGS) -g
SOURCE=snake.c spawner.c level.c utils.c list.c
BIN=a.out

all: clean $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)

debug: clean $(SOURCE)
	$(CC) $(SOURCE) $(DEBUGFLAGS) -o $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)