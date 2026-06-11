#include "segment.h"

int* compute_fully_allocated(process_scp* working_scp,
			     partial_alloc* feasible_guide) {
  int i, j, nst, nsc;
  
  double school_sum;
  int* sch_fully_alloc;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  sch_fully_alloc = malloc(nsc * sizeof(int));
  
  for (j = 1; j <= nsc; j++) {
    school_sum = 0.0;
    for (i = 1; i <= nst; i++) {
      school_sum += get_entry(feasible_guide, i, j);
    }
    if (school_sum > working_scp->quotas[j-1] - 0.000000001) {
      sch_fully_alloc[j-1] = 1;
    }
    else {
      sch_fully_alloc[j-1] = 0;
    }
  }

  return sch_fully_alloc;
}
 
int_sparse_matrix* initialize_theta(process_scp* working_scp, partial_alloc* feasible_guide,
				    int* favorites) {
  int i, j, done, nst;

  int_sparse_matrix* theta;
  
  nst = feasible_guide->no_students;

  theta = malloc(sizeof(int_sparse_matrix));
  *theta = new_int_sp_mat(working_scp);
  
  for (i = 1; i <= nst; i++) {
    if (get_entry(feasible_guide, i, favorites[i-1]) < 0.000000001) {
      
      set_int_entry(theta, i, favorites[i-1], 1);
    
      done = 0;
      j = 1;
      
      while (!done) {
	if (j != favorites[i-1] && get_entry(feasible_guide, i, j) > 0.000000001) {
	  done = 1;
	  set_int_entry(theta, i, j, -1);
	}
	else {
	  j++;
	}
      }
      
    }
  }

  return theta;
}

int* initialize_theta_sums(int_sparse_matrix* theta, int nst, int nsc) {
  int i, j;
  int* new_theta_sums;
  
  new_theta_sums = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_theta_sums[j-1] = 0;
    for (i = 1; i <= nst; i++) {
      new_theta_sums[j-1] += int_entry(theta, i, j);
    }
  }

  return new_theta_sums;
}

