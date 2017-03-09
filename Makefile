#!/usr/bin/make

.PHONY: libmigdb

all: libmigdb

libmigdb:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean
	$(MAKE) -C examples clean
	-@rm version

install:
	$(MAKE) -C src install

