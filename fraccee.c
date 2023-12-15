#include "fraccee.h"


void destroy_frac_cee(struct frac_cee my_frac) {
  int i;
  free(my_frac.quotas);
  
  for (i = 1; i <= my_frac.no_students; i++) {
    free(my_frac.maximums[i-1]);
  }
  free(my_frac.maximums);
}

void print_frac_cee(struct frac_cee* my_frac) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");

  printf("The time remaining is %1.2f.\n",my_frac->time_remaining);
  
  printf("There are %d students and %d schools\n", my_frac->no_students, my_frac->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < my_frac->no_schools; i++) {
    printf("%1.2f,",my_frac->quotas[i-1]);
  }
  printf("%1.2f)\n",my_frac->quotas[my_frac->no_schools-1]);
  
  
  printf("The maximums matrix is");
    for (i = 1; i <= my_frac->no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_frac->no_schools; j++) {
      printf("  %1.2f", my_frac->maximums[i-1][j-1]);
    }
  }
    printf("\n");
}

void copy_frac_cee(struct frac_cee* my_cee, struct frac_cee* copy) {
  int i, j;

  int nst = my_cee->no_students;
  int nsc = my_cee->no_schools;

  copy->no_students = my_cee->no_students;
  copy->no_schools = my_cee->no_schools;

  copy->quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    copy->quotas[j-1] = my_cee->quotas[j-1];
  }

  copy->maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    copy->maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      copy->maximums[i-1][j-1] = my_cee->maximums[i-1][j-1];
    }
  }

  copy->time_remaining = my_cee->time_remaining;
}

struct frac_cee make_toy_frac() {
  int i,j;
  struct frac_cee my_frac;
  
  my_frac.no_students = 4;
  my_frac.no_schools = 3;

  my_frac.time_remaining = 1.0;
  
  my_frac.quotas = malloc(3 * sizeof(double));
  my_frac.quotas[0] = 1.0;
  my_frac.quotas[1] = 2.0;
  my_frac.quotas[2] = 1.0;
  
  my_frac.maximums = malloc(4 * sizeof(double*));
  for (i = 1; i <= my_frac.no_students; i++) {
  my_frac.maximums[i-1] = malloc(3 * sizeof(double));
    for (j = 1; j <= my_frac.no_schools; j++) {
      my_frac.maximums[i-1][j-1] = 1.0; 
    }
  }
  
  return my_frac;
}

struct partial_alloc zero_alloc_for_frac(struct frac_cee* my_cee) {
  int i,j;
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = my_cee->no_students;
  my_partial_alloc.no_schools = my_cee->no_schools;
  
  my_partial_alloc.allocations = malloc(my_cee->no_students * sizeof(double*));
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    my_partial_alloc.allocations[i-1] = malloc(my_cee->no_schools * sizeof(double));
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      my_partial_alloc.allocations[i-1][j-1] = 0.0; 
    }
  }
  
  return my_partial_alloc;
}

int push_is_valid(int source, int target,
		  double** capacities, double** preflows, double* excess, int* labels) {
  if (excess[source] < 0.000001) {
    return 0;
  }
  if (capacities[source][target] - preflows[source][target] < 0.000001) {
    return 0;
  }
  if (labels[target] != labels[source] - 1) {
    return 0;
  }
  return 1;
}

void push(int source, int target, double** capacities, double** preflows, double* excess) {
  
  double res_cap, delta;

  res_cap = capacities[source][target] - preflows[source][target];
  if (excess[source] <= res_cap) {
    delta = excess[source];
  }
  else {
    delta = res_cap;
  }

  preflows[source][target]+=delta;
  preflows[target][source]-=delta;
  excess[source]-=delta;
  excess[target]+=delta;
}

