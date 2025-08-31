#ifndef PARTALLOC_H
#define PARTALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "schchprob.h"
#include "sprsmtrx.h"

struct partial_alloc {
  int no_students;
  int no_schools;
  struct dbl_sparse_matrix sparse;
};

struct pure_alloc {
  int no_students;
  int no_schools;
  struct int_sparse_matrix sparse;
};

double get_entry(struct partial_alloc* alloc, int i, int j);

void set_entry(struct partial_alloc* alloc, int i, int j, double val);

void increment_entry(struct partial_alloc* alloc, int i, int j, double incr);

struct partial_alloc compute_demands(struct process_scp* myscp, double* cutoffs);

double get_total_demand_for_student(struct process_scp* myscp, struct partial_alloc* alloc, int i);

double get_total_demand_for_school(struct partial_alloc* alloc, int j);

double* school_sums(struct partial_alloc* my_alloc);

double* excess_demands(struct process_scp* myscp, struct partial_alloc* demands);

double sum_of_excesses(struct process_scp* myscp, double* cutoffs);

double* demand_deficits(struct process_scp* myscp, struct partial_alloc* demands);

double sum_of_deficits(struct process_scp* myscp, double* cutoffs);

int partial_allocs_are_same(struct partial_alloc* first, struct partial_alloc* second);

int students_are_fully_allocated(struct partial_alloc* my_alloc);

int is_a_feasible_allocation(struct partial_alloc* my_alloc, struct process_scp* my_scp);

int partial_alloc_is_consistent(struct partial_alloc* my_alloc);

struct partial_alloc zero_alloc_for_process_scp(struct process_scp* my_scp);

struct partial_alloc zero_alloc_for_input_scp(struct input_sch_ch_prob* my_scp);

struct partial_alloc left_sub_process_feasible_guide(struct partial_alloc* feasible_guide,
					    struct subset* J_subset, struct subset* P_subset);

struct partial_alloc right_sub_process_feasible_guide(struct partial_alloc* feasible_guide,
					  struct subset* J_subset, struct subset* P_subset);

void increment_partial_alloc(struct partial_alloc* base, struct partial_alloc* increment,
			     struct index* stu_index,struct index* sch_index);

struct partial_alloc copy_of_partial_alloc(struct partial_alloc* given);

/* At the end we need to pass from a partial_alloc whose values (which
   are doubles) are all close to 0 and 1, to the corresponding pure
   allocation, whose values are in {0,1}. */

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc);

int get_pure_entry(struct pure_alloc* alloc, int i, int j);

void set_pure_entry(struct pure_alloc* alloc, int i, int j, int val);

int pure_alloc_is_valid(struct pure_alloc* my_pure_alloc);

void increment_pure_entry(struct pure_alloc* alloc, int i, int j, int incr);

void print_sparse_partial_alloc(struct partial_alloc* my_alloc);

void print_partial_alloc(struct partial_alloc* my_alloc);

void print_pure_alloc(struct pure_alloc* my_pure_alloc);

void destroy_partial_alloc(struct partial_alloc my_alloc);

void destroy_pure_alloc(struct pure_alloc my_pure_alloc);

#endif /* PARTALLOC_H */
