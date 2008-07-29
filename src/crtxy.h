/*
  crtxy.h

  CRT X-Y library (libcrtxy)

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - July 29, 2008
*/

#include <SDL.h>

#define XY_OPT_SCALE_FAST 0
#define XY_OPT_SCALE_BEST 0

#define XY_INIT_IGNORE_ENVVARS 0x1
#define XY_INIT_IGNORE_LIB_CONFIG_FILE 0x2

#define XY_POS_TOP 0x0
#define XY_POS_LEFT 0x0
#define XY_POS_HCENTER 0x1
#define XY_POS_VCENTER 0x2
#define XY_POS_RIGHT 0x4
#define XY_POS_BOTTOM 0x8

#define XY_SCALE_NONE 0
#define XY_SCALE_STRETCH 1
#define XY_SCALE_KEEP_ASPECT_WIDE 2
#define XY_SCALE_KEEP_ASPECT_TALL 3

#define XY_FIXED_SHIFT 16

#define XY_FIXED_MAX (0x7FFFFF / 2) << XY_FIXED_SHIFT
#define XY_FIXED_MIN (-(XY_FIXED_MAX) - 1) << XY_FIXED_SHIFT

#define XY_FIXED_NAN XY_FIXED_MAX

#define XY_bool Uint8
#define XY_TRUE 1
#define XY_FALSE 0

#define XY_WINDOWED 0
#define XY_FULLSCREEN_REQUEST 1
#define XY_FULLSCREEN_REQUIRED 2

typedef struct XY_options_s {
  int displayw, displayh;
  int displaybpp;
  int fullscreen;
  XY_bool antialias;
  XY_bool blur;
  XY_bool additive;
  XY_bool backgrounds;
  int scaling;
} XY_options;

typedef struct XY_bitmap_s {
  SDL_Surface * surf;
} XY_bitmap;

typedef Uint32 XY_color;

typedef Sint32 XY_fixed;

int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash);
void XY_quit(void);

XY_bitmap * XY_load_bitmap(char * filename);
XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer);
void XY_free_bitmap(XY_bitmap * bitmap);

void XY_set_background(XY_color color, XY_bitmap * bitmap,
                       XY_fixed x, XY_fixed y, int posflags, int scaling);

XY_color XY_getcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void XY_enable_background(XY_bool enable);

void XY_start_frame(int fps);
int XY_end_frame(XY_bool throttle);

void XY_draw_line(XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                  XY_color color);
void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color);

#define XY_mult(a,b) (((a) * (b)) >> XY_FIXED_SHIFT)
#define XY_div(a,b) ((b) == 0 ? XY_FIXED_NAN : ((a) << XY_FIXED_SHIFT) / (b))


XY_fixed XY_sin(int degrees);
XY_fixed XY_cos(int degrees);

XY_fixed XY_screenx_to_canvasx(int sx);
XY_fixed XY_screeny_to_canvasy(int sy);
void XY_screen_to_canvas(int sx, int sy, XY_fixed * cx, XY_fixed * cy);

int XY_canvasx_to_screenx(XY_fixed cx);
int XY_canvasy_to_screeny(XY_fixed cy);
void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy);

int XY_get_screenw(void);
int XY_get_screenh(void);

