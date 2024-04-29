#include "gcps_solver.h"

/* COMBINATORIC CALCULATIONS */


void get_alpha(struct process_cee* working_cee, struct index* alpha) {
  int i, j;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  for (i = 1; i <= nst; i++) {
    struct subset alpha_subset;
    alpha_subset = nullset(nsc);
    for (j = 1; j <= nsc; j++) {
      if (working_cee->maximums[i-1][j-1] > 0.000001 && working_cee->quotas[j-1] > 0.000001) {
	alpha_subset.indicator[j-1] = 1;
	alpha_subset.subset_size++;
      }
    }

    if (is_nullset(&alpha_subset)) {
      printf("The alpha for agent %d is empty.\n",i);
      exit(0);
    }
    
    alpha[i-1] = index_of_subset(&alpha_subset);
    destroy_subset(alpha_subset);
  }
}

void destroy_alpha_or_omega(struct index* alpha, int nst) {    
    for (int i = 1; i <= nst; i++) {
      destroy_index(alpha[i-1]);
    }
    free(alpha);
}

void destroy_theta(int** theta, int nst) {
  for (int i = 1; i <= nst; i++) {
    free(theta[i-1]);
  }
  free(theta);
}

int* get_favorites(struct process_cee* working_cee, int** preferences) {
  int i, done, probe;

  int nst = working_cee->no_students;

  int* favorites = malloc(nst * sizeof(int));

  for (i = 1; i <= nst; i++) {
    done = 0;
    probe = 0;
    while (!done) {
      if (working_cee->maximums[i-1][preferences[i-1][probe]-1] > 0.000001) {
	favorites[i-1] = preferences[i-1][probe];
	done = 1;
      }
      else {
	probe++;
      }
    }
  }

  return favorites;
}

int* compute_fully_allocated(struct process_cee* working_cee,
			     struct partial_alloc* feasible_guide) {
  int i, j;
  
  double school_sum;

  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  int* fully_allocated = malloc(nsc * sizeof(int));
  
  for (j = 1; j <= nsc; j++) {
    school_sum = 0.0;
    for (i = 1; i <= nst; i++) {
      school_sum += feasible_guide->allocations[i-1][j-1];
    }
    if (school_sum > working_cee->quotas[j-1] - 0.000001) {
      fully_allocated[j-1] = 1;
    }
    else {
      fully_allocated[j-1] = 0;
    }
  }

  return fully_allocated;
}

int** initialize_theta(struct partial_alloc* feasible_guide, int* favorites) {
  int i, j, done;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;
  
  int** theta = malloc(nst * sizeof (int*));
  for (i = 1; i <= nst; i++) {
    theta[i-1] = malloc(nsc * sizeof(int));
  }  

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      theta[i-1][j-1] = 0;
    }
  }
  
  for (i = 1; i <= nst; i++) {
    if (feasible_guide->allocations[i-1][favorites[i-1]-1] < 0.000001) {
      theta[i-1][favorites[i-1]-1] = 1;
    }
  }
  
  for (i = 1; i <= nst; i++) {
    if (feasible_guide->allocations[i-1][favorites[i-1]-1] < 0.000001) {
      
      theta[i-1][favorites[i-1]-1] = 1;
    
      done = 0;
      j = 1;
      while (!done) {
		if (j != favorites[i-1] && feasible_guide->allocations[i-1][j-1] > 0.000001) {
	  done = 1;
	  theta[i-1][j-1] = -1;
	}
	else {
	  j++;
	}
      }
    }
  }

  return theta;
}

int* initialize_theta_sums(int **theta, int nst, int nsc) {
  int i, j;
  
  int* new_theta_sums = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_theta_sums[j-1] = 0;
    for (i = 1; i <= nst; i++) {
      new_theta_sums[j-1] += theta[i-1][j-1];
    }
  }

  return new_theta_sums;
}

