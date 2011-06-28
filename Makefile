# Top level makefile

all:
	cd src && $(MAKE) $@
	$(MAKE) test
	
clean:
	cd src && $(MAKE) $@
	
test:
	cd src && $(MAKE) test

