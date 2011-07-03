# Circus Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall

# Circus build
CIRCUS_PATH = lib
CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/binding.c \
			 $(CIRCUS_PATH)/network.c $(CIRCUS_PATH)/message_handler.c \
			 $(CIRCUS_PATH)/events.c $(CIRCUS_PATH)/utils.c \
			 $(CIRCUS_PATH)/codes.c $(CIRCUS_PATH)/irc.c
CIRCUS_OBJ = $(CIRCUS_SRC:%.c=%.o)

# Test build
TEST_PATH = test
TEST_SRC = $(TEST_PATH)/minunit.c $(TEST_PATH)/test_hashtable.c \
		   $(TEST_PATH)/test_binding.c $(TEST_PATH)/test_network.c \
		   $(TEST_PATH)/test_message_handler.c $(TEST_PATH)/test_events.c \
		   $(TEST_PATH)/test_codes.c $(TEST_PATH)/test_utils.c \
		   $(TEST_PATH)/test.c
TEST_OBJ = $(TEST_SRC:%.c=%.o)

LIB = libcircus.a
LIB_TEST = libcircus-test
INSTALL_DIR = /usr/local/lib

ifdef prefix
	INSTALL_DIR = $(prefix)
endif


all: $(LIB)

$(LIB): lib test

lib: $(CIRCUS_OBJ) $(TEST_OBJ)
	$(AR) -rv $(CIRCUS_PATH)/$(LIB) $(CIRCUS_OBJ)
	$(LN) -o $(TEST_PATH)/$(LIB_TEST) $(TEST_OBJ) -L$(CIRCUS_PATH) -lcircus
	$(TEST_PATH)/$(LIB_TEST)

test: lib

install: all
	install -m 0444 $(CIRCUS_PATH)/$(LIB) $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(LIB)

clean-lib:
	rm -f $(CIRCUS_PATH)/*.o $(CIRCUS_PATH)/$(LIB)

clean-test:
	rm -f $(TEST_PATH)/*.o $(TEST_PATH)/$(LIB_TEST)

clean: clean-lib clean-test

