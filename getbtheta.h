#ifndef GETBTHETA_H
#define GETBTHETA_H

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
  
int_sparse_matrix* NEW_initialize_btheta(process_scp* working_scp, partial_alloc* feasible_guide,
					int* favorites, int* allowed_to_eat);

int* initialize_btheta_sums(int_sparse_matrix* btheta, int nst, int nsc);

int i_is_in_J_of_j(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* btheta,
		   int* favorites);

int NEW_i_is_in_J_of_j(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* btheta,
		       int* favorites, int* allowed_to_eat);

element_list* NEW_J_of_j(int j,
		  partial_alloc* feasible_guide,
		  int_sparse_matrix* btheta,
		  int* favorites,
		  int* allowed_to_eat);

element_list* NEW_J_of_P(subset* P, partial_alloc* feasible_guide, int_sparse_matrix* btheta,
		  int* favorites, int* allowed_to_eat);

int j_is_in_P_of_i(int i, int j, process_scp* working_scp);

subset P_of_i(int i, process_scp* working_scp);

element_list* NEW_P_of_i(int i, process_scp* working_scp);

subset P_of_J(subset* J, process_scp* working_scp);

element_list* NEW_next_J_increment(subset* J_subset, subset* P_increment,
				   partial_alloc* feasible_guide, int_sparse_matrix* btheta, 
				   int* favorites, int* allowed_to_eat);

subset next_P_increment(subset* J_increment, subset* P_subset, process_scp* working_scp);

int school_could_be_terminal(int* btheta_sums, int* sch_fully_alloc, int j);

int set_of_schools_has_terminal_element(int* btheta_sums, int* sch_fully_alloc,
					element_list* list);

correspondence* NEW_set_of_schools_for_each_student(element_list* domain, 
						    process_scp* working_scp);

correspondence* NEW_set_of_studants_for_each_school(element_list* domain,
						    partial_alloc* feasible_guide,
						    int_sparse_matrix* btheta,
						    int* favorites,
						    int* allowed_to_eat);

pivot* NEW_extract_pivot(correspondence_list* list, int j_h);

int pivot_is_valid(pivot* my_pivot,
		   int_sparse_matrix* btheta, int* btheta_sums,
		   partial_alloc* feasible_guide,
		   process_scp* working_scp,
		   int* favorites,
		   int* sch_fully_alloc);

int NEW_pivot_is_valid(pivot* my_pivot, correspondence_list* list);

void reuse_prior_pivots(pivot_list* old_list, pivot_list* new_list,
			int_sparse_matrix* btheta, int* btheta_sums,
			partial_alloc* feasible_guide, process_scp* working_scp,
			int* favorites, int* sch_fully_alloc,
			run_data* data);

int find_a_pivot_or_crit_pair_for_sch(int j,
				      int_sparse_matrix* btheta,  int* btheta_sums,
				      subset* P_subset, subset* J_subset,
				      process_scp* working_scp,
				      partial_alloc* feasible_guide,
				      int* favorites,
				      int* allowed_to_eat,
				      pivot_list* new_list,
				      run_data* data);

int find_a_btheta_or_crit_pair(int_sparse_matrix* btheta, int* btheta_sums,
			       subset* P_subset, subset* J_subset,
			       process_scp* working_scp,
			       partial_alloc* feasible_guide,
			       int* favorites,
			       int* allowed_to_eat,
			       int* sch_fully_alloc,
			       pivot_list* new_list,
			       run_data* data);

#endif /* GETBTHETA_H */
