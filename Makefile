CC ?= gcc
LINKER ?= $(CC)
NAME ?= exceptions
CFLAGS += -Wall -Wextra -g -pedantic -std=c99
OFILES := exceptions.o

FULLCC := $(CC) $(CFLAGS)
FULLLINK := $(LINKER)

.PHONY: all clean

all: exceptions.o

$(OFILES): %.o: %.c
	$(FULLCC) -c $*.c -o $@

%: %.c $(OFILES)
	$(FULLCC) -c $*.c -o $*.o
	$(FULLLINK) $(OFILES) $*.o -o $*
