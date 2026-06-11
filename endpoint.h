#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"
#include "sprsmtrx.h"

/* After a feasible direction theta for adjusting the feasible guide
   has been determined, we first compute how long this trajectory for
   the feasible guide and allocating favorites for the final_alloc can
   go on before some condition is violated, then we adjust the
   final_alloc, the feasible_guide, and the working_scp. */

double time_until_some_school_exhausted(int* favorites, process_scp* working_scp);

double time_until_feasible_guide_not_above_alloc(int_sparse_matrix* theta,
						     int* favorites,
						     process_scp* working_scp,
						 partial_alloc* feasible_guide);

double time_until_feasible_guide_not_feasible(int_sparse_matrix* theta,
						  process_scp* working_scp,
					      partial_alloc* feasible_guide);

double time_until_trajectory_change(int_sparse_matrix* theta, 
				    int* favorites, process_scp* working_scp,
				    partial_alloc* feasible_guide);

double time_until_feasible_guide_not_above_allocOLD(int_sparse_matrix* theta,
						     element_list* alpha, int* favorites,
						     process_scp* working_scp,
						     partial_alloc* feasible_guide);

double time_until_feasible_guide_not_feasibleOLD(int_sparse_matrix* theta,
						  element_list* alpha, process_scp* working_scp,
						  partial_alloc* feasible_guide);

double time_until_trajectory_changeOLD(int_sparse_matrix* theta, element_list* alpha,
					int* favorites, process_scp* working_scp,
					partial_alloc* feasible_guide);

void augment_partial_alloc(partial_alloc* my_alloc, int* favorites, double delta);

void adjust_feasible_guide(partial_alloc* feasible_guide, process_scp* myscp,
			   int_sparse_matrix* theta,
			   int* favorites, double delta);

void adjust_feasible_guideOLD(partial_alloc* feasible_guide, int_sparse_matrix* theta,
			       element_list* alpha, int* favorites, double delta);

void decrement_working_scp(process_scp* working_scp, int* favorites, double delta);

void move_to_endpoint_of_segment(int_sparse_matrix* theta, 
				     int* favorites, process_scp* working_scp,
				     partial_alloc* feasible_guide,
				     partial_alloc* final_alloc);

void move_to_endpoint_of_segmentOLD(int_sparse_matrix* theta, element_list* alpha,
				     int* favorites, process_scp* working_scp,
				     partial_alloc* feasible_guide,
				     partial_alloc* final_alloc);

#endif /* ENDPOINT_H */
