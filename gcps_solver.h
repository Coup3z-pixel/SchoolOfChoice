#ifndef GCPS_SOLVER_H
#define GCPS_SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "push_relabel.h"

void get_alpha(struct process_cee* working_cee, struct index* alpha, int* upcount);

void destroy_alpha(struct index* alpha, int nst, int* downcount);

void get_favorites(struct process_cee* working_cee, int** preferences, int* favorites);

void compute_fully_allocated(int* fully_allocated, struct process_cee* working_cee,
			     struct partial_alloc* feasible_guide);

void initialize_theta(int** theta, struct partial_alloc* feasible_guide, int* favorites);


void next_J_h(struct subset* next_J_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct index* omega, int** theta, int* favorites);

void next_P_h(struct subset* next_P_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide, struct process_cee* working_cee,
	      struct index* alpha, int** theta);

void compute_increments_and_o_h(struct subset* J_subset, struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_cee* working_cee,
				struct index* alpha, int** theta, int* favorites,
				int* fully_allocated, int sch, int* o_h, int* h_sum);

void revise_theta(int** theta, struct index* alpha,  int o_h, 
		  struct partial_alloc* feasible_guide,
		  struct process_cee* working_cee,
		  struct index_list* J_increments,
		  struct index_list* P_increments, int* favorites);

void mas_theta_or_find_crit_pair_for_sch(int j, int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, int* favorites,
					 int* fully_allocated, int* pivots, int* h_sum);

void massage_theta_or_find_critical_pair(int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, int* favorites,
					 int* fully_allocated, int* pivots, int* h_sum);
  
int theta_is_valid(int** theta, struct process_cee* working_cee, struct partial_alloc* feasible_guide,
		   struct index* alpha, int* favorites, int* fully_allocated);

double time_until_some_max_exhaustion(int* favorites, struct process_cee* working_cee);

double time_until_some_school_exhaustion(int* favorites, struct process_cee* working_cee);

double time_until_feasible_guide_not_above_alloc(int** theta, struct index* alpha, int* favorites,
						 struct process_cee* working_cee,
						 struct partial_alloc* feasible_guide);

double time_until_feasible_guide_not_feasible(int** theta, struct index* alpha,
					      struct process_cee* working_cee,
					      struct partial_alloc* feasible_guide);

double time_until_trajectory_change(int** theta, struct index* alpha, int* favorites,
				    struct process_cee* working_cee,
				    struct partial_alloc* feasible_guide);

void decrement_working_cee(struct process_cee* working_cee, int* favorites, double delta);

void compute_next_path_segment(struct process_scp* input, struct process_scp* working_scp,
			       struct partial_alloc* feasible_guide,
			       struct partial_alloc* final_alloc, 
			       struct subset* P_subset, struct subset* J_subset,
			       int* depth, int* pivots, int* h_sum);

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						int* segments, int* splits,
						int* pivots, int* h_sum); 

struct partial_alloc GCPS_allocation(struct process_scp* input, int* segments, int* splits,
				     int* pivots, int* h_sum); 

#endif /* GCPS_SOLVER_H */
