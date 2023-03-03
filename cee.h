#ifndef CEE_H
#define CEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subset.h"

struct int_cee {
  char name[20];
  int no_students;
  int no_schools;
  int* requirements;
  int* quotas;
  int* eligibility; /* In the computer the no_students x no_schools matrix is a one dimensional
                       array with no_students * no_schools entries. */
};

struct double_cee {
  char name[20];
  int no_students;
  int no_schools;
  double* requirements;
  double* quotas;
  int* eligibility; /* In the computer the no_students x no_schools matrix is a one dimensional
                       array with no_students * no_schools entries. */
};

struct int_cee make_toy_cee();

void destroy_int_cee(struct int_cee my_cee);

void destroy_double_cee(struct double_cee my_cee);

struct double_cee double_cee_from_int_cee(struct int_cee my_cee);

void print_int_cee(struct int_cee my_cee);

void print_double_cee(struct double_cee my_cee);

int minimum_gmc_inequality(struct double_cee my_cee, struct subset school_subset);

int gmc_holds(struct double_cee my_cee);

#endif /* CEE_H */
