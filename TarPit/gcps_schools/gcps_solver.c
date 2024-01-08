#include "push_relabel.h"


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

int get_theta_sum(int** theta, int nst, int j) {
  int theta_sum = 0;
  for (int i = 1; i <= nst; i++) {
    theta_sum += theta[i-1][j-1];
  }
  return theta_sum;
}

void revise_theta(int** theta, struct index* alpha, int o_h, 
		  struct partial_alloc* feasible_guide,
		  struct process_cee* working_cee,
		  struct index_list* J_increments,
		  struct index_list* P_increments, int* favorites) {
  int g, h, i, j, l, done, current_o, current_i;

  h = length_of_index_list(J_increments);
  
  int* P_schools = malloc((h+1) * sizeof(int));
  int* J_students = malloc(h * sizeof(int));

  P_schools[h] = o_h;
  current_o = o_h;
  for (g = h; g >= 1; g--) {
    
    done = 0;
    l = 1;
    while (!done) {
      i = element_of_list_set(J_increments,g,l);      
      if (index_has_element(&(alpha[i-1]),current_o) &&
	  (feasible_guide->allocations[i-1][current_o-1] <
	   working_cee->maximums[i-1][current_o-1] - 0.000001 ||
	  theta[i-1][current_o-1] < 0)) {
	done = 1;
	current_i = i;
	J_students[g-1] = i;
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
	  (feasible_guide->allocations[current_i-1][j-1] > 0.000001 ||
	   theta[current_i-1][j-1] > 1 ||
	   (theta[current_i-1][j-1] > 0 && j != favorites[i-1]))) {
	done = 1;
	current_o = j;
	P_schools[g-1] = j;
      }
      else {
	l++;
      }
    }
  }

  for (g = 1; g <= h; g++) {
    theta[J_students[g-1]-1][P_schools[g-1]-1]--;
    theta[J_students[g-1]-1][P_schools[g]-1]++;
  }

  free(P_schools);
  free(J_students);
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
	    (feasible_guide->allocations[i-1][j-1] > 0.000001 || theta[i-1][j-1] > 0) && 
	    (feasible_guide->allocations[i-1][j-1] > 0.000001 || j != favorites[i-1]
	     || theta[i-1][j-1] > 1)) {
	  add_element(next_J_subset,i);
	  add_element(J_subset,i);
	}
      }
    }
  }
}

void next_P_h(struct subset* next_P_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide, struct process_cee* working_cee,
	      struct index* alpha, int** theta) {
  int i, j, k;
  
  int nst = feasible_guide->no_students;
  
  for (i = 1; i <= nst; i++) {
    if (J_subset->indicator[i-1]) {
      for (k = 1; k <= alpha[i-1].no_elements; k++) {
	j = alpha[i-1].indices[k-1];
	if (!P_subset->indicator[j-1] &&
	    (feasible_guide->allocations[i-1][j-1] < working_cee->maximums[i-1][j-1] - 0.000001
	     || theta[i-1][j-1] < 0)) {
	  add_element(next_P_subset,j);
	  add_element(P_subset,j);
	}
      }
    }
  }
}

