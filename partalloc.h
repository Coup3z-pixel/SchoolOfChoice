#ifndef PARTALLOC_H
#define PARTALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subset.h"
#include "cee.h"

struct partial_alloc {
  int no_students;
  int no_schools;
  double* allocations; /* In the computer the no_students x no_schools matrix is a one dimensional
                       array with no_students * no_schools entries. */
};

struct partial_alloc zero_partial_alloc(struct double_cee my_cee);

void destroy_partial_alloc(struct partial_alloc my_partial_alloc);

void print_partial_alloc(struct partial_alloc my_partial_alloc);

#endif /* PARTALLOC_H */
