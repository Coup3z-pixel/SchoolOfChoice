#ifndef GCPS_SOLVER_H
#define GCPS_SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "push_relabel.h"

void get_alpha(struct frac_cee* working_cee, struct index* alpha);

void get_favorites(struct frac_cee* working_cee, int** preferences, int* favorites);

void initialize_theta(int** theta, struct partial_alloc* feasible_guide, int* favorites);


void next_J_h(struct subset* next_J_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct index* alpha, int** theta, int* favorites);

void next_P_h(struct subset* next_P_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide, struct frac_cee* working_cee,
	      struct index* alpha, int** theta);

void compute_increments_and_o_h(struct subset* J_subset, struct subset* P_subset,
				struct unordered_subset_list* J_increments,
				struct unordered_subset_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct frac_cee* working_cee,
				struct index* alpha, int** theta, int* favorites,
				int sch, int* o_h);

void revise_theta(int** theta, struct index* alpha, int o_h, 
		  struct partial_alloc* feasible_guide,
		  struct frac_cee* working_cee,
		  struct unordered_subset_list* J_increments,
		  struct unordered_subset_list* P_increments, int* favorites);

void mas_theta_or_find_crit_pair_for_sch(int j, int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, int* favorites);

void massage_theta_or_find_critical_pair(int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, int* favorites);

double time_until_some_max_exhaustion(int* favorites, struct frac_cee* working_cee);

double time_until_some_school_exhaustion(int* favorites, struct frac_cee* working_cee);

double time_until_feasible_guide_not_above_alloc(int** theta, int* favorites,
						 struct frac_cee* working_cee,
						 struct partial_alloc* feasible_guide);

double time_until_feasible_guide_not_feasible(int** theta, struct frac_cee* working_cee,
					      struct partial_alloc* feasible_guide);

double time_until_trajectory_change(int** theta, int* favorites,
				    struct frac_cee* working_cee,
				    struct partial_alloc* feasible_guide);

void decrement_working_cee(struct frac_cee* working_cee, int* favorites, double delta);

void destroy_GCPS_allocation_stuff(int nst, struct subset* P_subset, struct subset* J_subset,
				   int** theta, int* favorites, struct index* alpha,
				   struct frac_scp* working_scp);

struct partial_alloc GCPS_allocation_with_guide(struct frac_scp* input,
						struct partial_alloc* feasible_guide); 

struct partial_alloc GCPS_allocation(struct frac_scp* input); 

#endif /* GCPS_SOLVER_H */
