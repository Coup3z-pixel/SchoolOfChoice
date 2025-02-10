#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linprog.h"
#include "pushrelabel.h"
#include "pivot.h"
#include "endpoint.h"
#include "parser.h"
#include "sprsmtrx.h"

struct index*  get_alpha(struct process_scp* working_scp);

struct index get_active_school_index(struct index* alpha, int nst, int nsc);

struct index*  get_omega(struct index* alpha, struct index* active_school_index, int nst);

int* compute_fully_allocated(struct process_scp* working_scp,
			     struct partial_alloc* feasible_guide);

struct int_sparse_matrix* initialize_theta(struct process_scp* working_scp,
					       struct partial_alloc* feasible_guide,
					       int* favorites);

int* initialize_theta_sums(struct int_sparse_matrix* theta, int nst, int nsc);

int student_qualified_for_school(int i, int j, struct partial_alloc* feasible_guide,
				 struct int_sparse_matrix* theta, int* favorites);

int school_qualified_for_student(int i, int j, struct partial_alloc* feasible_guide,
				 struct int_sparse_matrix* theta,
				 struct process_scp* working_scp);

int pivot_is_valid(struct pivot* my_pivot, struct int_sparse_matrix* theta, int* theta_sums,
		   struct index* alpha, struct partial_alloc* feasible_guide,
		   struct process_scp* working_scp, int* favorites, int* sch_fully_alloc);

void reuse_prior_pivots(struct pivot_list* old_list, struct pivot_list* new_list,
			struct int_sparse_matrix* theta, int* theta_sums, struct index* alpha,
			struct partial_alloc* feasible_guide, struct process_scp* working_scp,
			int* favorites, int* sch_fully_alloc, int* no_old_pivots, int* h_sum);

struct subset next_J_g(struct subset* J_subset, struct subset* P_subset,
		       struct partial_alloc* feasible_guide, struct index* active_schools_index,
		       struct index* omega, struct int_sparse_matrix* theta, int* favorites);

struct subset next_P_g(struct subset* J_subset, struct subset* P_subset,
		       struct partial_alloc* feasible_guide,
		       struct process_scp* working_scp,
		       struct index* alpha, struct int_sparse_matrix* theta); 

void compute_increments_and_j_h(struct subset* J_subset, struct subset* P_subset,
				struct index_list* J_increments, struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_scp* working_scp,
				struct index* alpha, struct index* active_schools_index,
				struct index* omega, struct int_sparse_matrix* theta,
				int* theta_sums, int* favorites,
				int* sch_fully_alloc, int sch, int* j_h,
				int* critical_pair_found, int* h_sum);

struct pivot* extract_pivot(struct int_sparse_matrix* theta, struct index* alpha, int j_h, 
			    struct partial_alloc* feasible_guide,
			    struct process_scp* working_scp,
			    struct index_list* J_increments, struct index_list* P_increments,
			    int* favorites);

void mas_theta_or_find_crit_pair_for_sch(int j, struct int_sparse_matrix* theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* sch_fully_alloc, struct pivot_list* new_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum);

void massage_theta_or_find_critical_pair(struct int_sparse_matrix* theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha,
					 struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* sch_fully_alloc,
					 struct pivot_list* new_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum);

void compute_next_path_segment_or_find_critical_pair(struct process_scp* working_scp,
						     struct partial_alloc* feasible_guide,
						     struct partial_alloc* final_alloc,
						     struct pivot_list* probe_list,
						     struct subset* P_subset,
						     struct subset* J_subset,
						     int* critial_pair_found,
						     int* depth, 
						     int* no_new_pivots, int* no_old_pivots,
						     int* h_sum);

void print_theta(int** theta, int nst, int nsc);

void destroy_alpha_or_omega(struct index* alpha, int nst);

void destroy_theta(int** theta, int nst);

#endif /* SEGMENT_H */
