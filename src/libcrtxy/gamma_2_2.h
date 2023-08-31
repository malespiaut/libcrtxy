#pragma once

#include <stdint.h>

/* Tables for fast gamma-corrected blending, in fixed-point.
   Original code used to construct tables
   (c)2008 Solra Bizna <solra@bizna.name>
   who hereby releases this code into the public domain. */

/* For gamma: 2.2 */

/* 8-bit screen component goes in, 16-bit linear component comes out. */
extern uint16_t XY_gamma_screen_to_linear_2_2[256];

/* 16-bit linear component goes in, 8-bit screen component comes out.
   By shifting by an appropriate amount, you can theoretically shave orders of
   magnitude off this table's size. With sRGB, anyway; with straight gamma,
   you'll DRASTICALLY worsen the folding problem as described above. */
extern uint8_t XY_gamma_linear_to_screen_2_2[65536];
