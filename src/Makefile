#############################
# Custom sources to compile #
#############################

SRC = circus.c

######################
# Circus source code #
######################

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall

# Circus build
CIRCUS_PATH = lib
CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/hook.c \
			 $(CIRCUS_PATH)/network.c $(CIRCUS_PATH)/irc.c
CIRCUS_OBJ = $(CIRCUS_SRC:%.c=%.o)

# Test build
TEST_PATH = test
TEST_SRC = $(TEST_PATH)/minunit.c $(TEST_PATH)/test_hashtable.c \
		   $(TEST_PATH)/test_hook.c $(TEST_PATH)/test_network.c \
		   $(TEST_PATH)/test.c
TEST_OBJ = $(TEST_SRC:%.c=%.o)

# Binary build
ALL_SRC = $(CIRCUS_SRC) $(SRC)
ALL_OBJ = $(ALL_SRC:%.c=%.o)

BIN = circus

ifdef name
	BIN = $(name)
endif

BIN_TEST = $(BIN)-test


all: $(BIN)

$(BIN): lib test $(ALL_OBJ)
	$(LN) -o $(BIN) $(ALL_OBJ)
	
lib: $(CIRCUS_OBJ)
	
test: lib $(TEST_OBJ)
	$(LN) -o $(TEST_PATH)/$(BIN_TEST) $(TEST_OBJ) $(CIRCUS_OBJ)
	$(TEST_PATH)/$(BIN_TEST)
	
clean-lib:
	rm -f $(CIRCUS_PATH)/*.o
	
clean-test:
	rm -f $(TEST_PATH)/*.o $(TEST_PATH)/$(BIN_TEST)
	
clean: clean-lib clean-test
	rm -f *.o $(BIN)
