# Makefile for libcrtxy (CRT X-Y library)
#
# Bill Kendrick <bill@newbreedsoftware.com>
#
# July 28, 2008 - August 16, 2008

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
DOCDIR=$(PREFIX)/share/doc/libcrtxy

VER_MAJOR=0
VER_MINOR=0
VER_REV=3
VERSION=$(VER_MAJOR).$(VER_MINOR).$(VER_REV)

OBJ=crtxy.o


all:	libcrtxy.so libcrtxy.a crtxy-config

tests:	drawlines rockdodge polytest

clean:
	-rm libcrtxy.so
	-rm libcrtxy.a
	-rm crtxy.o
	-rm crtxy-config
	-rm drawlines.o rockdodge.o polytest.o
	-rm drawlines rockdodge polytest

releaseclean:
	-rm -rf build/libcrtxy-$(VERSION) build/libcrtxy-$(VERSION).tar.gz
	-if [ -d build ] ; then rmdir build ; fi

release:	releasedir
	cd build ; \
	    tar -czvf libcrtxy-$(VERSION).tar.gz libcrtxy-$(VERSION)

releasedir:	build/libcrtxy-$(VERSION)

build/libcrtxy-$(VERSION):
	mkdir -p build/libcrtxy-$(VERSION)
	find . -follow \
	    \( -wholename '*/CVS' \
	       -o -name .cvsignore \
	       -o -name '*~' \
	       -o -name 'build' \
	       -o -name '.#*' \) \
	    -prune -o \
	    -type f \
	    -exec cp --parents -vdp \{\} build/libcrtxy-$(VERSION)/ \;

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
	install -d $(DOCDIR)/html
	cp docs/html/*.* $(DOCDIR)/html/
	chmod 644 $(DOCDIR)/html/*.*

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

rockdodge.o:	src/rockdodge.c src/crtxy.h crtxy-config
	$(CC) -O2 -g -Wall $(shell ./crtxy-config --cflags) $< -c -o $@

rockdodge:	rockdodge.o
	$(CC) $< -o $@ $(shell ./crtxy-config --libs)

polytest.o:	src/polytest.c src/crtxy.h crtxy-config
	$(CC) -O2 -g -Wall $(shell ./crtxy-config --cflags) $< -c -o $@

polytest:	polytest.o
	$(CC) $< -o $@ $(shell ./crtxy-config --libs)

.PHONY: all clean install tests releaseclean releasedir release
