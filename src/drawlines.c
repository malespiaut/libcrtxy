/*
  drawlines.c

  Test app for CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 29, 2008
*/

#include <crtxy.h>

int main(int argc, char * argv[])
{
  int n, i, a, x, y;
  XY_options opts;
  XY_color black;
  XY_bool done;
  SDL_Event event;


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

  black = XY_getcolor(0x00, 0x00, 0x00, 0x00);

  n = 0;
  done = XY_FALSE;

  do
  {
    for (i = 0; i < 16; i++)
    {
      XY_start_frame(60);

      for (a = n; a < 360 + n; a++)
      {
        x = (XY_cos(a) * i) + (16 << XY_FIXED_SHIFT);
        y = (-XY_sin(a) * i) + (12 << XY_FIXED_SHIFT);

        XY_draw_point(x, y, black);
      }

      XY_end_frame(XY_TRUE);
    }

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        done = XY_TRUE;
    }
  }
  while (!done);

  XY_quit();

  return(0);
}

