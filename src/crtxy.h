/*! \mainpage The CRT X-Y library (libcrtxy)

\author Bill Kendrick <bill@newbreedsoftware.com>

http://libcrtxy.sf.net/

$Id: crtxy.h,v 1.39 2008/08/17 04:28:44 wkendrick Exp $

\section introSection Introduction

"libcrtxy" is meant to allow game programmers to develop
vector-graphics-style games like those from the late 1970s and early 1980s
(some examples: 'Lunar Lander', 'Asteroids', 'Red Baron', 'Star Wars',
'BattleZone', 'Quantum', 'Boxing Bugs', 'Speed Freak', 'Star Castle',
'Black Widow', 'Star Trek', 'Tempest', and 'War of the Worlds'.*)

It is named after the so-called "X-Y" arcade game displays,
such as the "Quadrascan" (created by Wells Gardner) and "Amplifone"
monitors used in Atari vector-based games.*

I am creating this library with the intent to port a number of my own
games (clones inspired by classic arcade games) to it:
ICBM3D, Vectoroids, 3D Pong.

Helper functions are also included for doing fixed-point math
(useful since your canvas size is not necessarily your screen size,
and can be used for sub-pixel movement) and trigonometry
(since most vector-based games involve things rotating).

(*) Names are trademarks of their respective trademark and copyright holders.

\section tocSection More about libcrtxy

\subsection tocGeneralSubsection General
\li \subpage backendsSubpage
\li \subpage optionsSubpage

\subsection tocGeneralSubsection Programming games with libcrtxy
\li \subpage installationSubpage "Installing libcrtxy"
\li \subpage buildingSubpage "Building Games with libcrtxy"

\subsection tocGeneralSubsection Running games that use libcrtxy
\li \subpage settingOptionsSubpage "Setting Options"

\todo Construct man pages
*/

/*! \page backendsSubpage Backends that libcrtxy can use for drawing

libcrtxy is being built on top of libSDL, the Simple DirectMedia Layer library
(http://www.libsdl.org/), and therefore uses it (and SDL_Image for bitmap
loading) as a backend.  It should be reasonable for someone to develop an
SDL+OpenGL backend for accelerated graphics.

For events (keyboard, mouse, joystick, etc.), your event loop,
sound effects, etc., you use SDL functions and types directly.
For video initialization, loading and displaying of bitmaps,
and drawing vectors, libcrtxy's "XY_" functions and types should be used.

\todo Add OpenGL support.

*/

/*! \page optionsSubpage Options for rendering quality that libcrtxy provides

Depending on the target system (e.g., a high-powered desktop PC or
an embedded handheld system with a slow CPU and no FPU), various
options can be set in libcrtxy.  On a slow system, fancy visual effects
intended to simulate an arcade experience can be disabled
(anti-aliasing, blurring, etc.).

Screen-size shouldn't matter to game-play, so physical screen size
in pixels (e.g., 640x480 or 1280x1024) is up to the end-user,
or person packaging your software for a particular target, as well.
Your game logic is based around a virtual canvas size, and line
positions are given using fixed-point values.

The options that can be set at runtime include:

\section optionsDisplaySection Display settings:

\li Screen width & height
\li Screen depth (16bpp, 24bpp or 32bpp)
\li Window or fullscreen (requested or required)
\todo Utilize SDL_ListModes()
\todo Native screen resolution when in fullscreen

\section optionsRenderingSection Rendering quality:
\li Alpha blending (on, off, or "fake")
\li Anti-aliasing
\li Gamma correction \todo Support gamma values
\li Backgrounds
\li Bitmap scaling (fast or best) \todo Implement best scaling

\section optionsSFXSection Special effects:
\li Blurring \todo Implement blur effect
\li Additive effect \todo Implement additive effect
\todo Persistence-of-vision effect

\section optionsHowSection How options get set:
The options that get used are determined by the following, and should
occur in this order:
\li Hard-coded defaults (set at the time libcrtxy is compiled)
\li System-wide "libcrtxy" configuration file (e.g.,
  "/etc/libcrtxy/libcrtxy.conf")
\li User's own "libcrtxy" configuration file (e.g.,
  "~/.libcrtxyrc")
\li Environment variables (e.g., "CRTXY_SCALE=FAST")
\li System-wide configuration file for the application (e.g.,
  "/etc/some_game/some_game.conf")
\li User's own configuration file for the application (e.g.,
  "~/.some_gamerc")
\li Command-line arguments (e.g.,
  "some_game --crtxy-bpp 32")

See also: \ref settingOptionsSubpage
*/

