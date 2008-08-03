                     README for libcrtxy (CRT X-Y Library)

                            http://libcrtxy.sf.net/
                  by Bill Kendrick <bill@newbreedsoftware.com>

                         July 29, 2008 - August 3, 2008
                                 Version: 0.0.1

     ----------------------------------------------------------------------

Purpose:

   "libcrtxy" is meant to allow game programmers to develop
   vector-graphics-style games like those from the late 1970s and early 1980s
   (some examples: 'Lunar Lander', 'Asteroids', 'Red Baron', 'Star Wars',
   'BattleZone', 'Quantum', 'Boxing Bugs', 'Speed Freak', 'Star Castle',
   'Black Widow', 'Star Trek', 'Tempest', and 'War of the Worlds'.*)

   It is named after the so-called "X-Y" arcade game displays, such as the
   "Quadrascan" (created by Wells Gardner) and "Amplifone" monitors used in
   Atari vector-based games.*

   I am creating this library with the intent to port a number of my own
   games (clones inspired by classic arcade games) to it: ICBM3D, Vectoroids,
   3D Pong.

   Helper functions are also included for doing fixed-point math (useful
   since your canvas size is not necessarily your screen size, and can be
   used for sub-pixel movement) and trigonometry (since most vector-based
   games involve things rotating).

   * Names are trademarks of their respective trademark and copyright
   holders.

Backends:

   libcrtxy is being built on top of libSDL, the Simple DirectMedia Layer
   library (http://www.libsdl.org/), and therefore uses it (and SDL_Image for
   bitmap loading) as a backend. It should be reasonable for someone to
   develop an SDL+OpenGL backend for accelerated graphics.

   libcrtxy really only lets the programmer do two things:
    1. set a background color, and optional background overlay image
       (consider translucent colored overlays in arcade games like 'Star
       Castle' and 'War of the Worlds', and the overlays for games on the
       Vectrex home video game system)
    2. draw lines (vectors) on the screen

   For events (keyboard, mouse, joystick, etc.), your event loop, sound
   effects, etc., you use SDL functions and types directly. For video
   initialization, loading and displaying of bitmaps, and drawing vectors,
   libcrtxy's "XY_" functions and types should be used.

Options:

   Depending on the target system (e.g., a high-powered desktop PC or an
   embedded handheld system with a slow CPU and no FPU), various options can
   be set in libcrtxy. On a slow system, fancy visual effects intended to
   simulate an arcade experience can be disabled (anti-aliasing, blurring,
   etc.).

   Screen-size shouldn't matter to game-play, so physical screen size in
   pixels (e.g., 640x480 or 1280x1024) is up to the end-user, or person
   packaging your software for a particular target, as well. Your game logic
   is based around a virtual canvas size, and line positions are given using
   fixed-point values.

   The options that can be set at runtime include:
     * Display settings:
          * Screen width & height
          * Screen depth (16bpp, 24bpp or 32bpp)
          * Window or fullscreen (requested or required)
     * Rendering quality:
          * Alpha blending (on, off, or "fake")
          * Anti-aliasing
          * Backgrounds
          * Bitmap scaling (fast or best)
     * Special effects:
          * Blurring
          * Additive effect

   The options that get used are determined by the following, and should
   occur in this order:
    1. Hard-coded defaults (set at the time libcrtxy is compiled)
    2. System-wide "libcrtxy" configuration file (e.g.,
       "/etc/libcrtxy/libcrtxy.conf")
    3. User's own "libcrtxy" configuration file (e.g., "~/.libcrtxyrc")
    4. Environment variables (e.g., "CRTXY_SCALE=FAST")
    5. System-wide configuration file for the application (e.g.,
       "/etc/some_game/some_game.conf")
    6. User's own configuration file for the application (e.g.,
       "~/.some_gamerc")
    7. Command-line arguments (e.g., "some_game --crtxy-bpp 32")
