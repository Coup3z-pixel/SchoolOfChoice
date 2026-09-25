#include "segment.h"

/* The next function begins at a partial allocation and feasible
   guide.  It attempts to compute a btheta, which is a direction for
   moving the feasible guide.  This attempt either succeeds, in which
   case we move everything to the new endpoint for the partial
   allocation and feasible guide, or it finds a critical pair. */

/*
int compute_next_path_segment_or_find_critical_pair(process_scp* working_scp,
						    partial_alloc* feasible_guide,
						    partial_alloc* final_alloc,
						    pivot_list* probe_list,
						    subset* P_subset, subset* J_subset,
						    run_data* data) {
  int nst, nsc, critical_pair_found;

  int* sch_fully_alloc;
  int* favorites;
  int_sparse_matrix* btheta;
  int* btheta_sums;
  pivot_list list_of_pivots;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;
  
  sch_fully_alloc = compute_fully_allocated_schools(working_scp, feasible_guide);
  favorites = get_favorites(working_scp);
  btheta = initialize_btheta(working_scp, feasible_guide, favorites);
  btheta_sums = initialize_btheta_sums(btheta, nst, nsc);

  list_of_pivots = void_pivot_list();
  
  reuse_prior_pivots(probe_list, &list_of_pivots, btheta, btheta_sums, feasible_guide,
		     working_scp, favorites, sch_fully_alloc, data);
  
  destroy_pivot_list(*probe_list);
  *probe_list = void_pivot_list();
    
  critical_pair_found = massage_btheta_or_find_critical_pair(btheta, btheta_sums,
							     P_subset, J_subset,
							     working_scp,
							     feasible_guide, 
							     favorites,
							     sch_fully_alloc,
							     &list_of_pivots,
							     data);
  
  if (!critical_pair_found) {
    data->no_segments++;

    move_to_endpoint_of_segment(btheta, favorites, working_scp, feasible_guide, final_alloc);    
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);
  
  free(sch_fully_alloc);  
  destroy_pivot_list(list_of_pivots);
  destroy_int_sp_mat(btheta);
  free(btheta);
  free(btheta_sums);
  free(favorites);

  return critical_pair_found;
}
*/

int NEW_compute_next_path_segment_or_find_critical_pair(process_scp* working_scp,
							partial_alloc* feasible_guide,
							partial_alloc* final_alloc,
							int allowed_speed_type,
							pivot_list* probe_list,
							subset* P_subset, subset* J_subset,
							run_data* data) {
  int i, nst, nsc, critical_pair_found;

  int* sch_fully_alloc;
  int* favorites;
  int* allowed_to_eat;
  double* eating_speeds;
  int_sparse_matrix* btheta;
  int* btheta_sums;
  pivot_list list_of_pivots;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  allowed_to_eat = malloc(nst * sizeof(int));
  eating_speeds = malloc(nst * sizeof(double));
  if (allowed_speed_type == 0) {
    for (i = 1; i <= nst; i++) {
      allowed_to_eat[i-1] = 1;
      eating_speeds[i-1] = 1.0;
    }
  }
  
  sch_fully_alloc = compute_fully_allocated_schools(working_scp, feasible_guide);
  favorites = get_favorites(working_scp);
  btheta = NEW_initialize_btheta(working_scp, feasible_guide, favorites, allowed_to_eat);
  btheta_sums = initialize_btheta_sums(btheta, nst, nsc);

  list_of_pivots = void_pivot_list();
  
  reuse_prior_pivots(probe_list, &list_of_pivots, btheta, btheta_sums, feasible_guide,
		     working_scp, favorites, sch_fully_alloc, data);
  
  destroy_pivot_list(*probe_list);
  *probe_list = void_pivot_list();
    
  critical_pair_found = find_a_btheta_or_crit_pair(btheta, btheta_sums,
						   P_subset, J_subset,
						   working_scp,
						   feasible_guide, 
						   favorites,
						   allowed_to_eat,
						   sch_fully_alloc,
						   &list_of_pivots,
						   data);
  
  if (!critical_pair_found) {
    data->no_segments++;

    NEW_move_to_endpoint_of_segment(btheta, favorites, eating_speeds,
				    working_scp, feasible_guide, final_alloc);    
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);
  
  free(sch_fully_alloc);  
  destroy_pivot_list(list_of_pivots);
  destroy_int_sp_mat(btheta);
  free(btheta);
  free(btheta_sums);
  free(favorites);
  free(allowed_to_eat);
  free(eating_speeds);

  return critical_pair_found;
}

/*
double compute_until_next_critical_pair(process_scp* working_scp,
					partial_alloc* feasible_guide,
					partial_alloc* final_alloc,
					pivot_list* probe_list,
					subset* P_subset, subset* J_subset,
					run_data* data) {
  int critical_pair_found;

  critical_pair_found = 0;

  while (!critical_pair_found && working_scp->time_remaining > 0.0000001) {

    critical_pair_found = compute_next_path_segment_or_find_critical_pair(working_scp,
									  feasible_guide,
									  final_alloc,
									  probe_list,
									  P_subset, J_subset,
									  data);
  }
  
  return working_scp->time_remaining;
}
*/

double NEW_compute_until_next_critical_pair(process_scp* working_scp,
					    partial_alloc* feasible_guide,
					    partial_alloc* final_alloc,
					    int allowed_speed_type,
					    pivot_list* probe_list,
					    subset* P_subset, subset* J_subset,
					    run_data* data) {
  int critical_pair_found;

  critical_pair_found = 0;

  while (!critical_pair_found && working_scp->time_remaining > 0.0000001) {

    critical_pair_found = NEW_compute_next_path_segment_or_find_critical_pair(working_scp,
									      feasible_guide,
									      final_alloc,
									      allowed_speed_type,
									      probe_list,
									      P_subset, J_subset,
									      data);
  }
  
  return working_scp->time_remaining;
}


void print_btheta(int** btheta, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      printf("%i    ", btheta[i-1][j-1]);
    }
    printf("\n");
  }
}

void destroy_btheta(int** btheta, int nst) {
  for (int i = 1; i <= nst; i++) {
    free(btheta[i-1]);
  }
  free(btheta);
}
