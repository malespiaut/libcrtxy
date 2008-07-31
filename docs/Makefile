# Makefile for libcrtxy docs

HTMLTOTXT=links -dump

all:	README.txt

README.txt:	html/README.html
	$(HTMLTOTXT) $< > $@

.PHONY: all

