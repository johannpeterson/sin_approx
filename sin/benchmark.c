
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics_double.h>

extern double sin_3(double x);

void test_value(double x) {
  double y, y1, y2, y3;
  y = gsl_sf_sin(x);
  y3 = sin_3(x);
  printf("gsl_sf_sin(%f) = %f\n", x, y);
  printf("Sin1(%f) = %f\n", x, y1);
  printf("Sin2(%f) = %f\n", x, y2);
  printf("Sin3(%f) = %f\n", x, y3);
  printf("\n");
}

gsl_rng * r;  /* global generator */

int main(int argc, char **argv) {
  int c;
  int points = 100;
  int cycles = 3;
  while ((c = getopt(argc, argv, "p:")) != -1) {
    switch (c) {
    case 'p':
      points = atoi(optarg);
      break;
    }
  }

  if (points < 1) {
    printf("Please enter a positive number of points to test.");
    exit(1);
  }

  double *x = malloc(points * sizeof(double));
  double *y1 = malloc(points * sizeof(double));
  double *y2 = malloc(points * sizeof(double));
  if (x == 0 || y1 == 0 || y2 == 0) {
    printf("Unable to allocate memory.");
    exit(1);
  }

  // set up RNG
  const gsl_rng_type * T;
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  printf ("generator type: %s\n", gsl_rng_name (r));
  printf ("seed = %lu\n", gsl_rng_default_seed);
  printf ("first value = %lu\n", gsl_rng_get (r));
  printf ("random double = %f\n", gsl_ran_flat(r, -M_PI, M_PI));

  for(int i=0; i < points; i++) {
    x[i] = gsl_ran_flat(r, 0, M_PI);
  }
  printf("x filled\n");
  double min, max;
  printf("mean(x) = %f\n", gsl_stats_mean(x, 1, points));
  gsl_stats_minmax(&min, &max, x, 1, points);
  printf ("range(x) = %f - %f\n", min, max);

  // timing
  clock_t begin, end;
  double time_spent;

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
  printf("time spent for sin_3: %f\n", time_spent);

  // clean up
  gsl_rng_free (r);
  free(x);
  free(y1);
  free(y2);
  return 0;
}
