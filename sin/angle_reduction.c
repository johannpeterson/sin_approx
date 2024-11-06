#include <gsl/gsl_math.h>

// This extracts the angle-reduction logic from
// gsl_sf_sin in the specfunc/trig.c file from GSL.

double gslReduceBase(double x) {
  const double P1 = 7.85398125648498535156e-1;
  const double P2 = 3.77489470793079817668e-8;
  const double P3 = 2.69515142907905952645e-15;

  const double abs_x = fabs(x);

  double y = floor(abs_x / (0.25 * M_PI));
  double z = ((abs_x - y * P1) - y * P2) - y * P3;
  double t = 8.0 * fabs(z) / M_PI - 1.0;

  return t;
}

// The function gslReducebase maps each octant
// onto [-1, +1].  So scale it to [0, Pi/4].

double gslReduce(double x) {
  double y = gslReduceBase(x);
  y = (y + 1)*M_PI_4/2;
  return y;
}