/** \page installationSubpage Installing libcrtxy

\section requirementsSection Requirements

libcrtxy requires the Simple DirectMedia Layer library (libSDL),
available from http://www.libsdl.org/

For support for various formats of bitmap images (PNG, JPEG, GIF, etc.),
SDL_image is also required, available from
http://www.libsdl.org/projects/SDL_image/

\section compilingSection Compiling

To compile libcrtxy, simply type <tt>make</tt>.

\subsection compilationOptionsSubsection Compilation Options

You may override the following <tt>Makefile</tt> variables via command-line
arguments to <tt>make</tt> (e.g., "<tt>make PREFIX=/home/username/opt/</tt>"):

\li <tt>PREFIX</tt> - Base path of where everything gets installed
(default: <tt>/usr/local</tt>)
\li <tt>CONFDIR</tt> - Path where libcrtxy's global configuration file
will be installed, and looked for. (default: <tt>$PREFIX/etc/libcrtxy</tt>,
unless <tt>PREFIX</tt> is <tt>/usr</tt>, in which case it is simply
<tt>/etc/libcrtxy</tt>)
\li <tt>LIBDIR</tt> - Path where libcrtxy's object files will be placed
(and where <tt>crtxy-config --libs</tt> will report them).
(default: <tt>$PREFIX/lib</tt>)
\li <tt>INCDIR</tt> - Path where libcrtxy's header files will be placed
(and where <tt>crtxy-config --cflags</tt> will report them).
(default: <tt>$PREFIX/include</tt>)
\li <tt>BINDIR</tt> - Path where the <tt>crtxy-config</tt> helper tool
will be installed. (default: <tt>$PREFIX/bin</tt>)

\todo Documentation installation
\todo Man page installation

\section installingSection Installing

To install libcrtxy's library files, header files, default global configuration
file and the <tt>crtxy-config</tt> helper tool, simply type
<tt>make install</tt>.

\b Note: Provide <tt>make</tt> with the same variable overrides you
gave it when installing (e.g.,
"<tt>make PREFIX=/home/username/opt/ install</tt>")

\section compilingTestsSection Compiling test apps

Once libcrtxy is installed, you can build the test applications that
came with the source. Type: <tt>make tests</tt>.

\li drawlines - Draws a sequences of polygons
\li rockdodge - A game-like example, where you control the thrust and
direction of a space ship in a field of asteroids (rocks).
Press [F] key to toggle between throttled (attempting max 30fps) and
unthrottled framerate modes.
\li polytest - Use the mouse to draw a sequence of attached lines, which
will form a polygon when you cross back over them.
\todo Explain polytest right-click for adding dots.

*/

/** \page buildingSubpage Building Games with libcrtxy

\section crtxy-configSection Using crtxy-config to compile and link

Use the <tt>crtxy-config</tt> command get the
options necessary to compile and link an application against libcrtxy.

  - <tt>crtxy-config --cflags</tt> \n
    This outputs compiler flags necessary to compile a C or C++ program
    with libcrtxy. \n
    Example: <tt>gcc game.c -c `crtxy-config --cflags`</tt>
  .
  - <tt>crtxy-config --libs</tt> \n
    This outputs linker flags necessary to link a program
    against libcrtxy as a shared library. \n
    Example: <tt>gcc -o game game.o other.o `crtxy-config --libs`</tt>
  .
  - <tt>crtxy-config --static-libs</tt> \n
    This outputs linker flags necessary to link a program
    against libcrtxy as a static library. \n
    Example: <tt>gcc -o game game.o other.o `crtxy-config --static-libs`</tt>
  .
  - <tt>crtxy-config --version</tt> \n
    This outputs the version of libcrtxy that is installed. It's useful for
    automated checking of whether the installed version of libcrtxy is
    compatible with what your application expects.
  .

\b Note: Since libcrtxy depends on libSDL, the output of
<tt>crtxy-config</tt> includes the output of libSDL's
<tt>sdl-config</tt> for --cflags, --libs and --static-libs.

\section crtxy-configSection Including libcrtxy's header

<tt>crtxy-config --cflags</tt> should have told your compiler where
to find libcrtxy's headers, so you should include the main header like this:

<tt>\#include "crtxy.h"</tt>

\b Note: libcrtxy depends on libSDL, so its <tt>SDL.h</tt> is
included automatically. SDL_image library's <tt>SDL_image.h</tt> may
also have been included. However, no harm is done by including them in your own
source.

*/

