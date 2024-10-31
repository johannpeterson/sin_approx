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

struct benchCycle {
  long          points;
  double        minx;
  double        maxx;
  double        mean_err;
  double        min_err;
  double        max_err;
  double        ops_per_sec_A;
  double        ops_per_sec_B;
};

void print_stats(struct benchCycle *stats, int cycles) {
  printf("cycle\tN points\tmin x\tmax x\tmean(err)\tmin(err)\tmax(err)\top/sec A\top/sec B\n");
  for (int c = 0; c < cycles; c++) {
    printf("%d\t%ld\t%e\t%e\t%e\t%e\t%e\t%e\t%e\n",
           c,
           stats[c].points,
           stats[c].minx,
           stats[c].maxx,
           stats[c].mean_err,
           stats[c].min_err,
           stats[c].max_err,
           stats[c].ops_per_sec_A,
           stats[c].ops_per_sec_B);
  }
}

int main(int argc, char **argv) {
  int c;
  int points = 10000;
  int cycles = 3;
  double min_x = -M_PI;
  double max_x = M_PI;
  while ((c = getopt(argc, argv, "c:p:m:M:h")) != -1) {
    switch (c) {
    case 'p':
      points = atoi(optarg);
      break;
    case 'c':
      cycles = atoi(optarg);
      break;
    case 'm':
      min_x = atof(optarg);
      break;
    case 'M':
      max_x = atof(optarg);
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
  struct benchCycle *cycle_log = malloc(cycles * sizeof(struct benchCycle));
  if (x == NULL || y1 == NULL || y2 == NULL || err == NULL || cycle_log == NULL) {
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
  double min, max;

  for (int c = 0; c < cycles; c++) {
    for (int i = 0; i < points; i++) {
      x[i] = gsl_ran_flat(r, min_x, max_x);
    }
    cycle_log[c].points = points;
    gsl_stats_minmax(&(cycle_log[c].minx), &(cycle_log[c].maxx), x, 1, points);

    // timing
    begin = clock();
    for (int i = 0; i < points; i++) {
      y1[i] = gsl_sf_sin(x[i]);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    cycle_log[c].ops_per_sec_A = points / time_spent;

    begin = clock();
    for (int i = 0; i < points; i++) {
      y2[i] = sin_3(x[i]);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    cycle_log[c].ops_per_sec_B = points / time_spent;

    for (int i = 0; i < points; i++) {
      err[i] = fabs(y1[i] - y2[i]);
    }
    cycle_log[c].mean_err = gsl_stats_mean(err, 1, points);
    gsl_stats_minmax(&cycle_log[c].min_err, &cycle_log[c].max_err, err, 1, points);
  }

  print_stats(cycle_log, cycles);

  // clean up
  gsl_rng_free(r);
  free(x);
  free(y1);
  free(y2);
  return 0;
}
