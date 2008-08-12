/*
  crtxy.h

  CRT X-Y library (libcrtxy)
  http://libcrtxy.sf.net/

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - August 12, 2008
*/

#ifndef _CRTXY_H
#define _CRTXY_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

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
#define XY_FIXED_HALF (1 << (XY_FIXED_SHIFT - 1)) /* Quick way to get '0.5' */

/* Limits for fixed-point values (for signed 32-bit ints) */
#define XY_FIXED_MAX 0x7FFFFFFF
#define XY_FIXED_MIN -(0x80000000)
#define XY_FIXED_NAN XY_FIXED_MAX /* Not-a-number (when you divide by zero */


/* --- Geometry types: --- */

/* Some standard thicknesses: */
#define XY_THIN XY_FIXED_ONE

/* Line type (struct): */
typedef struct XY_line_s {
  XY_fixed x1, y1;
  XY_fixed x2, y2;
  XY_color color;
  XY_fixed thickness;
} XY_line;

/* Multiple lines type (struct): */
typedef struct XY_lines_s {
  int count;
  int max;
  XY_line * lines;
} XY_lines;


/* --- Initialization and options flags and settings --- */

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

/* Quality settings for scaling bitmaps */
#define XY_OPT_SCALE_BEST 0 /* Blend to smooth any stretching */
#define XY_OPT_SCALE_FAST 1 /* Stretch pixels with no blending */

typedef struct XY_options_s {
  int displayw, displayh;  /* Size of window or fullscreen display */
  int displaybpp; /* Display depth (16bpp, 24bpp, 32bpp) */
  int fullscreen; /* Window, Want Fullscreen or Require Fullscreen? */
  int alpha;  /* Alpha-blend, fake it, or none at all (just on/off)? */
  XY_bool antialias;  /* Anti-alias lines (Xiaolin Wu) or not (Bresenham)? */
  XY_bool gamma_correction;  /* Gamma correction when anti-aliasing */ /* FIXME: Only doing 2.2 (close to sRGB) at the moment! */
  XY_bool blur;  /* Add blur effect? */
  XY_bool additive;  /* Additive pixel effect? */
  XY_bool backgrounds;  /* Support fullscreen background */
  int scaling;  /* Fast or Best scaling? */
} XY_options;

/* Where system-wide (global) and user's (local) config files live: */
/* FIXME: Set during build!!! */
#define XY_INIT_LIB_CONFIG_FILE_GLOBAL CONFDIR "/libcrtxy.conf"
#define XY_INIT_LIB_CONFIG_FILE_LOCAL ".libcrtxyrc"

/* Error constants: */
enum {
  XY_ERR_NONE, // No error
  XY_ERR_OPTION_BAD, // Bad value to an option
  XY_ERR_OPTION_UNKNOWN, // Unrecognized option
  XY_ERR_FILE_CANT_OPEN, // Cannot open file
  XY_ERR_MEM_CANT_ALLOC, // Cannot allocate memory
  XY_ERR_INIT_VIDEO, // Cannot initialize video subsystem
  XY_ERR_INIT_DISPLAY, // Cannot open display
  XY_ERR_INIT_UNSUPPORTED_BPP, // Unsupported (so far as we're concerned) bpp
  XY_ERR_BITMAP_CANT_DECODE, // Error decoding image file (IMG_Load failed)
  XY_ERR_BITMAP_CANT_CONVERT, // Cannot convert a surface to the display fmt.
  XY_ERR_BITMAP_CANT_SCALE, // Cannot scale an image (probably mem. alloc. fail)
  NUM_XY_ERRS
};

/* Line intersection results: */
enum {
  XY_INTERSECTION_NONE,
  XY_INTERSECTION_INTERSECTING,
  XY_INTERSECTION_PARALLEL,
  XY_INTERSECTION_COINCIDENT
};


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

/* - Options: - */

/* Set opts to default (libcrtxy's compiled-time) options */
void XY_default_options(XY_options * opts);

/* Load global, then local (user) libcrtxy config files into opts */
XY_bool XY_load_options(XY_options * opts);

/* Load arbitrary config file into opts */
XY_bool XY_load_options_from_file(char * fname, XY_options * opts,
                                  XY_bool ignore_unknowns);

/* Parse libcrtxy-related arguments into opts */
int XY_parse_options(int * argc, char * argv[], XY_options * opts);

/* Read any libcrtxy-related environment variables into opts */
XY_bool XY_parse_envvars(XY_options * opts);


/* - Init and quit: - */

/* Initialize SDL (video only) and libcrtxy (with rendering and video options
   from opts), and set the virtual canvas size */
XY_bool XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash);

/* Shut down libcrtxy and SDL */
void XY_quit(void);


/* - Errors: - */

/* Gets the most recent error code */
int XY_errcode(void);

/* Gets a string representing the most recent error code */
const char * XY_errstr(void);

/* Dumps options in opts to the file stream (eg, stderr or an opened logfile) */
void XY_print_options(FILE * fi, XY_options opts);


/* - Load and free bitmap: - */

/* Create a bitmap based on an image file */
XY_bitmap * XY_load_bitmap(char * filename);

/* Create a bitmap based on image data in a buffer */
XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer, int size);

/* Free a bitmap */
void XY_free_bitmap(XY_bitmap * bitmap);


/* - Setting background: - */

/* Set the background color, and optional bitmap, its position, and
   options for scaling it to the screen size.  Enables background bitmap. */
