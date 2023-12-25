#include "push_relabel.h"


void get_alpha(struct frac_cee* working_cee, struct index* alpha) {
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
    alpha[i-1] = index_of_subset(&alpha_subset);
    destroy_subset(alpha_subset);
  }
}

void destroy_alpha(struct index* alpha, int nst) {    
    for (int i = 1; i <= nst; i++) {
      destroy_index(alpha[i-1]);
    }
    free(alpha);
}

void get_favorites(struct frac_cee* working_cee, int** preferences, int* favorites) {
  int i, done, probe;

  int nst = working_cee->no_students;

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
}

void initialize_theta(int** theta, struct partial_alloc* feasible_guide, int* favorites) {
  int i, j, done;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      theta[i-1][j-1] = 0;
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
}


void revise_theta(int** theta, struct index* alpha, int o_h, 
		  struct partial_alloc* feasible_guide,
		  struct frac_cee* working_cee,
		  struct unordered_subset_list* J_increments,
		  struct unordered_subset_list* P_increments, int* favorites) {
  int g, h, i, j, l, done, current_o, current_i;

  h = length_of_unordered_list(J_increments);
  
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
      if (alpha[current_i-1].indices[j-1] &&
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
	      struct index* alpha, int** theta, int* favorites) {
  int i, j;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;

  for (j = 1; j <= nsc; j++) {
    if (P_subset->indicator[j-1]) {
      for (i = 1; i <= nst; i++) {
	if (!J_subset->indicator[i-1] &&
	    index_has_element(&(alpha[i-1]),j) &&
	    (0.0 < feasible_guide->allocations[i-1][j-1] - 0.000001
	     || theta[i-1][j-1] > 0) && 
	    (0.0 < feasible_guide->allocations[i-1][j-1] - 0.000001
	     || j != favorites[i-1] || theta[i-1][j-1] > 1)) {
	  add_element(next_J_subset,i);
	  add_element(J_subset,i);
	}
      }
    }
  }
}

void next_P_h(struct subset* next_P_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide, struct frac_cee* working_cee,
	      struct index* alpha, int** theta) {
  int i, j;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;
  
  for (i = 1; i <= nst; i++) {
    if (J_subset->indicator[i-1]) {
      for (j = 1; j <= nsc; j++) {
	if (!P_subset->indicator[j-1] &&
	    index_has_element(&(alpha[i-1]),j) &&
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
				struct unordered_subset_list* J_increments,
				struct unordered_subset_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct frac_cee* working_cee,
				struct index* alpha, int** theta, int* favorites,
				int sch, int* o_h) {
  int i, k;

  int done = 0;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;

  add_element(P_subset,sch);

  struct index first_P_index;
  first_P_index = index_of_subset(P_subset);
  add_subset_to_unordered_list(P_increments,&first_P_index);     
  destroy_index(first_P_index);

  while (!done) {
    struct subset next_J_subset = nullset(nst);
    next_J_h(&next_J_subset, J_subset, P_subset, feasible_guide, alpha, theta,
	     favorites);
	   
    if (is_nullset(&next_J_subset)) {
      done = 1;
      destroy_subset(next_J_subset);
    }
    else {
      struct index next_J_index = index_of_subset(&next_J_subset);
	   
      add_subset_to_unordered_list(J_increments,&next_J_index);     
  
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
	add_subset_to_unordered_list(P_increments,&next_P_index);     
	destroy_subset(next_P_subset);

	for (k = 1; k <= next_P_index.no_elements; k++) {
	  int sch_no = next_P_index.indices[k-1];
	  double seats_in_feas_guide = 0.0;
	  int theta_sum = 0;
	  for (i = 1; i <= nst; i++) {
	    seats_in_feas_guide += feasible_guide->allocations[i-1][sch_no-1];
	    theta_sum += theta[i-1][sch_no-1];
	  }
	  if (seats_in_feas_guide < working_cee->quotas[sch_no-1] || theta_sum < 0) {	    
	    done = 1;
	    *o_h = sch_no;
	  }
	}
	destroy_index(next_P_index);
      }
    }
  }
}

void mas_theta_or_find_crit_pair_for_sch(int sch, int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, int* favorites) {
  /*
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;
  */
  
  int* o_h = malloc(sizeof(int));
  
  struct unordered_subset_list* P_increments;
  struct unordered_subset_list* J_increments;
  P_increments = initialized_unordered_list();
  J_increments = initialized_unordered_list();

  *o_h = 0;

  compute_increments_and_o_h(J_subset, P_subset, J_increments, P_increments,
			     feasible_guide, working_cee,
			     alpha, theta, favorites, sch, o_h);
  
  if (*o_h != 0) {
    revise_theta(theta, alpha, *o_h, feasible_guide, working_cee,
		 J_increments, P_increments, favorites);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(o_h);
  destroy_unordered_list(J_increments);
  destroy_unordered_list(P_increments);
}
  
void massage_theta_or_find_critical_pair(int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, int* favorites) {
  int i, j, done, theta_sum;
  double school_sum;

  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;

  done = 0;
  j = 1;
  while (!done) {
    theta_sum = 0;
    for (i = 1; i <= nst; i++) {
      theta_sum+=theta[i-1][j-1];
    }
    school_sum = 0.0;
    for (i = 1; i <= nst; i++) {
      school_sum+=feasible_guide->allocations[i-1][j-1];
    }
    
    if (school_sum < working_cee->quotas[j-1] || theta_sum <= 0) {
      j++;
      if (j == nsc+1) {
	done = 1;
      }
    }
    else {

      mas_theta_or_find_crit_pair_for_sch(j, theta, P_subset, J_subset, working_cee,
					  feasible_guide, alpha, favorites);      
      j = 1;
      if (!is_nullset(P_subset)) {
	done = 1;
      } 
    }
  }
}

double time_until_some_max_exhaustion(int* favorites, struct frac_cee* working_cee) {
  int i;
  
  int nst = working_cee->no_students;
  
  double mnm = working_cee->time_remaining;;
  for (i = 1; i <= nst; i++) {
    mnm = min(mnm,working_cee->maximums[i-1][favorites[i-1]-1]);
  }

  return mnm;
}

double time_until_some_school_exhaustion(int* favorites, struct frac_cee* working_cee) {
  int i, j, student_count;

  double little_min;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;
  
  double mnm = working_cee->time_remaining;
  for (j = 1; j <= nsc; j++) {
    student_count = 0;
    for (i = 1; i <= nst; i++) {
      if (favorites[i-1] == j) {
	student_count++;
      }
    }
    if (student_count > 0) {
      little_min = working_cee->quotas[j-1]/student_count;
      mnm = min(mnm,little_min);
    }
  }

  return mnm;
}

double time_until_feasible_guide_not_feasible(int** theta, struct frac_cee* working_cee,
					      struct partial_alloc* feasible_guide)  {
  int i, j, slope;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  double individual_min, school_quota_min, temp_min, final_min;

  individual_min = working_cee->time_remaining;
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
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

  school_quota_min = working_cee->time_remaining;
  for (j = 1; j<= nsc; j++) {
    slope = 0;
    for (i = 1; i <= nst; i++) {
      slope+=theta[i-1][j-1];
    }
    if (slope > 0) {
      temp_min = working_cee->quotas[j-1]/slope;
      school_quota_min = min(school_quota_min, temp_min);
    }
  }

  final_min = min(individual_min, school_quota_min);
  return final_min;
}

double time_until_feasible_guide_not_above_alloc(int** theta, int* favorites,
						 struct frac_cee* working_cee,
						 struct partial_alloc* feasible_guide) {
  int i, j, slope;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  double lower_bound_min,temp_min;

  lower_bound_min = working_cee->time_remaining;;
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
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

double time_until_trajectory_change(int** theta, int* favorites,
				    struct frac_cee* working_cee,
				    struct partial_alloc* feasible_guide) {
  double final_min, time_til_some_max_exhaustion, time_til_some_school_exhaustion,
    time_til_feasible_guide_not_above, time_til_feasible_guide_not_feasible;
  
  final_min = working_cee->time_remaining;
  
  time_til_some_max_exhaustion = time_until_some_max_exhaustion(favorites, working_cee);
  final_min = min(final_min,time_til_some_max_exhaustion);

  time_til_some_school_exhaustion = time_until_some_school_exhaustion(favorites, working_cee);
  final_min = min(final_min,time_til_some_school_exhaustion);

  time_til_feasible_guide_not_above = time_until_feasible_guide_not_above_alloc(theta,
										favorites,
										working_cee,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_above);

  time_til_feasible_guide_not_feasible = time_until_feasible_guide_not_feasible(theta,
										working_cee,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_feasible);

  return final_min;
}

void decrement_working_cee(struct frac_cee* working_cee, int* favorites, double delta) {
  int i;
  
  int nst = working_cee->no_students;

  for (i = 1; i <= nst; i++ ) {
    working_cee->maximums[i-1][favorites[i-1]-1] -= delta; 
    working_cee->quotas[favorites[i-1]-1] -= delta;
  }

  working_cee->time_remaining -= delta;
}

void destroy_GCPS_allocation_stuff(int nst, struct subset* P_subset, struct subset* J_subset,
				   int** theta, int* favorites, 
				   struct frac_scp* working_scp) {
  int i;
  
  destroy_subset(*P_subset);
  destroy_subset(*J_subset);

  for (i = 1; i <= nst; i++) {
    free(theta[i-1]);
  }
  free(theta);
  free(favorites);
  destroy_frac_scp(*working_scp);
}

struct partial_alloc GCPS_allocation_with_guide(struct frac_scp* input,
						struct partial_alloc* feasible_guide) {
  
  int i;
  
  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools;  

  struct partial_alloc final_alloc;
  final_alloc = zero_alloc_for_frac(&(input->cee));
  
  struct frac_scp working_scp;
  copy_frac_scp(input, &working_scp);

  struct index* alpha;
  
  int* favorites;
  favorites = malloc(nst * sizeof(int));

  int** theta;
  theta = malloc(nst * sizeof (int*));
  for (i = 1; i <= nst; i++) {
    theta[i-1] = malloc(nsc * sizeof(int));
  }

  struct subset P_subset;
  struct subset J_subset;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);

  int before_split = 1;  
  while (before_split) {

    if ((working_scp.cee).time_remaining < 0.000001) {
      destroy_GCPS_allocation_stuff(nst, &P_subset, &J_subset, theta, favorites, 
				    &working_scp);
      return final_alloc;
    }

  
    alpha = malloc(nst * sizeof(struct index));
    get_alpha(&(working_scp.cee), alpha);

    get_favorites(&(working_scp.cee), input->preferences, favorites);
    
    initialize_theta(theta, feasible_guide, favorites);
    
    massage_theta_or_find_critical_pair(theta, &P_subset, &J_subset, &(working_scp.cee),
					feasible_guide, alpha, favorites);
    destroy_alpha(alpha, nst);
    
    if (!is_nullset(&P_subset)) {
      before_split = 0;
    }
    else {
      double delta = time_until_trajectory_change(theta, favorites,&(working_scp.cee),
						  feasible_guide);
      
      augment_partial_alloc(&final_alloc, favorites, delta);

      adjust_feasible_guide(feasible_guide, theta, favorites, delta);
      
      decrement_working_cee(&(working_scp.cee), favorites, delta);
    }
  }

  struct index J_index, P_index;

  if (J_subset.subset_size > 0) {
    struct frac_scp left_scp = critical_sub_frac_scp(&working_scp, &J_subset);

    struct partial_alloc left_feas_guide = left_feasible_guide(feasible_guide, &J_subset);
    struct partial_alloc left_increment = GCPS_allocation_with_guide(&left_scp,&left_feas_guide);

    J_index = index_of_subset(&J_subset);
    P_index = index_of_fullset(nsc);
    
    increment_partial_alloc(&final_alloc, &left_increment, &J_index, &P_index);

    destroy_frac_scp(left_scp);
    destroy_partial_alloc(left_feas_guide);
    destroy_partial_alloc(left_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }

  if (J_subset.subset_size < J_subset.large_set_size) {

    struct frac_scp right_scp = crit_compl_sub_frac_scp(&working_scp, &J_subset, &P_subset);

    struct partial_alloc right_feas_guide = right_feasible_guide(feasible_guide,
								 &J_subset,&P_subset);

    struct partial_alloc right_increment = GCPS_allocation_with_guide(&right_scp,
								      &right_feas_guide);

    J_index = index_of_complement(&J_subset);    
    P_index = index_of_complement(&P_subset);
    
    increment_partial_alloc(&final_alloc, &right_increment, &J_index, &P_index);
    destroy_frac_scp(right_scp);
    destroy_partial_alloc(right_feas_guide);
    destroy_partial_alloc(right_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }

  destroy_GCPS_allocation_stuff(nst, &P_subset, &J_subset, theta, favorites, 
				&working_scp);
  return final_alloc;
}


struct partial_alloc GCPS_allocation(struct frac_scp* input) {
  
  struct partial_alloc feasible_guide;
  feasible_guide = zero_alloc_for_frac(&(input->cee));
  push_relabel(&(input->cee), &feasible_guide);

  struct partial_alloc final_alloc = GCPS_allocation_with_guide(input,&feasible_guide);

  destroy_partial_alloc(feasible_guide); 

  return final_alloc;
}

