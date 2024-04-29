#ifndef GCPS_SOLVER_H
#define GCPS_SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "push_relabel.h"
#include "pivot.h"
#include "endpoint.h"

void get_alpha(struct process_cee* working_cee, struct index* alpha);

void destroy_alpha_or_omega_or_theta(struct index* alpha, int nst, int* downcount);

void destroy_theta(int** theta, int nst); 

int* get_favorites(struct process_cee* working_cee, int** preferences);

int* compute_fully_allocated(struct process_cee* working_cee,struct partial_alloc* feasible_guide);

int** initialize_theta(struct partial_alloc* feasible_guide, int* favorites);

int* initialize_theta_sums(int **theta, int nst, int nsc);

void next_J_h(struct subset* next_J_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct index* active_schools_index,
	      struct index* omega, int** theta, int* favorites);

void next_P_h(struct subset* next_P_subset, struct subset* J_subset,
	      struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct process_cee* working_cee,
	      struct index* alpha, int** theta);

void compute_increments_and_o_h(struct subset* J_subset, struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_cee* working_cee,
				struct index* alpha, struct index* active_schools_index,
				struct index* omega, int** theta, int* theta_sums, int* favorites,
				int* fully_allocated, int sch, int* o_h,
				int* critical_pair_found, int* h_sum);

struct pivot* extract_pivot(int** theta, struct index* alpha, int o_h, 
			   struct partial_alloc* feasible_guide,
			   struct process_cee* working_cee,
			   struct index_list* J_increments,
			   struct index_list* P_increments, int* favorites);

void mas_theta_or_find_crit_pair_for_sch(int j, int** theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated, struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum);

void massage_theta_or_find_critical_pair(int** theta, int* theta_sums, struct subset* P_subset,
					 struct subset* J_subset, struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_school_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated,
					 struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum);

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

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						struct pivot_list* probe_list,
						int* no_segments, int* no_splits,
						int* no_new_pivots,
						int* no_old_pivots, int* h_sum); 

struct partial_alloc GCPS_allocation(struct process_scp* input, int* no_segments, int* no_splits,
				     int* no_new_pivots, int* no_old_pivots, int* h_sum); 

#endif /* GCPS_SOLVER_H */
