/*
  Generate tables used for fast gamma-corrected blending, in fixed-point.

  Original code (c)2008 Solra Bizna <solra@bizna.name>
  "who hereby releases this code into the public domain.
  If you use it or learn from it, consider crediting me somewhere."

  Edited by Bill Kendrick <bill@newbreedsoftware.com>
  August 9, 2008
*/

/*
  PC monitors are usually uncalibrated and vary between 2.2 and 2.6.
  Macs are calibrated to 1.8 by default.
  Other OSes and hardware will vary.

  Note that with gamma higher than 2.0, 16 bits is technically not
  enough. Very dark screen colors will fold into black. (For sRGB, on the
  other hand, which is close enough to 2.2 gamma, 16 bits is plenty. sRGB
  conversion is more complicated and I won't go into it here.)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

typedef uint8_t Uint8;
typedef uint16_t Uint16;

int main(int argc, char * argv[])
{
  float target_gamma, reverse_target_gamma;
  int n;

  if (argc == 2)
    target_gamma = (float)atof(argv[1]);
  else
    target_gamma = 2.2;

  reverse_target_gamma = 1.f / target_gamma;

  printf("/* Tables for fast gamma-corrected blending, in fixed-point.\n"
         "   Original code used to construct tables\n"
         "   (c)2008 Solra Bizna <solra@bizna.name>\n"
         "   who hereby releases this code into the public domain. */\n\n");

  printf("/* For gamma: %.1f */\n\n", target_gamma);

  printf("/* 8-bit screen component goes in, 16-bit linear component comes out. */\n");
  printf("Uint16 XY_gamma_screen_to_linear_%d_%d[256] = {\n  ", (int)floor(target_gamma), (int)(fmod(target_gamma, 10)));
  for(n = 0; n < 256; ++n)
  {
    float f = powf(n/255.f, target_gamma);
    Uint16 s = (Uint16)floorf(f * 65535.f);
    printf("0x%04x /*%1.5f*/, ", s, f);
    if (n % 2 == 1)
      printf("\n  ");
  }
  printf("\n};\n\n");
  
  printf("/* 16-bit linear component goes in, 8-bit screen component comes out.\n"
  "   By shifting by an appropriate amount, you can theoretically shave orders of\n"
  "   magnitude off this table's size. With sRGB, anyway; with straight gamma,\n"
  "   you'll DRASTICALLY worsen the folding problem as described above. */\n\n");
  printf("Uint8 XY_gamma_linear_to_screen_%d_%d[65536] = {\n", (int)floor(target_gamma), (int)(fmod(target_gamma, 10)));
  printf("  /* BEWARE OVERRUN */\n  ");
  for(n = 0; n < 65536; ++n)
  {
    float f = powf(n/65535.f, reverse_target_gamma);
    Uint8 s = (Uint8)floorf(f * 255.f);
    printf("0x%02x /*%1.5f*/, ", s, f);
    if (n % 4 == 3)
      printf("\n  ");
  }
  printf("\n};\n\n");
}

