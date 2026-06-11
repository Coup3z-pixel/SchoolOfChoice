#ifndef PARTALLOC_H
#define PARTALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "schchprob.h"
#include "sprsmtrx.h"
#include "dgraph.h"

typedef struct Partial_Alloc {
  int no_students;
  int no_schools;
  dbl_sparse_matrix sparse;
} partial_alloc;

typedef struct Pure_Alloc {
  int no_students;
  int no_schools;
  int_sparse_matrix sparse;
} pure_alloc;

double get_entry(partial_alloc* alloc, int i, int j);

void set_entry(partial_alloc* alloc, int i, int j, double val);

double get_integer_entry(pure_alloc* alloc, int i, int j);

void set_integer_entry(pure_alloc* alloc, int i, int j, int val);

void increment_entry(partial_alloc* alloc, int i, int j, double incr);

double remaining_time(partial_alloc* alloc);

partial_alloc compute_demands(process_scp* myscp, double* cutoffs);

double get_total_demand_for_student(process_scp* myscp, partial_alloc* alloc, int i);

double get_total_demand_for_school(partial_alloc* alloc, int j);

double* school_sums(partial_alloc* my_alloc);

double* excess_demands(process_scp* myscp, partial_alloc* demands);

double sum_of_excesses(process_scp* myscp, double* cutoffs);

double* demand_deficits(process_scp* myscp, partial_alloc* demands);

double sum_of_deficits(process_scp* myscp, double* cutoffs);

int partial_allocs_are_same(partial_alloc* first, partial_alloc* second);

int students_are_fully_allocated(partial_alloc* my_alloc, process_scp* myscp);

int partial_alloc_is_consistent(partial_alloc* my_alloc);

int is_a_feasible_allocation(partial_alloc* my_alloc, process_scp* myscp);

int is_feasible_for_input_scp(partial_alloc* my_alloc, input_sch_ch_prob* myiscp);

int is_a_feasible_pure_alloc(pure_alloc* my_alloc, input_sch_ch_prob* myiscp);

int gives_some_student_nothing(partial_alloc* myalloc);

partial_alloc zero_alloc_for_process_scp(process_scp* myscp);

partial_alloc zero_alloc_for_input_scp(input_sch_ch_prob* myscp);

pure_alloc zero_pure_alloc_for_input_scp(input_sch_ch_prob* myscp);

partial_alloc left_sub_process_feasible_guide(partial_alloc* feasible_guide,
					    subset* J_subset, subset* P_subset);

partial_alloc right_sub_process_feasible_guide(partial_alloc* feasible_guide,
					  subset* J_subset, subset* P_subset);

void increment_partial_alloc(partial_alloc* base, partial_alloc* increment,
			     element_list* stu_index, element_list* sch_index);

partial_alloc copy_of_partial_alloc(partial_alloc* given);

int split_is_valid(partial_alloc* given, subset* J_subset, subset* P_subset);

/* At the end we need to pass from a partial_alloc whose values (which
   are doubles) are all close to 0 and 1, to the corresponding pure
   allocation, whose values are in {0,1}. */

pure_alloc pure_allocation_from_partial(partial_alloc* my_alloc);

partial_alloc partial_allocation_from_pure(pure_alloc* my_alloc);

int get_pure_entry(pure_alloc* alloc, int i, int j);

void set_pure_entry(pure_alloc* alloc, int i, int j, int val);

int pure_alloc_is_valid(pure_alloc* my_pure_alloc);

void increment_pure_entry(pure_alloc* alloc, int i, int j, int incr);

void print_sparse_partial_alloc(partial_alloc* my_alloc);

void print_partial_alloc(partial_alloc* my_alloc);

void fprint_partial_alloc(partial_alloc* my_alloc);

void print_pure_alloc(pure_alloc* my_pure_alloc);

void destroy_partial_alloc(partial_alloc my_alloc);

void destroy_pure_alloc(pure_alloc my_pure_alloc);

#endif /* PARTALLOC_H */
