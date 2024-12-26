#include "gcpscode.h"

/* Our first function provides a simple answer to the top level.  The
  actual computation has integer pointers -- no_segments, no_splits,
  no_new_pivots, no_old_pivotsm and h_sum -- that record the numbers
  of the indicated events. */

struct partial_alloc simple_GCPS_alloc(struct process_scp* input) {
  int* no_segments;
  int* no_splits;
  int* no_new_pivots;
  int* no_old_pivots;
  int* h_sum;

  struct partial_alloc answer;

  no_segments = malloc(sizeof(int));
  no_splits = malloc(sizeof(int));
  no_new_pivots = malloc(sizeof(int));
  no_old_pivots = malloc(sizeof(int));
  h_sum = malloc(sizeof(int));

  *no_segments = 0;
  *no_splits = 0;
  *no_new_pivots = 0;
  *no_old_pivots = 0;
  *h_sum = 0;

  answer = GCPS_allocation(input, no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum); 
  
  /*
  printf("There were %d segments, %d splits, %d new pivots, %d old pivots, and h_sum was %d.\n",
   *no_segments, *no_splits, *no_new_pivots, *no_old_pivots, *h_sum); 
   */

  /* The following routine can be used during code reorganization to check correctness. */

  /*
  char filename[20] = "allocate.mat";
  struct partial_alloc input_alloc = allocation_from_file(filename);

  if (!partial_allocs_are_same(&answer, &input_alloc)) {
    printf("We seem to have screwed things up.\n");
  }
  else {
    printf("It seems that everything is OK.\n");
  }
  destroy_partial_alloc(input_alloc);
  */
  
  free(no_segments);
  free(no_splits);
  free(no_new_pivots);
  free(no_old_pivots);
  free(h_sum);

  return answer;
}

/* The next function uses push_relabel to compute a feasible
allocation, then invokes GCPS_allocation_with_guide to compute the
allocation.  probe_list is a list of pivots.  The idea is that
computing pivots directly is onerous, and the pivots are likely to be
the same when we pass from one segment to the next, so we apply all
the valid old pivots before computing new ones.  */

struct partial_alloc GCPS_allocation(struct process_scp* input, int* no_segments,
				     int* no_splits, int* no_new_pivots,
				     int* no_old_pivots, int* h_sum) {
  struct partial_alloc feasible_guide;
  struct pivot_list probe_list;
  
  feasible_guide = zero_alloc_for_process_scp(input);
  
  push_relabel(input, &feasible_guide);
  
  probe_list = void_pivot_list();

  return GCPS_allocation_with_guide(input, &feasible_guide, &probe_list,
				    no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum);
}

/* GCPS_allocation_with_guide allocates each student's favorite school for as long as it
is possible to find a path above the allocation in the set of feasible allocations.  
This can end because time runs out, in which case the computation is complete, or it can end
because the search for a direction in the set of feasible allocations reveals a critical
pair, in which case we descend recursively to the computation of the two subproblems. */

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						struct pivot_list* probe_list,
						int* no_segments, int* no_splits,
						int* no_new_pivots,
						int* no_old_pivots,int* h_sum) {  
  int nst, nsc;

  double time_remaining; 
  
  struct partial_alloc final_alloc;
  struct subset P_subset, J_subset;
  
  final_alloc = zero_alloc_for_process_scp(input);
  
  nst = input->no_students;
  nsc = input->no_schools;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);

  time_remaining =  compute_until_next_critical_pair(input, feasible_guide,
						     &final_alloc, probe_list, &P_subset, &J_subset,
						     no_segments, no_new_pivots, no_old_pivots, h_sum);

  if (time_remaining < 0.000000001) {  

    destroy_subset(P_subset);
    destroy_subset(J_subset);
    destroy_process_scp(*input); 
    destroy_partial_alloc(*feasible_guide);
    destroy_pivot_list(*probe_list);
      
    return final_alloc;
  }

  /* fprintf(stderr, "J has %i out of %i elements and P has %i out of %i elements.\n",
     J_subset.subset_size, J_subset.large_set_size, P_subset.subset_size, P_subset.large_set_size);*/
  
  (*no_splits)++;

  descend_to_left_subproblem(input, &final_alloc, feasible_guide, probe_list,
			     &P_subset, &J_subset,
			     no_segments, no_splits, no_new_pivots,no_old_pivots, h_sum);
  

  descend_to_right_subproblem(input, &final_alloc, feasible_guide, probe_list,
			      &P_subset, &J_subset,
			      no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum);
  
  destroy_subset(P_subset);
  destroy_subset(J_subset);

  return final_alloc;
}

double compute_until_next_critical_pair(struct process_scp* working_scp,
					struct partial_alloc* feasible_guide,
					struct partial_alloc* final_alloc,
					struct pivot_list* probe_list,
					struct subset* P_subset, struct subset* J_subset,
					int* no_segments, int* no_new_pivots, int* no_old_pivots,
					int* h_sum) {
  int* critical_pair_found;
  critical_pair_found = malloc(sizeof(int));
  *critical_pair_found = 0;

  while (!*critical_pair_found && working_scp->time_remaining > 0.000000001) {

    compute_next_path_segment_or_find_critical_pair(working_scp, feasible_guide,
						    final_alloc, probe_list,
						    P_subset, J_subset,
						    critical_pair_found,
						    no_segments, no_new_pivots,
						    no_old_pivots, h_sum);
  }
    
  free(critical_pair_found);
  
  return working_scp->time_remaining;
}

