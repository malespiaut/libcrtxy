/*
  mathtest.c

  Test app for CRT X-Y library (libcrtxy)
  http://libcrtxy.sf.net/

  Bill Kendrick <bill@newbreedsoftware.com>

  December 25, 2008 - December 25, 2008
*/

#include <crtxy.h>
#include <math.h>
#include <stdio.h>

int
main(void)
{
  int choice;
  float val, math_answer;
  XY_fixed fixed_val, xy_answer;

  do
  {
    printf("libcrtxy mathtest\n");
    printf("Fixed point shift is %d\n\n", XY_FIXED_SHIFT);

    printf("(1) sqrt()\n"
           "(2) cosf() (angle in degrees)\n"
           "(0) exit\n"
           "Your choice? ");
    scanf("%d", &choice);

    if (choice >= 1 && choice <= 2)
    {
      printf("Value? ");
      scanf("%f", &val);

      fixed_val = val; // Convert to int
      fixed_val = fixed_val << XY_FIXED_SHIFT;
      printf("In fixed: %d\n\n", fixed_val);

      if (choice == 1)
      {
        xy_answer = XY_sqrt(fixed_val);
        math_answer = sqrtf(val);
      }
      else if (choice == 2)
      {
        xy_answer = XY_cos(val);
        math_answer = cosf(val * (M_PIf / 180.0f));
      }
      else
        xy_answer = math_answer = 0;

      printf("libxycrt answer: %g (%d)\n", (double)xy_answer / (double)(2 << (XY_FIXED_SHIFT - 1)), xy_answer);
      printf("Math lib answer: %g\n", (double)math_answer);
      printf("\n");
    }
  } while (choice != 0);

  return (0);
}