/* For posflags, combine (by or'ing together with the "|" operator), one
   horizontal choice (XY_POS_LEFT, XY_POS_HCENTER or XY_POS_RIGHT) with one
   vertical choice (XY_POS_TOP, XY_POS_VCENTER or XY_POS_BOTTOM).
   A value of 0 also represents top-left. */
/* x,y determine how far, in terms of canvas virtual world units (fixed point),
   to nudge the image after it's been positoned */
/* For scaling, use one of the following:
   XY_SCALE_NONE, XY_SCALE_STRETCH, XY_SCALE_KEEP_ASPECT_WIDE or
   XY_SCALE_KEEP_ASPECT_TALL */
XY_bool XY_set_background(XY_color color, XY_bitmap * bitmap,
                          XY_fixed x, XY_fixed y, int posflags, int scaling);

/* Enable or disable the background bitmap (affects next frame) */
void XY_enable_background(XY_bool enable);


/* - Colors: - */

/* Combines R, G, B and A values into an XY_color */
XY_color XY_setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Breaks an XY_color into its R, G, B and A values */
void XY_getcolor(XY_color c, Uint8 * r, Uint8 * g, Uint8 * b, Uint8 * a);


/* - Starting and ending a drawing frame: - */

/* Mark the start of a frame. Screen backbuffer is wiped to the background
   color and/or bitmap.  Allows setting of preferred FPS. */
void XY_start_frame(int fps);

/* Mark the end of a frame. Screen backbuffer is made visible.
   Optionally, pause until (1000/fps) miliseconds have passed since
   XY_start_frame() was called. (If 'throttle' is set to XY_TRUE.) */
int XY_end_frame(XY_bool throttle);


/* - Line collection manipulation - */

/* Create a collection: */
XY_lines * XY_new_lines(void);

/* Free a collection: */
void XY_free_lines(XY_lines * lines);

/* Start/reset a collection: */
void XY_start_lines(XY_lines * lines);

/* Add a new line: */
XY_bool XY_add_line(XY_lines * lines,
                    XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                    XY_color color, XY_fixed thickness);


/* - Drawing primitives: - */

/* Draw a line between (x1,y1) and (x2,y2) (in canvas virtual world units)
   in the specified color/alpha */
void XY_draw_line(XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                  XY_color color, XY_fixed thickness);

/* Draw a collection of lines */
void XY_draw_lines(XY_lines * lines);

/* Draw a line using an XY_line struct */
void XY_draw_one_line(XY_line line);

/* Draw a point at (x,y) (in canvas virtual world units)
   in the specified color/alpha */
void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color, XY_fixed thickness);


/* - Fixed-point math functions: - */

/* Multiply a * b */
#define XY_mult(a,b) (((a) >> XY_FIXED_SHIFT_HALF) * \
		      ((b) >> XY_FIXED_SHIFT_HALF))

/* Divide a / b.  XY_div makes sure b is not 0, returns Not-A-Number if so. */
#define XY_qdiv(a,b) (((a) / \
		       ((b) >> XY_FIXED_SHIFT_HALF)) \
                      << XY_FIXED_SHIFT_HALF)
#define XY_div(a,b) (((b) >> XY_FIXED_SHIFT_HALF) == 0 ? \
                     XY_FIXED_NAN : XY_qdiv((a),(b)))

#define XY_FIXED_DIV_ZERO ((1 << XY_FIXED_SHIFT_HALF) - 1)

/* Returns the fractional part of 'a'.  XY_rfpart returns one minus that. */
#define XY_fpart(a) ((a) & (XY_FIXED_ONE - 1))
#define XY_rfpart(a) (XY_FIXED_ONE - XY_fpart(a))

/* Returns the integer (whole) part of 'a' */
#define XY_ipart(a) ((a) - XY_fpart(a))

/* Rounds 'a' up to the nearest integer (whole). */
#define XY_round(a) (XY_ipart((a) + (1 << (XY_FIXED_SHIFT - 1))))

/* Returns cosine() and sine() of 'degrees'. ('degrees' is a non-fixed-point
   value, in degrees.  Values outside 0-359 are accounted for.) */
XY_fixed XY_cos(int degrees);
#define XY_sin(degrees) (XY_cos(90 - (degrees)))


/* - Screen/canvas conversions and queries: - */

/* Convert a screen coordinate (an integer; eg, where the mouse was clicked)
   into canvas virtual world units (fixed point) */
XY_fixed XY_screenx_to_canvasx(int sx);
XY_fixed XY_screeny_to_canvasy(int sy);
void XY_screen_to_canvas(int sx, int sy, XY_fixed * cx, XY_fixed * cy);

/* Convert a canvas virtual world coordinate (fixed point) into the
   nearest screen coordinate (an integer) */
XY_fixed XY_canvasx_to_screenx(int cx);
XY_fixed XY_canvasy_to_screeny(int cy);
void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy);

/* Returns the screen's width and height, in pixels (integer) */
int XY_get_screenw(void);
int XY_get_screenh(void);

/* Returns whether two lines intersect; optionally (if not NULL),
   return the (x,y) coordinates of the intersection (if possible), and
   the intersection result (parallel, coincident, not intersecting,
   intersecting) */
XY_bool XY_lines_intersect(XY_line line1, XY_line line2,
                           XY_fixed * intersect_x, XY_fixed * intersect_y,
                           int * result);

/* Returns whether any lines in one group intersect lines in another */
XY_bool XY_line_groups_intersect(XY_lines * lines1, XY_lines * lines2);


#ifdef __cplusplus
}
#endif

#endif /* _CRTXY_H */
