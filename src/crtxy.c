/*
  crtxy.c

  CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - August 1, 2008
*/

#include "crtxy.h"
#include <SDL_image.h>

SDL_Surface * XY_screen;
XY_fixed XY_canvasw, XY_canvash;
Uint32 XY_background_color;
Uint8 XY_background_r, XY_background_g, XY_background_b;
XY_bitmap * XY_background_bitmap;
XY_bool XY_background_bitmap_enabled;
SDL_Rect XY_background_dest;
Uint32 XY_want_fps, XY_start_time;

/* FIXME: Always putting to XY_screen, so we can simplify further... */
void (*putpixel) (SDL_Surface *, int, int, Uint32, Uint8 alph);
Uint32 (*getpixel) (SDL_Surface * surface, int x, int y);

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

void putpixel_16(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);
void putpixel_24(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);
void putpixel_32(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);

void putpixel_fakea_16(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);
void putpixel_fakea_24(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);
void putpixel_fakea_32(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);

void putpixel_reala_16(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);
void putpixel_reala_24(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);
void putpixel_reala_32(SDL_Surface * surface, int x, int y, Uint32 pixel, Uint8 alph);

Uint32 getpixel_16(SDL_Surface * surface, int x, int y);
Uint32 getpixel_24(SDL_Surface * surface, int x, int y);
Uint32 getpixel_32(SDL_Surface * surface, int x, int y);

#define XY_color_to_sdl_color(color) SDL_MapRGB(XY_screen->format, \
                                       ((color) >> 24) & 0xFF, \
                                       ((color) >> 16) & 0xFF, \
                                       ((color) >> 8) & 0xFF)

SDL_Surface * scale_surf(SDL_Surface * orig, int new_w, int new_h);


/* Public functions: */

int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash)
{
  int bpp, Bpp;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    /* FIXME: Set error */
    /* FIXME: Call SDL_QuitSubSystem(SDL_INIT_VIDEO) here? */
    return(-1);
  }

  XY_canvasw = canvasw;
  XY_canvash = canvash;

  bpp = 16;

  /* FIXME: Add possibility of using SDL_ListModes to determine a suitable mode */
  /* FIXME: Options */
  XY_screen = SDL_SetVideoMode(320, 240, bpp, SDL_SWSURFACE);

  if (XY_screen == NULL)
  {
    /* FIXME: Set error */
    SDL_Quit();
    return(-1);
  }

  XY_background_color = SDL_MapRGB(XY_screen->format, 0x00, 0x00, 0x00);
  XY_background_r = 0;
  XY_background_g = 0;
  XY_background_b = 0;
  XY_background_bitmap = NULL;
  XY_background_bitmap_enabled = XY_FALSE;
  XY_background_dest.x = 0;
  XY_background_dest.y = 0;

  /* Determine which functions to use, based on display we actually got,
     and rendering options: */

  Bpp= XY_screen->format->BytesPerPixel;

  if (Bpp == 4)
    getpixel = getpixel_32;
  else if (Bpp == 3)
    getpixel = getpixel_24;
  else if (Bpp == 2)
    getpixel = getpixel_16;
  else
  {
    /* Unsupported depth */
    /* FIXME: Set error */
    return(-1);
  }

  
  if (0) /* FIXME */
  {
    /* Fake alpha blending (blend with background color) */
    if (Bpp == 4)
      putpixel = putpixel_fakea_32;
    else if (Bpp == 3)
      putpixel = putpixel_fakea_24;
    else if (Bpp == 2)
      putpixel = putpixel_fakea_16;
  }
  else if (1) /* FIXME */
  {
    /* Real alpha blending (blend with current pixel) */
    if (Bpp == 4)
      putpixel = putpixel_reala_32;
    else if (Bpp == 3)
      putpixel = putpixel_reala_24;
    else if (Bpp == 2)
      putpixel = putpixel_reala_16;
  }
  else
  {
    /* No alpha blending */
    if (Bpp == 4)
      putpixel = putpixel_32;
    else if (Bpp == 3)
      putpixel = putpixel_24;
    else if (Bpp == 2)
      putpixel = putpixel_16;
  }

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

XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer, int size)
{
  SDL_RWops * rw;
  SDL_Surface * surf, * dispsurf;
  XY_bitmap * xyb;

  if (buffer == NULL || size == 0)
    return(NULL); /* FIXME: Set error */

  xyb = (XY_bitmap *) malloc(sizeof(XY_bitmap));
  if (xyb == NULL)
    return(NULL); /* FIXME: Set error */

  rw = SDL_RWFromMem((void *) buffer, size);

  surf = IMG_Load_RW(rw, 0);
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
  int w, h;
  int posx, posy;

  XY_background_r = (color >> 24) & 0xFF;
  XY_background_g = (color >> 16) & 0xFF;
  XY_background_b = (color >> 8) & 0xFF;
  XY_background_color = SDL_MapRGB(XY_screen->format,
                                   XY_background_r,
                                   XY_background_g,
                                   XY_background_b);

  /* FIXME: Adhere to bitmap option setting! */

  if (XY_background_bitmap != NULL)
    XY_free_bitmap(XY_background_bitmap);

  if (bitmap != NULL && bitmap->surf != NULL)
  {
    w = bitmap->surf->w;
    h = bitmap->surf->h;

    XY_background_bitmap_enabled = XY_TRUE;
    XY_background_bitmap = malloc(sizeof(XY_bitmap));
    /* FIXME: check for errors */

    if (scaling == XY_SCALE_STRETCH)
    {
      w = XY_screen->w;
      h = XY_screen->h;
    }
    else if (scaling == XY_SCALE_KEEP_ASPECT_WIDE)
    {
      h = (((h << XY_FIXED_SHIFT) / w) * XY_screen->w) >> XY_FIXED_SHIFT;
      w = XY_screen->w;
    }
    else if (scaling == XY_SCALE_KEEP_ASPECT_TALL)
    {
      w = (((w << XY_FIXED_SHIFT) / h) * XY_screen->h) >> XY_FIXED_SHIFT;
      h = XY_screen->h;
    }


    if (w != bitmap->surf->w || h != bitmap->surf->h)
    {
      /* What we calculated is different from what we have; scale it! */
      SDL_Surface * scaled_surf = scale_surf(bitmap->surf, w, h);
      if (scaled_surf == NULL)
        ; /* FIXME: Check for errors */
      XY_background_bitmap->surf = SDL_DisplayFormatAlpha(scaled_surf);
      if (XY_background_bitmap->surf == NULL)
        ; /* FIXME: Check for errors */
      SDL_FreeSurface(scaled_surf);
    }
    else
    {
      XY_background_bitmap->surf = SDL_DisplayFormatAlpha(bitmap->surf);
      /* FIXME: check for errors */
    }

    /* Position */
    if (posflags & XY_POS_HCENTER)
      posx = (XY_screen->w - w) / 2;
    else if (posflags & XY_POS_RIGHT)
      posx = XY_screen->w - w;
    else
      posx = 0;

    if (posflags & XY_POS_VCENTER)
      posy = (XY_screen->h - h) / 2;
    else if (posflags & XY_POS_BOTTOM)
      posy = XY_screen->h - h;
    else
      posy = 0;

    /* Nudge */
    XY_background_dest.x = posx + x;
    XY_background_dest.y = posy + y;
  }
  else
  {
    /* No bitmap being used */
    XY_background_bitmap_enabled = XY_FALSE;
    XY_background_bitmap = NULL;
  }
}

SDL_Surface * scale_surf(SDL_Surface * orig, int new_w, int new_h)
{
  SDL_Surface * s;
  int x, y;
  Uint32 tr, tg, tb, ta;
  Uint8 r, g, b, a;
  int tmp;
  XY_fixed src_x, src_y, xscale, yscale, f_orig_w, f_orig_h;
  void (*pp) (SDL_Surface *, int, int, Uint32, Uint8 alph);
  Uint32 (*gp) (SDL_Surface * surface, int x, int y);

  yscale = (orig->h << XY_FIXED_SHIFT) / new_h;
  xscale = (orig->w << XY_FIXED_SHIFT) / new_w;

  f_orig_w = orig->w << XY_FIXED_SHIFT;
  f_orig_h = orig->h << XY_FIXED_SHIFT;

  s = SDL_CreateRGBSurface(orig->flags,	/* SDL_SWSURFACE, */
			   new_w, new_h,
			   orig->format->BitsPerPixel,
			   orig->format->Rmask,
			   orig->format->Gmask,
			   orig->format->Bmask,
			   orig->format->Amask);
  /* FIXME: Check for errors */

  if (orig->format->BytesPerPixel == 2)
    gp = getpixel_16;
  else if (orig->format->BytesPerPixel == 3)
    gp = getpixel_24;
  else if (orig->format->BytesPerPixel == 4)
    gp = getpixel_32;

  if (s->format->BytesPerPixel == 2)
    pp = putpixel_16;
  else if (s->format->BytesPerPixel == 3)
    pp = putpixel_24;
  else if (s->format->BytesPerPixel == 4)
    pp = putpixel_32;

  for (y = 0; y < new_h; y++)
  {
    for (x = 0; x < new_w; x++)
    {
      tr = 0;
      tg = 0;
      tb = 0;
      ta = 0;

      tmp = 0;

      for (src_y = y * yscale; src_y < y * yscale + yscale &&
	   src_y < f_orig_h; src_y += XY_FIXED_ONE)
      {
	for (src_x = x * xscale; src_x < x * xscale + xscale &&
	     src_x < f_orig_w; src_x += XY_FIXED_ONE)
	{
	  SDL_GetRGBA(gp(orig, src_x >> XY_FIXED_SHIFT, src_y >> XY_FIXED_SHIFT),
		      orig->format, &r, &g, &b, &a);

	  tr = tr + r;
	  tb = tb + b;
	  tg = tg + g;
	  ta = ta + a;

	  tmp++;
	}
      }

      if (tmp != 0)
      {
	tr = tr / tmp;
	tb = tb / tmp;
	tg = tg / tmp;
	ta = ta / tmp;

	pp(s, x, y, SDL_MapRGB(s->format,
				(Uint8) tr,
				(Uint8) tg,
				(Uint8) tb),
	  (Uint8) ta);
      }
    }
  }

  return(s);
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
  Uint8 alph = (color & 0xff);
  XY_fixed dx, dy;
  XY_fixed m, b;

  if ((color & 0xff) == 0)
    return; /* Fully transparent! */

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
            putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor, alph);
        }
        else
        {
          for (y = fsy2 >> XY_FIXED_SHIFT; y <= fsy1 >> XY_FIXED_SHIFT; y++)
            putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor, alph);
        }

        fsx1 = fsx1 + dx;
      }
    }
    else
    {
      if (sy2 > sy1)
      {
        for (y = sy1; y <= sy2; y++)
          putpixel(XY_screen, sx1, y, sdlcolor, alph);
      }
      else
      {
        for (y = sy2; y <= sy1; y++)
          putpixel(XY_screen, sx1, y, sdlcolor, alph);
      }
    }
  }
}