int relabel_is_valid(int node, double** capacities, double** preflows,
		     double* excess, int* labels, int nst, int nsc) {
  int k;
  
  if (node == 0 || node == nst + nsc + 1 || excess[node] < 0.000001) {
    return 0;
  }

  for (k = 0; k <= nst+nst+1; k++) {
    if (capacities[node][k] - preflows[node][k] > 0.000001 && labels[node] > labels[k]) {
      return 0;
    }
  }

  return 1;
}

void relabel(int node, double** capacities, double** preflows, int* labels, int nst, int nsc) {
  int k, min, hit;

  hit = 0;
  min = -1;

  for (k = 0; k <= nst+nsc+1; k++) {
    if (capacities[node][k] - preflows[node][k] > 0.000001) {
      if (hit == 0) {
	min = labels[k];
      }
      else {
	if (labels[k] < min) {
	  min = labels[k];
	}
      }
      hit = 1;
    }
  }

  if (min == -1) {
    printf("We failed to find a valid target in relabel.\n");
    exit(0);
  }

  labels[node] = min+1;
}

int push_relabel(struct frac_cee* input, struct partial_alloc* max_flow_alloc) {
  int satisfies_gmc, done;
  
  int i, j, k, l;

  int active_node;
  
  int nst = input->no_students;
  int nsc = input->no_schools;

  double flow_value;

  /* The nodes are represented by the integers 0,...,nst+nsc+1, where 0 is the
     source, 1,...nst are the students, nst+1,...,nst+nsc are the schools, and
     nst+nsc+1 is the sink. */

  /* Initialize labels */
  
  int* labels;

  labels = malloc((nst + nsc + 2) * sizeof(int));
  labels[0] = 2 * nsc + 2;
  for (k = 1; k <= nst+nsc+1; k++) {
    labels[k] = 0;
  }

  /* Initialize capacities */
  
  double** capacities;

  capacities = malloc((nst+nsc+2) * sizeof(double*));
  for (k = 0; k <= nst+nsc+1; k++) {
    capacities[k] = malloc((nst+nsc+2) * sizeof(double));
    for (l = 0; l <= nst+nsc+1; l++) {
      capacities[k][l] = 0.0;
    }
  }

  for (i = 1; i <= nst; i++) {
    capacities[0][i] = input->time_remaining;
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      capacities[i][j] = input->maximums[i-1][j-1];;
    }
  }

  for (j = 1; j <= nsc; j++) {
    capacities[j][nst+nsc+1] = input->quotas[j-1];
  }

  /* Initialize preflows */
  
  double** preflows;

  preflows = malloc((nst+nsc+2) * sizeof(double*));
  for (k = 0; k <= nst+nsc+1; k++) {
    preflows[k] = malloc((nst+nsc+2) * sizeof(double));
    for (l = 0; l <= nst+nsc+1; l++) {
      preflows[k][l] = 0.0;
    }
  }

  for (i = 1; i <= nst; i++) {
    preflows[0][i] = input->time_remaining;
    preflows[i][0] = - input->time_remaining;
  }

  /* Initialize excess */

  double* excess;
  excess = malloc((nst+nsc+2) * sizeof(double));
  excess[0] = - nst * input->time_remaining;
  for (i = 1; i <= nst; i++) {
    excess[i] = input->time_remaining;
  }
  for (j = 1; j <= nsc; j++) {
    excess[nst+j] = 0.0;
  }
  excess[nst+nsc+1] = 0.0;

  /* Run the algorithm */
  
  active_node = 1;
  while (active_node < nst+nsc+1) {
    if (excess[active_node] < 0.000001) {
      active_node++;
    }
    else {
      if (relabel_is_valid(active_node, capacities, preflows, excess, labels, nst, nsc)) {
	relabel(active_node, capacities, preflows, labels, nst, nsc);
      }
      else {
	done = 0;
	k = 0;
	while (!done) {
	  if (push_is_valid(active_node, k, capacities, preflows, excess, labels)) {
	    done = 1;
	  }
	}
	push(active_node, k, capacities, preflows, excess);
      }
      active_node = 1;
    }
  }

  /* Report the result */
  
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      max_flow_alloc->allocations[i-1][j-1] = preflows[i-1][j-1];
    }
  }

  flow_value = 0.0;
  for (i = 1; i <= nst; i++) {
    flow_value+=preflows[0][i];
  }
  if (flow_value < nst * input->time_remaining - 0.000001) {
    satisfies_gmc = 0;
  }
  else {
    satisfies_gmc = 1;
  }
  

  /* Clean up capacities */
  for (k = 0; k <= nst+nsc+1; k++) {
    free(capacities[k]);
  }
  free(capacities);

  /* Clean up preflows */
  for (k = 0; k <= nst+nsc+1; k++) {
    free(preflows[k]);
  }
  free(preflows);

  /* Clean up labels */
  free(labels);

  /* Clean up excess */
  free(excess);

  return satisfies_gmc;
}