int i_is_in_J_of_j(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* theta,
		   int* favorites) {
  
  if (get_entry(feasible_guide, i, j) > 0.000000001) {
    return 1;
  }
  else {
    if (int_entry(theta, i, j) > 0 && (j != favorites[i-1] || int_entry(theta, i, j) > 1)) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

subset J_of_j(int j, partial_alloc* feasible_guide, int_sparse_matrix* theta, int* favorites) {
  int i, nst;
  
  subset answer;

  nst = feasible_guide->no_students;

  answer = nullset(nst);

  for (i = 1; i <= nst; i++) {
    if (i_is_in_J_of_j(i, j, feasible_guide, theta, favorites)) {
      add_element(&answer, i);
    }
  }

  return answer;
}

subset J_of_P(subset* P, partial_alloc* feasible_guide, int_sparse_matrix* theta, int* favorites) {
  int j, nst, nsc;
  
  subset answer;
  subset new_set;

  nst = feasible_guide->no_students;
  nsc = feasible_guide->no_schools;

  answer = nullset(nst);

  for (j = 1; j <= nsc; j++) {
    if (is_element(P, j)) {
      new_set = J_of_j(j, feasible_guide, theta, favorites);
      add_subset(&answer, &new_set);

      destroy_subset(new_set);
    }
  }

  return answer;
}

int j_is_in_P_of_i(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* theta,
		   process_scp* working_scp) {  
  if (int_entry(theta, i, j) < 0 || get_entry(feasible_guide, i, j) + 0.000000001 <
      is_eligible(working_scp, i, j) * working_scp->time_remaining) {
    return 1;
  }
  else {
    return 0;
  }
}

subset P_of_i(int i, partial_alloc* feasible_guide, int_sparse_matrix* theta,
	      process_scp* working_scp) {
  int j, nsc;
  
  subset answer;

  nsc = feasible_guide->no_schools;

  answer = nullset(nsc);

  for (j = 1; j <= nsc; j++) {
    if (j_is_in_P_of_i(i, j, feasible_guide, theta, working_scp)) {
      add_element(&answer, j);
    }
  }

  return answer;
}

subset P_of_J(subset* J, partial_alloc* feasible_guide, int_sparse_matrix* theta,
	      process_scp* working_scp) {
  int i, nst;
  
  subset answer;
  subset new_set;

  nst = feasible_guide->no_students;

  answer = nullset(nst);

  for (i = 1; i <= nst; i++) {
    if (is_element(J, i)) {      
      new_set = P_of_i(i, feasible_guide, theta, working_scp);
      add_subset(&answer, &new_set);
      destroy_subset(new_set);
    }
  }

  return answer;
}

pivot* extract_pivot(int_sparse_matrix* theta, int j_h, 
		     partial_alloc* feasible_guide,
		     process_scp* working_scp,
		     list_of_elt_lists* J_increments, list_of_elt_lists* P_increments,
		     int* favorites) {
  int g, h, i, j, l, done, current_o, current_i; 
  pivot* my_pivot;

  h = length_of_list_of_elt_lists(J_increments);

  my_pivot = malloc(sizeof(pivot));
  *my_pivot = initialized_pivot(h);

  my_pivot->schools[h] = j_h;
  
  current_o = j_h;
  for (g = h; g >= 1; g--) {
    
    done = 0;
    l = 1;
    while (!done) {
      i = element_of_list_set(J_increments,g,l);      
      if (is_eligible(working_scp, i, current_o) && 
	  j_is_in_P_of_i(i, current_o, feasible_guide, theta, working_scp)) {
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
      if (is_eligible(working_scp, i, j) &&
	  i_is_in_J_of_j(current_i, j, feasible_guide, theta, favorites)) {
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

int pivot_is_valid(pivot* my_pivot,
		   int_sparse_matrix* theta, int* theta_sums,
		   partial_alloc* feasible_guide,
		   process_scp* working_scp,
		   int* favorites,
		   int* sch_fully_alloc) {
  int g, h, i, j, k;
  h = my_pivot->h;
  
  if (!sch_fully_alloc[my_pivot->schools[0]-1] || theta_sums[my_pivot->schools[0]-1] <= 0) {
    return 0;
  }
  
  for (g = 1; g <= h; g++) {
    i = my_pivot->students[g-1];
    j = my_pivot->schools[g-1];
    k = my_pivot->schools[g];
    if (is_eligible(working_scp, i, j) || is_eligible(working_scp, i, k) ||
	!i_is_in_J_of_j(i, j, feasible_guide, theta, favorites) ||
	!j_is_in_P_of_i(i, k, feasible_guide, theta, working_scp)) {
      return 0;
    }
  }

  if (sch_fully_alloc[my_pivot->schools[h]-1] && theta_sums[my_pivot->schools[h]-1] >= 0) {
    return 0;
  }

  return 1;
}

void reuse_prior_pivots(pivot_list* old_list, pivot_list* new_list,
			int_sparse_matrix* theta,
			int* theta_sums, 
			partial_alloc* feasible_guide,
			process_scp* working_scp,
			int* favorites,
			int* sch_fully_alloc,
			run_data* data) {
  pivot_list_node* probe = old_list->first_node;
  if (probe->the_pivot != NULL) {
    if (pivot_is_valid(probe->the_pivot, theta, theta_sums,
		       feasible_guide, working_scp,
		       favorites, sch_fully_alloc)) {
      data->no_old_pivots++;
      data->h_sum++;
      execute_pivot(probe->the_pivot, theta, theta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
  while (probe->next != NULL) {
    probe = probe->next;
    if (pivot_is_valid(probe->the_pivot, theta, theta_sums,
		       feasible_guide, working_scp,
		       favorites, sch_fully_alloc)) {
      data->no_old_pivots++;
      data->h_sum++;
      execute_pivot(probe->the_pivot, theta, theta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
}

subset next_J_increment(subset* J_subset, subset* P_increment, partial_alloc* feasible_guide,
			int_sparse_matrix* theta, int* favorites) {
  subset answer;
  
  answer = J_of_P(P_increment, feasible_guide, theta, favorites);

  remove_subset(&answer, J_subset);

  return answer;
}

subset next_P_increment(subset* J_increment, subset* P_subset, partial_alloc* feasible_guide,
			int_sparse_matrix* theta, process_scp* working_scp) {
  subset answer;
  
  answer = P_of_J(J_increment, feasible_guide, theta, working_scp);
  remove_subset(&answer, P_subset);

  return answer;
}

int  compute_increments_and_j_h(subset* J_subset, subset* P_subset,
				list_of_elt_lists* J_increments, list_of_elt_lists* P_increments,
				partial_alloc* feasible_guide,
				process_scp* working_scp,
				int_sparse_matrix* theta,
				int* theta_sums,
				int* favorites,
				int* sch_fully_alloc,
				int sch,
				int* j_h,
				run_data* data) {
  int k, done, sch_no, critical_pair_found;
  
  subset next_J_subset, next_P_subset;
  element_list first_P_index, next_J_index, next_P_index;

  add_element(P_subset, sch);

  first_P_index = element_list_of_subset(P_subset);
  add_element_list_to_list_of_elt_lists(P_increments, &first_P_index);     
  destroy_element_list(first_P_index);

  done = 0;
  while (!done) {
    next_J_subset = next_J_increment(J_subset, P_subset, feasible_guide, theta, favorites);
    
    if (is_nullset(&next_J_subset)) {
      done = 1;
      destroy_subset(next_J_subset);
    }
    else {
      add_subset(J_subset, &next_J_subset);
      
      next_J_index = element_list_of_subset(&next_J_subset);
      add_element_list_to_list_of_elt_lists(J_increments, &next_J_index);     
      destroy_subset(next_J_subset);
      destroy_element_list(next_J_index);
	  
      next_P_subset = next_P_increment(J_subset, P_subset, feasible_guide, theta, working_scp);
      
      if (is_nullset(&next_P_subset)) {
	done = 1;
	destroy_subset(next_P_subset);
      }
      
      else {
	add_subset(P_subset, &next_P_subset);
	
	next_P_index = element_list_of_subset(&next_P_subset);
	add_element_list_to_list_of_elt_lists(P_increments, &next_P_index);     
	destroy_subset(next_P_subset);

	for (k = 1; k <= next_P_index.no_elements; k++) {
	  sch_no = next_P_index.indices[k-1];
	  if (!sch_fully_alloc[sch_no-1] || theta_sums[sch_no-1] < 0) {	    
	    done = 1;
	    *j_h = sch_no;
	  }
	}
	destroy_element_list(next_P_index);
      }
    }
  } 

  data->h_sum += length_of_list_of_elt_lists(J_increments);

  if (*j_h == 0) {
    critical_pair_found = 1;
  }
  else {
    critical_pair_found = 0;
  }
  
  if (critical_pair_found) {
    if (is_nullset(J_subset) || is_nullset(P_subset)) {
      fprintf(stderr, "We got a critical pair at the end of ciajh with empty J or P.\n");
      exit(0);
    }
  }

  return critical_pair_found;
}

int mas_theta_or_find_crit_pair_for_sch(int j,
					int_sparse_matrix* theta,  int* theta_sums,
					subset* P_subset, subset* J_subset,
					process_scp* working_scp,
					partial_alloc* feasible_guide,
					int* favorites,
					int* sch_fully_alloc,
					pivot_list* new_list,
					run_data* data)  {
  data->no_new_pivots++;

  int critical_pair_found;
  int* j_h = malloc(sizeof(int));
  pivot* my_pivot;  
  list_of_elt_lists* P_increments;
  list_of_elt_lists* J_increments;
  
  P_increments = initialized_list_of_elt_lists();
  J_increments = initialized_list_of_elt_lists();

  *j_h = 0;

  critical_pair_found = compute_increments_and_j_h(J_subset, P_subset, J_increments, P_increments,
						   feasible_guide,
						   working_scp, 
						   theta, theta_sums,
						   favorites,
						   sch_fully_alloc,
						   j,
						   j_h, 
						   data);
  
  if (!critical_pair_found) {
    my_pivot = extract_pivot(theta, *j_h, feasible_guide, working_scp, J_increments, P_increments,
			     favorites);
			
    execute_pivot(my_pivot,theta,theta_sums);    
    add_pivot_to_list(new_list,my_pivot);
    destroy_pivot_ptr(my_pivot);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(j_h);
  destroy_list_of_elt_lists(J_increments);
  destroy_list_of_elt_lists(P_increments);

  return critical_pair_found;
}

int massage_theta_or_find_critical_pair(int_sparse_matrix* theta, int* theta_sums,
					subset* P_subset, subset* J_subset,
					process_scp* working_scp,
					partial_alloc* feasible_guide,
					int* favorites,
					int* sch_fully_alloc,
					pivot_list* new_list,
					run_data* data) {
  int j, nsc, critical_pair_found; 

  critical_pair_found = 0;
  nsc = working_scp->no_schools;
  for (j = 1; j <= nsc && !critical_pair_found; j++) {
    if (sch_fully_alloc[j-1]) {      
      while (!critical_pair_found && theta_sums[j-1] > 0) {
	critical_pair_found = mas_theta_or_find_crit_pair_for_sch(j, theta, theta_sums,
								  P_subset, J_subset,
								  working_scp,
								  feasible_guide, 
								  favorites,
								  sch_fully_alloc,
								  new_list,
								  data);
      }
    }
  }

  return critical_pair_found;
}

/* The next function begins at a partial allocation and feasible
   guide.  It attempts to compute a theta, which is a direction for
   moving the feasible guide.  This attempt either succeeds, in which
   case we move everything to the new endpoint for the partial
   allocation and feasible guide, or it finds a critical pair. */

int compute_next_path_segment_or_find_critical_pair(process_scp* working_scp,
						    partial_alloc* feasible_guide,
						    partial_alloc* final_alloc,
						    pivot_list* probe_list,
						    subset* P_subset, subset* J_subset,
						    run_data* data) {
  int nst, nsc, critical_pair_found;

  int* sch_fully_alloc;
  int* favorites;
  int_sparse_matrix* theta;
  int* theta_sums;
  pivot_list list_of_pivots;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;
  
  sch_fully_alloc = compute_fully_allocated(working_scp, feasible_guide);
  favorites = get_favorites(working_scp);
  theta = initialize_theta(working_scp, feasible_guide, favorites);
  theta_sums = initialize_theta_sums(theta, nst, nsc);

  list_of_pivots = void_pivot_list();
  
  reuse_prior_pivots(probe_list, &list_of_pivots, theta, theta_sums, feasible_guide,
		     working_scp, favorites, sch_fully_alloc, data);
  
  destroy_pivot_list(*probe_list);
  *probe_list = void_pivot_list();
    
  critical_pair_found = massage_theta_or_find_critical_pair(theta, theta_sums,
							    P_subset, J_subset,
							    working_scp,
							    feasible_guide, 
							    favorites,
							    sch_fully_alloc,
							    &list_of_pivots,
							    data);
  
  if (!critical_pair_found) {
    data->no_segments++;

    move_to_endpoint_of_segment(theta, favorites, working_scp, feasible_guide, final_alloc);    
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);
  
  free(sch_fully_alloc);  
  destroy_pivot_list(list_of_pivots);
  destroy_int_sp_mat(theta);
  free(theta);
  free(theta_sums);
  free(favorites);

  return critical_pair_found;
}


void print_theta(int** theta, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      printf("%i    ", theta[i-1][j-1]);
    }
    printf("\n");
  }
}

void destroy_theta(int** theta, int nst) {
  for (int i = 1; i <= nst; i++) {
    free(theta[i-1]);
  }
  free(theta);
}
