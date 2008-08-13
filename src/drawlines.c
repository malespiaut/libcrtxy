/*
  drawlines.c

  Test app for CRT X-Y library (libcrtxy)
  http://libcrtxy.sf.net/

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - August 12, 2008
*/

#include <crtxy.h>

int main(int argc, char * argv[])
{
  int n, i, a, x1, y1, x2, y2;
  XY_options opts;
  XY_color black, color;
  XY_bool done, showbkgd;
  XY_bitmap * bkgd;
  SDL_Event event;
  int ret;

  XY_default_options(&opts);

  printf("\nDefault options:\n");
  XY_print_options(stdout, opts);


  ret = XY_load_options(&opts);
  if (!ret)
    return(1);

  printf("\nAfter loading options:\n");
  XY_print_options(stdout, opts);


  ret = XY_parse_envvars(&opts);
  if (!ret)
    return(1);

  printf("\nAfter parsing enviornment variables:\n");
  XY_print_options(stdout, opts);


  ret = XY_parse_options(&argc, argv, &opts);
  if (ret != 0)
  {
    fprintf(stderr, "Error setting libcrtxy options: %s\n", XY_errstr());
    fprintf(stderr, "Failed on %s\n", argv[ret]);
    return(1);
  }

  printf("\nAfter parsing options:\n");
  XY_print_options(stdout, opts);


  if (!XY_init(&opts, 32<<XY_FIXED_SHIFT, 24<<XY_FIXED_SHIFT))
  {
    fprintf(stderr, "Error initializing libcrtxy: %s\n", XY_errstr());
    XY_print_options(stderr, opts);
    return(1);
  }

  black = XY_setcolor(0x00, 0x00, 0x00, 0x00);

  bkgd = XY_load_bitmap("./testdata/test-bkgd2.png");

  XY_set_background(black, bkgd, 0, 0,
                    XY_POS_LEFT | XY_POS_TOP, XY_SCALE_STRETCH);

  n = 3;
  done = XY_FALSE;
  showbkgd = XY_TRUE;

  do
  {
    XY_start_frame(10);

    for (i = 0; i < 16; i++)
    {
      for (a = 0; a < 360; a = a + (360 / n))
      {
        x1 = (XY_cos(a) * i) + (16 << XY_FIXED_SHIFT);
        y1 = (-XY_sin(a) * i) + (12 << XY_FIXED_SHIFT);
        x2 = (XY_cos(a + (360 / n)) * i) + (16 << XY_FIXED_SHIFT);
        y2 = (-XY_sin(a + (360 / n)) * i) + (12 << XY_FIXED_SHIFT);

        color = XY_setcolor((a * 255) / 360,
                            0xff,
                            255 - ((a * 255) / 720),
                            i * 16);

        XY_draw_line(x1, y1, x2, y2, color, XY_THIN);
      }
    }

    XY_end_frame(XY_TRUE);

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        done = XY_TRUE;
      else if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE)
          done = XY_TRUE;
        else if (event.key.keysym.sym == SDLK_b)
        {
          showbkgd = !showbkgd;
          XY_enable_background(showbkgd);
        }
      }
    }

    n++;
    if (n >= 30)
      n = 3;
  }
  while (!done);

  XY_quit();

  return(0);
}

