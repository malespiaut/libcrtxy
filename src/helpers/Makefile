# Makefile for libcrtxy helpers
#
# Bill Kendrick <bill@newbreedsoftware.com>
# August 9, 2008 - August 10, 2008

all:	gamma trig

clean:
	-rm gamma trig

gamma:	gamma.c
	$(CC) $(CFLAGS) gamma.c -o gamma -lm

trig:	trig.c
	$(CC) $(CFLAGS) trig.c -o trig -lm