/** \page settingOptionsSubpage Setting Options

\section optionsAvailable Available Options

\subsection displayOptionsSubsection Display options
\li Screen width
\li Screen height
\li Screen color depth (bits per pixel (bpp))
\li Windowed, fullscreen required, or fullscreen requested

\subsection renderingQualitySubsection Rendering quality options
\li Alpha-blended lines (on, 'fake', or off)
\li Anti-aliased lines (on or off)
\li Gamma-corrected anti-aliasing (on or off) \todo Support gamma values
\li Backgrounds (on or off)
\li Background bitmap scaling quality (best or fast)

\subsection effectsSubsection Visual effects
\li Blur
\li Additive lines

\section optionsFromSection Where Options Can Get Set

Options such as rendering quality settings and screen resolution
can come from various places.  They are listed below, in the most
reasonable order that they should be picked up:

\li Defaults
\li Global libcrtxy configuration file
\li Local (user's) libcrtxy configuration file
\li Global game configuration file
\li Local (user's) game configuration file
\li Environment variables
\li Command-line options

\subsection optionsFromDefault Defaults
The <tt>XY_default_options()</tt> function sets some base values for
the various options, in case no others are sent elsewhere.
These are the values compiled into libcrtxy.

\subsection optionsFromFile Config. Files
The <tt>XY_load_options()</tt> function loads options from configuration
files specific to libcrtxy.  <tt>XY_load_options_from_file()</tt> may be
used by applications to load options from arbitrary files (such as a game's
own config. file).

\li <tt>crtxy-width=NNN</tt>
\li <tt>crtxy-height=NNN</tt>
\li <tt>crtxy-bpp={16|24|32|any}</tt>
\li <tt>crtxy-windowed</tt>
\li <tt>crtxy-fullscreen</tt>
\li <tt>crtxy-fullscreen-or-window</tt>
\li <tt>crtxy-alpha={on|fake|off}</tt>
\li <tt>crtxy-antialias={on|off}</tt>
\li <tt>crtxy-backgrounds={on|off}</tt>
\li <tt>crtxy-scaling={best|fast}</tt>
\li <tt>crtxy-gamma-correction={on|off}</tt> \todo Support gamma values
\li <tt>crtxy-blur={on|off}</tt>
\li <tt>crtxy-additive={on|off}</tt>

\subsection optionsFromEnvironment Environment Variables
<tt>XY_parse_envvars()</tt> examines the application's
runtime enviroment for libcrtxy-related variables.

\li <tt>CRTXY_WIDTH</tt>
\li <tt>CRTXY_HEIGHT</tt>
\li <tt>CRTXY_BPP</tt> (16|24|32|ANY)
\li <tt>CRTXY_FULLSCREEN</tt> (ON|OPTIONAL|OFF)
\li <tt>CRTXY_ALPHA</tt> (ON|FAKE|OFF)
\li <tt>CRTXY_ANTIALIAS</tt> (ON|OFF)
\li <tt>CRTXY_BACKGROUNDS</tt> (ON|OFF)
\li <tt>CRTXY_SCALING</tt> (BEST|FAST)
\li <tt>CRTXY_GAMMA_CORRECTION</tt> (ON|OFF) \todo Support gamma values
\li <tt>CRTXY_BLUR</tt> (ON|OFF)
\li <tt>CRTXY_ADDITIVE</tt> (ON|OFF)

\subsection optionsFromCommandLine Command-Line Arguments
Finally, the <tt>XY_parse_options()</tt> function can look for and parse
and libcrtxy-related options found in the command-line arguments to an
application.

\li <tt>--crtxy-width NNN</tt>
\li <tt>--crtxy-height NNN</tt>
\li <tt>--crtxy-bpp {16|24|32|any}</tt>
\li <tt>--crtxy-windowed</tt>
\li <tt>--crtxy-fullscreen</tt>
\li <tt>--crtxy-fullscreen-or-window</tt>
\li <tt>--crtxy-alpha {on|fake|off}</tt>
\li <tt>--crtxy-antialias {on|off}</tt>
\li <tt>--crtxy-backgrounds {on|off}</tt>
\li <tt>--crtxy-scaling {best|fast}</tt>
\li <tt>--crtxy-gamma-correction {on|off}</tt> \todo Support gamma values
\li <tt>--crtxy-blur {on|off}</tt>
\li <tt>--crtxy-additive {on|off}</tt>
\li <tt>--help-crtxy</tt> - Presents a list of libcrtxy-related usage, and quits.

*/


