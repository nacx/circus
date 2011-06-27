CIRCUS_PATH = ../lib

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall

CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/hook.c $(CIRCUS_PATH)/network.c
ALL_SRC = $(CIRCUS_SRC) test.c test_hashtable.c test_hook.c test_network.c

OBJECTS = $(ALL_SRC:%.c=%.o)
BINARY = circus

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(LN) -o $(BINARY)-test $(OBJECTS)
	./$(BINARY)-test
	
clean:
	rm -f *.o $(BINARY)-test
