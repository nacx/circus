# Top level makefile

all:
	cd src && $(MAKE) $@
	$(MAKE) test
	
test:
	cd src && $(MAKE) test
	
$(TARGETS) clean:
	cd src && $(MAKE) $@
