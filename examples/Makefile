#!/usr/bin/make

SOURCES:=$(wildcard *.c) $(wildcard *.cc)
TARGETS:=$(SOURCES:%.c=%)
TARGETS:=$(TARGETS:%.cc=%)

all: $(TARGETS)

CFLAGS=-O0 -Wall -gstabs+3 -I../src
CXXFLAGS=-O0 -Wall -gstabs+3 -I../src
LDLIBS=

$(TARGETS): ../src/libmigdb.a

clean:
	-@$(RM) *.o *.a .*~ $(TARGETS) 2> /dev/null