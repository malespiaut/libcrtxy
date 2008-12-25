/*
  crtxy.c

  CRT X-Y library (libcrtxy)
  http://libcrtxy.sf.net/

  Bill Kendrick <bill@newbreedsoftware.com>

  July 29, 2008 - December 25, 2008
*/

#include "crtxy.h"
#include <SDL_image.h>

#include "gamma_2_2.h"

#define XY_DIRTY_RECT_STEP 128

SDL_Surface * XY_screen;
XY_fixed XY_canvasw, XY_canvash;
Uint32 XY_background_color;
Uint8 XY_background_r, XY_background_g, XY_background_b;
XY_bitmap * XY_background_bitmap;
XY_bool XY_background_bitmap_possible, XY_antialias, XY_gamma_correction;
XY_bool XY_background_bitmap_enabled;
SDL_Rect XY_background_dest;
Uint32 XY_want_fps, XY_start_time;
XY_err XY_err_code;
SDL_Rect * XY_dirty_rects, * XY_dirty_rects_erasure, * XY_dirty_rects_all;
int XY_dirty_rect_count, XY_dirty_rect_erasure_count;
int XY_dirty_rect_max;
XY_bool XY_background_change;

void (*putpixel) (SDL_Surface *, int, int, Uint32, XY_fixed alph,
                  Uint16 * gamma_s2l, Uint8 * gamma_l2s);
Uint32 (*getpixel) (SDL_Surface * surface, int x, int y);
SDL_Surface * (*scale_surf) (SDL_Surface * orig, int new_w, int new_h);

