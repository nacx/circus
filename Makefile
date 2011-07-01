# Top level makefile

TARGETS = lib test clean clean-lib clean-test

all:
	cd src && $(MAKE) $@

${TARGETS}:
	cd src && $(MAKE) $@
