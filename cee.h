#ifndef CEE_H
#define CEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subset.h"

struct int_cee {
  int no_students;
  int no_schools;
  int* quotas;
  int** priority;
};

struct double_cee {
  int no_students;
  int no_schools;
  double* quotas;
  int** priority;
};

void destroy_int_cee(struct int_cee my_cee);

void destroy_double_cee(struct double_cee my_cee);

void print_int_cee(struct int_cee* my_cee);

void print_double_cee(struct double_cee* my_cee);

struct int_cee make_toy_cee();

struct double_cee double_cee_from_int_cee(struct int_cee* my_cee);

struct double_cee make_toy_double_cee();


#endif /* CEE_H */
