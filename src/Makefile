#############################
# Custom sources to compile #
#############################

SRC = main.c

######################
# Circus source code #
######################

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall

CIRCUS_PATH = lib
CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/hook.c \
			 $(CIRCUS_PATH)/network.c
ALL_SRC = $(SRC) $(CIRCUS_SRC)
OBJECTS = $(ALL_SRC:%.c=%.o)

TEST_PATH = test
TEST_SRC = $(TEST_PATH)/test.c $(TEST_PATH)/test_hashtable.c \
		   $(TEST_PATH)/test_hook.c $(TEST_PATH)/test_network.c
TEST_OBJ = $(TEST_SRC:%.c=%.o)

BINARY = circus

ifdef name
	BINARY = $(name)
endif


all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(LN) -o $(BINARY) $(OBJECTS)
	
test: $(TEST_OBJ)
	cd test && $(MAKE)
	
clean:
	rm -f $(CIRCUS_PATH)/*.o
	rm -f $(TEST_PATH)/*.o $(TEST_PATH)/$(BINARY)-test
	rm -f *.o $(BINARY)
