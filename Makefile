# Circus top-level Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

TARGETS = lib test install uninstall clean-lib clean-test

all:
	$(MAKE) $@ -C src
	$(MAKE) examples

$(TARGETS):
	$(MAKE) $@ -C src

clean: clean-lib clean-test clean-examples

examples: lib
	$(MAKE) $@ -C examples

clean-examples:
	$(MAKE) clean -C examples

