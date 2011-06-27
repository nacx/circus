CIRCUS_PATH = ../lib

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall

CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/hook.c
ALL_SRC = $(CIRCUS_SRC) test.c test_hashtable.c test_hook.c

OBJECTS = $(ALL_SRC:%.c=%.o)
BINARY = circus

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(LN) -o $(BINARY)-test $(OBJECTS)
	./$(BINARY)-test
	
clean:
	rm -f *.o $(BINARY)-test
