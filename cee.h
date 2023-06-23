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

void destroy_int_cee(struct int_cee* my_cee);

void destroy_double_cee(struct double_cee* my_cee);

void print_int_cee(struct int_cee* my_cee);

void print_double_cee(struct double_cee* my_cee);

struct int_cee make_toy_cee();

struct double_cee double_cee_from_int_cee(struct int_cee* my_cee);

struct double_cee make_toy_double_cee();

void sub_double_cee(struct double_cee* given_cee, struct double_cee* sub_cee,
		    int no_stu, int no_sch, int* stu_index, int* sch_index);

int* popular_schools(struct double_cee* my_cee);

void relatedness_matrix(struct double_cee* my_cee, int* popular, struct square_matrix* rel_mat);

int minimum_gmc_inequality(struct double_cee* my_cee, struct subset* school_subset);

int gmc_holds(struct double_cee* my_cee);

#endif /* CEE_H */
