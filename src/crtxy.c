/*
  crtxy.c

  CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 31, 2008
*/

#include "crtxy.h"
#include <SDL_image.h>

SDL_Surface * XY_screen;
XY_fixed XY_canvasw, XY_canvash;
Uint32 XY_background_color;
XY_bitmap * XY_background_bitmap;
XY_bool XY_background_bitmap_enabled;
SDL_Rect XY_background_dest;
Uint32 XY_want_fps, XY_start_time;

int XY_trig[91] = {
  65536,  65526,  65496,  65446,  65376,
  65287,  65177,  65048,  64898,  64729,
  64540,  64332,  64104,  63856,  63589,
  63303,  62997,  62672,  62328,  61966,
  61584,  61183,  60764,  60326,  59870,
  59396,  58903,  58393,  57865,  57319,
  56756,  56175,  55578,  54963,  54332,
  53684,  53020,  52339,  51643,  50931,
  50203,  49461,  48703,  47930,  47143,
  46341,  45525,  44695,  43852,  42995,
  42126,  41243,  40348,  39441,  38521,
  37590,  36647,  35693,  34729,  33754,
  32768,  31772,  30767,  29753,  28729,
  27697,  26656,  25607,  24550,  23486,
  22415,  21336,  20252,  19161,  18064,
  16962,  15855,  14742,  13626,  12505,
  11380,  10252,   9121,   7987,   6850,
   5712,   4572,   3430,   2287,   1144,
      0
};


/* Private functions and macros: */

void putpixel(SDL_Surface * surface, int x, int y, Uint32 pixel);

#define XY_color_to_sdl_color(color) SDL_MapRGBA(XY_screen->format, \
                                       ((color) >> 24) & 0xFF, \
                                       ((color) >> 16) & 0xFF, \
                                       ((color) >> 8) & 0xFF, \
                                       (color) & 0xFF)


/* Public functions: */

int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    /* FIXME: Set error */
    /* FIXME: Call SDL_QuitSubSystem(SDL_INIT_VIDEO) here? */
    return(-1);
  }

  XY_canvasw = canvasw;
  XY_canvash = canvash;

  /* FIXME: Options */
  XY_screen = SDL_SetVideoMode(320, 240, 24, SDL_SWSURFACE);

  if (XY_screen == NULL)
  {
    /* FIXME: Set error */
    SDL_Quit();
    return(-1);
  }

  XY_background_color = SDL_MapRGB(XY_screen->format, 0x00, 0x00, 0x00);
  XY_background_bitmap = NULL;
  XY_background_bitmap_enabled = XY_FALSE;
  XY_background_dest.x = 0;
  XY_background_dest.y = 0;

  return(0);
}

void XY_quit(void)
{
  /* FIXME: Use SDL_QuitSubSystem(SDL_INIT_VIDEO) instead? */
  if (XY_background_bitmap != NULL)
    XY_free_bitmap(XY_background_bitmap);

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
  XY_background_dest.x = x;
  XY_background_dest.y = y;
}

XY_color XY_setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  return ((r << 24) | (g << 16) | (b << 8) | a);
}

void XY_getcolor(XY_color c, Uint8 * r, Uint8 * g, Uint8 * b, Uint8 * a)
{
  *r = (c >> 24) & 0xff;
  *r = (c >> 16) & 0xff;
  *r = (c >> 8) & 0xff;
  *a = c & 0xff;
}

void XY_enable_background(XY_bool enable)
{
  XY_background_bitmap_enabled = enable;
}

void XY_start_frame(int fps)
{
  /* FIXME: Dirty rects? */
  SDL_FillRect(XY_screen, NULL, XY_background_color);

  if (XY_background_bitmap != NULL && XY_background_bitmap_enabled == XY_TRUE)
    SDL_BlitSurface(XY_background_bitmap->surf, NULL,
                    XY_screen, &XY_background_dest);

  XY_want_fps = (fps == 0 ? 1 : fps);
  XY_start_time = SDL_GetTicks();
}