#ifndef _CRTXY_H
#define _CRTXY_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup generalGroup General types.
 *  @{
 */

/**
 * Boolean type.
 */
typedef enum {
  XY_FALSE, /**< False */
  XY_TRUE /**< True */
} XY_bool;

/** @} */

/** @defgroup bitmapGroup Background bitmap management.
 *  @{
 *
 *  \todo Load bitmaps from an SDL_Surface
 *  \todo Load bitmaps from RGBA pixel data
 */

/**
 * Bitmap type.
 */
typedef struct XY_bitmap_s {
  SDL_Surface * surf;
} XY_bitmap;

/** @} */

/** @defgroup colorGroup Color manipulation and conversion.
 *  @{
 */
/**
 * Color type.
 */
typedef Uint32 XY_color;

/** @} */


/** @defgroup fixedGroup Fixed-point.
 *  @{
 */

/**
 * Fixed-point type.
 */
typedef Sint32 XY_fixed;

/* Fixed-point math settings  */

#define XY_FIXED_SHIFT 16 /**< How much to shift ints to get a fixed-point value. */
#define XY_FIXED_SHIFT_HALF 8 /**< For half-shift during mult & divide macros. */
#define XY_FIXED_ONE (1 << XY_FIXED_SHIFT) /**< Quick way to get '1' as an XY_fixed. */
#define XY_FIXED_HALF (1 << (XY_FIXED_SHIFT - 1)) /**< Quick way to get '0.5' as an XY_fixed. */

/* Limits for fixed-point values (for signed 32-bit ints) */

#define XY_FIXED_MAX 0x7FFFFFFF /**< Maximum value an XY_fixed can hold. */
#define XY_FIXED_MIN -(0x80000000) /**< Minimum value an XY_fixed can hold. */
#define XY_FIXED_NAN XY_FIXED_MAX /**< Not-a-number (NAN), occurs when you divide by zero */

/** @} */


/** @defgroup geometryGroup Geometry.
 *  @{
 *
 * \todo Add inside-polygon test
 * \todo Add distance calculator
 * \todo Add wrapped lines to a group (a la Asteroids)
 * \todo Clip lines in a group
 */

/* Some standard thicknesses: */

#define XY_THIN XY_FIXED_ONE /**< Minimum line thickness. */

/**
 * Represents a single line
 */
typedef struct XY_line_s {
  XY_fixed x1; /**< X coordinate of line starting point */
  XY_fixed y1; /**< Y coordinate of line starting point */
  XY_fixed x2; /**< X coordinate of line end point */
  XY_fixed y2; /**< Y coordinate of line end point */
  XY_color color; /**< Color of line */
  XY_fixed thickness; /**< Thickness of line \todo Implement thick lines */
} XY_line;

/**
 * A group of lines
 */
typedef struct XY_lines_s {
  int count; /**< How many lines are currently in the group */
  int max; /**< How many lines the group can hold (used for memory allocation/reallocation) */
  XY_line * lines; /**< The lines */
} XY_lines;

/**
 * Line intersection results:
 */
typedef enum {
  XY_INTERSECTION_NONE, /**< Lines do not intersect and are not parallel */
  XY_INTERSECTION_INTERSECTING, /**< Lines intersect at a point */
  XY_INTERSECTION_PARALLEL, /**< Lines are parallel (do not intersect) */
  XY_INTERSECTION_COINCIDENT /**< Lines are coincident (and, hence, intersect) */
} XY_intersection;

/** @} */


/** @defgroup optionsGroup Loading and setting options for rendering quality.
 *  @{
 */

/**
 * Display settings for options.fullscreen
 */
typedef enum {
  XY_OPT_WINDOWED, /**< Display in a window */
  XY_OPT_FULLSCREEN_REQUEST, /**< Fullscreen; fall back to window */
  XY_OPT_FULLSCREEN_REQUIRED /**< Fullscreen; abort if we cannot */
} XY_opt_fullscreen;

/**
 * Alpha-blending settings for options.alpha
 */
typedef enum {
  XY_OPT_ALPHA_BLEND, /**< Combine current pixel with new pixel */
  XY_OPT_ALPHA_FAKE, /**< Combine background color with new pixel */
  XY_OPT_ALPHA_OFF /**< Just draw new pixel */
} XY_opt_alpha;

/**
 * Bitmap scaling quality settings for options.scaling
 */
typedef enum {
  XY_OPT_SCALE_BEST, /**< Blend to smooth any stretching */
  XY_OPT_SCALE_FAST /**< Stretch pixels with no blending */
} XY_opt_scaling;

/**
 * Structure containing libcrtxy options (rendering level, screen size, etc.)
 */
typedef struct XY_options_s {
  int displayw;  /**< Width of window or fullscreen display. */
  int displayh;  /**< Height of window or fullscreen display. */
  int displaybpp; /**< Display depth (16bpp, 24bpp, 32bpp). */
  XY_opt_fullscreen fullscreen; /**< Window, Want Fullscreen or Require Fullscreen? */
  XY_opt_alpha alpha;  /**< Alpha-blend, fake it, or none at all (just on/off)? */
  XY_bool antialias;  /**< Anti-alias lines (Xiaolin Wu) or not (Bresenham)? */
  XY_bool gamma_correction;  /**< Gamma correction when anti-aliasing  \todo Support gamma values (only doing 2.2 (close to sRGB) at the moment) */
  XY_bool blur;  /**< Add blur effect? */
  XY_bool additive;  /**< Additive pixel effect? */
  XY_bool backgrounds;  /**< Support fullscreen background */
  XY_opt_scaling scaling;  /**< Fast or Best scaling? */
} XY_options;

/* Where system-wide (global) and user's (local) config files live: */
#define XY_INIT_LIB_CONFIG_FILE_GLOBAL CONFDIR "/libcrtxy.conf" /**< Global configuration file \todo Make config location configurable at build */ /* FIXME */
#define XY_INIT_LIB_CONFIG_FILE_LOCAL ".libcrtxyrc" /**< User configuration file (in $HOME) \todo Make config location configurable at build */ /* FIXME */

/** @} */


/** @defgroup errorGroup Error reporting.
 *  @{
 */
typedef enum {
  XY_ERR_NONE, /**< No error */
  XY_ERR_OPTION_BAD, /**< Bad value to an option */
  XY_ERR_OPTION_UNKNOWN, /**< Unrecognized option */
  XY_ERR_FILE_CANT_OPEN, /**< Cannot open file */
  XY_ERR_MEM_CANT_ALLOC, /**< Cannot allocate memory */
  XY_ERR_INIT_VIDEO, /**< Cannot initialize video subsystem */
  XY_ERR_INIT_DISPLAY, /**< Cannot open display */
  XY_ERR_INIT_UNSUPPORTED_BPP, /**< Unsupported (so far as we're concerned) bpp */
  XY_ERR_BITMAP_CANT_DECODE, /**< Error decoding image file (IMG_Load failed) */
  XY_ERR_BITMAP_CANT_CONVERT, /**< Cannot convert a surface to the display fmt. */
  XY_ERR_BITMAP_CANT_SCALE, /**< Cannot scale an image (probably mem. alloc. fail) */
  XY_ERR_LINES_INVALID, /**< Invalid (NULL?) group of lines */
  NUM_XY_ERRS
} XY_err;

/** @} */


/** @defgroup bitmapPositioningGroup Bitmap positioning flags.
  * @ingroup bitmapGroup
  * @{
  */

#define XY_POS_TOP 0x0 /**< Position background at the top (default) */
#define XY_POS_LEFT 0x0 /**< Position background on the left (default) */
#define XY_POS_HCENTER 0x1 /**< Horizontally center background */
#define XY_POS_VCENTER 0x2 /**< Vertically center background */
#define XY_POS_RIGHT 0x4 /**< Position background at the right */
#define XY_POS_BOTTOM 0x8 /**< Position background at the bottom */

/** @} */

/** @defgroup bitmapScalingGroup Options for scaling bitmaps that don't match screen/window size.
  * @ingroup bitmapGroup
  * @{
  */
#define XY_SCALE_NONE 0 /**< Do not stretch; clip or show solid border(s) */
#define XY_SCALE_STRETCH 1 /**< Stretch; aspect ratio can be altered */
#define XY_SCALE_KEEP_ASPECT_WIDE 2 /**< Stretch w/o alt'ing aspect; fit width */
#define XY_SCALE_KEEP_ASPECT_TALL 3 /**< Stretch w/o alt'ing aspect; fit height */

/** @} */


/** @ingroup optionsGroup
  * @{
  */

/**
 * Set opts to default (libcrtxy's compiled-time) options.
 * Call this first, to get a base set of options, in case no other
 * means is available.
 *
 * \param opts is a pointer to an options structure to fill.
 */
void XY_default_options(XY_options * opts);

/**
 * Load global, then local (user) libcrtxy config files into opts.
 *
 * \param opts is a pointer to an options structure to fill.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_load_options(XY_options * opts);

/**
 * Load arbitrary config file into opts.
 *
 * \param fname is the name of a file to load options from.
 * \param opts is a pointer to an options structure to fill.
 * \param ignore_unknowns set to XY_TRUE to prevent function from aborting
 * on unrecognized lines (useful if you want to let users put libcrtxy
 * configuration options in an app-specific config file.)
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 *
 * \todo Support a callback function for processing non-libcrtxy-related
 * options without processing files twice.
 */
XY_bool XY_load_options_from_file(char * fname, XY_options * opts,
                                  XY_bool ignore_unknowns);

/**
 * Parse libcrtxy-related arguments into opts.
 *
 * \param argc is a count of arguments to parse.
 * \param argv is an array of arguments to parse.
 * \param opts is a pointer to an options structure to fill.
 * \return On success: 0 on success. On failure, an index into argv[] of an
 * offending argument, and sets error code to one of the following:
 * \li FIXME
 */
int XY_parse_options(int * argc, char * argv[], XY_options * opts);

/**
 * Read any libcrtxy-related environment variables into opts.
 *
 * \param opts is a pointer to an options structure to fill.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_parse_envvars(XY_options * opts);

/** @} */


/** @defgroup initGroup Initializing and quitting libcrtxy.
  * @{
  */

/**
 * Initialize SDL (video only) and libcrtxy (with rendering and video options
 * from opts), and set the virtual canvas size.
 *
 * \param opts is a pointer to an options structure that has been filled.
 * \param canvasw is the width (in XY_fixed units) of a virtual canvas which
 * will be scaled to the real display.
 * \param canvash is the height (in XY_fixed units) of a virtual canvas which
 * will be scaled to the real display.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 *
 * \todo Allow setting window class (SDL_VIDEO_X11_WMCLASS=xxx.yyy)
 * \todo Allow enabling/disabling screensaver (SDL_VIDEO_ALLOW_SCREENSAVER=1)
 * \todo Allow positioning the window (SDL_VIDEO_WINDOW_POS=center/nopref/...)
 * \todo Allow setting window icon (SDL_WM_SetIcon())
 * \todo Allow setting window title (SDL_WM_SetCaption())
 * \todo Allow hiding mouse (SDL_ShowCursor()) (separate function?)
 * \todo Allow resizable windows (SDL_RESIZABLE)
 */
XY_bool XY_init(XY_options * opts, XY_fixed canvasw, XY_fixed canvash);

/**
 * Shut down libcrtxy and SDL.
 */
void XY_quit(void);

/** @} */


/** @ingroup errorGroup
  * @{
  */

/**
 * Gets the most recent error code.
 * 
 * \return the latest error code value.
 */
XY_err XY_errcode(void);

/**
 * Gets a string representing the most recent error code.
 *
 * \return a string containing a human-readable message describing the latest
 * error code value.
 */
const char * XY_errstr(void);

/**
 * Dumps options in opts to the file stream (eg, stderr or an opened logfile).
 *
 * \param fi is a file pointer to output to (stderr or stdout could be used, or
 * a file that you've opened for write or append using fopen())
 * \param opts is a pointer to an options structure that has been filled.
 */
void XY_print_options(FILE * fi, XY_options opts);

/** @} */


/** @ingroup bitmapGroup
 *  @{
 */

/**
 * Create a bitmap based on an image file.
 *
 * \param filename is the name of an image file to attempt to load.
 * \return an XY_bitmap pointer on success, or NULL on failure (and set error
 * code).
 */
XY_bitmap * XY_load_bitmap(char * filename);

/**
 * Create a bitmap based on image data in a buffer.
 * 
 * \param buffer is a pointer to memory containing image file data.
 * \param size is the size of the image file data.
 * \return an XY_bitmap pointer on success, or NULL on failure (and set error
 * code).
 */
XY_bitmap * XY_load_bitmap_from_buffer(unsigned char * buffer, int size);

/**
 * Free a bitmap.
 *
 * \param bitmap is an \ref XY_bitmap pointer to free. (Do not use the pointer
 * any more! You may reuse your variable, if you create a new bitmap, of
 * course.)
 */
void XY_free_bitmap(XY_bitmap * bitmap);


/* - Setting background: - */

/**
 * Set the background color, and optional bitmap, its position, and
 * options for scaling it to the screen size.  Enables background bitmap.
 *
 * \param color is an \ref XY_color for the display's background. (The entire
 * display will be this color, if no bitmap is provided, otherwise any part
 * of the display not covered by the bitmap will be this color.
 * Lines alpha-blended or anti-aliased in 'fake' rendering mode will blend
 * against this color, as well.)
 * \param bitmap is an \ref XY_bitmap pointer for a background image to use.
 * It may be NULL if no background image is desired.
 * \param x represents how far right (or left, if negative) to nudge the
 * background image after it has been positioned, in canvas units. Use 0 for
 * no nudging.
 * \param y represents how far down (or up, if negative) to nudge the
 * background image after it has been positioned, in canvas units. Use 0 for
 * no nudging.
 * \param posflags determines how to position a bitmap. Use the "|" (or)
 * bitwise operator to combine one horizontal choice (\ref XY_POS_LEFT,
 * \ref XY_POS_HCENTER or \ref XY_POS_RIGHT) with one vertical choice
 * (\ref XY_POS_TOP, \ref XY_POS_VCENTER or \ref XY_POS_BOTTOM).
 * Use 0 as a shortcut for 'top left'.
 * \param scaling describes how the bitmap should be scaled. Use one of
 * the following: \ref XY_SCALE_NONE, \ref XY_SCALE_STRETCH,
 * \ref XY_SCALE_KEEP_ASPECT_WIDE or \ref XY_SCALE_KEEP_ASPECT_TALL.
 * \return \ref XY_TRUE on success, or \ref XY_FALSE on failure, and
 * sets error code.
 * \todo Support repeating backgrounds
 * \todo Support color overlays
 * \todo Support scaling bitmaps, relative to canvas
 */
XY_bool XY_set_background(XY_color color, XY_bitmap * bitmap,
                          XY_fixed x, XY_fixed y, int posflags, int scaling);

/**
 * Enable or disable the background bitmap (affects next frame).
 *
 * \param enable set to XY_TRUE enables background bitmap (if any), and
 * XY_FALSE disables it.
 */
void XY_enable_background(XY_bool enable);

/** @} */


/** @ingroup colorGroup
  * @{
  */

/**
 * Combines values for R, G, B and A components into an XY_color. 
 *
 * \param r Red component (between 0 and 255).
 * \param g Green component (between 0 and 255).
 * \param b Blue component (between 0 and 255).
 * \param a Alpha component (between 0 (transparent) and 255 (opaque)).
 * \return An XY_color representing the RGBA values provided.
 */
XY_color XY_setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/**
 * Breaks an XY_color into its R, G, B and A components.
 *
 * \param c An XY_color from which color components should be extracted.
 * \param r Pointer to a variable to contain the red component.
 * \param g Pointer to a variable to contain the blue component.
 * \param b Pointer to a variable to contain the green component.
 * \param a Pointer to a variable to contain the alpha component
 * (0 represents transparent, 255 represents opaque).
 */
void XY_getcolor(XY_color c, Uint8 * r, Uint8 * g, Uint8 * b, Uint8 * a);

/** @} */


/** @defgroup frameGroup Starting and ending a drawing frame.
  * @{
  */

/**
 * Mark the start of a frame. Screen backbuffer is wiped to the background
 * color and/or bitmap.  Also setting of preferred FPS.
 *
 * \param fps The requested FPS, if XY_end_frame()'s throttle option is used.
 */
void XY_start_frame(int fps);

/**
 * Mark the end of a frame. Screen backbuffer is made visible.
 * Optionally, pause until (1000/fps) milliseconds have passed since
 * XY_start_frame() was called. (If 'throttle' is set to XY_TRUE.)
 *
 * \param throttle causes XY_end_frame() to pause the application (by calling
 * SDL_Delay()) so that the amount of time between the last XY_start_frame()
 * call and now is approximately 1000/fps milliseconds, if set to XY_TRUE.
 * Otherwise, pauses only 1ms, to relinquish control to the OS.
 * \return The number of milliseconds since the last XY_start_frame() call.
 * (This will be approximately '1000/fps', if throttle is XY_TRUE and the
 * system was able to do everything in between quickly enough.)
 *
 * \todo Get dirty rectangle merging to work.
 * \todo Subdivide dirty rectangles (to waste less around diagonal lines)
 */
int XY_end_frame(XY_bool throttle);

/** @} */


/** @defgroup lineCollectionGroup Line collection manipulation.
  * @ingroup geometryGroup
  * @{
  */

/**
 * Create a new line collection.
 *
 * \return a new XY_lines pointer, with no lines.
 */
XY_lines * XY_new_lines(void);

/**
 * Duplicates a collection.
 *
 * \param lines is an \ref XY_lines pointer from which you want to copy.
 * \return a pointer to a new \ref XY_lines with all lines from 'lines' copied
 * to it on success, or NULL on failure, and sets error code.
 */
XY_lines * XY_duplicate_lines(XY_lines * lines);

/**
 * Free a line collection.
 *
 * \param lines is an \ref XY_lines pointer to free. (Do not use the pointer any
 * more!  You may reuse your variable, if you create a new bitmap, of course.)
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_free_lines(XY_lines * lines);

/**
 * Reset a line collection so that it contains no lines.
 * This allows you to reconstruct a line collection (without making a new one)
 * at different times (e.g., at the start of a new frame).
 *
 * \param lines is an \ref XY_lines pointer to reset.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_start_lines(XY_lines * lines);

/**
 * Add a line to a line collection.
 *
 * \param lines is an \ref XY_lines pointer to add a line to.
 * \param x1 is the X coordinate of the new line's starting point.
 * \param y1 is the Y coordinate of the new line's starting point.
 * \param x2 is the X coordinate of the new line's ending point.
 * \param y2 is the Y coordinate of the new line's ending point.
 * \param color is an \ref XY_color representing the color and transparency of
 * the new line.
 * \param thickness is an \ref XY_fixed representing the thickness of the new
 * line.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_add_line(XY_lines * lines,
                    XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                    XY_color color, XY_fixed thickness);

/**
 * Translate all lines within a collection.
 *
 * \param lines is an \ref XY_lines pointer containing a collection of lines to
 * translate.
 * \param x is the offset (positive for right, negative for left) by which to
 * translate all of the lines horizontally.
 * \param y is the offset (positive for down, negative for up) by which to
 * translate all of the lines vertically.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_translate_lines(XY_lines * lines,
                           XY_fixed x, XY_fixed y);

/**
 * Scale all lines within a collection (centered around the origin (0,0)).
 *
 * \param lines is an \ref XY_lines pointer containing a collection of lines to
 * scale.
 * \param xscale is the scale to change all of the lines' X coordinates.
 * \param yscale is the scale to change all of the lines' Y coordinates.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_scale_lines(XY_lines * lines, XY_fixed xscale, XY_fixed yscale);

/**
 * Rotate all lines within a collection (centered around the origin (0,0)).
 *
 * \param lines is an \ref XY_lines pointer containing a collection of lines to
 * scale.
 * \param angle is angle (in degrees) to rotate each line in the collection.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 * \todo Implement line rotation
 */
XY_bool XY_rotate_lines(XY_lines * lines, int angle);

/** @} */


/** @defgroup drawingGroup Drawing primitives.
  * @{
  */

/**
 * Draw a single line between (x1,y1) and (x2,y2)
 * (in canvas virtual world units) and in the specified color/alpha
 * and thickness.
 *
 * \param x1 is the X coordinate of the line's starting point.
 * \param y1 is the Y coordinate of the line's starting point.
 * \param x2 is the X coordinate of the line's ending point.
 * \param y2 is the Y coordinate of the line's ending point.
 * \param color is an \ref XY_color representing the color and transparency of
 * the line.
 * \param thickness is an \ref XY_fixed representing the thickness of the line.
 * \todo Create line clip routine (for efficiency)
 */
void XY_draw_line(XY_fixed x1, XY_fixed y1, XY_fixed x2, XY_fixed y2,
                  XY_color color, XY_fixed thickness);

/**
 * Draw a collection of lines.
 *
 * \param lines is an \ref XY_lines pointer with a collection of lines to draw.
 * \return On success: \ref XY_TRUE. On failure, \ref XY_FALSE, and sets
 * error code to one of the following:
 * \li FIXME
 */
XY_bool XY_draw_lines(XY_lines * lines);

/**
 * Draw a single line using an XY_line struct.
 *
 * \param line is an \ref XY_line struct representing a line to draw.
 */
void XY_draw_one_line(XY_line line);

/**
 * Draw a point at (x,y) (in canvas virtual world units)
 * in the specified color/alpha and thickness.
 *
 * \param x is the X coordinate of the point.
 * \param y is the Y coordinate of the point.
 * \param color is an \ref XY_color representing the color and transparency of
 * the point.
 * \param thickness is an \ref XY_fixed representing the thickness of the point.
 */
void XY_draw_point(XY_fixed x, XY_fixed y, XY_color color, XY_fixed thickness);

/** @} */


/** @defgroup fixedMathGroup Fixed-point math functions.
  * @ingroup fixedGroup
  * @{
  */

/**
 * Multiply two XY_fixed values.
 *
 * \param a multiplicand
 * \param b multiplicand
 * \return a multiplied by b, approximately
 */
#define XY_mult(a,b) (((a) >> XY_FIXED_SHIFT_HALF) * \
		      ((b) >> XY_FIXED_SHIFT_HALF))

