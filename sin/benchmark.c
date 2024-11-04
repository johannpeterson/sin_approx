#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_statistics_double.h>

#include "mysin.h"

char* help_string =
  "benchmark - Timing program for sin functions.\n"
  "usage: benchmark [h] [-p npoints] [-c ncycles] [-m min -M max]\n\n"
  "    -p npoints          The number of x values to use for each cycle of testing\n"
  "                        Default 10000\n"
  "    -c ncycles          The number of test cycles to perform.\n"
  "                        Default 3\n"
  "    -m min -M max       Specify a range from which to draw x values for testing Sin(x).\n"
  "                        Defaults min=-Pi max=Pi\n"
  "    -A fname\n"
  "    -B fname            Choose functions to test by name.\n"
  "    -L                  List available functions.\n"
  "    -h                  Display this help.\n";

typedef double (*f_ptr)(double);

struct function_item {
  const char*   f_name;
  f_ptr         f_ptr;
};

struct function_item function_lookup[] = {
  {"sin1",      &sin_1},
  {"sin2",      &sin_2},
  {"sin3",      &sin_3},
  {"gslsin",    &gsl_sf_sin},
  {"reduce",    &reduce},
  {"gslReduce", &gslReduce},
  {"",          NULL}
};

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

void list_functions() {
  fprintf(stderr, "Available function options:\n");
  for(int i = 0; function_lookup[i].f_ptr != NULL; i++) {
    fprintf(stderr, "\t%s\n", function_lookup[i].f_name);
  }
}

f_ptr get_function(const char *f_name) {
  f_ptr f = NULL;
  int i = 0;
  for(int i = 0; function_lookup[i].f_ptr != NULL; i++) {
    if( 0 == strcmp(f_name, function_lookup[i].f_name) ) {
      if( (f = function_lookup[i].f_ptr) == NULL ) break;
    }
  }
  return f;
}

void print_stats(struct benchCycle *stats, int cycles) {
  printf("%5s %12s %12s %12s %12s %12s %12s %12s %12s\n",
         "cycle", "N points", "min x", "max x", "mean(err)",
         "min(err)", "max(err)", "op/sec A", "op/sec B");
  for (int c = 0; c < cycles; c++) {
    printf("%5d %12ld %12e %12e %12e %12e %12e %12e %12e\n",
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

void print_stats_old(struct benchCycle *stats, int cycles) {
  printf("cycle\t    N points\t       min x\t       max x\t    mean(err)\t    min(err)\t    max(err)\t    op/sec A\t    op/sec B\n");
  for (int c = 0; c < cycles; c++) {
    printf("%5d\t%12ld\t%12e\t%12e\t%12e\t%12e\t%12e\t%12e\t%12e\n",
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

gsl_rng *r; /* global random number generator */

int main(int argc, char **argv) {
  int points = 10000;
  int cycles = 3;
  double x_point;
  int single_point = 0;
  double min_x = -M_PI;
  double max_x = M_PI;
  struct function_item fA = {
    "sin3", NULL};
  struct function_item fB = {
    "gslsin", NULL};

  int c;
  while ((c = getopt(argc, argv, "c:p:m:M:hA:B:Lx:")) != -1) {
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
      fprintf(stderr, "%s\n", help_string);
      exit(0);
    case 'A':
      fA.f_name = optarg;
      break;
    case 'B':
      fB.f_name = optarg;
      break;
    case 'L':
      list_functions();
      exit(0);
    case 'x':
      x_point = atof(optarg);
      single_point = 1;
      break;
    }
  }

  // check parameters
  if (points < 1) {
    fprintf(stderr, "Please specify a positive number of test points per cycle.");
    exit(1);
  }
  if (cycles < 1) {
    fprintf(stderr, "Please specify a positive number of test cycles.");
    exit(1);
  }
  fA.f_ptr = get_function(fA.f_name);
  fB.f_ptr = get_function(fB.f_name);
  if (fA.f_ptr == NULL) {
    fprintf(stderr, "Unable to find function: %s\n", fA.f_name);
    exit(1);
  } else {
    fprintf(stderr, "function A = %s\n", fA.f_name);
  }
  if (fB.f_ptr == NULL) {
    fprintf(stderr, "Unable to find function: %s\n", fB.f_name);
    exit(1);
  } else {
    fprintf(stderr, "function B = %s\n", fB.f_name);
  }

  if (single_point) {
    double y1 = fA.f_ptr(x_point);
    double y2 = fB.f_ptr(x_point);
    double err = y1 - y2;
    printf("%24s %31s %31s\n", "x", "fA(x)", "fB(x)");
    printf("%+-20.17e\t%+-20.17e\t%+-20.17e\n", x_point, y1, y2);
    exit(0);
  }

  // allocate memory
  double *x = malloc(points * sizeof(double));
  double *y1 = malloc(points * sizeof(double));
  double *y2 = malloc(points * sizeof(double));
  double *err = malloc(points * sizeof(double));
  struct benchCycle *cycle_log = malloc(cycles * sizeof(struct benchCycle));
  if (x == NULL || y1 == NULL || y2 == NULL || err == NULL || cycle_log == NULL) {
    fprintf(stderr, "Unable to allocate memory.");
    exit(1);
  }

  // set up RNG
  const gsl_rng_type *T;
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc(T);

  fprintf(stderr, "generator type: %s\n", gsl_rng_name(r));
  fprintf(stderr, "seed = %lu\n", gsl_rng_default_seed);

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
      y1[i] = fA.f_ptr(x[i]);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    cycle_log[c].ops_per_sec_A = points / time_spent;

    begin = clock();
    for (int i = 0; i < points; i++) {
      y2[i] = fB.f_ptr(x[i]);
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
  free(err);
  free(cycle_log);
  return 0;
}