void compute_increments_and_o_h(struct subset* J_subset, struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_cee* working_cee,
				struct index* alpha, struct index* active_schools_index,
				struct index* omega, int** theta, int* favorites,
				int* fully_allocated, int sch, int* o_h,
				int* critical_pair_found, int* h_sum) {
  int k, theta_sum, sch_no;

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
	  theta_sum = get_theta_sum(theta,nst,sch_no);
	  if (!fully_allocated[sch_no-1] || theta_sum < 0) {	    
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

void mas_theta_or_find_crit_pair_for_sch(int sch, int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated, int* critical_pair_found,
					 int* pivots, int* h_sum) {

  (*pivots)++;
  
  int* o_h = malloc(sizeof(int));
  
  struct index_list* P_increments;
  struct index_list* J_increments;
  P_increments = initialized_index_list();
  J_increments = initialized_index_list();

  *o_h = 0;

  compute_increments_and_o_h(J_subset, P_subset, J_increments, P_increments,
			     feasible_guide, working_cee, alpha, active_schools_index,
			     omega, theta, favorites,
			     fully_allocated, sch, o_h, critical_pair_found, h_sum);
  
  if (!*critical_pair_found) {
    revise_theta(theta, alpha, *o_h, feasible_guide, working_cee,
		 J_increments, P_increments, favorites);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(o_h);
  destroy_index_list(J_increments);
  destroy_index_list(P_increments);
}
  
void massage_theta_or_find_critical_pair(int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct process_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega,int* favorites,
					 int* fully_allocated, int* critical_pair_found,
					 int* pivots, int* h_sum) {
  int j, k, theta_sum;

  int nst = feasible_guide->no_students;

  k = 1;
  while (!*critical_pair_found && k <= active_schools_index->no_elements) {
    j = active_schools_index->indices[k-1];
    if (fully_allocated[j-1]) {
      theta_sum = get_theta_sum(theta,nst,j);
      while (!*critical_pair_found && theta_sum > 0) {
	mas_theta_or_find_crit_pair_for_sch(j, theta, P_subset, J_subset, working_cee,
					    feasible_guide, alpha, active_schools_index, omega,
					    favorites, fully_allocated,
					    critical_pair_found, pivots, h_sum);
	theta_sum = get_theta_sum(theta,nst,j);
      }
    }
    k++;
  }
}
  
int theta_is_valid(int** theta, struct process_cee* working_cee,
		   struct partial_alloc* feasible_guide,
		   struct index* alpha, int* favorites, int* fully_allocated) {
  int i, j;

  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (!index_has_element(&(alpha[i-1]),j) && theta[i-1][j-1] != 0) {
	printf("Nonzero theta of i = %d for ineligible school j = %d.\n",i,j);
	printf("The working_cee is \n");
	print_process_cee(working_cee);
	return 0;
      }
      if (feasible_guide->allocations[i-1][j-1] < 0.000001 && theta[i-1][j-1] < 0) {
	printf("Negative theta of zero allocation.\n");
	return 0;
      }
      if (feasible_guide->allocations[i-1][j-1] < 0.000001 && favorites[i-1] == j &&
	  theta[i-1][j-1] < 1) {
	printf("Nonpositive theta of zero allocation of favorite.\n");
	return 0;
      }
      if (feasible_guide->allocations[i-1][j-1] > working_cee->maximums[i-1][j-1] -0.000001 && 
	  theta[i-1][j-1] > 0) {
	printf("Positive theta in spite of max allocation.\n");
	return 0;
      }
    }
  }

  for (i = 1; i <= nst; i++) {
    int sum = 0;
    for (j = 1; j <= nsc; j++) {
      sum += theta[i-1][j-1];
    }
    if (sum != 0) {
      printf("The sum of theta for agent %d is not zero.\n",i);
    }
  }

  for (j = 1; j <= nsc; j++) {
    int total_theta = 0;
    for (i = 1; i <= nst; i++) {
      total_theta += theta[i-1][j-1];
    }
      
    if (fully_allocated[j-1] && total_theta > 0) {
      printf("Positive theta_sum for fully allocated school %d.\n", j);
      return 0;
    }
  }

  return 1;
}

double time_until_some_max_exhaustion(int* favorites, struct process_cee* working_cee) {
  int i;
  
  int nst = working_cee->no_students;
  
  double mnm = working_cee->time_remaining;;
  for (i = 1; i <= nst; i++) {
    mnm = min(mnm,working_cee->maximums[i-1][favorites[i-1]-1]);
  }

  return mnm;
}

double time_until_some_school_exhaustion(int* favorites, struct process_cee* working_cee) {
  int i, j;

  double little_min, minimum;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  int* student_count = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    student_count[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    student_count[favorites[i-1]-1]++;
  }
  
  minimum = working_cee->time_remaining;
  for (j = 1; j <= nsc; j++) {
    if (student_count[j-1] > 0) {
      little_min = working_cee->quotas[j-1]/student_count[j-1];
      minimum = min(minimum,little_min);
    }
  }

  free(student_count);
  
  return minimum;
}

double time_until_feasible_guide_not_feasible(int** theta, struct index* alpha,
					      struct process_cee* working_cee,
					      struct partial_alloc* feasible_guide)  {
  int i, j, k, slope;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  double individual_min, school_quota_min, temp_min, final_min;

  individual_min = working_cee->time_remaining;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      slope = theta[i-1][j-1];
      if (slope < 0) {
	temp_min = - feasible_guide->allocations[i-1][j-1]/slope;
      }
      if (slope > 0) {
	temp_min = working_cee->maximums[i-1][j-1]/slope;
      }
      if (slope != 0) {
	individual_min = min(individual_min, temp_min);
      }
    }
  }

  double* unalloc_quota = malloc(nsc * sizeof(double));
  int* slopes = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    unalloc_quota[j-1] = working_cee->quotas[j-1];
    slopes[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      unalloc_quota[j-1] -= feasible_guide->allocations[i-1][j-1];
      slopes[j-1] += theta[i-1][j-1];
    }
  }

  school_quota_min = working_cee->time_remaining;
  for (j = 1; j <= nsc; j++) {
    if (slopes[j-1] > 0) {
      temp_min = unalloc_quota[j-1]/slopes[j-1];      
      school_quota_min = min(school_quota_min, temp_min);
    }
  }

  free(unalloc_quota);
  free(slopes);
  final_min = min(individual_min, school_quota_min);
  
  return final_min;
}

