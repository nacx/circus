# Circus top-level Makefile
# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

TARGETS = lib test install uninstall clean-lib clean-test

all:
	cd src && $(MAKE) $@
	$(MAKE) examples

$(TARGETS):
	cd src && $(MAKE) $@

clean: clean-lib clean-test clean-examples

examples: lib
	cd examples && $(MAKE) $@

clean-examples:
	cd examples && $(MAKE) clean

