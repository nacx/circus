# Circus examples Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

TARGETS = listen welcome oper binding callback

LIBDIR = ../src/lib

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall -I$(LIBDIR)
LDFLAGS = -L$(LIBDIR) -lcircus

all: $(TARGETS)

examples: all

$(TARGETS):
	$(CC) $(CFLAGS) -c $@.c
	$(LN) -o $@ $@.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGETS)

