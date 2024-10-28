#include <stdio.h>
#include <gsl/gsl_sf_trig.h>

extern double sin_1(double x);
extern double sin_2(double x);

void test_value(double x) {
  double y, y1, y2;
  y = gsl_sf_sin(x);
  y1 = sin_1(x);
  y2 = sin_2(x);
  printf("gsl_sf_sin(%f) = %f\n", x, y);
  printf("Sin1(%f) = %f\n", x, y1);
  printf("Sin2(%f) = %f\n\n", x, y2);
}

int main(int argc, const char * argv[]) {
  test_value(0.0);
  test_value(1);
  test_value(1.5708);
  test_value(3.14);

  return 0;
}