void next_J_h(struct subset* next_J_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct index* active_schools_index,
	      struct index* omega, int** theta, int* favorites) {
  int i, j, k, l;

  for (k = 1; k <= active_schools_index->no_elements; k++) {
    j = active_schools_index->indices[k-1];
    if (P_subset->indicator[j-1]) {
      for (l = 1; l <= omega[k-1].no_elements; l++) {
	i = omega[k-1].indices[l-1];
	if (!J_subset->indicator[i-1] &&
	    student_qualified_for_school(i,j,feasible_guide,theta,favorites)) {
	  add_element(next_J_subset,i);
	  add_element(J_subset,i);
	}
      }
    }
  }
}

void next_P_h(struct subset* next_P_subset, struct subset* J_subset,
	      struct subset* P_subset, struct partial_alloc* feasible_guide,
	      struct process_cee* working_cee,
	      struct index* alpha, int** theta) {
  int i, j, k;
  
  int nst = feasible_guide->no_students;
  
  for (i = 1; i <= nst; i++) {
    if (J_subset->indicator[i-1]) {
      for (k = 1; k <= alpha[i-1].no_elements; k++) {
	j = alpha[i-1].indices[k-1];
	if (!P_subset->indicator[j-1] &&
	    school_qualified_for_student(i, j, feasible_guide, theta,
					 working_cee)) {
	  add_element(next_P_subset,j);
	  add_element(P_subset,j);
	}
      }
    }
  }
}

