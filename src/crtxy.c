/*
  crtxy.c

  CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 29, 2008
*/

#include "crtxy.h"
#include <SDL_image.h>

SDL_Surface * XY_screen;
XY_fixed XY_canvasw, XY_canvash;
Uint32 XY_background_color;
XY_bitmap * XY_background_bitmap;
XY_bool XY_background_bitmap_enabled;
int XY_background_x, XY_background_y;
Uint32 XY_want_fps, XY_start_time;

int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash)
{
  SDL_Init(SDL_INIT_VIDEO); /* FIXME: Test error */

  XY_canvasw = canvasw;
  XY_canvash = canvash;

  XY_screen = SDL_SetVideoMode(320, 240, 24, SDL_SWSURFACE); /* FIXME: Options */
  /* FIXME: Test error */

  XY_background_color = SDL_MapRGB(XY_screen->format, 0x00, 0x00, 0x00);
  XY_background_bitmap = NULL;
  XY_background_bitmap_enabled = XY_FALSE;
  XY_background_x = XY_background_y = 0;

  return(0);
}

void XY_quit(void)
{
  SDL_Quit();
}

XY_bitmap * XY_load_bitmap(char * filename)
{
  SDL_Surface * surf, * dispsurf;
  XY_bitmap * xyb;

  xyb = (XY_bitmap *) malloc(sizeof(XY_bitmap));
  if (xyb == NULL)
    return(NULL); /* FIXME: Set error */

  surf = IMG_Load(filename);
  if (surf == NULL)
  {
    free(xyb);
    return(NULL); /* FIXME: Set error */
  }

  dispsurf = SDL_DisplayFormatAlpha(surf);
  SDL_FreeSurface(surf);
  if (dispsurf == NULL)
  {
    free(xyb);
    return(NULL); /* FIXME: Set error */
  }

  xyb->surf = dispsurf;

  return(xyb);
}

XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer)
{
  /* FIXME: Do it */
  return(NULL);
}

void XY_free_bitmap(XY_bitmap * bitmap)
{
  if (bitmap != NULL)
  {
    if (bitmap->surf != NULL)
      SDL_FreeSurface(bitmap->surf);

    free(bitmap);
  }
}

void XY_set_background(XY_color color, XY_bitmap * bitmap,
                       XY_fixed x, XY_fixed y, int posflags, int scaling)
{
  XY_background_color = SDL_MapRGB(XY_screen->format,
				   (color >> 24) & 0xFF,
				   (color >> 16) & 0xFF,
				   (color >> 8) & 0xFF);

  /* FIXME: Scale bitmap */
  XY_background_bitmap = bitmap;
  XY_background_bitmap_enabled = XY_TRUE;

  /* FIXME: Deal with posflags and scaling */
  XY_background_x = x;
  XY_background_y = y;
}

XY_color XY_getcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  return ((r << 24) | (g << 16) | (b << 8) | a);
}

void XY_enable_background(XY_bool enable)
{
  XY_background_bitmap_enabled = enable;
}

void XY_start_frame(int fps)
{
  SDL_FillRect(XY_screen, NULL, XY_background_color);

  XY_want_fps = (fps == 0 ? 1 : fps);
  XY_start_time = SDL_GetTicks();
}

int XY_end_frame(XY_bool throttle)
{
  Uint32 end_time;

  SDL_Flip(XY_screen);
  end_time = SDL_GetTicks();

  if (throttle)
  {
    if (end_time - XY_start_time < (1000 / XY_want_fps))
      SDL_Delay(XY_start_time + (1000 / XY_want_fps) - end_time);

    end_time = SDL_GetTicks();
  }

  return(end_time - XY_start_time);
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

