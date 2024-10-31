#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_statistics_double.h>

#include "mysin.h"

char* help_string =
  "benchmark - Timing program for sin functions.\n"
  "usage: benchmark [-p npoints] [-c ncycles] [-m min -M max]\n\n"
  "    -p npoints          The number of x values to use for each cycle of testing\n"
  "                        Default 10000\n"
  "    -c ncycles          The number of test cycles to perform.\n"
  "                        Default 3\n"
  "    -m min -M max       Specify a range from which to draw x values for testing Sin(x).\n"
  "                        Defaults min=-Pi max=Pi\n";

gsl_rng *r; /* global random number generator */

int main(int argc, char **argv) {
  int c;
  int points = 10000;
  int cycles = 3;
  double min = -M_PI;
  double max = M_PI;
  while ((c = getopt(argc, argv, "c:p:m:M:h")) != -1) {
    switch (c) {
    case 'p':
      points = atoi(optarg);
      break;
    case 'c':
      cycles = atoi(optarg);
      break;
    case 'm':
      min = atof(optarg);
      break;
    case 'M':
      max = atof(optarg);
      break;
    case 'h':
      printf("%s\n", help_string);
      exit(0);
    }
  }

  // check parameters
  if (points < 1) {
    printf("Please specify a positive number of test points per cycle.");
    exit(1);
  }
  if (cycles < 1) {
    printf("Please specify a positive number of test cycles.");
    exit(1);
  }

  // allocate memory
  double *x = malloc(points * sizeof(double));
  double *y1 = malloc(points * sizeof(double));
  double *y2 = malloc(points * sizeof(double));
  double *err = malloc(points * sizeof(double));
  if (x == 0 || y1 == 0 || y2 == 0 || err == 0) {
    printf("Unable to allocate memory.");
    exit(1);
  }

  // set up RNG
  const gsl_rng_type *T;
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc(T);

  printf("generator type: %s\n", gsl_rng_name(r));
  printf("seed = %lu\n", gsl_rng_default_seed);

  clock_t begin, end;
  double time_spent;

  for (int c = 1; c <= cycles; c++) {
    printf("cycle: %i\n", c);
    for (int i = 0; i < points; i++) {
      x[i] = gsl_ran_flat(r, 10e9 - M_PI, 10e9 + M_PI);
    }

    // timing
    begin = clock();
    for (int i = 0; i < points; i++) {
      y1[i] = gsl_sf_sin(x[i]);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time for gsl_sf_sin: %f\n", time_spent);

    begin = clock();
    for (int i = 0; i < points; i++) {
      y2[i] = sin_3(x[i]);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time for sin_3: %f\n", time_spent);

    for (int i = 0; i < points; i++) {
      err[i] = y1[i] - y2[i];
    }
    double min, max;
    printf("mean(err) = %e\n", gsl_stats_mean(err, 1, points));
    gsl_stats_minmax(&min, &max, err, 1, points);
    printf("range(err) = %e - %e\n", min, max);
  }

  // clean up
  gsl_rng_free(r);
  free(x);
  free(y1);
  free(y2);
  return 0;
}
