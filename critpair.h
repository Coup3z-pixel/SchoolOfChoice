#ifndef CRITPAIR_H
#define CRITPAIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linprog.h"
#include "pushrelabel.h"
#include "pivot.h"
#include "endpoint.h"
#include "parser.h"

int** initialize_theta(struct partial_alloc* feasible_guide, int* favorites);

/* theta_sums[j-1] is the sum of the theta[i-1][j-1] */

int* initialize_theta_sums(int **theta, int nst, int nsc);

int student_qualified_for_school(int i, int j, struct partial_alloc* feasible_guide,int** theta,
				 int* favorites);

int school_qualified_for_student(int i, int j,
				 struct partial_alloc* feasible_guide,
				 int** theta,
				 struct process_scp* working_cee);

int pivot_is_valid(struct pivot* my_pivot, int** theta, int* theta_sums, struct index* alpha,
		   struct partial_alloc* feasible_guide, struct process_scp* working_cee,
		   int* favorites, int* fully_allocated);


void reuse_prior_pivots(struct pivot_list* old_list, struct pivot_list* new_list,
		       int** theta, int* theta_sums, struct index* alpha,
		       struct partial_alloc* feasible_guide, struct process_scp* working_cee,
			int* favorites, int* fully_allocated, int* no_old_pivots, int* h_sum);

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

struct pivot* extract_pivot(int** theta, struct index* alpha, int o_h, 
			   struct partial_alloc* feasible_guide,
			   struct process_scp* working_scp,
			   struct index_list* J_increments,
			   struct index_list* P_increments, int* favorites);

void next_J_h(struct subset* next_J_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct index* active_schools_index,
	      struct index* omega, int** theta, int* favorites);

void next_P_h(struct subset* next_P_subset, struct subset* J_subset,
	      struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct process_scp* working_scp,
	      struct index* alpha, int** theta);

/* alpha[i-1] is the index of the set of schools i might consume in working_scp */

void get_alpha(struct process_scp* working_scp, struct index* alpha);

/* fully_allocated[j-1] is 1 if feasible_guide fully allocates working_scp's j, 0 otherwise */

int* compute_fully_allocated(struct process_scp* working_scp,struct partial_alloc* feasible_guide);

void print_theta(int** theta, int nst, int nsc);

void destroy_alpha_or_omega(struct index* alpha, int nst);

void destroy_theta(int** theta, int nst);

#endif /* CRITPAIR_H */