int XY_end_frame(XY_bool throttle)
{
  Uint32 end_time;

  /* FIXME: Dirty rects? */
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
  int sx1, sy1, sx2, sy2;
  int y;
  XY_fixed fsx1, fsy1, fsx2, fsy2;
  Uint32 sdlcolor = XY_color_to_sdl_color(color);
  XY_fixed dx, dy;
  XY_fixed m, b;

  XY_canvas_to_screen(x1, y1, &sx1, &sy1);
  XY_canvas_to_screen(x2, y2, &sx2, &sy2);

  fsx1 = sx1 << XY_FIXED_SHIFT;
  fsy1 = sy1 << XY_FIXED_SHIFT;
  fsx2 = sx2 << XY_FIXED_SHIFT;
  fsy2 = sy2 << XY_FIXED_SHIFT;

  /* FIXME: clip!  if (XY_clip(&sx1, &sy1, &sx2, &sy2)) */
  {
    dx = fsx2 - fsx1;
    dy = fsy2 - fsy1;

    if (dx != 0)
    {
      m = XY_div(dy, dx);

      b = fsy1 - XY_mult(m, fsx1);

      if (fsx2 >= fsx1)
        dx = XY_FIXED_ONE;
      else
        dx = -XY_FIXED_ONE;

      while (fsx1 != fsx2)
      {
        fsy1 = XY_mult(m, fsx1) + b;
        fsy2 = XY_mult(m, (fsx1 + dx)) + b;

        if (fsy2 > fsy1)
        {
          for (y = fsy1 >> XY_FIXED_SHIFT; y <= fsy2 >> XY_FIXED_SHIFT; y++)
            putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor);
        }
        else
        {
          for (y = fsy2 >> XY_FIXED_SHIFT; y <= fsy1 >> XY_FIXED_SHIFT; y++)
            putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor);
        }

        fsx1 = fsx1 + dx;
      }
    }
    else
    {
      if (sy2 > sy1)
      {
        for (y = sy1; y <= sy2; y++)
          putpixel(XY_screen, sx1, y, sdlcolor);
      }
      else
      {
        for (y = sy2; y <= sy1; y++)
          putpixel(XY_screen, sx1, y, sdlcolor);
      }
    }
  }
}

void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color)
{
  int sx, sy;
  Uint32 sdlcolor = XY_color_to_sdl_color(color);

  XY_canvas_to_screen(x, y, &sx, &sy);

  putpixel(XY_screen, sx, sy, sdlcolor);
}

XY_fixed XY_cos(int degrees)
{
  while (degrees >= 360) degrees -= 360;
  while (degrees < 0) degrees += 360;

  if (degrees < 90)
    return(XY_trig[degrees]);

  else if (degrees <= 180)
    return(-XY_trig[180 - degrees]);

  else if (degrees <= 270)
    return(-XY_trig[degrees - 180]);

  else
    return(XY_trig[360 - degrees]);
}

XY_fixed XY_screenx_to_canvasx(int sx)
{
  return(sx * XY_canvasw) / XY_screen->w;
}

XY_fixed XY_screeny_to_canvasy(int sy)
{
  return(sy * XY_canvash) / XY_screen->h;
}

void XY_screen_to_canvas(int sx, int sy, XY_fixed * cx, XY_fixed * cy)
{
  *cx = XY_screenx_to_canvasx(sx);
  *cy = XY_screeny_to_canvasy(sy);
}

int XY_canvasx_to_screenx(XY_fixed cx)
{
  return(cx * XY_screen->w) / XY_canvasw;
}

int XY_canvasy_to_screeny(XY_fixed cy)
{
  return(cy * XY_screen->h) / XY_canvash;
}

void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy)
{
  *sx = XY_canvasx_to_screenx(cx);
  *sy = XY_canvasx_to_screenx(cy);
}

int XY_get_screenw(void)
{
  return(XY_screen->w);
}

int XY_get_screenh(void)
{
  return(XY_screen->h);
}

/* FIXME: Handle alpha value if alpha option is enabled;
   For 'dumb' mode (XY_ALPHA_FAKE), always just blend with background color;
   For 'real' mode (XY_ALPHA_BLEND), blend with the current pixel;
   For 'off' mode (XY_ALPHA_NONE), just use the RBG value, ignore Alpha
     (unless it's 0!)
*/

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
      return;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

/* FIXME: Implement Xiaolin Wu's antialiased line algorithm
   ( http://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm ) */

