#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getbtheta.h"

element_list get_active_school_index(element_list* alpha, int nst, int nsc);

/*
int compute_next_path_segment_or_find_critical_pair(process_scp* working_scp,
						    partial_alloc* feasible_guide,
						    partial_alloc* final_alloc,
						    pivot_list* probe_list,
						    subset* P_subset,
						    subset* J_subset,
						    run_data* data);
*/

int NEW_compute_next_path_segment_or_find_critical_pair(process_scp* working_scp,
							partial_alloc* feasible_guide,
							partial_alloc* final_alloc,
							int allowed_speed_type,
							pivot_list* probe_list,
							subset* P_subset, subset* J_subset,
							run_data* data);

/*
double compute_until_next_critical_pair(process_scp* working_scp,
					partial_alloc* feasible_guide,
					partial_alloc* final_alloc,
					pivot_list* probe_list,
					subset* P_subset, subset* J_subset,
					run_data* data);
*/

double NEW_compute_until_next_critical_pair(process_scp* working_scp,
					    partial_alloc* feasible_guide,
					    partial_alloc* final_alloc,
					    int allowed_speed_type,
					    pivot_list* probe_list,
					    subset* P_subset, subset* J_subset,
					    run_data* data);

void print_btheta(int** btheta, int nst, int nsc);

void destroy_btheta(int** btheta, int nst);

#endif /* SEGMENT_H */
