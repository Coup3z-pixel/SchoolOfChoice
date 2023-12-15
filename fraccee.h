#ifndef FRACCEE_H
#define FRACCEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subset.h"
#include "partalloc.h"

struct frac_cee {
  int no_students;
  int no_schools;
  double* quotas;
  double** maximums;
  double time_remaining;
};

struct frac_scp {
  struct frac_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  double time_remaining;
};

void destroy_frac_cee(struct frac_cee my_frac);

void print_frac_cee(struct frac_cee* my_frac);

void copy_frac_cee(struct frac_cee* my_cee, struct frac_cee* copy);

struct frac_cee make_toy_frac();

struct partial_alloc zero_alloc_for_frac(struct frac_cee* my_cee); 

int push_is_valid(int source, int target,
		  double** capacities, double** preflows, double* excess, int* labels);

void push(int source, int target, double** capacities, double** preflows, double* excess);

int relabel_is_valid(int node, double** capacities, double** preflows,
		     double* excess, int* labels, int nst, int nsc);

void relabel(int node, double** capacities, double** preflows, int* labels, int nst, int nsc);

int push_relabel(struct frac_cee* input, struct partial_alloc* max_flow_alloc);

void get_alpha(struct frac_cee* working_cee, struct index* alpha);

void get_favorites(struct frac_cee* working_cee, int** preferences, int* favorites);

void initialize_theta(int** theta, struct partial_alloc* final_alloc_ptr,
		      struct partial_alloc* feasible_guide_ptr, int* favorites);

int compute_J_h(struct index* next_J_index, struct subset* J_subset, struct subset* P_subset,
		struct partial_alloc* final_alloc_ptr, struct partial_alloc* feasible_guide_ptr,
		struct index* alpha, int nsc, int nst);

int compute_P_h(struct index* next_P_index, struct subset* J_subset, struct subset* P_subset,
		struct partial_alloc* feasible_guide_ptr, struct frac_cee* working_cee,
		struct index* alpha, int nsc, int nst);

void revise_theta(int** theta, int o_h, struct frac_cee* working_cee,
		  struct partial_alloc* final_alloc_ptr, struct partial_alloc* feasible_guide_ptr,
		  struct unordered_subset_list* J_increments,
		  struct unordered_subset_list* P_increments);

void mas_theta_or_find_crit_pair_for_sch(int j, int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* final_alloc_ptr,
					 struct partial_alloc* feasible_guide_ptr,
					 struct index* alpha);

void massage_theta_or_find_critical_pair(int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* final_alloc_ptr,
					 struct partial_alloc* feasible_guide_ptr,
					 struct index* alpha);

struct partial_alloc GCPS_allocation(struct frac_scp* input); 

#endif /* FRACCEE_H */