/**
 * Divide one XY_fixed value by another.
 *
 * \param a numerator
 * \param b denominator
 * \return a divided by b, approximately.
 */
#define XY_qdiv(a,b) (((a) / \
		       ((b) >> XY_FIXED_SHIFT_HALF)) \
                      << XY_FIXED_SHIFT_HALF)

/**
 * Divide one XY_fixed value by another.
 * Returns Not-A-Number if denominator is (sufficient close to) zero.
 *
 * \param a numerator
 * \param b denominator
 * \return a divided by b, approximately, or \ref XY_FIXED_NAN if b is
 * zero (approximately)
 */
#define XY_div(a,b) (((b) >> XY_FIXED_SHIFT_HALF) == 0 ? \
                     XY_FIXED_NAN : XY_qdiv((a),(b)))

/**
 * The maximum value that, when XY_div() is used to divide two digits,
 * the denominator may be, and be considered zero (due to shifting to
 * reduce precision loss.)
 */
#define XY_FIXED_DIV_ZERO ((1 << XY_FIXED_SHIFT_HALF) - 1)

/**
 * Return the fractional part of 'a'.
 */
#define XY_fpart(a) ((a) & (XY_FIXED_ONE - 1))

/**
 * Return one minus the fractional part of 'a'.
 */
#define XY_rfpart(a) (XY_FIXED_ONE - XY_fpart(a))

