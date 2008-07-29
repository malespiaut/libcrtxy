/*
  crtxy.c

  CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 29, 2008
*/

#include "crtxy.h"

int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash)
{
  return(0);
}

void XY_quit(void)
{
}

XY_bitmap * XY_load_bitmap(char * filename)
{
  return(NULL);
}

XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer)
{
  return(NULL);
}

void XY_free_bitmap(XY_bitmap * bitmap)
{
}

void XY_set_background(XY_color color, XY_bitmap * bitmap,
                       XY_fixed x, XY_fixed y, int posflags, int scaling)
{
}

XY_color XY_getcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  return ((r << 24) | (g << 16) | (b << 8) | a);
}

void XY_enable_background(XY_bool enable)
{
}

void XY_start_frame(int fps)
{
}

int XY_end_frame(void)
{
  return(1);
}

void XY_draw_line(XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                  XY_color color)
{
}

void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color)
{
}

XY_fixed XY_sin(int degrees)
{
  return(0);
}

XY_fixed XY_cos(int degrees)
{
  return(0);
}

XY_fixed XY_screenx_to_canvasx(int sx)
{
  return(0);
}

XY_fixed XY_screeny_to_canvasy(int sy)
{
  return(0);
}

void XY_screen_to_canvas(int sx, int sy, XY_fixed * cx, XY_fixed * cy)
{
}


int XY_canvasx_to_screenx(XY_fixed cx)
{
  return(0);
}

int XY_canvasy_to_screeny(XY_fixed cy)
{
  return(0);
}

void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy)
{
}

