#!/usr/bin/make
PREFIX=/usr

all: libmigdb.a

SOURCES:=$(wildcard *.c)
OBJECTS:=$(SOURCES:%.c=%.o)

CFLAGS=-O2 -Wall -gstabs+3 -I.
CXXFLAGS=-O2 -Wall -gstabs+3
LDLIBS=

%.o: mi_gdb.h

libmigdb.a: $(OBJECTS)
	ar rcs $@ $^

clean:
	-@$(RM) *.o *.a .*~ 2> /dev/null

install:
	install -d -m 755 $(PREFIX)/lib
	install -m 644 libmigdb.a $(PREFIX)/lib
	install -d -m 755 $(PREFIX)/include
	install -m 644 mi_gdb.h $(PREFIX)/include

make uninstall:
	-@$(RM) $(PREFIX)/lib/libmigdb.a 2> /dev/null
	-@$(RM) $(PREFIX)/include/mi_gdb.h 2> /dev/null