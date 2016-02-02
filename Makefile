CC=gcc
CFLAGS=-I. -lpthread -lncurses -lm
DEBUGFLAGS=$(CFLAGS) -g
SOURCE=spawner.c level.c utils.c list.c filereader.c
MAIN=snake.c $(SOURCE)
TEST=test/$(FILE) $(SOURCE)
BIN=a.out

all: clean $(MAIN)
	$(CC) $(MAIN) $(CFLAGS) -o $(BIN)

debug: clean $(MAIN)
	$(CC) $(MAIN) $(DEBUGFLAGS) -o $(BIN)
	
test: clean $(TEST)
	$(CC) $(TEST) $(CFLAGS) -o $(BIN)

test_debug: clean $(TEST)
	$(CC) $(TEST) $(DEBUGFLAGS) -o $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)