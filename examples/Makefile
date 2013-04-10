# Circus examples Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

TARGETS = welcome oper binding callback logger modes

INCLUDEDIR = ../src/lib
LIBDIR = ../src/lib

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall -ansi -pedantic -I$(INCLUDEDIR)
LDFLAGS = -L$(LIBDIR) -lcircus -lpthread

all: $(TARGETS)

examples: all

$(TARGETS):
	$(CC) $(CFLAGS) -c $@.c
	$(LN) -o $@ $@.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGETS)

.PHONY: clean