void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color)
{
  int sx, sy;
  Uint32 sdlcolor = XY_color_to_sdl_color(color);
  Uint8 alph = (color & 0xff);

  XY_canvas_to_screen(x, y, &sx, &sy);

  putpixel(XY_screen, sx, sy, sdlcolor, alph);
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

void putpixel_16(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 2;

    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h || alph == 0)
      return;

    *(Uint16 *)p = pixel;
}

void putpixel_24(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 3;

    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h || alph == 0)
      return;

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
    } else {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
    }
}

void putpixel_32(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;

    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h || alph == 0)
      return;

    *(Uint32 *)p = pixel;
}

void putpixel_fakea_16(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 2;
  Uint8 antialph;
  Uint8 r1, g1, b1;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    antialph = 255 - alph;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);

    r = ((int) (r1 * alph) + (int) (XY_background_r * antialph)) / 256;
    g = ((int) (g1 * alph) + (int) (XY_background_g * antialph)) / 256;
    b = ((int) (b1 * alph) + (int) (XY_background_b * antialph)) / 256;

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint16 *)p = pixel;
}

void putpixel_fakea_24(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 3;
  Uint8 antialph;
  Uint8 r1, g1, b1;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    antialph = 255 - alph;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);

    r = ((int) (r1 * alph) + (int) (XY_background_r * antialph)) / 256;
    g = ((int) (g1 * alph) + (int) (XY_background_g * antialph)) / 256;
    b = ((int) (b1 * alph) + (int) (XY_background_b * antialph)) / 256;

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
    p[0] = (pixel >> 16) & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = pixel & 0xff;
  } else {
    p[0] = pixel & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = (pixel >> 16) & 0xff;
  }
}

void putpixel_fakea_32(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
  Uint8 antialph;
  Uint8 r1, g1, b1;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    antialph = 255 - alph;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);

    r = ((int) (r1 * alph) + (int) (XY_background_r * antialph)) / 256;
    g = ((int) (g1 * alph) + (int) (XY_background_g * antialph)) / 256;
    b = ((int) (b1 * alph) + (int) (XY_background_b * antialph)) / 256;

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint32 *)p = pixel;
}

void putpixel_reala_16(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 2;
  Uint8 antialph;
  Uint8 r1, g1, b1, r2, g2, b2;
  Uint32 oldpixel;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    antialph = 255 - alph;

    oldpixel = *(Uint16 *)p;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);
    SDL_GetRGB(oldpixel, surface->format, &r2, &g2, &b2);

    r = ((int) (r1 * alph) + (int) (r2 * antialph)) / 256;
    g = ((int) (g1 * alph) + (int) (g2 * antialph)) / 256;
    b = ((int) (b1 * alph) + (int) (b2 * antialph)) / 256;

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint16 *)p = pixel;
}

