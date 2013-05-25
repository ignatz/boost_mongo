# Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
# Distributed under the terms of the LGPLv3 or newer.

build: configure
	./waf build

install: configure
	./waf install

configure:
	./waf configure

clean:
	./waf clean

distclean:
	./waf distclean

dist:
	./waf dist

.PHONY: build install configure clean distclean
