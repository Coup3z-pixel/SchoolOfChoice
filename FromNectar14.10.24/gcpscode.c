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

  char filename[20] = "allocate.mat";
  struct partial_alloc input_alloc = allocation_from_file(filename);

  if (!partial_allocs_are_same(&answer, &input_alloc)) {
    printf("We seem to have screwed things up.\n");
  }
  else {
    printf("It seems that everything is OK.\n");
  }
  destroy_partial_alloc(input_alloc);

  
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
  struct partial_alloc final_alloc;
  
  feasible_guide = zero_alloc_for_process_scp(input);
  push_relabel(input, &feasible_guide);
  
  probe_list = void_pivot_list();

  final_alloc = GCPS_allocation_with_guide(input, &feasible_guide, &probe_list,
					   no_segments, no_splits, no_new_pivots,
					   no_old_pivots, h_sum);

  destroy_partial_alloc(feasible_guide);
  destroy_pivot_list(probe_list);

  return final_alloc;
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
  
  struct process_scp working_scp;
  struct partial_alloc final_alloc;
  struct subset P_subset, J_subset;
  int* critical_pair_found;
  
  final_alloc = zero_alloc_for_process_scp(input);
  
  copy_process_scp(input, &working_scp);

  nst = input->no_students;
  nsc = input->no_schools;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);
  
  critical_pair_found = malloc(sizeof(int));
  *critical_pair_found = 0;
  
  while (!*critical_pair_found) {

    compute_next_path_segment_or_find_critical_pair(input, &working_scp, feasible_guide,
						    &final_alloc, probe_list,
						    &P_subset, &J_subset,
						    critical_pair_found,
						    no_segments, no_new_pivots,
						    no_old_pivots, h_sum);

    if (working_scp.time_remaining < 0.000001) {  

      /* destroy_pivot_list(*probe_list); */
      destroy_subset(P_subset);
      destroy_subset(J_subset);
      destroy_process_scp(working_scp);
      free(critical_pair_found);
      
      return final_alloc;
    }
  }
  
  free(critical_pair_found);

  if (J_subset.subset_size > 0 && J_subset.subset_size < J_subset.large_set_size &&
      P_subset.subset_size > 0 && P_subset.subset_size < P_subset.large_set_size) {    
    (*no_splits)++;
  }

  if (J_subset.subset_size > 0) {
    descend_to_left_subproblem(&working_scp, &final_alloc, feasible_guide, probe_list,
			       &P_subset, &J_subset,
			       no_segments, no_splits, no_new_pivots,no_old_pivots, h_sum);
  }

  if (J_subset.subset_size < J_subset.large_set_size) {
    descend_to_right_subproblem(&working_scp, &final_alloc, feasible_guide, probe_list,
				&P_subset, &J_subset,
				no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum);
  }
  
  destroy_subset(P_subset);
  destroy_subset(J_subset);
  destroy_process_scp(working_scp);

  return final_alloc;
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

  int nsc = working_scp->no_schools;
  
  left_scp = critical_sub_process_scp(working_scp, J_subset);    
  left_feas_guide = left_feasible_guide(feasible_guide, J_subset);
  left_list = left_reduced_pivot_list(probe_list, J_subset, P_subset);
    
  left_increment = GCPS_allocation_with_guide(&left_scp, &left_feas_guide, &left_list,
						no_segments, no_splits, no_new_pivots,
						no_old_pivots, h_sum);
    
  J_index = index_of_subset(J_subset);
  P_index = index_of_fullset(nsc);
  increment_partial_alloc(final_alloc, &left_increment, &J_index, &P_index);

  destroy_pivot_list(left_list);
  destroy_process_scp(left_scp);
  destroy_partial_alloc(left_feas_guide);
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

  right_scp = crit_compl_sub_process_scp(working_scp, J_subset, P_subset);
  right_feas_guide = right_feasible_guide(feasible_guide, J_subset, P_subset);
  right_list = right_reduced_pivot_list(probe_list, J_subset, P_subset);

  right_increment = GCPS_allocation_with_guide(&right_scp,&right_feas_guide, &right_list,
					       no_segments, no_splits, no_new_pivots,
					       no_old_pivots, h_sum);
    
  J_index = index_of_complement(J_subset);    
  P_index = index_of_complement(P_subset);
  increment_partial_alloc(final_alloc, &right_increment, &J_index, &P_index);
    
  destroy_pivot_list(right_list);
  destroy_process_scp(right_scp);
  destroy_partial_alloc(right_feas_guide);  
  destroy_partial_alloc(right_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

/* The next function begins at a partial allocation and feasible
   guide.  It attempts to compute a theta, which is a direction for
   moving the feasible guide.  This attempt either succeeds, in which
   case we move everything to the new endpoint for the partial
   allocation and feasible guide, or it finds a critical pair. */

void compute_next_path_segment_or_find_critical_pair(struct process_scp* input,
						     struct process_scp* working_scp,
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
  int** theta;
  int* theta_sums;
  struct pivot_list list_of_pivots;

  nst = input->no_students;
  nsc = input->no_schools;

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
  theta = initialize_theta(feasible_guide, favorites);
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
    move_to_endpoint_of_segment(theta, alpha, favorites, working_scp, feasible_guide, final_alloc);
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);
  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_index(active_school_index);  
  free(fully_allocated);  
  destroy_pivot_list(list_of_pivots);
  destroy_theta(theta, nst);
  free(theta_sums);
  destroy_alpha_or_omega(alpha, nst);
  free(favorites);
}
  
