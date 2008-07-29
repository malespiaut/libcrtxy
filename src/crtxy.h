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

typedef struct XY_options_s {
  int displayw, displayh;
  XY_bool antialias;
  XY_bool blur;
  XY_bool additive;
  XY_bool backgrounds;
  int scaling;
} XY_options;

typedef Uint32 XY_color;

typedef Sint32 XY_fixed;

int XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash);

