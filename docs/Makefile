# Makefile for libcrtxy docs

HTMLTOTXT=links -dump

all:	README.txt API.txt

README.txt:	html/README.html
	$(HTMLTOTXT) $< > $@

API.txt:	html/API.html
	$(HTMLTOTXT) $< > $@

.PHONY: all