void massage_theta_or_find_critical_pair(int** theta, int* theta_sums, struct subset* P_subset,
					 struct subset* J_subset, struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega,int* favorites,
					 int* fully_allocated,
					 struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum) {
  int j, k; 

  k = 1;
  while (!*critical_pair_found && k <= active_schools_index->no_elements) {
    j = active_schools_index->indices[k-1];
    if (fully_allocated[j-1]) {
      while (!*critical_pair_found && theta_sums[j-1] > 0) {
	mas_theta_or_find_crit_pair_for_sch(j, theta, theta_sums, P_subset, J_subset, working_scp,
					    feasible_guide, alpha, active_schools_index, omega,
					    favorites, fully_allocated, my_list,
					    critical_pair_found, no_new_pivots, h_sum);
      }
    }
    k++;
  }
}


void mas_theta_or_find_crit_pair_for_sch(int sch, int** theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated, struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum) {

  (*no_new_pivots)++;
  
  int* o_h = malloc(sizeof(int));
  struct pivot* my_pivot;  
  struct index_list* P_increments;
  struct index_list* J_increments;
  
  P_increments = initialized_index_list();
  J_increments = initialized_index_list();

  *o_h = 0;

  compute_increments_and_o_h(J_subset, P_subset, J_increments, P_increments,
			     feasible_guide, working_scp, alpha, active_schools_index,
			     omega, theta, theta_sums, favorites, fully_allocated,
			     sch, o_h, critical_pair_found, h_sum);
  
  if (!*critical_pair_found) {
    my_pivot = extract_pivot(theta, alpha, *o_h, feasible_guide, working_scp,
			     J_increments, P_increments, favorites);
			
    execute_pivot(my_pivot,theta,theta_sums);
    
    add_pivot_to_list(my_list,my_pivot);

    destroy_pivot_ptr(my_pivot);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(o_h);
  destroy_index_list(J_increments);
  destroy_index_list(P_increments);
}


void compute_increments_and_o_h(struct subset* J_subset,
				struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_scp* working_scp,
				struct index* alpha,
				struct index* active_schools_index,
				struct index* omega, int** theta, int* theta_sums,
				int* favorites,
				int* fully_allocated, int sch, int* o_h,
				int* critical_pair_found, int* h_sum) {
  int k, nst, nsc, done, sch_no;
  
  struct subset next_J_subset, next_P_subset;
  struct index first_P_index, next_J_index, next_P_index;

  done = 0;
  
  nst = feasible_guide->no_students;
  nsc = feasible_guide->no_schools;

  add_element(P_subset,sch);

  first_P_index = index_of_subset(P_subset);
  add_index_to_index_list(P_increments,&first_P_index);     
  destroy_index(first_P_index);

  while (!done) {
    next_J_subset = nullset(nst);
    next_J_h(&next_J_subset, J_subset, P_subset, feasible_guide, active_schools_index,
	     omega, theta, favorites);
	   
    if (is_nullset(&next_J_subset)) {
      done = 1;
      destroy_subset(next_J_subset);
    }
    else {
      next_J_index = index_of_subset(&next_J_subset);
	   
      add_index_to_index_list(J_increments,&next_J_index);     
  
      destroy_subset(next_J_subset);
      destroy_index(next_J_index);
    }
    
    if (!done) {
      next_P_subset = nullset(nsc);
      next_P_h(&next_P_subset, J_subset, P_subset, feasible_guide, working_scp, alpha, theta);
      if (is_nullset(&next_P_subset)) {
	done = 1;
	destroy_subset(next_P_subset);
      }
      else {
	next_P_index = index_of_subset(&next_P_subset);
	add_index_to_index_list(P_increments,&next_P_index);     
	destroy_subset(next_P_subset);

	for (k = 1; k <= next_P_index.no_elements; k++) {
	  sch_no = next_P_index.indices[k-1];
	  if (!fully_allocated[sch_no-1] || theta_sums[sch_no-1] < 0) {	    
	    done = 1;
	    *o_h = sch_no;
	  }
	}
	destroy_index(next_P_index);
      }
    }
  }

  *h_sum += length_of_index_list(J_increments);

  if (*o_h == 0) {
    *critical_pair_found = 1;
  }
}

