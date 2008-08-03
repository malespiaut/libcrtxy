# Makefile for libcrtxy (CRT X-Y library)
#
# Bill Kendrick <bill@newbreedsoftware.com>
#
# July 28, 2008 - August 2, 2008

PREFIX=/usr/local

# 'System-wide' Config file:
ifeq ($(PREFIX),/usr)
  CONFDIR:=$(DESTDIR)/etc/libcrtxy
else
  CONFDIR:=$(DESTDIR)$(PREFIX)/etc/libcrtxy
endif

SDL_CFLAGS=$(shell sdl-config --cflags)
CFLAGS=-O2 -g -Wall \
  $(SDL_CFLAGS) \
  -DPREFIX=\"$(PREFIX)\" -DCONFDIR=\"$(CONFDIR)\" \
  -DVERSION="$(VERSION)"
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include
BINDIR=$(PREFIX)/bin

VER_MAJOR=0
VER_MINOR=0
VER_REV=1
VERSION=$(VER_MAJOR).$(VER_MINOR).$(VER_REV)

OBJ=crtxy.o


all:	libcrtxy.so libcrtxy.a crtxy-config

tests:	drawlines

clean:
	-rm libcrtxy.so
	-rm libcrtxy.a
	-rm crtxy.o
	-rm crtxy-config
	-rm drawlines.o
	-rm drawlines

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
	install -d $(CONFDIR)
	install -m 644 src/libcrtxy.conf-max $(CONFDIR)/libcrtxy.conf

libcrtxy.so:	$(OBJ)
	$(CC) $(CFLAGS) -shared $^ -o libcrtxy.so

libcrtxy.a:	$(OBJ)
	ar rcs libcrtxy.a $^

crtxy.o:	src/crtxy.c src/crtxy.h
	$(CC) $(CFLAGS) $< -c -o $@

crtxy-config:	src/crtxy-config.sh.in
	sed -e s=__VERSION__=$(VERSION)= \
	    -e s=__INCDIR__=$(INCDIR)= \
            -e s=__LIBDIR__=$(LIBDIR)= \
	    $< > $@
	chmod a+x $@

drawlines.o:	src/drawlines.c src/crtxy.h crtxy-config
	$(CC) -O2 -g -Wall $(shell ./crtxy-config --cflags) $< -c -o $@

drawlines:	drawlines.o
	$(CC) $< -o $@ $(shell ./crtxy-config --libs)

.PHONY: all clean install tests