void compute_increments_and_o_h(struct subset* J_subset,
				struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_cee* working_cee,
				struct index* alpha,
				struct index* active_schools_index,
				struct index* omega, int** theta, int* theta_sums,
				int* favorites,
				int* fully_allocated, int sch, int* o_h,
				int* critical_pair_found, int* h_sum) {
  int k, sch_no; 

  int done = 0;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;

  add_element(P_subset,sch);

  struct index first_P_index;
  first_P_index = index_of_subset(P_subset);
  add_index_to_index_list(P_increments,&first_P_index);     
  destroy_index(first_P_index);

  while (!done) {
    struct subset next_J_subset = nullset(nst);
    next_J_h(&next_J_subset, J_subset, P_subset, feasible_guide, active_schools_index,
	     omega, theta, favorites);
	   
    if (is_nullset(&next_J_subset)) {
      done = 1;
      destroy_subset(next_J_subset);
    }
    else {
      struct index next_J_index = index_of_subset(&next_J_subset);
	   
      add_index_to_index_list(J_increments,&next_J_index);     
  
      destroy_subset(next_J_subset);
      destroy_index(next_J_index);
    }
    
    if (!done) {
      struct subset next_P_subset = nullset(nsc);
      next_P_h(&next_P_subset, J_subset, P_subset, feasible_guide, working_cee, alpha, theta);
      if (is_nullset(&next_P_subset)) {
	done = 1;
	destroy_subset(next_P_subset);
      }
      else {
	struct index next_P_index = index_of_subset(&next_P_subset);
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

struct pivot* extract_pivot(int** theta, struct index* alpha, int o_h, 
			   struct partial_alloc* feasible_guide,
			   struct process_cee* working_cee,
			   struct index_list* J_increments,
			   struct index_list* P_increments, int* favorites) {
  int g, h, i, j, l, done, current_o, current_i;

  h = length_of_index_list(J_increments);

  struct pivot* my_pivot = malloc(sizeof(struct pivot));
  *my_pivot = initialized_pivot(h);

  my_pivot->schools[h] = o_h;
  
  current_o = o_h;
  for (g = h; g >= 1; g--) {
    
    done = 0;
    l = 1;
    while (!done) {
      i = element_of_list_set(J_increments,g,l);      
      if (index_has_element(&(alpha[i-1]),current_o) && 
	  school_qualified_for_student(i, current_o, feasible_guide, theta,
					 working_cee)) {
	done = 1;
	current_i = i;

	my_pivot->students[g-1] = i;
	
      }
      else {	
	l++;
      }
    }
      
    done = 0;
    l = 1;
    while (!done) {
      j = element_of_list_set(P_increments,g,l);
      if (index_has_element(&(alpha[current_i-1]),j) &&
	  student_qualified_for_school(i,j,feasible_guide,theta,favorites)) {
	done = 1;
	current_o = j;

	my_pivot->schools[g-1] = j;
	
      }
      else {
	l++;
      }
    }
  }

  return my_pivot;
}

void mas_theta_or_find_crit_pair_for_sch(int sch, int** theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated, struct pivot_list* my_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum) {

  /*  (*no_pivots)++; */
  (*no_new_pivots)++;
  
  int* o_h = malloc(sizeof(int));
  
  struct index_list* P_increments;
  struct index_list* J_increments;
  P_increments = initialized_index_list();
  J_increments = initialized_index_list();

  *o_h = 0;

  compute_increments_and_o_h(J_subset, P_subset, J_increments, P_increments,
			     feasible_guide, working_cee, alpha, active_schools_index,
			     omega, theta, theta_sums, favorites,
			     fully_allocated, sch, o_h, critical_pair_found, h_sum);
  
  if (!*critical_pair_found) {
    struct pivot* my_pivot = extract_pivot(theta, alpha, *o_h, 
					   feasible_guide,
					   working_cee,
					   J_increments, P_increments,
					   favorites);

    /* - This check seemed to work perfectly.
    if (!pivot_is_valid(my_pivot, theta, theta_sums, alpha, feasible_guide, working_cee,
			favorites, fully_allocated)) {
      printf("Something is wrong, it seems.\n");
      exit(0);
    }
    */


    
			
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
  
void massage_theta_or_find_critical_pair(int** theta, int* theta_sums, struct subset* P_subset,
					 struct subset* J_subset, struct process_cee* working_cee,
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
	mas_theta_or_find_crit_pair_for_sch(j, theta, theta_sums, P_subset, J_subset, working_cee,
					    feasible_guide, alpha, active_schools_index, omega,
					    favorites, fully_allocated, my_list,
					    critical_pair_found, no_new_pivots, h_sum);
      }
    }
    k++;
  }
}

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
  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools;

  /* For each student i, alpha[i-1] is the set of schools that are possible for i.
     The active_schools are those that are possible for some student.  For each active
     school j, omega[j-1] is the set of students who might attend j. */

  struct index* alpha = malloc(nst * sizeof(struct index));
  get_alpha(&(working_scp->cee), alpha);
  struct subset active_schools = union_of_indices(alpha, nsc, nst);
  struct index active_school_index = index_of_subset(&active_schools);  
  destroy_subset(active_schools);  
  struct index* omega = reverse_of_correspondence_graph(alpha, &active_school_index, nst);

  /* fully_allocated[j-1] == 1 if j is fully allocated in feasible_guide, 0 otherwise. */
  
  int* fully_allocated = compute_fully_allocated(&(working_scp->cee),feasible_guide);
  int* favorites = get_favorites(&(working_scp->cee), input->preferences);
  int** theta = initialize_theta(feasible_guide, favorites);
  int* theta_sums = initialize_theta_sums(theta,nst,nsc);

  struct pivot_list list_of_pivots = void_pivot_list();

  reuse_prior_pivots(probe_list, &list_of_pivots, theta, theta_sums, alpha, feasible_guide,
		     &(working_scp->cee), favorites, fully_allocated, no_old_pivots, h_sum);

  destroy_pivot_list(*probe_list);
  *probe_list = void_pivot_list();
    
  massage_theta_or_find_critical_pair(theta, theta_sums, P_subset, J_subset, &(working_scp->cee),
				      feasible_guide, alpha, &active_school_index, omega,
				      favorites, fully_allocated, &list_of_pivots,
				      critical_pair_found, no_new_pivots, h_sum);
  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_index(active_school_index);  
  free(fully_allocated);  
  
  if (!*critical_pair_found) {
    (*no_segments)++;
    move_to_endpoint_of_segment(theta, alpha, favorites, working_scp, feasible_guide, final_alloc);
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);

  destroy_pivot_list(list_of_pivots);

  destroy_theta(theta, nst);
  free(theta_sums);
  destroy_alpha_or_omega(alpha, nst);
  free(favorites);
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

  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools;
  
  struct partial_alloc final_alloc;
  
  final_alloc = zero_alloc_for_process_scp(&(input->cee));
  
  struct process_scp working_scp;
  copy_process_scp(input, &working_scp);

  struct subset P_subset = nullset(nsc);
  struct subset J_subset = nullset(nst);

  /*  struct pivot_list probe_list = void_pivot_list(); */
  
  int* critical_pair_found = malloc(sizeof(int));
  *critical_pair_found = 0;
  while (!*critical_pair_found) {

    compute_next_path_segment_or_find_critical_pair(input, &working_scp, feasible_guide,
						    &final_alloc, probe_list,
						    &P_subset, &J_subset,
						    critical_pair_found,
						    no_segments, no_new_pivots,
						    no_old_pivots, h_sum);

    if ((working_scp.cee).time_remaining < 0.000001) {  

      destroy_pivot_list(*probe_list);
    
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

  struct index J_index, P_index;

  if (J_subset.subset_size > 0) {
    
    struct process_scp left_scp = critical_sub_process_scp(&working_scp, &J_subset);
    struct partial_alloc left_feas_guide = left_feasible_guide(feasible_guide, &J_subset);
    struct pivot_list left_list = left_reduced_pivot_list(probe_list,&J_subset,&P_subset);
    
    struct partial_alloc left_increment = GCPS_allocation_with_guide(&left_scp, &left_feas_guide,
								     &left_list,
								     no_segments, no_splits,
								     no_new_pivots,
								     no_new_pivots, h_sum);

    destroy_process_scp(left_scp);
    destroy_partial_alloc(left_feas_guide);
    
    J_index = index_of_subset(&J_subset);
    P_index = index_of_fullset(nsc);
    
    increment_partial_alloc(&final_alloc, &left_increment, &J_index, &P_index);

    destroy_partial_alloc(left_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }

  if (J_subset.subset_size < J_subset.large_set_size) {

    struct process_scp right_scp = crit_compl_sub_process_scp(&working_scp, &J_subset, &P_subset);
    struct partial_alloc right_feas_guide = right_feasible_guide(feasible_guide,
								 &J_subset,&P_subset);
    struct pivot_list right_list = right_reduced_pivot_list(probe_list,&J_subset,&P_subset);

    struct partial_alloc right_increment = GCPS_allocation_with_guide(&right_scp,&right_feas_guide,
								      &right_list,
								      no_segments, no_splits,
								      no_new_pivots,
								      no_old_pivots, h_sum);

    destroy_process_scp(right_scp);
    destroy_partial_alloc(right_feas_guide);  
    
    J_index = index_of_complement(&J_subset);    
    P_index = index_of_complement(&P_subset);
    
    increment_partial_alloc(&final_alloc, &right_increment, &J_index, &P_index);
    
    destroy_partial_alloc(right_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }

  destroy_pivot_list(*probe_list);
  
  destroy_subset(P_subset);
  destroy_subset(J_subset);
  destroy_process_scp(working_scp);

  return final_alloc;
}

/* The next function uses push_relabel to compute a feasible allocation, then invokes
GCPS_allocation_with_guide to compute the allocation.  The integer pointers -- no_segments, 
no_splits, no_pivots, and h_sum -- are for recording the numbers of the indicated events. */

struct partial_alloc GCPS_allocation(struct process_scp* input, int* no_segments,
				     int* no_splits, int* no_new_pivots,
				     int* no_old_pivots, int* h_sum) {
  
  struct partial_alloc feasible_guide = zero_alloc_for_process_scp(&(input->cee));
  push_relabel(&(input->cee), &feasible_guide);

  struct pivot_list probe_list = void_pivot_list();

  struct partial_alloc final_alloc = GCPS_allocation_with_guide(input, &feasible_guide,
								&probe_list,
								no_segments, no_splits,
								no_new_pivots,
								no_old_pivots, h_sum);

  destroy_partial_alloc(feasible_guide);

  return final_alloc;
}

