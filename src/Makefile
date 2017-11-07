# Circus Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall -ansi -pedantic
LDFLAGS = -lcircus -lpthread

ifdef DEBUG
    CFLAGS += -DDEBUG
endif

# Installation directory
PREFIX ?= /usr/local

# Circus build
CIRCUS_PATH = lib
CIRCUS_SRC = $(CIRCUS_PATH)/hashtable.c $(CIRCUS_PATH)/binding.c \
			 $(CIRCUS_PATH)/network.c $(CIRCUS_PATH)/listener.c \
			 $(CIRCUS_PATH)/events.c $(CIRCUS_PATH)/utils.c \
			 $(CIRCUS_PATH)/codes.c $(CIRCUS_PATH)/irc.c \
			 $(CIRCUS_PATH)/debug.c $(CIRCUS_PATH)/version.c \
			 $(CIRCUS_PATH)/dispatcher.c
CIRCUS_OBJ = $(CIRCUS_SRC:%.c=%.o)
LIB_NAME = libcircus.a
LIB = $(CIRCUS_PATH)/$(LIB_NAME)

# Test build
TEST_PATH = test
TEST_SRC = $(TEST_PATH)/minunit.c $(TEST_PATH)/test_hashtable.c \
		   $(TEST_PATH)/test_binding.c $(TEST_PATH)/test_listener.c \
		   $(TEST_PATH)/test_events.c $(TEST_PATH)/test_codes.c \
		   $(TEST_PATH)/test_utils.c $(TEST_PATH)/test_version.c \
		   $(TEST_PATH)/test_irc.c $(TEST_PATH)/test_network.c \
		   $(TEST_PATH)/test_dispatcher.c $(TEST_PATH)/test.c
TEST_OBJ = $(TEST_SRC:%.c=%.o)
LIB_TEST = libcircus-test

# Benchmark build
BNCHK = circus-bnchk
BNCHK_SRC = $(TEST_PATH)/bnchk.c
BNCHK_OBJ = $(BNCHK_SRC:%.c=%.o)


all: $(LIB) benchmark

$(LIB): lib

gen-version:
	sh -x gen-version.sh

lib: gen-version $(CIRCUS_OBJ)
	$(AR) -rv $(LIB) $(CIRCUS_OBJ)

test: $(TEST_OBJ)
	test -f $(LIB) || $(MAKE) lib
	$(LN) -o $(TEST_PATH)/$(LIB_TEST) $(TEST_OBJ) -L$(CIRCUS_PATH) $(LDFLAGS)
	$(TEST_PATH)/$(LIB_TEST)

benchmark: $(BNCHK_OBJ)
	test -f $(LIB) || $(MAKE) lib
	$(LN) -o $(TEST_PATH)/$(BNCHK) $(BNCHK_OBJ) -L$(CIRCUS_PATH) $(LDFLAGS)

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
	rm -f $(CIRCUS_OBJ) $(LIB)

clean-test:
	rm -f $(TEST_OBJ) $(TEST_PATH)/$(LIB_TEST)

clean-benchmark:
	rm -f $(BNCHK_OBJ) $(TEST_PATH)/$(BNCHK)

clean: clean-lib clean-test clean-benchmark

.PHONY: gen-version install uninstall clean-lib clean-test clean-benchmark clean

