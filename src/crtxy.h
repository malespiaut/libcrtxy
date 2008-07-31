/*
  crtxy.h

  CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 31, 2008
*/

#ifndef _CRTXY_H
#define _CRTXY_H

#include <SDL.h>

/* --- General types --- */

/* Boolean */
typedef enum {
  XY_FALSE,
  XY_TRUE
} XY_bool;

/* Bitmap type (struct): */
typedef struct XY_bitmap_s {
  SDL_Surface * surf;
} XY_bitmap;

/* Color type: */
typedef Uint32 XY_color;


/* --- Fixed-point type --- */

/* Fixed type: */
typedef Sint32 XY_fixed;

/* Fixed-point math settings  */
#define XY_FIXED_SHIFT 16 /* How much to shift ints to get a fixed-point val */
#define XY_FIXED_SHIFT_HALF 8 /* For half-shift during mult & divide macros */
#define XY_FIXED_ONE (1 << XY_FIXED_SHIFT) /* Quick way to get '1' in fixed */

/* Limits for fixed-point values */
#define XY_FIXED_MAX (0x7FFFFF << XY_FIXED_SHIFT)
#define XY_FIXED_MIN ((-(XY_FIXED_MAX) - 1) << XY_FIXED_SHIFT)
#define XY_FIXED_NAN XY_FIXED_MAX /* Not-a-number (when you divide by zero */


/* --- Initialization and options flags and settings --- */

/* Flags for initialization override options: */
/* NOTE: Can be OR'ed together */
#define XY_INIT_IGNORE_ENVVARS 0x1 /* Ignore environment variables */
#define XY_INIT_IGNORE_LIB_CONFIG_FILES 0x2 /* Ignore settings file(s) */

/* Display settings */
/* (options.fullscreen) */
#define XY_OPT_WINDOWED 0 /* Display in a window */
#define XY_OPT_FULLSCREEN_REQUEST 1 /* Fullscreen; fall back to window */
#define XY_OPT_FULLSCREEN_REQUIRED 2 /* Fullscreen; abort if we cannot */

/* Kinds of alpha-blending we want: */
/* (options.alpha) */
#define XY_OPT_ALPHA_BLEND 0 /* Combine current pixel with new pixel */
#define XY_OPT_ALPHA_FAKE 1 /* Combine background color with new pixel */
#define XY_OPT_ALPHA_OFF 2 /* Just draw new pixel */

/* Kinds of anti-aliasing we want: */
/* (options.antialias) */
#define XY_OPT_ANTIALIAS_BLEND 0 /* Combine current pixel with aa'd pixel */
#define XY_OPT_ANTIALIAS_FAKE 1 /* Combine background color with aa'd pixel */
#define XY_OPT_ANTIALIAS_OFF 2 /* No anti-aliasing */

/* Quality settings for scaling bitmaps */
#define XY_OPT_SCALE_BEST 0 /* Blend to smooth any stretching */
#define XY_OPT_SCALE_FAST 1 /* Stretch pixels with no blending */

typedef struct XY_options_s {
  int displayw, displayh;  // Size of window or fullscreen display
  int displaybpp; // Display depth (16bpp, 24bpp, 32bpp)
  int fullscreen; // Window, Want Fullscreen or Require Fullscreen?
  int alpha;  // Alpha-blend, fake it, or none at all (just on/off)?
  int antialias;  // Anti-alias lines, fake it, or no antialiasing?
  XY_bool blur;  // Add blur effect?
  XY_bool additive;  // Additive pixel effect?
  XY_bool backgrounds;  // Support fullscreen background
  int scaling;  // Fast or Best scaling?
} XY_options;

/* Where system-wide (global) and user's (local) config files live: */
/* FIXME: Set during build!!! */
#define XY_INIT_LIB_CONFIG_FILE_GLOBAL "/usr/local/share/libcrtxy/libcrtxy.conf"
#define XY_INIT_LIB_CONFIG_FILE_LOCAL "~/.libcrtxyrc"


/* --- Runtime flags and options --- */

/* Flags for how to position backgrounds on the display */
/* NOTE: Can be OR'ed together */
#define XY_POS_TOP 0x0 /* Position background at the top (default) */
#define XY_POS_LEFT 0x0 /* Position background on the left (default) */
#define XY_POS_HCENTER 0x1 /* Horizontally center background */
#define XY_POS_VCENTER 0x2 /* Vertically center background */
#define XY_POS_RIGHT 0x4 /* Position background at the right */
#define XY_POS_BOTTOM 0x8 /* Position background at the bottom */

/* Settings for how to scale bitmaps that don't match screen/window size: */
#define XY_SCALE_NONE 0 /* Do not stretch; clip or show solid border(s) */
#define XY_SCALE_STRETCH 1 /* Stretch; aspect ratio can be altered */
#define XY_SCALE_KEEP_ASPECT_WIDE 2 /* Stretch w/o alt'ing aspect; fit width */
#define XY_SCALE_KEEP_ASPECT_TALL 3 /* Stretch w/o alt'ing aspect; fit height */


/* --- Public function prototypes --- */

/* Init and quit: */
int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash);
void XY_quit(void);

/* Load and free bitmap: */
XY_bitmap * XY_load_bitmap(char * filename);
XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer);
void XY_free_bitmap(XY_bitmap * bitmap);

/* Setting background: */

void XY_set_background(XY_color color, XY_bitmap * bitmap,
                       XY_fixed x, XY_fixed y, int posflags, int scaling);
void XY_enable_background(XY_bool enable);


/* Colors: */

XY_color XY_setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void XY_getcolor(XY_color c, Uint8 * r, Uint8 * g, Uint8 * b, Uint8 * a);


/* Starting and ending a drawing frame: */

void XY_start_frame(int fps);
int XY_end_frame(XY_bool throttle);


/* Drawing primitives: */

void XY_draw_line(XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                  XY_color color);
void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color);


/* Fixed-point math functions: */

#define XY_mult(a,b) (((a) >> XY_FIXED_SHIFT_HALF) * \
		      ((b) >> XY_FIXED_SHIFT_HALF))
#define XY_qdiv(a,b) (((a) / \
		       ((b) >> XY_FIXED_SHIFT_HALF)) \
                      << XY_FIXED_SHIFT_HALF)
#define XY_div(a,b) ((b) == 0 ? XY_FIXED_NAN : XY_qdiv((a),(b)))

XY_fixed XY_cos(int degrees);
#define XY_sin(degrees) (XY_cos(90 - (degrees)))


/* Screen/canvas conversions and queries: */

XY_fixed XY_screenx_to_canvasx(int sx);
XY_fixed XY_screeny_to_canvasy(int sy);
void XY_screen_to_canvas(int sx, int sy, XY_fixed * cx, XY_fixed * cy);

int XY_canvasx_to_screenx(int cx);
int XY_canvasy_to_screeny(int cy);
void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy);

int XY_get_screenw(void);
int XY_get_screenh(void);

#endif /* _CRTXY_H */
