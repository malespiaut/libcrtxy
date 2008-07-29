/*
  drawlines.c

  Test app for CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 29, 2008
*/

#include <crtxy.h>

int main(int argc, char * argv[])
{
  int i, chg;
  XY_options opts;

  /* XY_parse_options(argc, argv, &opts); */

  if (XY_init(&opts, 32<<XY_FIXED_SHIFT, 24<<XY_FIXED_SHIFT) < 0)
  {
    fprintf(stderr, "Error initializing XY_init()\n");
    /* XY_errstr() */
    /* XY_print_options(stderr, opts); */
    return(1);
  }

  XY_set_background(XY_getcolor(0xFF, 0xFF, 0xFF, 0x00), NULL,
                    0, 0, 0, 0);

  for (i = 0; i < 10; i++)
  {
    XY_start_frame(10);
    chg = XY_end_frame(XY_TRUE);
    printf("change = %d\n", chg);
  }

  XY_quit();

  return(0);
}

