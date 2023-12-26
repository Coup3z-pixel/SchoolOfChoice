#ifndef PARTALLOC_H
#define PARTALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "schchprob.h"

struct partial_alloc {
  int no_students;
  int no_schools;
  double** allocations; 
};

struct pure_alloc {
  int no_students;
  int no_schools;
  int** allocations; 
};

struct partial_alloc zero_partial_alloc(struct double_cee* my_cee);

void destroy_partial_alloc(struct partial_alloc my_partial_alloc);

void destroy_pure_alloc(struct pure_alloc my_pure_alloc);

void print_partial_alloc(struct partial_alloc* my_partial_alloc);

void print_pure_alloc(struct pure_alloc my_pure_alloc);

void augment_partial_alloc(struct partial_alloc* my_alloc, int* favorites, double delta);

void adjust_feasible_guide(struct partial_alloc* feasible_guide, int** theta, int* favorites,
			   double delta);

struct partial_alloc allocate_until_new_time(struct sch_ch_prob* my_scp,
					     double new_time_remaining);

void increment_partial_alloc(struct partial_alloc* base, struct partial_alloc* increment,
			     struct index* stu_index,struct index* sch_index);

double* school_sums(struct partial_alloc* my_alloc);

int is_feasible_allocation(struct sch_ch_prob* my_scp, struct partial_alloc* my_alloc);

struct partial_alloc left_feasible_guide(struct partial_alloc* feasible_guide,
					 struct subset* J_subset);

struct partial_alloc right_feasible_guide(struct partial_alloc* feasible_guide,
					  struct subset* J_subset, struct subset* P_subset);

int partial_allocs_are_same(struct partial_alloc* first, struct partial_alloc* second);

#endif /* PARTALLOC_H */
