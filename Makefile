# Top level makefile

TARGETS = test lib clean clean-lib clean-test

all:
	cd src && $(MAKE) $@
	
${TARGETS}:
	cd src && $(MAKE) $@
