#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pivot.h"
#include "endpoint.h"
#include "parser.h"
#include "sprsmtrx.h"

typedef struct Run_Data {
  int no_segments;
  int no_splits;
  int no_old_pivots;
  int no_new_pivots;
  int h_sum;
} run_data;

element_list get_active_school_index(element_list* alpha, int nst, int nsc);

int* compute_fully_allocated(process_scp* working_scp, partial_alloc* feasible_guide);

int_sparse_matrix* initialize_theta(process_scp* working_scp, partial_alloc* feasible_guide,
					       int* favorites);

int* initialize_theta_sums(int_sparse_matrix* theta, int nst, int nsc);

int i_is_in_J_of_j(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* theta,
		   int* favorites);

subset J_of_j(int j, partial_alloc* feasible_guide, int_sparse_matrix* theta, int* favorites);

subset J_of_P(subset* P, partial_alloc* feasible_guide, int_sparse_matrix* theta, int* favorites);

int j_is_in_P_of_i(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* theta,
		   process_scp* working_scp);

subset P_of_i(int i, partial_alloc* feasible_guide, int_sparse_matrix* theta,
	      process_scp* working_scp);

subset P_of_J(subset* J, partial_alloc* feasible_guide, int_sparse_matrix* theta,
	      process_scp* working_scp);

pivot* extract_pivot(int_sparse_matrix* theta, int j_h, 
		     partial_alloc* feasible_guide,
		     process_scp* working_scp,
		     list_of_elt_lists* J_increments, list_of_elt_lists* P_increments,
		     int* favorites);

int pivot_is_valid(pivot* my_pivot,
		   int_sparse_matrix* theta, int* theta_sums,
		   partial_alloc* feasible_guide,
		   process_scp* working_scp,
		   int* favorites,
		   int* sch_fully_alloc);

void reuse_prior_pivots(pivot_list* old_list, pivot_list* new_list,
			int_sparse_matrix* theta, int* theta_sums,
			partial_alloc* feasible_guide, process_scp* working_scp,
			int* favorites, int* sch_fully_alloc,
			run_data* data);

subset next_J_increment(subset* J_subset, subset* P_increment, partial_alloc* feasible_guide,
			int_sparse_matrix* theta, int* favorites);

subset next_P_increment(subset* J_increment, subset* P_subset, partial_alloc* feasible_guide,
			int_sparse_matrix* theta, process_scp* working_scp);

int compute_increments_and_j_h(subset* J_subset, subset* P_subset,
			       list_of_elt_lists* J_increments, list_of_elt_lists* P_increments,
			       partial_alloc* feasible_guide,
			       process_scp* working_scp,
			       int_sparse_matrix* theta, int* theta_sums,
			       int* favorites,
			       int* sch_fully_alloc,
			       int sch,
			       int* j_h,
			       run_data* data);

int mas_theta_or_find_crit_pair_for_sch(int j,
					int_sparse_matrix* theta, int* theta_sums,
					subset* P_subset, subset* J_subset,
					process_scp* working_scp,
					partial_alloc* feasible_guide,
					int* favorites,
					int* sch_fully_alloc,
					pivot_list* new_list,
					run_data* data);

int massage_theta_or_find_critical_pair(int_sparse_matrix* theta, int* theta_sums,
					subset* P_subset, subset* J_subset,
					process_scp* working_scp,
					partial_alloc* feasible_guide,
					int* favorites,
					int* sch_fully_alloc,
					pivot_list* new_list,
					run_data* data);

int compute_next_path_segment_or_find_critical_pair(process_scp* working_scp,
						    partial_alloc* feasible_guide,
						    partial_alloc* final_alloc,
						    pivot_list* probe_list,
						    subset* P_subset,
						    subset* J_subset,
						    run_data* data);

void print_theta(int** theta, int nst, int nsc);

void destroy_theta(int** theta, int nst);

#endif /* SEGMENT_H */
