#ifndef GCPSCODE_H
#define GCPSCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gcpscommon.h"

struct partial_alloc simple_GCPS_alloc(struct process_scp* input); 

struct partial_alloc GCPS_allocation(struct process_scp* input, int* no_segments, int* no_splits,
				     int* no_new_pivots, int* no_old_pivots, int* h_sum); 

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						struct pivot_list* probe_list,
						int* no_segments, int* no_splits,
						int* no_new_pivots,
						int* no_old_pivots, int* h_sum); 

void compute_next_path_segment_or_find_critical_pair(struct process_scp* input,
						     struct process_scp* working_scp,
						     struct partial_alloc* feasible_guide,
						     struct partial_alloc* final_alloc,
						     struct pivot_list* probe_list,
						     struct subset* P_subset,
						     struct subset* J_subset,
						     int* critial_pair_found,
						     int* depth, 
						     int* no_new_pivots, int* no_old_pivots,
						     int* h_sum);

void descend_to_left_subproblem(struct process_scp* working_scp, struct partial_alloc* final_alloc,
				struct partial_alloc* feasible_guide, 
				struct pivot_list* probe_list,
				struct subset* P_subset, struct subset* J_subset,
				int* no_segments, int* no_splits, int* no_new_pivots,
				int* no_old_pivots, int* h_sum);

void descend_to_right_subproblem(struct process_scp* working_scp,
				 struct partial_alloc* final_alloc,
				 struct partial_alloc* feasible_guide, 
				 struct pivot_list* probe_list,
				 struct subset* P_subset, struct subset* J_subset,
				 int* no_segments, int* no_splits, int* no_new_pivots,
				 int* no_old_pivots, int* h_sum);

void massage_theta_or_find_critical_pair(int** theta, int* theta_sums, struct subset* P_subset,
					 struct subset* J_subset, struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_school_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated,
					 struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum);

void mas_theta_or_find_crit_pair_for_sch(int j, int** theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated, struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum);

void compute_increments_and_o_h(struct subset* J_subset, struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_scp* working_scp,
				struct index* alpha, struct index* active_schools_index,
				struct index* omega, int** theta, int* theta_sums, int* favorites,
				int* fully_allocated, int sch, int* o_h,
				int* critical_pair_found, int* h_sum);

#endif /* GCPSCODE_H */