/**
 * Returns the integer (whole) part of 'a'.
 */
#define XY_ipart(a) ((a) - XY_fpart(a))

/**
 * Rounds 'a' up to the nearest integer (whole).
 */
#define XY_round(a) (XY_ipart((a) + (1 << (XY_FIXED_SHIFT - 1))))

/**
 * Returns cosine() of 'degrees'. ('degrees' is a non-fixed-point
 * value, in degrees.  Values outside 0-359 are accounted for.)
 *
 * \param degrees is an angle, in degrees (between 0 and 359).
 * \return cosine() of the angle, in XY_fixed.
 */
XY_fixed XY_cos(int degrees);

/**
 * Returns sine() of 'degrees'. ('degrees' is a non-fixed-point
 * value, in degrees.  Values outside 0-359 are accounted for.)
 *
 * \param degrees is an angle, in degrees (between 0 and 359).
 * \return sine() of the angle, in XY_fixed.
 */
#define XY_sin(degrees) (XY_cos(90 - (degrees)))

/** @} */


/** @defgroup screenToCanvasGroup Screen/canvas conversions and queries.
  * @{
  */

/**
 * Convert a screen coordinate (an integer; eg, where the mouse was clicked)
 * into canvas virtual world units (fixed point).
 *
 * \param sx is a screen X coordinate.
 * \return the screen X coordinate scaled to a corresponding coordinate
 * in canvas units.
 */