void get_alpha(struct frac_cee* working_cee, struct index* alpha) {
  int i, j;

  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  for (i = 1; i <= nst; i++) {
    struct subset alpha_subset;
    alpha_subset = nullset(nsc);
    for (j = 1; j <= nsc; j++) {
      if (working_cee->maximums[i-1][j-1] > 0.000001) {
	alpha_subset.indicator[j-1] = 1;
      }
    }
    alpha[i-1] = index_of_subset(&alpha_subset);
    destroy_subset(alpha_subset);
  }
}

void get_favorites(struct frac_cee* working_cee, int** preferences, int* favorites) {
  int i, done, probe;

  int nst = working_cee->no_students;

  for (i = 1; i <= nst; i++) {
    done = 0;
    probe = 0;
    while (!done) {
      if (working_cee->maximums[i-1][preferences[i-1][probe]] > 0.000001) {
	favorites[i-1] = preferences[i-1][probe];
	done = 1;
      }
      else {
	probe++;
      }
    }
  }
}

void initialize_theta(int** theta, struct partial_alloc* final_alloc_ptr,
		      struct partial_alloc* feasible_guide_ptr, int* favorites) {
  int i, j, done;
  
  int nst = final_alloc_ptr->no_students;
  int nsc = final_alloc_ptr->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      theta[i-1][j-1] = 0;
    }
    if (final_alloc_ptr->allocations[i-1][favorites[i-1]-1] ==
	feasible_guide_ptr->allocations[i-1][favorites[i-1]-1]) {
      theta[i-1][favorites[i-1]-1] = 1;
    }
    done = 0;
    j = 1;
    while (!done) {
      if (j != favorites[i-1] && feasible_guide_ptr->allocations[i-1][j-1] > 0.000001) {
	done = 1;
	theta[i-1][j-1] = -1;
      }
      else {
	j++;
      }
    }
  }
}

int compute_J_h(struct index* next_J_index, struct subset* J_subset, struct subset* P_subset,
		struct partial_alloc* final_alloc_ptr, struct partial_alloc* feasible_guide_ptr,
		struct index* alpha, int nsc, int nst) {
  int i, j, new_student;
  
  struct subset next_J_subset;
  next_J_subset = nullset(nst);
  
  new_student = 0;
  for (j = 1; j <= nsc; j++) {
    if (P_subset->indicator[j-1]) {
      for (i = 1; i <= nst; i++) {
	if (!J_subset->indicator[i-1] &&
	    alpha[i-1].indices[j-1] &&
	    final_alloc_ptr->allocations[i-1][j-1] <
	    feasible_guide_ptr->allocations[i-1][j-1] - 0.000001) {
	  new_student = 1;
	  next_J_subset.indicator[i-1] = 1;
	  J_subset->indicator[i-1] = 1;
	}
      }
    }
  }
  
  *next_J_index = index_of_subset(&next_J_subset); 
  destroy_subset(next_J_subset);
  return new_student;
}

