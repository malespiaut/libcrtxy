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

  printf("screen is %d x %d\n", XY_get_screenw(), XY_get_screenh());

  printf("0,0 in canvas is: %d,%d (%d,%d)\n",
    XY_screenx_to_canvasx(0),
    XY_screeny_to_canvasy(0),
    XY_screenx_to_canvasx(0)>>XY_FIXED_SHIFT,
    XY_screeny_to_canvasy(0)>>XY_FIXED_SHIFT);

  printf("320,240 in canvas is: %d,%d (%d,%d)\n",
    XY_screenx_to_canvasx(320),
    XY_screeny_to_canvasy(240),
    XY_screenx_to_canvasx(320)>>XY_FIXED_SHIFT,
    XY_screeny_to_canvasy(240)>>XY_FIXED_SHIFT);

  printf("160,120 in canvas is: %d,%d (%d,%d)\n",
    XY_screenx_to_canvasx(160),
    XY_screeny_to_canvasy(120),
    XY_screenx_to_canvasx(160)>>XY_FIXED_SHIFT,
    XY_screeny_to_canvasy(120)>>XY_FIXED_SHIFT);

  for (i = 0; i < 10; i++)
  {
    XY_start_frame(10);
    chg = XY_end_frame(XY_TRUE);
    printf("change = %d\n", chg);
  }

  XY_quit();

  return(0);
}

