#include <stdio.h>

extern double sin_1(double x);

int main(int argc, const char * argv[]) {
  double x, y;
  x = 2.0;
  y = sin_1(x);
  printf("Sin1(%f) = %f\n", x, y);
  return 0;
}
