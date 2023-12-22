#ifndef FRACCEE_H
#define FRACCEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
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
};

void destroy_frac_cee(struct frac_cee my_frac);

void destroy_frac_scp(struct frac_scp my_scp);

void print_frac_cee(struct frac_cee* my_frac);

void print_frac_scp(struct frac_scp* my_frac);

void copy_frac_cee(struct frac_cee* my_cee, struct frac_cee* copy);

void copy_frac_scp(struct frac_scp* my_scp, struct frac_scp* copy);

struct frac_cee make_toy_frac_cee();

struct frac_scp make_toy_frac_scp();

struct partial_alloc zero_alloc_for_frac(struct frac_cee* my_cee);

struct frac_scp critical_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset);

struct frac_scp crit_compl_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset);


int push_is_valid(int source, int target,
		  double** capacities, double** preflows, double* excess, int* labels);

void push(int source, int target, double** capacities, double** preflows, double* excess);

int relabel_is_valid(int node, double** capacities, double** preflows,
		     double* excess, int* labels, int nst, int nsc);

void relabel(int node, double** capacities, double** preflows, int* labels, int nst, int nsc);

int push_relabel(struct frac_cee* input, struct partial_alloc* max_flow_alloc);


void get_alpha(struct frac_cee* working_cee, struct index* alpha);

void get_favorites(struct frac_cee* working_cee, int** preferences, int* favorites);

void initialize_theta(int** theta, struct partial_alloc* final_alloc,
		      struct partial_alloc* feasible_guide, int* favorites);


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
				   struct frac_scp* working_scp,
				   struct partial_alloc* feasible_guide);

struct partial_alloc GCPS_allocation(struct frac_scp* input); 

#endif /* FRACCEE_H */