int compute_P_h(struct index* next_P_index, struct subset* J_subset, struct subset* P_subset,
		struct partial_alloc* feasible_guide_ptr, struct frac_cee* working_cee,
		struct index* alpha, int nsc, int nst) {
  int i, j, new_school;
  
  struct subset next_P_subset;
  next_P_subset = nullset(nst);
  
  new_school = 0;
  for (i = 1; i <= nst; i++) {
    if (J_subset->indicator[i-1]) {
      for (j = 1; j <= nsc; j++) {
	if (!P_subset->indicator[j-1] &&
	    alpha[i-1].indices[j-1] &&
	    feasible_guide_ptr->allocations[i-1][j-1] <
	    working_cee->maximums[i-1][j-1] - 0.000001) {
	  new_school = 1;
	  next_P_subset.indicator[j-1] = 1;
	  P_subset->indicator[j-1] = 1;
	}
      }
    }
  }
  
  *next_P_index = index_of_subset(&next_P_subset); 
  destroy_subset(next_P_subset);
  return new_school;
}

void revise_theta(int** theta, int o_h, struct frac_cee* working_cee,
		  struct partial_alloc* final_alloc_ptr, struct partial_alloc* feasible_guide_ptr,
		  struct unordered_subset_list* J_increments,
		  struct unordered_subset_list* P_increments) {
  int g, h, i, j, l, done, current_o, current_i, C;

  int nst = working_cee->no_students;
  
  h = length_of_unordered_list(J_increments);
  int* P_schools = malloc((h+1) * sizeof(int));
  int* J_students = malloc(h * sizeof(int));
  P_schools[h-1] = o_h;
  current_o = o_h;
  for (g = h; g >= 1; g--) {
    
    done = 0;
    l = 1;
    while (!done) {
      i = element_of_list_set(J_increments,g,l);
      if (final_alloc_ptr->allocations[i-1][current_o-1] <
	  feasible_guide_ptr->allocations[i-1][current_o-1] - 0.000001) {
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
      if (feasible_guide_ptr->allocations[current_i-1][j-1] <
	  working_cee->maximums[current_i-1][j-1] - 0.000001) {
	done = 1;
	current_o = j;
	P_schools[g-1] = j;
      }
      else {
	l++;
      }
    }
  }

  C = 0;
  for (i = 1; i <= nst; i++) {
    C+= theta[P_schools[0]-1][i-1];
  }

  for (g = 1; g <= h; g++) {
    theta[J_students[g-1]-1][P_schools[g-1]-1] -= C;
    theta[J_students[g-1]-1][P_schools[g]-1] += C;
  }
}

void mas_theta_or_find_crit_pair_for_sch(int sch, int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* final_alloc_ptr,
					 struct partial_alloc* feasible_guide_ptr,
					 struct index* alpha) {
  int subsets_computed, modify_theta, o_h, j, k;

  double barp_sum;
  modify_theta = 0;
  
  int nst = final_alloc_ptr->no_students;
  int nsc = final_alloc_ptr->no_schools;
  
  struct unordered_subset_list P_increments;
  struct unordered_subset_list J_increments;
  P_increments = *initialized_unordered_list();
  J_increments = *initialized_unordered_list();

  *P_subset = singleton_subset(sch, nsc);
  *J_subset = nullset(nst);

  struct index first_P_index;
  first_P_index = index_of_subset(P_subset); 
  add_subset_to_unordered_list(&P_increments,&first_P_index);     
  destroy_index(first_P_index); 

  subsets_computed = 0;
  while(!subsets_computed) {
    struct index next_J_index;
    if (!compute_J_h(&next_J_index, J_subset, P_subset, final_alloc_ptr, feasible_guide_ptr,
		     alpha, nsc, nst)) {
      subsets_computed = 1;
      destroy_index(next_J_index);
    }
    else{
      add_subset_to_unordered_list(&J_increments,&next_J_index);
      destroy_index(next_J_index);

      struct index next_P_index;
      if (!compute_P_h(&next_P_index, J_subset, P_subset, feasible_guide_ptr, working_cee,
		       alpha, nsc, nst)) {
	subsets_computed = 1;
	destroy_index(next_P_index);
      }
      else{
	add_subset_to_unordered_list(&J_increments,&next_P_index);

	o_h = 0;
	for (k = 1; k <= next_P_index.no_elements; k++) {
	  barp_sum = 0.0;
	  for (j = 1; j <= nsc; j++) {
	    barp_sum+=feasible_guide_ptr->allocations[next_P_index.indices[k-1]-1][j-1];
	  }
	  if (barp_sum < working_cee->quotas[next_P_index.indices[k-1]-1] - 0.000001) {
	    modify_theta = 1;
	    revise_theta(theta, o_h, working_cee, final_alloc_ptr, feasible_guide_ptr,
			 &J_increments, &P_increments);
	  }
	}
	destroy_index(next_P_index);
      }
    }

    if (modify_theta) {
      destroy_subset(*J_subset);
      destroy_subset(*P_subset);
      *J_subset = nullset(nst);
      *P_subset = nullset(nsc);
    }

    destroy_unordered_list(&P_increments);
    destroy_unordered_list(&J_increments);
  }
}
  
void massage_theta_or_find_critical_pair(int** theta, struct subset* P_subset,
					 struct subset* J_subset, struct frac_cee* working_cee,
					 struct partial_alloc* final_alloc_ptr,
					 struct partial_alloc* feasible_guide_ptr,
					 struct index* alpha) {
  int i, j, done, theta_sum;
  double school_sum;
  
  int nst = final_alloc_ptr->no_students;
  int nsc = final_alloc_ptr->no_schools;

  done = 0;
  j = 1;
  while (!done && j <= nsc) {
    
    theta_sum = 0;
    for (i = 1; i <= nst; i++) {
      theta_sum+=theta[i-1][j-1];
    }
    school_sum = 0.0;
    for (i = 1; i <= nst; i++) {
      school_sum+=feasible_guide_ptr->allocations[i-1][j-1];
    }
    
    if (school_sum < working_cee->quotas[j-1] || theta_sum <= 0) {
      j++;
    }
    else {
      mas_theta_or_find_crit_pair_for_sch(j, theta, P_subset, J_subset, working_cee,
					  final_alloc_ptr, feasible_guide_ptr, alpha);
    }
  }
}

struct partial_alloc GCPS_allocation(struct frac_scp* input) {
  int i,j;
  
  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools; 

  int before_split = 1;
  
  struct partial_alloc final_alloc;
  struct partial_alloc feasible_guide;
  struct frac_cee working_cee;

  struct index* alpha;
  alpha = malloc(nst * sizeof(struct index));
  int* favorites;
  favorites = malloc(nst * sizeof(int));

  struct subset P_subset;
  struct subset J_subset;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);

  int** theta;
  theta = malloc(nst * sizeof (int*));
  for (i = 1; i <= nst; i++) {
    theta[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      theta[i-1][j-1] = 0;
    }
  }
  
  copy_frac_cee(&(input->cee), &working_cee);
  final_alloc = zero_alloc_for_frac(&(input->cee));
  feasible_guide = zero_alloc_for_frac(&(input->cee));
  push_relabel(&(input->cee), &feasible_guide);

  while (before_split) {
    get_alpha(&working_cee, alpha);
    get_favorites(&working_cee, input->preferences, favorites);
    initialize_theta(theta, &final_alloc, &feasible_guide, favorites);

    massage_theta_or_find_critical_pair(theta, &P_subset, &J_subset, &working_cee,
					&final_alloc, &feasible_guide, alpha);
  }

  destroy_subset(P_subset);
  destroy_subset(J_subset);

  for (i = 1; i <= nst; i++) {
    free(theta[i-1]);
  }
  free(theta);
  free(favorites);
  for (i = 1; i <= nst; i++) {
    destroy_index(alpha[i-1]);
  }
  free(alpha);
  destroy_frac_cee(working_cee);
  destroy_partial_alloc(feasible_guide);
  return final_alloc;
}

