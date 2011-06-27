#############################
# Custom sources to compile #
#############################

SRC = main.c


######################
# Circus source code #
######################

CC = gcc
LN = $(CC)
CFLAGS = -pipe -O2 -Wall

CIRCUS_SRC = lib/hashtable.c lib/hook.c
ALL_SRC = $(SRC) $(CIRCUS_SRC)

OBJECTS = $(ALL_SRC:%.c=%.o)
BINARY = circus

ifdef name
	BINARY = $(name)
endif


all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(LN) -o $(BINARY) $(OBJECTS)
	
clean:
	rm -f lib/*.o
	rm -f *.o $(BINARY)
