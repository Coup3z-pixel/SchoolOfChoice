#include "segment.h"

struct index* get_alpha(struct process_scp* working_scp) {
  int i, j, nst, nsc;
  struct subset alpha_subset;
  struct index* alpha;
  
  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  alpha = malloc(nst * sizeof(struct index));

  for (i = 1; i <= nst; i++) {
    alpha_subset = nullset(nsc);
    for (j = 1; j <= nsc; j++) {
      if (is_eligible(working_scp, i, j) == 1 && working_scp->time_remaining > 0.000000001 &&
	  working_scp->quotas[j-1] > 0.000000001) {
	alpha_subset.indicator[j-1] = 1;
	alpha_subset.subset_size++;
      }
    }
    
    alpha[i-1] = index_of_subset(&alpha_subset);
    destroy_subset(alpha_subset);
  }

  return alpha;
}

struct index get_active_school_index(struct index* alpha, int nst, int nsc) {

  struct subset active_schools;
  struct index active_school_index;
  
  active_schools = union_of_indices(alpha, nsc, nst);
  active_school_index = index_of_subset(&active_schools);  
  destroy_subset(active_schools);

  return active_school_index;
}

struct index*  get_omega(struct index* alpha, struct index* active_school_index, int nst) {
  return reverse_of_correspondence_graph(alpha, active_school_index, nst);
}