double time_until_feasible_guide_not_above_alloc(int** theta, struct index* alpha, int* favorites,
						 struct process_cee* working_cee,
						 struct partial_alloc* feasible_guide) {
  int i, j, k, slope;
  
  int nst = working_cee->no_students;

  double lower_bound_min,temp_min;

  lower_bound_min = working_cee->time_remaining;;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      slope = theta[i-1][j-1];
      if (favorites[i-1] == j) {
	slope--;
      }
      if (slope < 0) {
	temp_min = - feasible_guide->allocations[i-1][j-1]/slope;
	lower_bound_min = min(lower_bound_min, temp_min);
      }
    }
  }

  return lower_bound_min;
}

double time_until_trajectory_change(int** theta, struct index* alpha, int* favorites,
				    struct process_cee* working_cee,
				    struct partial_alloc* feasible_guide) {
  double final_min, time_til_some_max_exhaustion, time_til_some_school_exhaustion,
    time_til_feasible_guide_not_above, time_til_feasible_guide_not_feasible;
  
  final_min = working_cee->time_remaining;
  
  time_til_some_max_exhaustion = time_until_some_max_exhaustion(favorites, working_cee);
  final_min = min(final_min,time_til_some_max_exhaustion);

  time_til_some_school_exhaustion = time_until_some_school_exhaustion(favorites, working_cee);
  final_min = min(final_min,time_til_some_school_exhaustion);

  time_til_feasible_guide_not_above = time_until_feasible_guide_not_above_alloc(theta, alpha,
										favorites,
										working_cee,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_above);

  time_til_feasible_guide_not_feasible = time_until_feasible_guide_not_feasible(theta, alpha,
										working_cee,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_feasible);

  return final_min;
}

void decrement_working_cee(struct process_cee* working_cee, int* favorites, double delta) {
  int i;
  
  int nst = working_cee->no_students;

  for (i = 1; i <= nst; i++ ) {
    working_cee->maximums[i-1][favorites[i-1]-1] -= delta; 
    working_cee->quotas[favorites[i-1]-1] -= delta;
  }

  working_cee->time_remaining -= delta;
}