const int XY_trig[91] = {
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

const char * XY_errstr_txt[NUM_XY_ERRS] = {
  "",
  "Bad argument to option",
  "Unknown option",
  "Can't open file",
  "Can't allocate memory",
  "Can't initialize SDL video",
  "Can't open display",
  "Unsupported display depth (bpp)",
  "Can't decode bitmap",
  "Can't convert surface",
  "Can't scale surface"
};


/* Private functions and macros: */

void blend(Uint8 * dest_r, Uint8 * dest_g, Uint8 * dest_b,
           Uint8 src1_r, Uint8 src1_g, Uint8 src1_b,
           Uint8 src2_r, Uint8 src2_g, Uint8 src2_b,
           XY_fixed alpha,
           Uint16 * gamma_s2l, Uint8 * gamma_l2s);

void putpixel_16(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void putpixel_24(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void putpixel_32(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);

void putpixel_fakea_16(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void putpixel_fakea_24(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void putpixel_fakea_32(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);

void putpixel_reala_16(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void putpixel_reala_24(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void putpixel_reala_32(SDL_Surface * surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);

Uint32 getpixel_16(SDL_Surface * surface, int x, int y);
Uint32 getpixel_24(SDL_Surface * surface, int x, int y);
Uint32 getpixel_32(SDL_Surface * surface, int x, int y);

#define XY_color_to_sdl_color(color) SDL_MapRGB(XY_screen->format, \
                                       ((color) >> 24) & 0xFF, \
                                       ((color) >> 16) & 0xFF, \
                                       ((color) >> 8) & 0xFF)

SDL_Surface * scale_surf_best(SDL_Surface * orig, int new_w, int new_h);
SDL_Surface * scale_surf_fast(SDL_Surface * orig, int new_w, int new_h);

void XY_draw_line_xiaolinwu(XY_fixed fsx1, XY_fixed fsy1,
                            XY_fixed fsx2, XY_fixed fsy2,
                            XY_color color,
                            Uint16 * gamma_s2l, Uint8 * gamma_l2s);
void XY_draw_line_bresenham(XY_fixed fsx1, XY_fixed fsy1,
                            XY_fixed fsx2, XY_fixed fsy2,
                            XY_color);

int XY_grab_envvar(char * v, int * i, int * o, char * s);
void XY_complain_envvar(char * v, char * okay);

void XY_add_dirty_rect(int x1, int y1, int x2, int y2);
void XY_merge_dirty_rects(SDL_Rect * rects, int * cnt);
XY_bool XY_rects_intersect(SDL_Rect * rects, int r1, int r2);
void XY_rects_combine(SDL_Rect * rects, int r1, int r2);


/* Public functions: */

void XY_default_options(XY_options * opts)
{
  opts->displayw = 320;
  opts->displayh = 240;
  opts->displaybpp = 16;
  opts->fullscreen = XY_OPT_WINDOWED;
  opts->alpha = XY_OPT_ALPHA_BLEND;
  opts->antialias = XY_TRUE;
  opts->gamma_correction = XY_TRUE;
  opts->blur = XY_FALSE;
  opts->additive = XY_FALSE;
  opts->backgrounds = XY_TRUE;
  opts->scaling = XY_OPT_SCALE_FAST;
}

int XY_grab_envvar(char * v, int * i, int * o, char * s)
{
  if (getenv(v) != NULL)
  {
    s = getenv(v);
    *i = atoi(s);
    if (strcmp(s, "ON") == 0 || strcmp(s, "on") == 0)
      *o = 1;
    else if (strcmp(s, "OFF") == 0 || strcmp(s, "off") == 0)
      *o = 0;
    else
      *o = -1;

    return(1);
  }
  else
  {
    s = "";
    *i = 0;
    *o = -1;

    return(0);
  }
}

void XY_complain_envvar(char * v, char * okay)
{
  fprintf(stderr, "Error: Bad libcrtxy environment variable setting: %s=%s\n",
          v, getenv(v));

  if (okay != NULL)
    fprintf(stderr, "  %s must be one of: %s\n", v, okay);

  XY_err_code = XY_ERR_OPTION_BAD;
}


XY_bool XY_parse_envvars(XY_options * opts)
{
  int nextint, nexton;
  char * nextstr = "";

  XY_err_code = XY_ERR_NONE;

  if (XY_grab_envvar("CRTXY_WIDTH", &nextint, &nexton, nextstr))
  {
    if (nextint != 0)
      opts->displayw = nextint;
    else
    {
      XY_complain_envvar("CRTXY_WIDTH", NULL);
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_HEIGHT", &nextint, &nexton, nextstr))
  {
    if (nextint != 0)
      opts->displayh = nextint;
    else
    {
      XY_complain_envvar("CRTXY_HEIGHT", NULL);
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_BPP", &nextint, &nexton, nextstr))
  {
    if (nextint == 16 || nextint == 24 || nextint == 32)
      opts->displaybpp = nextint;
    else if (strcmp(nextstr, "ANY") == 0 || strcmp(nextstr, "any") == 0)
      opts->displaybpp = 0;
    else
    {
      XY_complain_envvar("CRTXY_BPP", "16|24|32|ANY");
      return(XY_FALSE);
    }
  }
  
  if (XY_grab_envvar("CRTXY_FULLSCREEN", &nextint, &nexton, nextstr))
  {
    if (strcmp(nextstr, "ON") == 0 || strcmp(nextstr, "on") == 0)
      opts->fullscreen = XY_OPT_FULLSCREEN_REQUIRED;
    else if (strcmp(nextstr, "OPTIONAL") == 0 || strcmp(nextstr, "optional") == 0)
      opts->alpha = XY_OPT_FULLSCREEN_REQUEST;
    else if (strcmp(nextstr, "OFF") == 0 || strcmp(nextstr, "off") == 0)
      opts->alpha = XY_OPT_WINDOWED;
  }

  if (XY_grab_envvar("CRTXY_ALPHA", &nextint, &nexton, nextstr))
  {
    if (strcmp(nextstr, "ON") == 0 || strcmp(nextstr, "on") == 0)
      opts->alpha = XY_OPT_ALPHA_BLEND;
    else if (strcmp(nextstr, "FAKE") == 0 || strcmp(nextstr, "fake") == 0)
      opts->alpha = XY_OPT_ALPHA_FAKE;
    else if (strcmp(nextstr, "OFF") == 0 || strcmp(nextstr, "off") == 0)
      opts->alpha = XY_OPT_ALPHA_OFF;
    else
    {
      XY_complain_envvar("CRTXY_ALPHA", "ON|FAKE|OFF");
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_GAMMA_CORRECTION", &nextint, &nexton, nextstr))
  {
    if (nexton != -1)
      opts->gamma_correction = (nexton == 1);
    else
    {
      XY_complain_envvar("CRTXY_GAMMA_CORRECTION", "ON|OFF");
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_ANTIALIAS", &nextint, &nexton, nextstr))
  {
    if (nexton != -1)
      opts->antialias = (nexton == 1);
    else
    {
      XY_complain_envvar("CRTXY_ANTIALIAS", "ON|OFF");
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_BLUR", &nextint, &nexton, nextstr))
  {
    if (nexton != -1)
      opts->blur = (nexton == 1);
    else
    {
      XY_complain_envvar("CRTXY_BLUR", "ON|OFF");
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_ADDITIVE", &nextint, &nexton, nextstr))
  {
    if (nexton != -1)
      opts->additive = (nexton == 1);
    else
    {
      XY_complain_envvar("CRTXY_ADDITIVE", "ON|OFF");
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_BACKGROUNDS", &nextint, &nexton, nextstr))
  {
    if (nexton != -1)
      opts->backgrounds = (nexton == 1);
    else
    {
      XY_complain_envvar("CRTXY_BACKGROUNDS", "ON|OFF");
      return(XY_FALSE);
    }
  }

  if (XY_grab_envvar("CRTXY_SCALING", &nextint, &nexton, nextstr))
  {
    if (strcmp(nextstr, "BEST") == 0 || strcmp(nextstr, "best") == 0)
      opts->scaling = XY_OPT_SCALE_BEST;
    else if (strcmp(nextstr, "FAST") == 0 || strcmp(nextstr, "fast") == 0)
      opts->scaling = XY_OPT_SCALE_FAST;
    else
    {
      XY_complain_envvar("CRTXY_SCALING", "BEST|FAST");
      return(XY_FALSE);
    }
  }

  return(XY_TRUE);
}

int XY_parse_options(int * argc, char * argv[], XY_options * opts)
{
  int i, j, eat, err_arg;
  int nextint;
  char * nextstr;

  XY_err_code = XY_ERR_NONE;
  err_arg = 0;

  for (i = 0; i < *argc && XY_err_code == XY_ERR_NONE; i++)
  {
    eat = 0;

    if (strcmp(argv[i], "--") == 0)
    {
      /* Double-dash says "end of options", so break out */
      i = *argc;
    }
    else
    {
      if (i < (*argc - 1))
      {
        nextint = atoi(argv[i + 1]);
        nextstr = argv[i + 1];
      }
      else
      {
        nextint = 0;
        nextstr = "";
      }

      if (strcmp(argv[i], "--help-crtxy") == 0)
      {
        printf("\n");
        printf("%s [options] ...\n", argv[0]);
        printf("\n");
        printf("Options specific to libcrtxy drawing library:\n");
        printf("  --crtxy-width WIDTH         Set screen width (pixels)\n");
        printf("  --crtxy-height HEIGHT       Set screen height (pixels)\n");
        printf("  --crtxy-bpp [16|24|32|any]  Set depth (bits per pixel)\n");
        printf("  --crtxy-fullscreen          Fullscreen mode required\n");
        printf("  --crtxy-fullscreen-or-window  Fullscreen mode; fallback to windowed\n");
        printf("  --crtxy-windowed            Windowed mode\n");
        printf("  --crtxy-alpha [on|off|fake] Alpha-blending ('fake' is against bgkd color\n");
        printf("  --crtxy-antialias [on|off]  Anti-aliased (smoother) lines\n");
        printf("  --crtxy-gamma-correction [on|off]  Gamma-corrected anti-aliased lines\n");
        printf("  --crtxy-blur [on|off]       Blur all lines effect\n");
        printf("  --crtxy-additive [on|off]   Brighten lines that cross\n");
        printf("  --crtxy-backgrounds [on|off]  Enable or disable background bitmaps\n");
        printf("  --crtxy-scaling [best|fast] Set bitmap scaling quality\n");
        printf("\n");
        
        exit(0);
      }
      else if (strcmp(argv[i], "--crtxy-width") == 0)
      {
        if (nextint != 0)
        {
          opts->displayw = nextint;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-height") == 0)
      {
        if (nextint != 0)
        {
          opts->displayh = nextint;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-bpp") == 0)
      {
        if (nextint == 16 || nextint == 24 || nextint == 32)
        {
          opts->displaybpp = nextint;
          eat = 2;
        }
        else if (strcmp(nextstr, "any") == 0)
        {
          opts->displaybpp = 0;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-fullscreen") == 0)
      {
        opts->fullscreen = XY_OPT_FULLSCREEN_REQUIRED;
        eat = 1;
      }
      else if (strcmp(argv[i], "--crtxy-fullscreen-or-window") == 0)
      {
        opts->fullscreen = XY_OPT_FULLSCREEN_REQUEST;
        eat = 1;
      }
      else if (strcmp(argv[i], "--crtxy-windowed") == 0)
      {
        opts->fullscreen = XY_OPT_WINDOWED;
        eat = 1;
      }
      else if (strcmp(argv[i], "--crtxy-alpha") == 0)
      {
        if (strcmp(nextstr, "on") == 0)
        {
          opts->alpha = XY_OPT_ALPHA_BLEND;
          eat = 2;
        }
        else if (strcmp(nextstr, "fake") == 0)
        {
          opts->alpha = XY_OPT_ALPHA_FAKE;
          eat = 2;
        }
        else if (strcmp(nextstr, "off") == 0)
        {
          opts->alpha = XY_OPT_ALPHA_OFF;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-antialias") == 0)
      {
        if (strcmp(nextstr, "on") == 0)
        {
          opts->antialias = XY_TRUE;
          eat = 2;
        }
        else if (strcmp(nextstr, "off") == 0)
        {
          opts->antialias = XY_FALSE;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-gamma-correction") == 0)
      {
        if (strcmp(nextstr, "on") == 0)
        {
          opts->gamma_correction = XY_TRUE;
          eat = 2;
        }
        else if (strcmp(nextstr, "off") == 0)
        {
          opts->gamma_correction = XY_FALSE;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-blur") == 0)
      {
        if (strcmp(nextstr, "on") == 0)
        {
          opts->blur = XY_TRUE;
          eat = 2;
        }
        else if (strcmp(nextstr, "off") == 0)
        {
          opts->blur = XY_FALSE;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-additive") == 0)
      {
        if (strcmp(nextstr, "on") == 0)
        {
          opts->additive = XY_TRUE;
          eat = 2;
        }
        else if (strcmp(nextstr, "off") == 0)
        {
          opts->additive = XY_FALSE;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-backgrounds") == 0)
      {
        if (strcmp(nextstr, "on") == 0)
        {
          opts->backgrounds = XY_TRUE;
          eat = 2;
        }
        else if (strcmp(nextstr, "off") == 0)
        {
          opts->backgrounds = XY_FALSE;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strcmp(argv[i], "--crtxy-scaling") == 0)
      {
        if (strcmp(nextstr, "best") == 0)
        {
          opts->scaling = XY_OPT_SCALE_BEST;
          eat = 2;
        }
        else if (strcmp(nextstr, "fast") == 0)
        {
          opts->scaling = XY_OPT_SCALE_FAST;
          eat = 2;
        }
        else
        {
          XY_err_code = XY_ERR_OPTION_BAD;
          err_arg = i;
        }
      }
      else if (strstr(argv[i], "--crtxy") == argv[i])
      {
        XY_err_code = XY_ERR_OPTION_UNKNOWN;
        err_arg = i;
      }
    }

    if (eat > 0 && XY_err_code == XY_ERR_NONE)
    {
      for (j = i; j + eat < *argc; j++)
        argv[j] = argv[j + eat];

      *argc -= eat;
      i--;
    }
  }

  return(err_arg);
}

XY_bool XY_load_options(XY_options * opts)
{
  XY_bool res;
  char * fname;

  res = XY_load_options_from_file(XY_INIT_LIB_CONFIG_FILE_GLOBAL,
                                  opts, XY_FALSE);
  if (res == XY_FALSE && XY_err_code != XY_ERR_FILE_CANT_OPEN)
    return(XY_FALSE);

  if (getenv("HOME") != NULL)
  {
    fname = (char *) malloc(strlen(getenv("HOME")) +
                            strlen(XY_INIT_LIB_CONFIG_FILE_LOCAL) + 2);
    if (fname == NULL)
    {
      XY_err_code = XY_ERR_MEM_CANT_ALLOC;
      return(XY_FALSE);
    }

    sprintf(fname, "%s/%s", getenv("HOME"), XY_INIT_LIB_CONFIG_FILE_LOCAL);

    res = XY_load_options_from_file(fname, opts, XY_FALSE);
    if (res == XY_FALSE && XY_err_code != XY_ERR_FILE_CANT_OPEN)
      return(XY_FALSE);
  }

  return(XY_TRUE);
}

XY_bool XY_load_options_from_file(char * fname, XY_options * opts,
                                  XY_bool ignore_unknowns)
{
  FILE * fi;
  char line[256];
  int i, err;
  int nextint, nexton;
  char * nextstr;
  char * res;

  XY_err_code = XY_ERR_NONE;

  fi = fopen(fname, "r");
  if (fi == NULL)
  {
    XY_err_code = XY_ERR_FILE_CANT_OPEN;
    return(XY_FALSE);
  }

  i = 0;
  while (!feof(fi) && err == XY_ERR_NONE)
  {
    res = fgets(line, sizeof(line), fi);
    if (feof(fi))
      break;
    if (strlen(line) > 0)
      line[strlen(line) - 1] = '\0';
    i++;

    if (strchr(line, '=') != NULL)
    {
      nextstr = (char *) (strchr(line, '=') + 1);
      nextint = atoi(nextstr);
      if (strcmp(nextstr, "on") == 0)
        nexton = 1;
      else if (strcmp(nextstr, "off") == 0)
        nexton = 0;
      else
        nexton = -1;
    }
    else
    {
      nextint = 0;
      nextstr = "";
    }


    if (line[0] == '#' || line[0] == '\0')
      ; /* Comment or blank; ignore this line */
    else if (strstr(line, "crtxy-width=") == line)
    {
      if (nextint != 0)
        opts->displayw = nextint;
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-height=") == line)
    {
      if (nextint != 0)
        opts->displayh = nextint;
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-bpp=") == line)
    {
      if (nextint == 16 || nextint == 24 || nextint == 32)
        opts->displaybpp = nextint;
      else if (strcmp(nextstr, "any") == 0)
        opts->displaybpp = 0;
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strcmp(line, "crtxy-windowed") == 0)
    {
      opts->fullscreen = XY_OPT_WINDOWED;
    }
    else if (strcmp(line, "crtxy-fullscreen") == 0)
    {
      opts->fullscreen = XY_OPT_FULLSCREEN_REQUIRED;
    }
    else if (strcmp(line, "crtxy-fullscreen-or-window") == 0)
    {
      opts->fullscreen = XY_OPT_FULLSCREEN_REQUEST;
    }
    else if (strstr(line, "crtxy-alpha=") == line)
    {
      if (nexton != -1)
        opts->alpha = (nexton ? XY_OPT_ALPHA_BLEND : XY_OPT_ALPHA_OFF);
      else if (strcmp(nextstr, "fake") == 0)
        opts->alpha = XY_OPT_ALPHA_FAKE;
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-antialias=") == line)
    {
      if (nexton != -1)
        opts->antialias = (nexton == 1);
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-gamma-correction=") == line)
    {
      if (nexton != -1)
        opts->gamma_correction = (nexton == 1);
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-blur=") == line)
    {
      if (nexton != -1)
        opts->blur = (nexton == 1);
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-additive=") == line)
    {
      if (nexton != -1)
        opts->additive = (nexton == 1);
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-backgrounds=") == line)
    {
      if (nexton != -1)
        opts->backgrounds = (nexton == 1);
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else if (strstr(line, "crtxy-scaling=") == line)
    {
      if (strcmp(nextstr, "best") == 0)
        opts->scaling = XY_OPT_SCALE_BEST;
      else if (strcmp(nextstr, "fast") == 0)
        opts->scaling = XY_OPT_SCALE_FAST;
      else
        XY_err_code = XY_ERR_OPTION_BAD;
    }
    else
    {
      if (ignore_unknowns == XY_FALSE)
      {
        fprintf(stderr, "Error: Unknown libcrtxy option, %s line %d: %s\n", fname, i, line);
        err = XY_ERR_OPTION_UNKNOWN;
      }
    }
  }
  fclose(fi);

  return(XY_err_code == XY_ERR_NONE);
}

XY_bool XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash)
{
  int Bpp;

  XY_err_code = XY_ERR_NONE;

  if (SDL_WasInit(0) != 0)
  {
    /* Caller initialized SDL before calling XY_init() */

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
      /* Caller did not yet initialize video, so we must */

      if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
      {
        XY_err_code = XY_ERR_INIT_VIDEO;
        return(XY_FALSE);
      }
    }
  }
  else
  {
    /* None of SDL was init yet, so call SDL_Init() directly: */

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      XY_err_code = XY_ERR_INIT_VIDEO;
      return(XY_FALSE);
    }
  }

  XY_canvasw = canvasw;
  XY_canvash = canvash;

  XY_dirty_rect_count = 0;
  XY_dirty_rect_max = XY_DIRTY_RECT_STEP;
  XY_dirty_rects = (SDL_Rect *) malloc(sizeof(SDL_Rect) * XY_dirty_rect_max);
  if (XY_dirty_rects == NULL)
  {
    SDL_Quit();
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(XY_FALSE);
  }

  XY_dirty_rects_erasure = (SDL_Rect *) malloc(sizeof(SDL_Rect) *
                                               XY_dirty_rect_max);
  if (XY_dirty_rects_erasure == NULL)
  {
    free(XY_dirty_rects);
    SDL_Quit();
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(XY_FALSE);
  }
  
  XY_dirty_rects_all = (SDL_Rect *) malloc(sizeof(SDL_Rect) *
                                           XY_dirty_rect_max * 2);
  if (XY_dirty_rects_all == NULL)
  {
    free(XY_dirty_rects);
    free(XY_dirty_rects_erasure);
    SDL_Quit();
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(XY_FALSE);
  }
  

  /* FIXME: Add possibility of using SDL_ListModes to determine a suitable mode */

  if (opts->fullscreen == XY_OPT_WINDOWED)
  {
    XY_screen = SDL_SetVideoMode(opts->displayw, opts->displayh,
                                 opts->displaybpp, SDL_SWSURFACE);
  }
  else
  {
    XY_screen = SDL_SetVideoMode(opts->displayw, opts->displayh,
                                 opts->displaybpp,
                                 SDL_SWSURFACE | SDL_FULLSCREEN);

    if (XY_screen == NULL)
    {
      if (opts->fullscreen == XY_OPT_FULLSCREEN_REQUEST)
      {
        XY_screen = SDL_SetVideoMode(opts->displayw, opts->displayh,
                                     opts->displaybpp, SDL_SWSURFACE);
      }
    }
  }

  if (XY_screen == NULL)
  {
    SDL_Quit();
    XY_err_code = XY_ERR_INIT_DISPLAY;
    return(XY_FALSE);
  }

  XY_background_color = SDL_MapRGB(XY_screen->format, 0x00, 0x00, 0x00);
  XY_background_r = 0;
  XY_background_g = 0;
  XY_background_b = 0;
  XY_background_bitmap = NULL;
  XY_background_bitmap_enabled = XY_FALSE;
  XY_background_dest.x = 0;
  XY_background_dest.y = 0;

  XY_background_bitmap_possible = opts->backgrounds;

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
    XY_err_code = XY_ERR_INIT_UNSUPPORTED_BPP;
    return(XY_FALSE);
  }

 
  XY_antialias = opts->antialias;
  XY_gamma_correction = opts->gamma_correction;

  if (opts->alpha == XY_OPT_ALPHA_FAKE)
  {
    /* Fake alpha blending (blend with background color) */
    if (Bpp == 4) putpixel = putpixel_fakea_32;
    else if (Bpp == 3) putpixel = putpixel_fakea_24;
    else if (Bpp == 2) putpixel = putpixel_fakea_16;
  }
  else if (opts->alpha == XY_OPT_ALPHA_BLEND)
  {
    /* Real alpha blending (blend with current pixel) */
    if (Bpp == 4) putpixel = putpixel_reala_32;
    else if (Bpp == 3) putpixel = putpixel_reala_24;
    else if (Bpp == 2) putpixel = putpixel_reala_16;
  }
  else
  {
    /* No alpha blending */
    if (Bpp == 4) putpixel = putpixel_32;
    else if (Bpp == 3) putpixel = putpixel_24;
    else if (Bpp == 2) putpixel = putpixel_16;
  }

  if (opts->scaling == XY_OPT_SCALE_BEST)
    scale_surf = scale_surf_best;
  else
    scale_surf = scale_surf_fast;

  return(XY_TRUE);
}

void XY_quit(void)
{
  if (XY_background_bitmap != NULL)
    XY_free_bitmap(XY_background_bitmap);

  if (XY_dirty_rects != NULL)
    free(XY_dirty_rects);
  if (XY_dirty_rects_erasure != NULL)
    free(XY_dirty_rects_erasure);
  if (XY_dirty_rects_all != NULL)
    free(XY_dirty_rects_all);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

XY_err XY_errcode(void)
{
  return(XY_err_code);
}

const char * XY_errstr(void)
{
  return(XY_errstr_txt[XY_err_code]);
}

void XY_print_options(FILE * fi, XY_options opts)
{
  fprintf(fi, "Screen size: %d x %d pixels\n", opts.displayw, opts.displayh);
  fprintf(fi, "Screen depth: %d bpp\n", opts.displaybpp);
  fprintf(fi, "Fullscreen: %s\n",
    (opts.fullscreen == XY_OPT_WINDOWED ? "no" :
     (opts.fullscreen == XY_OPT_FULLSCREEN_REQUEST ? "requested" :
      "required")));
  fprintf(fi, "Alpha-blending: %s\n",
    (opts.alpha == XY_OPT_ALPHA_BLEND ? "real" :
     (opts.alpha == XY_OPT_ALPHA_FAKE ? "fake" :
      "off")));
  fprintf(fi, "Antialiasing: %s\n",
    (opts.antialias ? "on" : "off"));
  fprintf(fi, "Gamma Correction: %s\n",
    (opts.gamma_correction ? "on" : "off"));
  fprintf(fi, "Blurring: %s\n",
    (opts.blur ? "on" : "off"));
  fprintf(fi, "Additive effect: %s\n",
    (opts.additive ? "on" : "off"));
  fprintf(fi, "Background bitmaps: %s\n",
    (opts.backgrounds ? "on" : "off"));
  fprintf(fi, "Scaling: %s\n",
    (opts.scaling == XY_OPT_SCALE_BEST ? "best" :
     "fast"));
}

XY_bitmap * XY_load_bitmap(char * filename)
{
  SDL_Surface * surf, * dispsurf;
  XY_bitmap * xyb;

  XY_err_code = XY_ERR_NONE;

  xyb = (XY_bitmap *) malloc(sizeof(XY_bitmap));
  if (xyb == NULL)
  {
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(NULL);
  }

  surf = IMG_Load(filename);
  if (surf == NULL)
  {
    free(xyb);
    XY_err_code = XY_ERR_BITMAP_CANT_DECODE;
    return(NULL);
  }

  dispsurf = SDL_DisplayFormatAlpha(surf);
  SDL_FreeSurface(surf);
  if (dispsurf == NULL)
  {
    free(xyb);
    XY_err_code = XY_ERR_BITMAP_CANT_CONVERT;
    return(NULL);
  }

  xyb->surf = dispsurf;

  return(xyb);
}

XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer, int size)
{
  SDL_RWops * rw;
  SDL_Surface * surf, * dispsurf;
  XY_bitmap * xyb;

  XY_err_code = XY_ERR_NONE;

  if (buffer == NULL || size == 0)
  {
    XY_err_code = XY_ERR_BITMAP_CANT_DECODE;
    return(NULL);
  }

  xyb = (XY_bitmap *) malloc(sizeof(XY_bitmap));
  if (xyb == NULL)
  {
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(NULL);
  }

  rw = SDL_RWFromMem((void *) buffer, size);

  surf = IMG_Load_RW(rw, 0);
  if (surf == NULL)
  {
    free(xyb);
    XY_err_code = XY_ERR_BITMAP_CANT_DECODE;
    return(NULL);
  }

  dispsurf = SDL_DisplayFormatAlpha(surf);
  SDL_FreeSurface(surf);
  if (dispsurf == NULL)
  {
    free(xyb);
    XY_err_code = XY_ERR_BITMAP_CANT_CONVERT;
    return(NULL);
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

XY_bool XY_set_background(XY_color color, XY_bitmap * bitmap,
                          XY_fixed x, XY_fixed y, int posflags, int scaling)
{
  int w, h;
  int posx, posy;

  XY_err_code = XY_ERR_NONE;

  XY_background_change = XY_TRUE;

  XY_background_r = (color >> 24) & 0xFF;
  XY_background_g = (color >> 16) & 0xFF;
  XY_background_b = (color >> 8) & 0xFF;
  XY_background_color = SDL_MapRGB(XY_screen->format,
                                   XY_background_r,
                                   XY_background_g,
                                   XY_background_b);

  if (XY_background_bitmap != NULL)
    XY_free_bitmap(XY_background_bitmap);

  if (bitmap != NULL && bitmap->surf != NULL && XY_background_bitmap_possible)
  {
    w = bitmap->surf->w;
    h = bitmap->surf->h;

    XY_background_bitmap_enabled = XY_TRUE;
    XY_background_bitmap = malloc(sizeof(XY_bitmap));
    if (XY_background_bitmap == NULL)
    {
      XY_err_code = XY_ERR_MEM_CANT_ALLOC;
      return(XY_FALSE);
    }

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
      {
        free(XY_background_bitmap);
        XY_background_bitmap = NULL;
        XY_err_code = XY_ERR_BITMAP_CANT_SCALE;
        return(XY_FALSE);
      }
      XY_background_bitmap->surf = SDL_DisplayFormatAlpha(scaled_surf);
      SDL_FreeSurface(scaled_surf);
    }
    else
    {
      XY_background_bitmap->surf = SDL_DisplayFormatAlpha(bitmap->surf);
    }

    if (XY_background_bitmap->surf == NULL)
    {
      free(XY_background_bitmap);
      XY_background_bitmap = NULL;
      XY_err_code = XY_ERR_BITMAP_CANT_CONVERT;
      return(XY_FALSE);
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

  return(XY_TRUE);
}

SDL_Surface * scale_surf_best(SDL_Surface * orig, int new_w, int new_h)
{
  /* FIXME: Implement it! */
  return(scale_surf_fast(orig, new_w, new_h));
}

SDL_Surface * scale_surf_fast(SDL_Surface * orig, int new_w, int new_h)
{
  SDL_Surface * s;
  int x, y;
  Uint32 tr, tg, tb, ta;
  Uint8 r, g, b, a;
  int tmp;
  XY_fixed src_x, src_y, xscale, yscale, f_orig_w, f_orig_h;
  void (*pp) (SDL_Surface *, int, int, Uint32, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s);
  Uint32 (*gp) (SDL_Surface * surface, int x, int y);

  /* FIXME: Test whether orig is actually set: */

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
  if (s == NULL)
    return(NULL); /* FIXME: Error */

  if (SDL_MUSTLOCK(s))
    SDL_LockSurface(s);

  if (SDL_MUSTLOCK(orig))
    SDL_LockSurface(orig);

  if (orig->format->BytesPerPixel == 2)
    gp = getpixel_16;
  else if (orig->format->BytesPerPixel == 3)
    gp = getpixel_24;
  else /* if (orig->format->BytesPerPixel == 4) */
    gp = getpixel_32;

  if (s->format->BytesPerPixel == 2)
    pp = putpixel_16;
  else if (s->format->BytesPerPixel == 3)
    pp = putpixel_24;
  else /* if (s->format->BytesPerPixel == 4) */
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
	ta = (ta * XY_FIXED_ONE) / tmp;

	pp(s, x, y, SDL_MapRGB(s->format,
				(Uint8) tr,
				(Uint8) tg,
				(Uint8) tb),
	  (XY_fixed) ta, NULL, NULL);
      }
    }
  }

  if (SDL_MUSTLOCK(orig))
    SDL_UnlockSurface(orig);

  if (SDL_MUSTLOCK(s))
    SDL_UnlockSurface(s);

  return(s);
}

XY_color XY_setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  return ((r << 24) | (g << 16) | (b << 8) | a);
}

void XY_getcolor(XY_color c, Uint8 * r, Uint8 * g, Uint8 * b, Uint8 * a)
{
  *r = (c >> 24) & 0xff;
  *g = (c >> 16) & 0xff;
  *b = (c >> 8) & 0xff;
  *a = c & 0xff;
}

void XY_enable_background(XY_bool enable)
{
  if (XY_background_bitmap_enabled != enable)
    XY_background_change = XY_TRUE;

  XY_background_bitmap_enabled = enable;
}

void XY_start_frame(int fps)
{
  int i;
  SDL_Rect src;
  SDL_Rect * dest;

  if (XY_background_change)
  {
    SDL_FillRect(XY_screen, NULL, XY_background_color);

    if (XY_background_bitmap != NULL && XY_background_bitmap_enabled == XY_TRUE)
      SDL_BlitSurface(XY_background_bitmap->surf, NULL,
                      XY_screen, &XY_background_dest);
  }
  else
  {
    memcpy(XY_dirty_rects_erasure, XY_dirty_rects,
           sizeof(SDL_Rect) * XY_dirty_rect_count);
    XY_dirty_rect_erasure_count = XY_dirty_rect_count;

    for (i = 0; i < XY_dirty_rect_count; i++)
    {
      SDL_FillRect(XY_screen, &(XY_dirty_rects_erasure[i]),
                   XY_background_color);

      if (XY_background_bitmap != NULL &&
          XY_background_bitmap_enabled == XY_TRUE)
      {
        dest = &(XY_dirty_rects_erasure[i]);
        src.x = dest->x - XY_background_dest.x;
        src.y = dest->y - XY_background_dest.y;
        src.w = dest->w;
        src.h = dest->h;

        if (src.x >= 0 && src.y >= 0 &&
            src.x < XY_background_bitmap->surf->w &&
            src.y < XY_background_bitmap->surf->h)
        {
          SDL_BlitSurface(XY_background_bitmap->surf, &src,
                          XY_screen, dest);
        }
      }
    }
  }

  XY_dirty_rect_count = 0;

  XY_want_fps = (fps == 0 ? 1 : fps);
  XY_start_time = SDL_GetTicks();
}

int XY_end_frame(XY_bool throttle)
{
  Uint32 end_time;
  int sz, sz2, n;
  void * * ptr;

  if (XY_background_change)
  {
    SDL_Flip(XY_screen);
    XY_background_change = XY_FALSE;
  }
  else
  {
    XY_merge_dirty_rects(XY_dirty_rects, &XY_dirty_rect_count);

    sz = (sizeof(SDL_Rect) * XY_dirty_rect_count);
    sz2 = (sizeof(SDL_Rect) * XY_dirty_rect_erasure_count);

    ptr = (void*) XY_dirty_rects_all;

    memset(ptr, 0, sz + sz2);
    memcpy(ptr, XY_dirty_rects, sz);

    ptr = (void *) ((int) ptr + sz);
    memcpy(ptr, XY_dirty_rects_erasure, sz2);

    n = XY_dirty_rect_count + XY_dirty_rect_erasure_count;

    XY_merge_dirty_rects(XY_dirty_rects_all, &n);

    SDL_UpdateRects(XY_screen, n, XY_dirty_rects_all);
  }
 
  SDL_Delay(1); /* Give up to the OS! */
 
  end_time = SDL_GetTicks();

  if (throttle)
  {
    if (end_time - XY_start_time < (1000 / XY_want_fps))
      SDL_Delay(XY_start_time + (1000 / XY_want_fps) - end_time);

    end_time = SDL_GetTicks();
  }

  return(end_time - XY_start_time);
}

XY_lines * XY_new_lines(void)
{
  XY_lines * l;

  l = (XY_lines *) malloc(sizeof(XY_lines));
  if (l == NULL)
  {
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(NULL);
  }

  l->count = 0;
  l->max = 128;
  l->lines = (XY_line *) malloc(sizeof(XY_line) * l->max);

  if (l->lines == NULL)
  {
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    free(l);
    return(NULL);
  }

  return(l);
}

XY_lines * XY_duplicate_lines(XY_lines * src)
{
  XY_lines * l;

  if (src == NULL || src->lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(NULL);
  }

  l = (XY_lines *) malloc(sizeof(XY_lines));
  if (l == NULL)
  {
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    return(NULL);
  }

  l->count = src->count;
  l->max = ((src->count + 127) / 128) * 128;
  l->lines = (XY_line *) malloc(sizeof(XY_line) * l->max);

  if (l->lines == NULL)
  {
    XY_err_code = XY_ERR_MEM_CANT_ALLOC;
    free(l);
    return(NULL);
  }

  memcpy(l->lines, src->lines, sizeof(XY_line) * l->count);

  return(l);
}

XY_bool XY_start_lines(XY_lines * lines)
{
  if (lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(XY_FALSE);
  }

  lines->count = 0;

  return(XY_TRUE);
}

XY_bool XY_add_line(XY_lines * lines,
                    XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                    XY_color color, XY_fixed thickness)
{
  if (lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(XY_FALSE);
  }

  if (lines->count >= lines->max)
  {
    lines->max += 128;
    lines->lines = (XY_line *) realloc(lines->lines,
                                       sizeof(XY_line) * lines->max);

    if (lines->lines == NULL)
    {
      XY_err_code = XY_ERR_MEM_CANT_ALLOC;
      return(XY_FALSE);
    }
  }

  lines->lines[lines->count].x1 = x1;
  lines->lines[lines->count].y1 = y1;
  lines->lines[lines->count].x2 = x2;
  lines->lines[lines->count].y2 = y2;
  lines->lines[lines->count].color = color;
  lines->lines[lines->count].thickness = thickness;

  lines->count++;

  return(XY_TRUE);
}

XY_bool XY_translate_lines(XY_lines * lines,
                           XY_fixed x, XY_fixed y)
{
  int i;

  if (lines == NULL || lines->lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(XY_FALSE);
  }

  for (i = 0; i < lines->count; i++)
  {
    lines->lines[i].x1 += x;
    lines->lines[i].y1 += y;
    lines->lines[i].x2 += x;
    lines->lines[i].y2 += y;
  }

  return(XY_TRUE);
}

XY_bool XY_scale_lines(XY_lines * lines, XY_fixed xscale, XY_fixed yscale)
{
  int i;

  if (lines == NULL || lines->lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(XY_FALSE);
  }

  for (i = 0; i < lines->count; i++)
  {
    lines->lines[i].x1 = XY_mult(lines->lines[i].x1, xscale);
    lines->lines[i].y1 = XY_mult(lines->lines[i].y1, yscale);
    lines->lines[i].x2 = XY_mult(lines->lines[i].x2, xscale);
    lines->lines[i].y2 = XY_mult(lines->lines[i].y2, yscale);
  }

  return(XY_TRUE);
}

XY_bool XY_rotate_lines(XY_lines * lines, int angle)
{
  /* FIXME: Do it */

  return(XY_TRUE);
}

XY_bool XY_free_lines(XY_lines * lines)
{
  if (lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(XY_FALSE);
  }

  lines->count = 0;
  lines->max = 0;
  if (lines->lines != NULL)
  {
    free(lines->lines);
    lines->lines = NULL;
  }
  return(XY_TRUE);
}

XY_bool XY_draw_lines(XY_lines * lines)
{
  int i;

  if (lines == NULL || lines->lines == NULL)
  {
    XY_err_code = XY_ERR_LINES_INVALID;
    return(XY_FALSE);
  }

  for (i = 0; i < lines->count; i++)
    XY_draw_one_line(lines->lines[i]);

  return(XY_TRUE);
}

void XY_draw_one_line(XY_line line)
{
  XY_draw_line(line.x1, line.y1,
               line.x2, line.y2,
               line.color, line.thickness);
}

void XY_draw_line(XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                  XY_color color, XY_fixed thickness)
{
  int sx1, sy1, sx2, sy2;
  XY_fixed fsx1, fsy1, fsx2, fsy2;

  if ((color & 0xff) == 0 || thickness < XY_FIXED_ONE)
    return; /* Fully transparent or thickness < 1.0 ! */

  XY_canvas_to_screen(x1, y1, &sx1, &sy1);
  XY_canvas_to_screen(x2, y2, &sx2, &sy2);

  fsx1 = sx1 << XY_FIXED_SHIFT;
  fsy1 = sy1 << XY_FIXED_SHIFT;
  fsx2 = sx2 << XY_FIXED_SHIFT;
  fsy2 = sy2 << XY_FIXED_SHIFT;

  /* FIXME: Take thickness into account! */

  if (XY_antialias)
  {
    if (XY_gamma_correction)
    {
      XY_draw_line_xiaolinwu(fsx1, fsy1, fsx2, fsy2, color,
                             XY_gamma_screen_to_linear_2_2,
                             XY_gamma_linear_to_screen_2_2);
    }
    else
    {
      XY_draw_line_xiaolinwu(fsx1, fsy1, fsx2, fsy2, color, NULL, NULL);
    }
  }
  else
  {
    XY_draw_line_bresenham(fsx1, fsy1, fsx2, fsy2, color);
  }

  XY_add_dirty_rect(sx1, sy1, sx2 + 1, sy2 + 1);
}

void XY_draw_line_xiaolinwu(XY_fixed fsx1, XY_fixed fsy1,
                            XY_fixed fsx2, XY_fixed fsy2,
                            XY_color color,
                            Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint32 sdlcolor = XY_color_to_sdl_color(color);
  XY_fixed dx, dy, ftmp, gradient;
  XY_fixed xend, yend, xgap, ygap, xpxl1, ypxl1, xpxl2, ypxl2, interx, intery;
  XY_fixed brightness1, brightness2, x, y;
  Uint8 alph;
  XY_fixed fixed_alph;

  if (SDL_MUSTLOCK(XY_screen))
    SDL_LockSurface(XY_screen);

  alph = (color & 0xff);
  fixed_alph = (alph << XY_FIXED_SHIFT) / 255;

  dx = fsx2 - fsx1;
  dy = fsy2 - fsy1;

  if (abs(dx) > abs(dy))
  {
    /* "Horizontal" lines */
    if (fsx2 < fsx1)
    {
      ftmp = fsx1;
      fsx1 = fsx2;
      fsx2 = ftmp;

      ftmp = fsy1;
      fsy1 = fsy2;
      fsy2 = ftmp;
    }

    gradient = XY_div(dy, dx);

    /* First endpoint */
    xend = XY_round(fsx1);

    yend = fsy1 + XY_mult(gradient, (xend - fsx1));
    xgap = XY_rfpart(fsx1 + XY_FIXED_HALF);
    xpxl1 = xend >> XY_FIXED_SHIFT;
    ypxl1 = XY_ipart(yend) >> XY_FIXED_SHIFT;
   
    brightness1 = XY_mult(XY_rfpart(yend), xgap);
    brightness2 = XY_mult(XY_fpart(yend), xgap);
  
    putpixel(XY_screen, xpxl1, ypxl1,
             sdlcolor, XY_mult(brightness1, fixed_alph), gamma_s2l, gamma_l2s);
    putpixel(XY_screen, xpxl1, ypxl1 + 1,
             sdlcolor, XY_mult(brightness2, fixed_alph), gamma_s2l, gamma_l2s);
  
    intery = yend + gradient;
  
    /* Second endpoint */
    xend = XY_round(fsx2);
  
    yend = fsy2 + XY_mult(gradient, (xend - fsx2));
    xgap = XY_fpart(fsx2 + XY_FIXED_HALF);
    xpxl2 = xend >> XY_FIXED_SHIFT;
    ypxl2 = XY_ipart(yend) >> XY_FIXED_SHIFT;
  
    brightness1 = XY_mult(XY_rfpart(yend), xgap);
    brightness2 = XY_mult(XY_fpart(yend), xgap);
  
    putpixel(XY_screen, xpxl2, ypxl2,
             sdlcolor, XY_mult(brightness1, fixed_alph), gamma_s2l, gamma_l2s);
    putpixel(XY_screen, xpxl2, ypxl2 + 1,
             sdlcolor, XY_mult(brightness2, fixed_alph), gamma_s2l, gamma_l2s);
  
    /* Main loop */
    for (x = xpxl1 + 1; x < xpxl2; x++)
    {
      brightness1 = XY_rfpart(intery);
      brightness2 = XY_fpart(intery);
  
      putpixel(XY_screen, x, XY_ipart(intery) >> XY_FIXED_SHIFT,
               sdlcolor, XY_mult(brightness1, fixed_alph), gamma_s2l, gamma_l2s);
      putpixel(XY_screen, x, (XY_ipart(intery) >> XY_FIXED_SHIFT) + 1,
               sdlcolor, XY_mult(brightness2, fixed_alph), gamma_s2l, gamma_l2s);
  
      intery += gradient;
    }
  }
  else
  {
    /* "Vertical" lines */
    if (fsy2 < fsy1)
    {
      ftmp = fsx1;
      fsx1 = fsx2;
      fsx2 = ftmp;

      ftmp = fsy1;
      fsy1 = fsy2;
      fsy2 = ftmp;
    }

    gradient = XY_div(dx, dy);

    /* First endpoint */
    yend = XY_round(fsy1);

    xend = fsx1 + XY_mult(gradient, (yend - fsy1));
    ygap = XY_rfpart(fsy1 + XY_FIXED_HALF);
    xpxl1 = XY_ipart(xend) >> XY_FIXED_SHIFT;
    ypxl1 = yend >> XY_FIXED_SHIFT;
   
    brightness1 = XY_mult(XY_rfpart(xend), ygap);
    brightness2 = XY_mult(XY_fpart(xend), ygap);
  
    putpixel(XY_screen, xpxl1, ypxl1,
             sdlcolor, XY_mult(brightness1, fixed_alph), gamma_s2l, gamma_l2s);
    putpixel(XY_screen, xpxl1 + 1, ypxl1,
             sdlcolor, XY_mult(brightness2, fixed_alph), gamma_s2l, gamma_l2s);
  
    interx = xend + gradient;
  
    /* Second endpoint */
    yend = XY_round(fsy2);
  
    xend = fsx2 + XY_mult(gradient, (yend - fsy2));
    ygap = XY_fpart(fsy2 + XY_FIXED_HALF);
    xpxl2 = XY_ipart(xend) >> XY_FIXED_SHIFT;
    ypxl2 = yend >> XY_FIXED_SHIFT;
  
    brightness1 = XY_mult(XY_rfpart(xend), ygap);
    brightness2 = XY_mult(XY_fpart(xend), ygap);
  
    putpixel(XY_screen, xpxl2, ypxl2,
             sdlcolor, XY_mult(brightness1, fixed_alph), gamma_s2l, gamma_l2s);
    putpixel(XY_screen, xpxl2 + 1, ypxl2,
             sdlcolor, XY_mult(brightness2, fixed_alph), gamma_s2l, gamma_l2s);
  
    /* Main loop */
    for (y = ypxl1 + 1; y < ypxl2; y++)
    {
      brightness1 = XY_rfpart(interx);
      brightness2 = XY_fpart(interx);
  
      putpixel(XY_screen, XY_ipart(interx) >> XY_FIXED_SHIFT, y,
               sdlcolor, XY_mult(brightness1, fixed_alph), gamma_s2l, gamma_l2s);
      putpixel(XY_screen, (XY_ipart(interx) >> XY_FIXED_SHIFT) + 1, y,
               sdlcolor, XY_mult(brightness2, fixed_alph), gamma_s2l, gamma_l2s);
  
      interx += gradient;
    }
  }

  if (SDL_MUSTLOCK(XY_screen))
    SDL_UnlockSurface(XY_screen);
}

void XY_draw_line_bresenham(XY_fixed fsx1, XY_fixed fsy1,
                            XY_fixed fsx2, XY_fixed fsy2,
                            XY_color color)
{
  Uint32 sdlcolor = XY_color_to_sdl_color(color);
  XY_fixed alph = ((color & 0xff) << XY_FIXED_SHIFT) / 255;
  XY_fixed dx, dy;
  XY_fixed m, b;
  int y;

  if (SDL_MUSTLOCK(XY_screen))
    SDL_LockSurface(XY_screen);

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
            putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor, alph, NULL, NULL);
        }
        else
        {
          for (y = fsy2 >> XY_FIXED_SHIFT; y <= fsy1 >> XY_FIXED_SHIFT; y++)
            putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor, alph, NULL, NULL);
        }

        fsx1 = fsx1 + dx;
      }
    }
    else
    {
      if (fsy2 > fsy1)
      {
        for (y = fsy1 >> XY_FIXED_SHIFT; y <= fsy2 >> XY_FIXED_SHIFT; y++)
          putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor, alph, NULL, NULL);
      }
      else
      {
        for (y = fsy2 >> XY_FIXED_SHIFT; y <= fsy1 >> XY_FIXED_SHIFT; y++)
          putpixel(XY_screen, fsx1 >> XY_FIXED_SHIFT, y, sdlcolor, alph, NULL, NULL);
      }
    }
  }
  
  if (SDL_MUSTLOCK(XY_screen))
    SDL_UnlockSurface(XY_screen);
}

/* FIXME: Can probably just be a macro */
void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color, XY_fixed thickness)
{
  XY_draw_line(x, y, x, y, color, thickness);
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
  return(((cx >> XY_FIXED_SHIFT_HALF) * XY_screen->w) / (XY_canvasw >> XY_FIXED_SHIFT_HALF));
}

int XY_canvasy_to_screeny(XY_fixed cy)
{
  return(((cy >> XY_FIXED_SHIFT_HALF) * XY_screen->h) / (XY_canvash >> XY_FIXED_SHIFT_HALF));
}

void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy)
{
  *sx = XY_canvasx_to_screenx(cx);
  *sy = XY_canvasy_to_screeny(cy);
}

int XY_get_screenw(void)
{
  return(XY_screen->w);
}

int XY_get_screenh(void)
{
  return(XY_screen->h);
}

void putpixel_16(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 2;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h || alph == 0)
    return;

  *(Uint16 *)p = pixel;
}

void putpixel_24(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
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

void putpixel_32(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h || alph == 0)
    return;

  *(Uint32 *)p = pixel;
}

void blend(Uint8 * dest_r, Uint8 * dest_g, Uint8 * dest_b,
           Uint8 src1_r, Uint8 src1_g, Uint8 src1_b,
           Uint8 src2_r, Uint8 src2_g, Uint8 src2_b,
           XY_fixed alpha,
           Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  XY_fixed antialpha;

  antialpha = XY_FIXED_ONE - alpha;

  if (gamma_s2l == NULL || gamma_l2s == NULL)
  {
    *dest_r = (((XY_fixed) (src1_r * alpha) + (XY_fixed) (src2_r * antialpha))) >> XY_FIXED_SHIFT;
    *dest_g = (((XY_fixed) (src1_g * alpha) + (XY_fixed) (src2_g * antialpha))) >> XY_FIXED_SHIFT;
    *dest_b = (((XY_fixed) (src1_b * alpha) + (XY_fixed) (src2_b * antialpha))) >> XY_FIXED_SHIFT;
  }
  else
  {
    *dest_r = gamma_l2s[(XY_mult(gamma_s2l[src2_r], antialpha) +
                         XY_mult(gamma_s2l[src1_r], alpha))];
    *dest_g = gamma_l2s[(XY_mult(gamma_s2l[src2_g], antialpha) +
                         XY_mult(gamma_s2l[src1_g], alpha))];
    *dest_b = gamma_l2s[(XY_mult(gamma_s2l[src2_b], antialpha) +
                         XY_mult(gamma_s2l[src1_b], alpha))];
  }
}

void putpixel_fakea_16(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 2;
  Uint8 r1, g1, b1;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < XY_FIXED_ONE)
  {
    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);

    blend(&r, &g, &b,
          r1, g1, b1, XY_background_r, XY_background_g, XY_background_b,
          alph, gamma_s2l, gamma_l2s);

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint16 *)p = pixel;
}

void putpixel_fakea_24(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 3;
  Uint8 r1, g1, b1;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < XY_FIXED_ONE)
  {
    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);

    blend(&r, &g, &b,
          r1, g1, b1, XY_background_r, XY_background_g, XY_background_b,
          alph, gamma_s2l, gamma_l2s);

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

void putpixel_fakea_32(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
  Uint8 r1, g1, b1;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < XY_FIXED_ONE)
  {
    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);

    blend(&r, &g, &b,
          r1, g1, b1, XY_background_r, XY_background_g, XY_background_b,
          alph, gamma_s2l, gamma_l2s);

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint32 *)p = pixel;
}

void putpixel_reala_16(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 2;
  Uint8 r1, g1, b1, r2, g2, b2;
  Uint32 oldpixel;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (XY_FIXED_ONE)
  {
    oldpixel = *(Uint16 *)p;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);
    SDL_GetRGB(oldpixel, surface->format, &r2, &g2, &b2);

    blend(&r, &g, &b,
          r1, g1, b1, r2, g2, b2,
          alph, gamma_s2l, gamma_l2s);

    pixel = SDL_MapRGB(surface->format, r, g, b);
  }

  *(Uint16 *)p = pixel;
}

void putpixel_reala_24(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 3;
  Uint8 r1, g1, b1, r2, g2, b2;
  Uint32 oldpixel;
  Uint8 r, g, b;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < XY_FIXED_ONE)
  {
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      oldpixel = (p[0] << 16) | (p[1] << 8) | p[2];
    } else {
      oldpixel = (p[2] << 16) | (p[1] << 8) | p[0];
    }

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);
    SDL_GetRGB(oldpixel, surface->format, &r2, &g2, &b2);

    blend(&r, &g, &b,
          r1, g1, b1, r2, g2, b2,
          alph, gamma_s2l, gamma_l2s);

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

void putpixel_reala_32(SDL_Surface *surface, int x, int y, Uint32 pixel, XY_fixed alph, Uint16 * gamma_s2l, Uint8 * gamma_l2s)
{
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
  Uint8 r1, g1, b1, r2, g2, b2;
  Uint8 r, g, b;
  Uint32 oldpixel;

  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  if (alph == 0)
    return;
  else if (alph < 255)
  {
    oldpixel = *(Uint32 *)p;

    SDL_GetRGB(pixel, surface->format, &r1, &g1, &b1);
    SDL_GetRGB(oldpixel, surface->format, &r2, &g2, &b2);

    blend(&r, &g, &b,
          r1, g1, b1, r2, g2, b2,
          alph, gamma_s2l, gamma_l2s);

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

  return *(Uint32 *) p;		/* 32-bit display */
}

void XY_add_dirty_rect(int x1, int y1, int x2, int y2)
{
  int tmp;

  if (XY_dirty_rects == NULL)
    return;

  if (XY_dirty_rect_count >= XY_dirty_rect_max)
  {
    XY_dirty_rect_max += XY_DIRTY_RECT_STEP;
    XY_dirty_rects = (SDL_Rect *) realloc(XY_dirty_rects,
                       sizeof(SDL_Rect) * XY_dirty_rect_max);
    XY_dirty_rects_erasure = (SDL_Rect *) realloc(XY_dirty_rects_erasure,
                               sizeof(SDL_Rect) * XY_dirty_rect_max);
    XY_dirty_rects_all = (SDL_Rect *) realloc(XY_dirty_rects_all,
                           sizeof(SDL_Rect) * XY_dirty_rect_max * 2);

    if (XY_dirty_rects == NULL || XY_dirty_rects_erasure == NULL ||
        XY_dirty_rects_all == NULL)
      return;
  }

  if (x1 <= x2)
  {
    x1 = x1 - 1;
    x2 = x2 + 1;
  }
  else
  {
    tmp = x1;
    x1 = x2 - 1;
    x2 = tmp + 1;
  }

  if (y1 <= y2)
  {
    y1 = y1 - 1;
    y2 = y2 + 1;
  }
  else
  {
    tmp = y1;
    y1 = y2 - 1;
    y2 = tmp + 1;
  }

  if (x1 < 0)
    x1 = 0;
  if (x1 >= XY_screen->w)
    x1 = XY_screen->w - 1;
  if (y1 < 0) 
    y1 = 0;
  if (y1 >= XY_screen->h)
    y1 = XY_screen->h - 1;

  if (x2 < 0)
    x2 = 0;
  if (x2 >= XY_screen->w)
    x2 = XY_screen->w - 1;
  if (y2 < 0)
    y2 = 0;
  if (y2 >= XY_screen->h)
    y2 = XY_screen->h - 1;

  if (x2 - x1 + 1 == 0 || y2 - y1 + 1 == 0)
    return;

  XY_dirty_rects[XY_dirty_rect_count].x = x1;
  XY_dirty_rects[XY_dirty_rect_count].w = x2 - x1 + 1;

  XY_dirty_rects[XY_dirty_rect_count].y = y1;
  XY_dirty_rects[XY_dirty_rect_count].h = y2 - y1 + 1;

  XY_dirty_rect_count++;
}

void XY_merge_dirty_rects(SDL_Rect * rects, int * cnt)
{
  int i, j;
  XY_bool done;

  /* FIXME: Make rect. merging work! */

  //return;

  do
  {
    done = XY_TRUE;

    for (i = 0; i < (*cnt) - 1; i++)
    {
      for (j = i + 1; j < *cnt; j++)
      {
        if (XY_rects_intersect(rects, i, j))
        {
          XY_rects_combine(rects, i, j);

          if (j < (*cnt) - 1)
          {
            memcpy(&(rects[j]), &(rects[(*cnt) - 1]), sizeof(SDL_Rect));
          }
          *cnt = (*cnt) - 1;
        }
      }
    }
  }
  while (!done);
}

XY_bool XY_rects_intersect(SDL_Rect * rects, int r1, int r2)
{
  int left1, right1, top1, bottom1;
  int left2, right2, top2, bottom2;

  left1 = rects[r1].x;
  right1 = rects[r1].x + rects[r1].w - 1;
  top1 = rects[r1].y;
  bottom1 = rects[r1].y + rects[r1].h - 1;

  left2 = rects[r2].x;
  right2 = rects[r2].x + rects[r2].w - 1;
  top2 = rects[r2].y;
  bottom2 = rects[r2].y + rects[r2].h - 1;

  return !(left1 >= right2 || right1 <= left2 ||
           top1 >= bottom2 || bottom1 <= top2);
}

void XY_rects_combine(SDL_Rect * rects, int r1, int r2)
{
  int right1, bottom1;
  int right2, bottom2;

  right1 = rects[r1].x + rects[r1].w - 1;
  bottom1 = rects[r1].y + rects[r1].h - 1;

  right2 = rects[r2].x + rects[r2].w - 1;
  bottom2 = rects[r2].y + rects[r2].h - 1;

  if (rects[r2].x < rects[r1].x)
    rects[r1].x = rects[r2].x;

  if (right2 > right1)
    right1 = right2;

  rects[r1].w = right1 - rects[r1].x + 1;

  if (rects[r2].y < rects[r1].y)
    rects[r1].y = rects[r2].y;

  if (bottom2 > bottom1)
    bottom1 = bottom2;

  rects[r1].h = bottom1 - rects[r1].y + 1;
}

/* Based on Line-Line Intersection Method With C Code Sample
   Public Domain, 2006 by Darel Rex Finley
   http://www.alienryderflex.com/intersect/ */

XY_bool XY_lines_intersect(XY_line line1, XY_line line2,
                           XY_fixed * intersect_x, XY_fixed * intersect_y,
                           XY_intersection * result)
{
  XY_fixed distAB, theCos, theSin, newX, ABpos;
  XY_fixed Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;

  Ax = line1.x1;
  Ay = line1.y1;
  Bx = line1.x2;
  By = line1.y2;

  Cx = line2.x1;
  Cy = line2.y1;
  Dx = line2.x2;
  Dy = line2.y2;

  if ((Ax == Bx && Ay == By) || (Cx == Dx && Cy == Dy))
  {
    /* Fail if either line segment is zero-length */
    /* FIXME: We should test if the point is on the line... */
    if (result != NULL)
      *result = XY_INTERSECTION_NONE;
    return(XY_FALSE);
  }

  if ((Ax == Cx && Ay == Cy) || (Bx == Cx && By == Cy) ||
      (Ax == Dx && Ay == Dy) || (Bx == Dx && By == Dy))
  {
    /* Fail if the segments share an end-point */
    /* FIXME: We should have a 'share an end-point' result */
    if (result != NULL)
      *result = XY_INTERSECTION_NONE;
    return(XY_FALSE);
  }

  /* Step 1: Translate the system so that point A is in the origin */
  Bx -= Ax;
  By -= Ay;
  Cx -= Ax;
  Cy -= Ay;
  Dx -= Ax;
  Dy -= Ay;

  /* Discover the length of segment A-B */
  distAB = XY_sqrt(XY_mult(Bx, Bx) + XY_mult(By, By));

  /* Step 2: Rotate the system so that point B is on the positive X axis */
  theCos = XY_div(Bx, distAB);
  theSin = XY_div(By, distAB);

  newX = XY_mult(Cx, theCos) + XY_mult(Cy, theSin);
  Cy = XY_mult(Cy, theCos) - XY_mult(Cx, theSin);
  Cx = newX;

  newX = XY_mult(Dx, theCos) + XY_mult(Dy, theSin);
  Dy = XY_mult(Dy, theCos) - XY_mult(Dx, theSin);
  Dx = newX;

  if ((Cy < 0 && Dy < 0) || (Cy >= 0 && Dy >= 0))
  {
    /* Fail if segment C-D doesn't cross line A-B */
    if (result != NULL)
      *result = XY_INTERSECTION_NONE;
    return(XY_FALSE);
  }

  /* Step 3: Discover the position of the intersection point along line A-B */
  ABpos = Dx + XY_div(XY_mult(Cx - Dx, Dy), Dy - Cy);

  if (ABpos < 0 || ABpos > distAB)
  {
    /* Fail if segment C-D crosses line A-B outside of segment A-B */
    if (result != NULL)
      *result = XY_INTERSECTION_NONE;
    return(XY_FALSE);
  }

  /* Step 4: Apply the discovered position to line A-B in the original
     coordinate system */


  if (intersect_x != NULL)
    *intersect_x = Ax + XY_mult(ABpos, theCos);

  if (intersect_y != NULL)
    *intersect_y = Ay + XY_mult(ABpos, theSin);

  if (result != NULL)
    *result = XY_INTERSECTION_INTERSECTING;

  return (XY_TRUE);
}

/* Based on 'Doing It Fast'
   By Bob Pendleton, 1993, 1997
   http://gameprogrammer.com/4-fixed.html */

XY_fixed XY_sqrt(XY_fixed i)
{
  XY_fixed root;
  XY_fixed next;

  if (i < XY_FIXED_ONE)
    return(0);

  next = i >> 2;

  do
  {
    root = next;
    next = (next + XY_div(i, next)) >> 1;
  }
  while (root != next);

  return root;
}

/* Based on lines_intersect (xlines.c) by Mukesh Prasad,
   from Graphics Gems II.
   http://tog.acm.org/GraphicsGems/gemsii/xlines.c
   And http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/ */

#if 0
XY_bool XY_lines_intersect(XY_line line1, XY_line line2,
                           XY_fixed * intersect_x, XY_fixed * intersect_y,
                           XY_intersection * result)
{
  XY_fixed x1, y1, x2, y2, x3, y3, x4, y4, x, y, ua;
  XY_fixed a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
  XY_fixed r1, r2, r3, r4;         /* 'Sign' values */
  XY_fixed denom, offset, numa, numb;     /* Intermediate values */

  x1 = line1.x1;
  y1 = line1.y1;
  x2 = line1.x2;
  y2 = line1.y2;
  x3 = line2.x1;
  y3 = line2.y1;
  x4 = line2.x2;
  y4 = line2.y2;

  /* Compute a1, b1, c1, where line joining points 1 and 2
     is "a1 x  +  b1 y  +  c1  =  0".  */

  a1 = y2 - y1;
  b1 = x1 - x2;
  c1 = XY_mult(x2, y1) - XY_mult(x1, y2);

  /* Compute r3 and r4. */

  r3 = XY_mult(a1, x3) + XY_mult(b1, y3) + c1;
  r4 = XY_mult(a1, x4) + XY_mult(b1, y4) + c1;

  /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
     same side of line 1, the line segments do not intersect. */

  if (r3 != 0 &&
      r4 != 0 &&
      ((r3 > 0 && r4 > 0) || (r3 < 0 && r4 < 0))) /* Same signs */
  {
    if (result != NULL)
      *result = XY_INTERSECTION_NONE;

    return(XY_FALSE);
  }

  /* Compute a2, b2, c2 */

  a2 = y4 - y3;
  b2 = x3 - x4;
  c2 = XY_mult(x4, y3) - XY_mult(x3, y4);

  /* Compute r1 and r2 */

  r1 = XY_mult(a2, x1) + XY_mult(b2, y1) + c2;
  r2 = XY_mult(a2, x2) + XY_mult(b2, y2) + c2;

  /* Check signs of r1 and r2.  If both point 1 and point 2 lie
     on same side of second line segment, the line segments do
     not intersect. */

  if (r1 != 0 &&
      r2 != 0 &&
      ((r1 > 0 && r2 > 0) || (r1 < 0 && r2 < 0))) /* Same signs */
  {
    if (result != NULL)
      *result = XY_INTERSECTION_NONE;

    return(XY_FALSE);
  }

  /* Line segments intersect: compute intersection point. */

  denom = XY_mult(a1, b2) - XY_mult(a2, b1);
  numa = XY_mult(b1, c2) - XY_mult(b2, c1);
  numb = XY_mult(a2, c1) - XY_mult(a1, c2);
  if (denom <= XY_FIXED_DIV_ZERO)
  {
    if (result != NULL)
    {
      if (numa == 0 && numb == 0)
        *result = XY_INTERSECTION_COINCIDENT;
      else
        *result = XY_INTERSECTION_PARALLEL;
    }

    return(XY_FALSE);
  }

  offset = ((denom < 0) ? -(denom / 2) : (denom / 2));

  /* The denom/2 is to get rounding instead of truncating.  It
     is added or subtracted to the numerator, depending upon the
     sign of the numerator. */

  ua = XY_div(
         (XY_mult((x4 - x3), (y1 - y3)) -
          XY_mult((y4 - y3), (x1 - x3))),
         (XY_mult((y4 - y3), (x2 - x1)) -
          XY_mult((x4 - x3), (y2 - y1))));

  x = x1 + XY_mult(ua, (x2 - x1));
  y = y1 + XY_mult(ua, (y2 - x1));

  /* FIXME: Remove debug: */
  /* printf("%d,%d - %d,%d  and  %d,%d - %d,%d  intersect at %d,%d\n", x1, y1, x2, y2, x3, y3, x4, y4, x, y); */

  if (intersect_x != NULL)
    *intersect_x = x;

  if (intersect_y != NULL)
    *intersect_y = y;

  if (result != NULL)
    *result = XY_INTERSECTION_INTERSECTING;

  return (XY_TRUE);
}
#endif

XY_bool XY_line_groups_intersect(XY_lines * lines1, XY_lines * lines2)
{
  int i, j;

  if (lines1 == NULL || lines1->count == 0 || lines1->lines == NULL ||
      lines2 == NULL || lines2->count == 0 || lines2->lines == NULL)
    return(XY_FALSE);

  for (i = 0; i < lines1->count; i++)
  {
    for (j = 0; j < lines2->count; j++)
    {
      if (XY_lines_intersect(lines1->lines[i], lines2->lines[j],
                             NULL, NULL, NULL))
        return(XY_TRUE);
    }
  }

  return(XY_FALSE);
}