void descend_to_left_subproblem(struct process_scp* working_scp, struct partial_alloc* final_alloc,
				struct partial_alloc* feasible_guide, 
				struct pivot_list* probe_list,
				struct subset* P_subset, struct subset* J_subset,
				int* no_segments, int* no_splits, int* no_new_pivots,
				int* no_old_pivots, int* h_sum) {  
  struct process_scp left_scp;
  struct partial_alloc left_feas_guide;
  struct partial_alloc left_increment;
  struct pivot_list left_list;
  struct index J_index, P_index;
  
  left_scp = left_sub_process_scp(working_scp, J_subset, P_subset);    
  left_feas_guide = left_sub_process_feasible_guide(feasible_guide, J_subset, P_subset);  
  left_list = left_reduced_pivot_list(probe_list, J_subset, P_subset);
    
  left_increment = GCPS_allocation_with_guide(&left_scp, &left_feas_guide, &left_list,
						no_segments, no_splits, no_new_pivots,
						no_old_pivots, h_sum);
    
  J_index = index_of_subset(J_subset);
  P_index = index_of_subset(P_subset);
  
  increment_partial_alloc(final_alloc, &left_increment, &J_index, &P_index);

  destroy_partial_alloc(left_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

void descend_to_right_subproblem(struct process_scp* working_scp,
				 struct partial_alloc* final_alloc,
				 struct partial_alloc* feasible_guide, 
				 struct pivot_list* probe_list,
				 struct subset* P_subset, struct subset* J_subset,
				 int* no_segments, int* no_splits, int* no_new_pivots,
				 int* no_old_pivots, int* h_sum) {
  struct process_scp right_scp;
  struct partial_alloc right_feas_guide;
  struct partial_alloc right_increment;
  struct pivot_list right_list;
  struct index J_index, P_index;

  right_scp = right_sub_process_scp(working_scp, J_subset, P_subset);
  destroy_process_scp(*working_scp); 
  
  right_feas_guide = right_sub_process_feasible_guide(feasible_guide, J_subset, P_subset);
  destroy_partial_alloc(*feasible_guide);
  
  right_list = right_reduced_pivot_list(probe_list, J_subset, P_subset);
  destroy_pivot_list(*probe_list);
  

  right_increment = GCPS_allocation_with_guide(&right_scp, &right_feas_guide, &right_list,
					       no_segments, no_splits, no_new_pivots,
					       no_old_pivots, h_sum);    
  J_index = index_of_complement(J_subset);    
  P_index = index_of_complement(P_subset);
  
  increment_partial_alloc(final_alloc, &right_increment, &J_index, &P_index);

  destroy_partial_alloc(right_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

/* The next function begins at a partial allocation and feasible
   guide.  It attempts to compute a theta, which is a direction for
   moving the feasible guide.  This attempt either succeeds, in which
   case we move everything to the new endpoint for the partial
   allocation and feasible guide, or it finds a critical pair. */

void compute_next_path_segment_or_find_critical_pair(struct process_scp* working_scp,
						     struct partial_alloc* feasible_guide,
						     struct partial_alloc* final_alloc,
						     struct pivot_list* probe_list,
						     struct subset* P_subset,
						     struct subset* J_subset,
						     int* critical_pair_found,
						     int* no_segments,
						     int* no_new_pivots,
						     int* no_old_pivots, int* h_sum) {

  
  int nst, nsc;

  struct index* alpha;
  struct subset active_schools;
  struct index active_school_index;
  struct index* omega;
  int* fully_allocated;
  int* favorites;
  /*  int** theta; */
  struct int_sparse_matrix* theta;
  int* theta_sums;
  struct pivot_list list_of_pivots;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  /* For each student i, alpha[i-1] is the set of schools that are possible for i.
     The active_schools are those that are possible for some student.  For each active
     school j, omega[j-1] is the set of students who might receive more j. */

  alpha = malloc(nst * sizeof(struct index));
  get_alpha(working_scp, alpha);
  active_schools = union_of_indices(alpha, nsc, nst);
  active_school_index = index_of_subset(&active_schools);  
  destroy_subset(active_schools);  
  omega = reverse_of_correspondence_graph(alpha, &active_school_index, nst);

  /* fully_allocated[j-1] == 1 if j is fully allocated in feasible_guide, 0 otherwise. */
  
  fully_allocated = compute_fully_allocated(working_scp,feasible_guide);
  favorites = get_favorites(working_scp);
  /*  theta = initialize_theta(feasible_guide, favorites); */
  theta = initialize_theta(working_scp, feasible_guide, favorites);
  /* theta_sums = initialize_theta_sums(theta,nst,nsc); */
  theta_sums = initialize_theta_sums(theta,nst,nsc); 

  list_of_pivots = void_pivot_list();
  
  reuse_prior_pivots(probe_list, &list_of_pivots, theta, theta_sums, alpha, feasible_guide,
		     working_scp, favorites, fully_allocated, no_old_pivots, h_sum);
  
  destroy_pivot_list(*probe_list);
  
  *probe_list = void_pivot_list();
    
  massage_theta_or_find_critical_pair(theta, theta_sums, P_subset, J_subset, working_scp,
				      feasible_guide, alpha, &active_school_index, omega,
				      favorites, fully_allocated, &list_of_pivots,
				      critical_pair_found, no_new_pivots, h_sum);
  
  if (!*critical_pair_found) {
    (*no_segments)++;
    move_to_endpoint_of_segment(theta, alpha, favorites, working_scp,
				    feasible_guide, final_alloc);
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);
  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_index(active_school_index);  
  free(fully_allocated);  
  destroy_pivot_list(list_of_pivots);
  destroy_int_sp_mat(theta);
  free(theta);
  free(theta_sums);
  destroy_alpha_or_omega(alpha, nst);
  free(favorites);
}