void compute_next_path_segment_or_find_critical_pair(struct process_scp* input,
						     struct process_scp* working_scp,
						     struct partial_alloc* feasible_guide,
						     struct partial_alloc* final_alloc, 
						     struct subset* P_subset,
						     struct subset* J_subset,
						     int* critical_pair_found,
						     int* segments, int* pivots, int* h_sum) {
  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools;  

  struct index* alpha = malloc(nst * sizeof(struct index));
  get_alpha(&(working_scp->cee), alpha);
  struct subset active_schools = union_of_indices(alpha, nsc, nst);
  struct index active_school_index = index_of_subset(&active_schools);  
  destroy_subset(active_schools);
  
  struct index* omega = reverse_of_correspondence_graph(alpha, &active_school_index, nst);
  int* favorites = get_favorites(&(working_scp->cee), input->preferences);
  int* fully_allocated = compute_fully_allocated(&(working_scp->cee),feasible_guide);
  int** theta = initialize_theta(feasible_guide, favorites);
    
  massage_theta_or_find_critical_pair(theta, P_subset, J_subset, &(working_scp->cee),
				      feasible_guide, alpha, &active_school_index, omega,
				      favorites, fully_allocated,
				      critical_pair_found, pivots, h_sum);
  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_index(active_school_index);  
  free(fully_allocated);  
  
  if (!*critical_pair_found) {

    (*segments)++;
  
    double delta = time_until_trajectory_change(theta, alpha, favorites,&(working_scp->cee),
						feasible_guide);
    augment_partial_alloc(final_alloc, favorites, delta);
    adjust_feasible_guide(feasible_guide, theta, alpha, favorites, delta);
    decrement_working_cee(&(working_scp->cee), favorites, delta);
  }

  destroy_theta(theta, nst);
  destroy_alpha_or_omega(alpha, nst);
  free(favorites);
}

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						int* segments, int* splits,
						int* pivots, int* h_sum) {

  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools;
  
  struct partial_alloc final_alloc;
  
  final_alloc = zero_alloc_for_process_scp(&(input->cee));
  
  struct process_scp working_scp;
  copy_process_scp(input, &working_scp);

  struct subset P_subset;
  struct subset J_subset;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);

  
  int* critical_pair_found = malloc(sizeof(int));
  *critical_pair_found = 0;
  while (!*critical_pair_found) {

    compute_next_path_segment_or_find_critical_pair(input, &working_scp, feasible_guide,
						    &final_alloc, &P_subset, &J_subset,
						    critical_pair_found, segments, pivots, h_sum);

    if ((working_scp.cee).time_remaining < 0.000001) {  
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
    (*splits)++;
  }

  struct index J_index, P_index;

  if (J_subset.subset_size > 0) {
    struct process_scp left_scp = critical_sub_process_scp(&working_scp, &J_subset);

    struct partial_alloc left_feas_guide = left_feasible_guide(feasible_guide, &J_subset);
    
    struct partial_alloc left_increment = GCPS_allocation_with_guide(&left_scp, &left_feas_guide,
								     segments, splits,
								     pivots, h_sum);

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

    struct partial_alloc right_increment = GCPS_allocation_with_guide(&right_scp,
								      &right_feas_guide, segments,
								      splits, pivots, h_sum);

    destroy_process_scp(right_scp);
    destroy_partial_alloc(right_feas_guide);
    
    J_index = index_of_complement(&J_subset);    
    P_index = index_of_complement(&P_subset);
    
    increment_partial_alloc(&final_alloc, &right_increment, &J_index, &P_index);
    
    destroy_partial_alloc(right_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }
  
  destroy_subset(P_subset);
  destroy_subset(J_subset);
  destroy_process_scp(working_scp);

  return final_alloc;
}


struct partial_alloc GCPS_allocation(struct process_scp* input, int* segments, int* splits,
				     int* pivots, int* h_sum) {
  
  struct partial_alloc feasible_guide;
  feasible_guide = zero_alloc_for_process_scp(&(input->cee));
  push_relabel(&(input->cee), &feasible_guide);

  struct partial_alloc final_alloc = GCPS_allocation_with_guide(input, &feasible_guide, segments,
								splits, pivots, h_sum);

  destroy_partial_alloc(feasible_guide);

  return final_alloc;
}

