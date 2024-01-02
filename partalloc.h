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


void destroy_partial_alloc(struct partial_alloc my_partial_alloc);

void destroy_pure_alloc(struct pure_alloc my_pure_alloc);

void print_partial_alloc(struct partial_alloc* my_partial_alloc);

void print_pure_alloc(struct pure_alloc my_pure_alloc);

struct partial_alloc zero_alloc_for_process_scp(struct process_cee* my_cee);

void augment_partial_alloc(struct partial_alloc* my_alloc, int* favorites, double delta);

void adjust_feasible_guide(struct partial_alloc* feasible_guide, int** theta, struct index* alpha,
			   int* favorites, double delta);

void increment_partial_alloc(struct partial_alloc* base, struct partial_alloc* increment,
			     struct index* stu_index,struct index* sch_index);

double* school_sums(struct partial_alloc* my_alloc);


struct partial_alloc left_feasible_guide(struct partial_alloc* feasible_guide,
					 struct subset* J_subset);

struct partial_alloc right_feasible_guide(struct partial_alloc* feasible_guide,
					  struct subset* J_subset, struct subset* P_subset);

int partial_allocs_are_same(struct partial_alloc* first, struct partial_alloc* second);

#endif /* PARTALLOC_H */