int* compute_fully_allocated(struct process_scp* working_scp,
			     struct partial_alloc* feasible_guide) {
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
 
struct int_sparse_matrix* initialize_theta(struct process_scp* working_scp,
					       struct partial_alloc* feasible_guide,
					       int* favorites) {
  int i, j, done, nst;
  
  nst = feasible_guide->no_students;

  struct int_sparse_matrix* theta;

  theta = malloc(sizeof(struct int_sparse_matrix));
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

int* initialize_theta_sums(struct int_sparse_matrix* theta, int nst, int nsc) {
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

int student_qualified_for_school(int i, int j, struct partial_alloc* feasible_guide,
				     struct int_sparse_matrix* theta, int* favorites) {
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

int school_qualified_for_student(int i, int j,
				 struct partial_alloc* feasible_guide,
				 struct int_sparse_matrix* theta,
				 struct process_scp* working_scp) {
  if (int_entry(theta, i, j) < 0 || get_entry(feasible_guide, i, j) + 0.000000001 <
      is_eligible(working_scp, i, j) * working_scp->time_remaining) {
    return 1;
  }
  else {
    return 0;
  }
}

int pivot_is_valid(struct pivot* my_pivot, struct int_sparse_matrix* theta, int* theta_sums,
		       struct index* alpha, struct partial_alloc* feasible_guide,
		       struct process_scp* working_scp, int* favorites, int* sch_fully_alloc) {
  int g, h, i, j, k;
  h = my_pivot->h;
  
  if (!sch_fully_alloc[my_pivot->schools[0]-1] || theta_sums[my_pivot->schools[0]-1] <= 0) {
    return 0;
  }
  
  for (g = 1; g <= h; g++) {
    i = my_pivot->students[g-1];
    j = my_pivot->schools[g-1];
    k = my_pivot->schools[g];
    if (!index_has_element(&alpha[i-1],j) || !index_has_element(&alpha[i-1],k) ||
	!student_qualified_for_school(i, j, feasible_guide, theta, favorites) ||
	!school_qualified_for_student(i, k, feasible_guide, theta, working_scp)) {
      return 0;
    }
  }

  if (sch_fully_alloc[my_pivot->schools[h]-1] && theta_sums[my_pivot->schools[h]-1] >= 0) {
    return 0;
  }

  return 1;
}

void reuse_prior_pivots(struct pivot_list* old_list, struct pivot_list* new_list,
			struct int_sparse_matrix* theta, int* theta_sums, struct index* alpha,
			struct partial_alloc* feasible_guide, struct process_scp* working_scp,
			int* favorites, int* sch_fully_alloc, int* no_old_pivots, int* h_sum) {
  struct pivot_list_node* probe = old_list->first_node;
  if (probe->the_pivot != NULL) {
    if (pivot_is_valid(probe->the_pivot, theta, theta_sums, alpha, feasible_guide, working_scp,
		       favorites, sch_fully_alloc)) {
      (*no_old_pivots)++;
      *h_sum += probe->the_pivot->h;
      execute_pivot(probe->the_pivot, theta, theta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
  while (probe->next != NULL) {
    probe = probe->next;
    if (pivot_is_valid(probe->the_pivot, theta, theta_sums, alpha, feasible_guide, working_scp,
		       favorites, sch_fully_alloc)) {
      (*no_old_pivots)++;
      *h_sum += probe->the_pivot->h;
      execute_pivot(probe->the_pivot, theta, theta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
}

struct subset next_J_g(struct subset* J_subset, struct subset* P_subset,
		  struct partial_alloc* feasible_guide,
		  struct index* active_schools_index,
		  struct index* omega, struct int_sparse_matrix* theta, int* favorites) {
  int i, j, k, l, nst;

  struct subset next_J_subset;
  
  nst = feasible_guide->no_students;
  next_J_subset = nullset(nst);

  for (k = 1; k <= active_schools_index->no_elements; k++) {
    j = active_schools_index->indices[k-1];
    if (P_subset->indicator[j-1]) {
      for (l = 1; l <= omega[k-1].no_elements; l++) {
	i = omega[k-1].indices[l-1];
	if (!J_subset->indicator[i-1] &&
	    student_qualified_for_school(i, j, feasible_guide, theta, favorites)) {
	  add_element(&next_J_subset,i);
	  add_element(J_subset,i);
	}
      }
    }
  }

  return next_J_subset;
}

struct subset next_P_g(struct subset* J_subset, struct subset* P_subset,
		       struct partial_alloc* feasible_guide,
		       struct process_scp* working_scp,
		       struct index* alpha, struct int_sparse_matrix* theta) {
  int i, j, k, nst, nsc;

  struct subset next_P_subset;
  
  nst = feasible_guide->no_students;
  nsc = feasible_guide->no_schools;
  next_P_subset = nullset(nsc);
  
  for (i = 1; i <= nst; i++) {
    if (J_subset->indicator[i-1]) {
      for (k = 1; k <= alpha[i-1].no_elements; k++) {
	j = alpha[i-1].indices[k-1];
	if (!P_subset->indicator[j-1] &&
	    school_qualified_for_student(i, j, feasible_guide, theta, working_scp)) {
	  add_element(&next_P_subset,j);
	  add_element(P_subset,j);
	}
      }
    }
  }
  
  return next_P_subset;
}

void compute_increments_and_j_h(struct subset* J_subset, struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_scp* working_scp,
				struct index* alpha, struct index* active_schools_index,
				struct index* omega, struct int_sparse_matrix* theta,
				int* theta_sums, int* favorites,
				int* sch_fully_alloc, int sch, int* j_h,
				int* critical_pair_found, int* h_sum) {
  int k, done, sch_no;
  
  struct subset next_J_subset, next_P_subset;
  struct index first_P_index, next_J_index, next_P_index;

  done = 0;

  add_element(P_subset,sch);

  first_P_index = index_of_subset(P_subset);
  add_index_to_index_list(P_increments,&first_P_index);     
  destroy_index(first_P_index);

  while (!done) {
    next_J_subset = next_J_g(J_subset, P_subset, feasible_guide, active_schools_index,
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
      next_P_subset = next_P_g(J_subset, P_subset, feasible_guide, working_scp, alpha, theta);
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
	  if (!sch_fully_alloc[sch_no-1] || theta_sums[sch_no-1] < 0) {	    
	    done = 1;
	    *j_h = sch_no;
	  }
	}
	destroy_index(next_P_index);
      }
    }
  }

  *h_sum += length_of_index_list(J_increments);

  if (*j_h == 0) {
    *critical_pair_found = 1;
  }
}

struct pivot* extract_pivot(struct int_sparse_matrix* theta, struct index* alpha, int j_h, 
			    struct partial_alloc* feasible_guide,
			    struct process_scp* working_scp,
			    struct index_list* J_increments,
			    struct index_list* P_increments, int* favorites) {
  int g, h, i, j, l, done, current_o, current_i;
  struct pivot* my_pivot;

  h = length_of_index_list(J_increments);

  my_pivot = malloc(sizeof(struct pivot));
  *my_pivot = initialized_pivot(h);

  my_pivot->schools[h] = j_h;
  
  current_o = j_h;
  for (g = h; g >= 1; g--) {
    
    done = 0;
    l = 1;
    while (!done) {
      i = element_of_list_set(J_increments,g,l);      
      if (index_has_element(&(alpha[i-1]),current_o) && 
	  school_qualified_for_student(i, current_o, feasible_guide, theta,
					 working_scp)) {
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
	  student_qualified_for_school(i, j, feasible_guide, theta, favorites)) {
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

void mas_theta_or_find_crit_pair_for_sch(int j, struct int_sparse_matrix* theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* sch_fully_alloc, struct pivot_list* new_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum)  {
  (*no_new_pivots)++;
  
  int* j_h = malloc(sizeof(int));
  struct pivot* my_pivot;  
  struct index_list* P_increments;
  struct index_list* J_increments;
  
  P_increments = initialized_index_list();
  J_increments = initialized_index_list();

  *j_h = 0;

  compute_increments_and_j_h(J_subset, P_subset, J_increments, P_increments,
			     feasible_guide, working_scp, alpha, active_schools_index,
			     omega, theta, theta_sums, favorites, sch_fully_alloc,
			     j, j_h, critical_pair_found, h_sum);
  
  if (!*critical_pair_found) {
    my_pivot = extract_pivot(theta, alpha, *j_h, feasible_guide, working_scp,
				 J_increments, P_increments, favorites);
			
    execute_pivot(my_pivot,theta,theta_sums);
    
    add_pivot_to_list(new_list,my_pivot);

    destroy_pivot_ptr(my_pivot);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(j_h);
  destroy_index_list(J_increments);
  destroy_index_list(P_increments);
}

void massage_theta_or_find_critical_pair(struct int_sparse_matrix* theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* sch_fully_alloc,
					 struct pivot_list* new_list,
					 int* critical_pair_found,
					 int* no_new_pivots, int* h_sum) {
  int j, k; 

  k = 1;
  while (!*critical_pair_found && k <= active_schools_index->no_elements) {
    j = active_schools_index->indices[k-1];
    if (sch_fully_alloc[j-1]) {      
      while (!*critical_pair_found && theta_sums[j-1] > 0) {
	mas_theta_or_find_crit_pair_for_sch(j, theta, theta_sums, P_subset, J_subset, working_scp,
					    feasible_guide, alpha, active_schools_index, omega,
					    favorites, sch_fully_alloc, new_list,
					    critical_pair_found, no_new_pivots, h_sum);
      }
    }
    k++;
  }
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
  struct index active_school_index;
  struct index* omega;
  int* sch_fully_alloc;
  int* favorites;
  struct int_sparse_matrix* theta;
  int* theta_sums;
  struct pivot_list list_of_pivots;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  /* For each student i, alpha[i-1] is the set of schools that are possible for i.
     The active_schools are those that are possible for some student.  For each active
     school j, omega[j-1] is the set of students who might receive more j. */

  alpha = get_alpha(working_scp);
  active_school_index = get_active_school_index(alpha, nst, nsc);
  omega = get_omega(alpha, &active_school_index, nst);
  
  sch_fully_alloc = compute_fully_allocated(working_scp,feasible_guide);
  favorites = get_favorites(working_scp);
  theta = initialize_theta(working_scp, feasible_guide, favorites);
  theta_sums = initialize_theta_sums(theta,nst,nsc); 

  list_of_pivots = void_pivot_list();
  
  reuse_prior_pivots(probe_list, &list_of_pivots, theta, theta_sums, alpha, feasible_guide,
		     working_scp, favorites, sch_fully_alloc, no_old_pivots, h_sum);
  
  destroy_pivot_list(*probe_list);
  
  *probe_list = void_pivot_list();
    
  massage_theta_or_find_critical_pair(theta, theta_sums, P_subset, J_subset, working_scp,
				      feasible_guide, alpha, &active_school_index, omega,
				      favorites, sch_fully_alloc, &list_of_pivots,
				      critical_pair_found, no_new_pivots, h_sum);
  
  if (!*critical_pair_found) {
    (*no_segments)++;
    move_to_endpoint_of_segment(theta, alpha, favorites, working_scp,
				    feasible_guide, final_alloc);
  }

  concatenate_pivot_lists(probe_list, &list_of_pivots);
  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_alpha_or_omega(alpha, nst);
  destroy_index(active_school_index);  
  free(sch_fully_alloc);  
  destroy_pivot_list(list_of_pivots);
  destroy_int_sp_mat(theta);
  free(theta);
  free(theta_sums);
  free(favorites);
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
