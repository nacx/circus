# Circus top-level Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

TARGETS = lib test install uninstall clean-lib clean-test

LIB = src/lib/libcircus.a

all: lib examples
	@echo "\n*** Done! Run 'make test' to make sure everything is working as expected! ***\n"

$(TARGETS):
	$(MAKE) $@ -C src

clean: clean-lib clean-test clean-examples

examples:
	test -f $(LIB) || $(MAKE) lib
	$(MAKE) $@ -C examples

clean-examples:
	$(MAKE) clean -C examples

.PHONY: examples clean clean-lib clean-test clean-examples