XY_fixed XY_screenx_to_canvasx(int sx);

/**
 * Convert a screen coordinate (an integer; eg, where the mouse was clicked)
 * into canvas virtual world units (fixed point).
 *
 * \param sy is a screen Y coordinate.
 * \return the screen Y coordinate scaled to a corresponding coordinate
 * in canvas units.
 */
XY_fixed XY_screeny_to_canvasy(int sy);

/**
 * Convert a screen coordinate (an integer; eg, where the mouse was clicked)
 * into canvas virtual world units (fixed point).
 *
 * \param sx is a screen X coordinate.
 * \param sy is a screen Y coordinate.
 * \param cx is a pointer to a variable into which the screen X coordinate,
 * scaled to a corresponding coordinate in canvas units, is to be placed.
 * \param cy is a pointer to a variable into which the screen Y coordinate,
 * scaled to a corresponding coordinate in canvas units, is to be placed.
 */
void XY_screen_to_canvas(int sx, int sy, XY_fixed * cx, XY_fixed * cy);

/**
 * Convert a canvas virtual world coordinate (fixed point) into the
 * nearest screen coordinate (an integer).
 *
 * \param cx is an X coordinate in canvas units.
 * \return the screen X coordinate most closely corresponding to cx.
 */
XY_fixed XY_canvasx_to_screenx(int cx);