void putpixel_reala_24(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 3;
  Uint8 antialph;
  Uint8 r1, g1, b1, r2, g2, b2;
  Uint32 oldpixel;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    antialph = 255 - alph;

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      oldpixel = (p[0] << 16) | (p[1] << 8) | p[2];
    } else {
      oldpixel = (p[2] << 16) | (p[1] << 8) | p[0];
    }

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);
    SDL_GetRGB(oldpixel, surface->format, &r2, &g2, &b2);

    r = ((int) (r1 * alph) + (int) (r2 * antialph)) / 256;
    g = ((int) (g1 * alph) + (int) (g2 * antialph)) / 256;
    b = ((int) (b1 * alph) + (int) (b2 * antialph)) / 256;

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
    p[0] = (pixel >> 16) & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = pixel & 0xff;
  } else {
    p[0] = pixel & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = (pixel >> 16) & 0xff;
  }
}

void putpixel_reala_32(SDL_Surface *surface, int x, int y, Uint32 pixel, Uint8 alph)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
  Uint8 antialph;
  Uint8 r1, g1, b1, r2, g2, b2;
  Uint8 r, g, b;
  Uint32 oldpixel;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    antialph = 255 - alph;

    oldpixel = *(Uint32 *)p;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);
    SDL_GetRGB(oldpixel, surface->format, &r2, &g2, &b2);

    r = ((int) (r1 * alph) + (int) (r2 * antialph)) / 256;
    g = ((int) (g1 * alph) + (int) (g2 * antialph)) / 256;
    b = ((int) (b1 * alph) + (int) (b2 * antialph)) / 256;

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint32 *)p = pixel;
}

/* Get a pixel: */
Uint32 getpixel_16(SDL_Surface * surface, int x, int y)
{
  Uint8 *p;

  /* get the X/Y values within the bounds of this surface */
  if ((unsigned) x > (unsigned) surface->w - 1u)
    x = (x < 0) ? 0 : surface->w - 1;
  if ((unsigned) y > (unsigned) surface->h - 1u)
    y = (y < 0) ? 0 : surface->h - 1;

  /* Set a pointer to the exact location in memory of the pixel
     in question: */

  p = (Uint8 *) (((Uint8 *) surface->pixels) +	/* Start at top of RAM */
		 (y * surface->pitch) +	/* Go down Y lines */
		 (x * 2));	/* Go in X pixels */


  /* Return the correctly-sized piece of data containing the
   * pixel's value (an 8-bit palette value, or a 16-, 24- or 32-bit
   * RGB value) */

  return (*(Uint16 *) p);
}

/* Get a pixel: */
Uint32 getpixel_24(SDL_Surface * surface, int x, int y)
{
  Uint8 *p;
  Uint32 pixel;

  /* get the X/Y values within the bounds of this surface */
  if ((unsigned) x > (unsigned) surface->w - 1u)
    x = (x < 0) ? 0 : surface->w - 1;
  if ((unsigned) y > (unsigned) surface->h - 1u)
    y = (y < 0) ? 0 : surface->h - 1;

  /* Set a pointer to the exact location in memory of the pixel
     in question: */

  p = (Uint8 *) (((Uint8 *) surface->pixels) +	/* Start at top of RAM */
		 (y * surface->pitch) +	/* Go down Y lines */
		 (x * 3));	/* Go in X pixels */


  /* Return the correctly-sized piece of data containing the
   * pixel's value (an 8-bit palette value, or a 16-, 24- or 32-bit
   * RGB value) */

  /* Depending on the byte-order, it could be stored RGB or BGR! */

  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    pixel = p[0] << 16 | p[1] << 8 | p[2];
  else
    pixel = p[0] | p[1] << 8 | p[2] << 16;

  return pixel;
}

/* Get a pixel: */
Uint32 getpixel_32(SDL_Surface * surface, int x, int y)
{
  Uint8 *p;

  /* get the X/Y values within the bounds of this surface */
  if ((unsigned) x > (unsigned) surface->w - 1u)
    x = (x < 0) ? 0 : surface->w - 1;
  if ((unsigned) y > (unsigned) surface->h - 1u)
    y = (y < 0) ? 0 : surface->h - 1;

  /* Set a pointer to the exact location in memory of the pixel
     in question: */

  p = (Uint8 *) (((Uint8 *) surface->pixels) +	/* Start at top of RAM */
		 (y * surface->pitch) +	/* Go down Y lines */
		 (x * 4));	/* Go in X pixels */


  /* Return the correctly-sized piece of data containing the
   * pixel's value (an 8-bit palette value, or a 16-, 24- or 32-bit
   * RGB value) */

  return *(Uint32 *) p;		// 32-bit display
}

/* FIXME: Implement Xiaolin Wu's antialiased line algorithm
   ( http://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm ) */

