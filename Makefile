# Makefile for libcrtxy (CRT X-Y library)
#
# Bill Kendrick <bill@newbreedsoftware.com>
#
# July 28, 2008 - July 28, 2008

CFLAGS=-g -Wall -pedantic -DVERSION="$(VERSION)"
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include
BINDIR=$(PREFIX)/bin
PREFIX=/usr/local

VER_MAJOR=0
VER_MINOR=0
VER_REV=1
VERSION=$(VER_MAJOR).$(VER_MINOR).$(VER_REV)

OBJ=crtxy.o


all:	libcrtxy.so libcrtxy.a crtxy-config

clean:
	-rm libcrtxy.so
	-rm libcrtxy.a
	-rm crtxy.o
	-rm crtxy-config

install:
	install -d $(LIBDIR)
	install -m 644 libcrtxy.a $(LIBDIR)/libcrtxy.a
	install libcrtxy.so $(LIBDIR)/libcrtxy.so.$(VERSION)
	-rm $(LIBDIR)/libcrtxy.so.$(VER_MAJOR).$(VER_MINOR)
	ln -s $(LIBDIR)/libcrtxy.so.$(VERSION) $(LIBDIR)/libcrtxy.so.$(VER_MAJOR).$(VER_MINOR)
	-rm $(LIBDIR)/libcrtxy.so.$(VER_MAJOR)
	ln -s $(LIBDIR)/libcrtxy.so.$(VERSION) $(LIBDIR)/libcrtxy.so.$(VER_MAJOR)
	install -d $(INCDIR)
	install -m 644 src/crtxy.h $(INCDIR)/
	install -d $(BINDIR)
	install crtxy-config $(BINDIR)

libcrtxy.so:	$(OBJ)
	$(CC) $(CFLAGS) -shared $^ -o libcrtxy.so

libcrtxy.a:	$(OBJ)
	ar rcs libcrtxy.a $^

crtxy.o:	src/crtxy.c
	$(CC) $(CFLAGS) $< -c -o $@

crtxy-config:	src/crtxy-config.sh.in
	sed -e s=__VERSION__=$(VERSION)= \
	    -e s=__INCDIR__=$(INCDIR)= \
            -e s=__LIBDIR__=$(LIBDIR)= \
	    $< > $@

.PHONY: all clean install