/**
 * Convert a canvas virtual world coordinate (fixed point) into the
 * nearest screen coordinate (an integer).
 *
 * \param cy is an Y coordinate in canvas units.
 * \return the screen Y coordinate most closely corresponding to cy.
 */
XY_fixed XY_canvasy_to_screeny(int cy);

/**
 * Convert a canvas virtual world coordinate (fixed point) into the
 * nearest screen coordinate (an integer).
 *
 * \param cx is an X coordinate in canvas units.
 * \param cy is an Y coordinate in canvas units.
 * \param sx is a pointer to a variable into which a screen X coordinate
 * most closely corresponding to cx is to be placed.
 * \param sy is a pointer to a variable into which a screen Y coordinate
 * most closely corresponding to cy is to be placed.
 */
void XY_canvas_to_screen(XY_fixed cx, XY_fixed cy, int * sx, int * sy);

/**
 * Returns the screen's current width, in pixels (integer).
 *
 * \return screen width, in pixels.
 */
int XY_get_screenw(void);

/**
 * Returns the screen's current height, in pixels (integer).
 *
 * \return screen height, in pixels.
 */
int XY_get_screenh(void);

/** @} */


/** @defgroup intersectionGroup Intersection tests.
  * @ingroup geometryGroup
  * @{
  */

/**
 * Returns whether two lines intersect.
 * Optionally (if not NULL), return the (x,y) coordinates of the intersection
 * (if possible), and how the lines intersect (or don't): parallel, coincident,
 * not intersecting, or intersecting.
 *
 * \param line1 is an \ref XY_line structure containing a line.
 * \param line2 is an \ref XY_line structure containing a line.
 * \param intersect_x is a pointer to a variable into which the X coordinates
 * of the intersection (if any) occurred; may be NULL to ignore.
 * \param intersect_y is a pointer to a variable into which the Y coordinates
 * of the intersection (if any) occurred; may be NULL to ignore.
 * \param result is a pointer to an XY_intersection variable into which
 * the type of intersection (if any) occurred; may be NULL to ignore.
 * \return \ref XY_TRUE if the lines intersect or are coincident,
 * \ref XY_FALSE if they do not intersect or are parallel.
 */
XY_bool XY_lines_intersect(XY_line line1, XY_line line2,
                           XY_fixed * intersect_x, XY_fixed * intersect_y,
                           XY_intersection * result);

/**
 * Returns whether any lines in one group intersect any lines in another.
 *
 * \param lines1 is an \ref XY_lines pointer containing a collection of lines.
 * \param lines2 is an \ref XY_lines pointer containing a collection of lines.
 * \return \ref XY_TRUE if any lines in 'lines1' intersect or are coincident
 * with any lines in 'lines2', \ref XY_FALSE if not.
 */
XY_bool XY_line_groups_intersect(XY_lines * lines1, XY_lines * lines2);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _CRTXY_H */
