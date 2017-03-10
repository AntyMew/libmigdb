#!/usr/bin/make

.PHONY: libmigdb

all: libmigdb

libmigdb:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean
	$(MAKE) -C examples clean
	-@$(RM) version

install:
	$(MAKE) -C src install

uninstall:
	$(MAKE) -C src uninstall