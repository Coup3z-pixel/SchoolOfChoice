#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"

/* After a feasible direction theta for adjusting the feasible guide
   has been determined, we first compute how long this trajectory for
   the feasible guide and allocating favorites for the final_alloc can
   go on before some condition is violated, then we adjust the
   final_alloc, the feasible_guide, and the working_scp. */

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

void augment_partial_alloc(struct partial_alloc* my_alloc, int* favorites, double delta);

void adjust_feasible_guide(struct partial_alloc* feasible_guide, int** theta, struct index* alpha,
			   int* favorites, double delta);

void decrement_working_cee(struct process_cee* working_cee, int* favorites, double delta);

void move_to_endpoint_of_segment(int** theta, struct index* alpha, int* favorites,
				 struct process_scp* working_scp,
				 struct partial_alloc* feasible_guide,
				 struct partial_alloc* final_alloc);

#endif /* ENDPOINT_H */
