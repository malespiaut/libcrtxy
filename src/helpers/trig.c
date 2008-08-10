#include <stdio.h>
#include <math.h>

int main()
{
  long i, xxx;

  xxx = 1 << 16;

  for (i = 0; i < 360; i++)
  {
    printf("%6.0f, ", cos(i * M_PI / 180) * xxx);

    if (i == 90)
      printf("\n\n");
  }

  return(0);
}

