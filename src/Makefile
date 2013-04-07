# Circus Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall -ansi -pedantic
LDFLAGS = -lpthread -lcircus

ifdef DEBUG
    CFLAGS += -DDEBUG
endif

# Circus build
CIRCUS_PATH = lib
CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/binding.c \
			 $(CIRCUS_PATH)/network.c $(CIRCUS_PATH)/message_handler.c \
			 $(CIRCUS_PATH)/events.c $(CIRCUS_PATH)/utils.c \
			 $(CIRCUS_PATH)/codes.c $(CIRCUS_PATH)/irc.c \
			 $(CIRCUS_PATH)/debug.c $(CIRCUS_PATH)/version.c
CIRCUS_OBJ = $(CIRCUS_SRC:%.c=%.o)

# Test build
TEST_PATH = test
TEST_SRC = $(TEST_PATH)/minunit.c $(TEST_PATH)/test_hashtable.c \
		   $(TEST_PATH)/test_binding.c $(TEST_PATH)/test_message_handler.c \
		   $(TEST_PATH)/test_events.c $(TEST_PATH)/test_codes.c \
		   $(TEST_PATH)/test_utils.c $(TEST_PATH)/test_version.c \
		   $(TEST_PATH)/test_irc.c $(TEST_PATH)/test_network.c \
		   $(TEST_PATH)/test.c
TEST_OBJ = $(TEST_SRC:%.c=%.o)

LIB_NAME = libcircus.a
LIB = $(CIRCUS_PATH)/$(LIB_NAME)
LIB_TEST = libcircus-test
PREFIX ?= /usr/local


all: $(LIB)

$(LIB): lib

gen-version:
	sh gen-version.sh

lib: gen-version $(CIRCUS_OBJ)
	$(AR) -rv $(LIB) $(CIRCUS_OBJ)

test: $(TEST_OBJ)
	test -f $(LIB) || $(MAKE) lib
	$(LN) -o $(TEST_PATH)/$(LIB_TEST) $(TEST_OBJ) -L$(CIRCUS_PATH) $(LDFLAGS)
	$(TEST_PATH)/$(LIB_TEST)

install: 
	test -f $(LIB) || $(MAKE) lib
	install -d -m 0755 $(PREFIX)/lib
	install -d -m 0755 $(PREFIX)/include/circus
	install -m 0444 $(LIB) $(PREFIX)/lib
	install -m 0444 $(CIRCUS_PATH)/*.h $(PREFIX)/include/circus

uninstall:
	rm -f $(PREFIX)/lib/$(LIB_NAME)
	rm -rf $(PREFIX)/include/circus

clean-lib:
	rm -f $(CIRCUS_PATH)/*.o $(LIB)

clean-test:
	rm -f $(TEST_PATH)/*.o $(TEST_PATH)/$(LIB_TEST)

clean: clean-lib clean-test

.PHONY: gen-version install uninstall clean-lib clean-test clean

