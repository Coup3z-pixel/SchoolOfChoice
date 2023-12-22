#include "fraccee.h"


void destroy_frac_cee(struct frac_cee my_frac) {
  int i;
  free(my_frac.quotas);
  
  for (i = 1; i <= my_frac.no_students; i++) {
    free(my_frac.maximums[i-1]);
  }
  free(my_frac.maximums);
}

void destroy_frac_scp(struct frac_scp my_scp) {
  int i;
  
  int nst = (my_scp.cee).no_students;

  destroy_frac_cee(my_scp.cee);
  
  free(my_scp.no_eligible_schools);
  
  for (i = 1; i <= nst; i++) {
    free(my_scp.preferences[i-1]);
  }
  free(my_scp.preferences);
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

void print_frac_scp(struct frac_scp* my_scp) {
  int i,j;
  print_frac_cee(&my_scp->cee);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= my_scp->cee.no_students; i++) {
    printf("%i:  ", i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= my_scp->no_eligible_schools[i-1]; j++) {
      printf("%i  ",  my_scp->preferences[i-1][j-1]);
    }
    printf("\n");
  }
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

void copy_frac_scp(struct frac_scp* my_scp, struct frac_scp* copy) {
  int i, j;

  int nst = (my_scp->cee).no_students;
  
  copy_frac_cee(&(my_scp->cee),&(copy->cee));
  
  copy->no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    (copy->no_eligible_schools)[i-1] = (my_scp->no_eligible_schools)[i-1];
  }

  copy->preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    (copy->preferences)[i-1] = malloc((copy->no_eligible_schools)[i-1] * sizeof(int));
    for (j = 1; j <= (my_scp->no_eligible_schools)[i-1]; j++) {
      (copy->preferences)[i-1][j-1] = (my_scp->preferences)[i-1][j-1];
    }
  }  
}

struct frac_cee make_toy_frac_cee() {
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

  my_frac.maximums[1][1] = 0.0;
  
  return my_frac;
}

struct frac_scp make_toy_frac_scp() {
  struct frac_scp my_scp;
  my_scp.cee = make_toy_frac_cee();

  my_scp.no_eligible_schools = malloc(4 * sizeof(int));
  my_scp.no_eligible_schools[0] = 3;
  my_scp.no_eligible_schools[1] = 2;
  my_scp.no_eligible_schools[2] = 3;
  my_scp.no_eligible_schools[3] = 3;

  my_scp.preferences = malloc(4 * sizeof(int*));
  my_scp.preferences[0] = malloc(3 * sizeof(int));
  my_scp.preferences[1] = malloc(2 * sizeof(int));
  my_scp.preferences[2] = malloc(3 * sizeof(int));
  my_scp.preferences[3] = malloc(3 * sizeof(int));

  my_scp.preferences[0][0] = 1;
  my_scp.preferences[1][0] = 1;
  my_scp.preferences[2][0] = 1;
  my_scp.preferences[3][0] = 1;

  my_scp.preferences[0][1] = 2;
  my_scp.preferences[1][1] = 3;
  my_scp.preferences[2][1] = 2;
  my_scp.preferences[3][1] = 2;

  my_scp.preferences[0][2] = 3;
  my_scp.preferences[2][2] = 3;
  my_scp.preferences[3][2] = 3;

  return my_scp;
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

struct frac_scp critical_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset) {
  struct frac_scp new_scp;
  
  int i, j, k;
  struct index J_index;
  J_index = index_of_subset(J_subset);

  int nst = J_subset->subset_size;
  int nsc = my_scp->cee.no_schools;

  new_scp.cee.no_students = nst;
  new_scp.cee.no_students = nsc;

  (new_scp.cee).quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.cee.quotas[j-1] = my_scp->cee.quotas[j-1];
  }

  new_scp.cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_scp.cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      new_scp.cee.maximums[i-1][j-1] = my_scp->cee.maximums[J_index.indices[i-1]-1][j-1];
    }
  }

  new_scp.cee.time_remaining = my_scp->cee.time_remaining;

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = my_scp->no_eligible_schools[J_index.indices[i-1]-1];
  }
  
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= new_scp.no_eligible_schools[i-1]; k++) {
      new_scp.preferences[i-1][k-1] = my_scp->preferences[J_index.indices[i-1]-1][k];
    }
  }

  destroy_index(J_index);
  return new_scp;    
}

struct frac_scp crit_compl_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset) {
  struct frac_scp new_scp;
  
  int i, j, k, l, count;

  struct subset J_compl, P_compl;
  J_compl = complement_of_subset(J_subset);
  P_compl = complement_of_subset(P_subset);
  struct index J_index, P_index;
  J_index = index_of_subset(&J_compl);
  P_index = index_of_subset(&P_compl);
  
  int* reverse_P_index = malloc(my_scp->cee.no_schools * sizeof(int));
  count = 0;
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    if (P_compl.indicator[j] == 1) {
      count++;
      reverse_P_index[j-1] = count;
    }
    else {
      reverse_P_index[j-1] = 0;
    }
  }

  int nst = J_compl.subset_size;
  int nsc = P_compl.subset_size;

  new_scp.cee.no_students = nst;
  new_scp.cee.no_students = nsc;

  (new_scp.cee).quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.cee.quotas[j-1] = my_scp->cee.quotas[P_index.indices[j-1]-1];
    for (k = 1; k <= my_scp->cee.no_students; k++) {
      if (J_subset->indicator[k-1] == 1) {
	new_scp.cee.quotas[j-1] -= my_scp->cee.maximums[k][P_index.indices[j-1]-1];
      }
    }
  }

  new_scp.cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_scp.cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      new_scp.cee.maximums[i-1][j-1] =
	my_scp->cee.maximums[J_index.indices[i-1]-1][P_index.indices[j-1]-1];
    }
  }

  new_scp.cee.time_remaining = my_scp->cee.time_remaining;

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = 0;
    for (l = 1; l <= my_scp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.no_eligible_schools[i-1]++;
      }
    }
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int*));
    count = 0;
    for (l = 1; l <= my_scp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.preferences[i-1][count] =
	  reverse_P_index[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1];
	count++;
      }
    }
  }

  destroy_subset(J_compl);
  destroy_subset(P_compl);
  destroy_index(J_index);
  destroy_index(P_index);
  free(reverse_P_index);
  
  return new_scp;    
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

    printf("relabel_is_valid failed at the beginning.\n");
    
    return 0;
  }

  for (k = 0; k <= nst+nsc+1; k++) {
    if (capacities[node][k] - preflows[node][k] > 0.000001 && labels[node] > labels[k]) {
      /*
      if (node == 2) {
	printf("For some reason we can't relabel with node = 3 and k = %d.\n", k);
      }
      */
      return 0;
    }
  }

  return 1;
}

void relabel(int node, double** capacities, double** preflows, int* labels, int nst, int nsc) {
  int k, min, hit;

  hit = 0;

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
      capacities[i][nst+j] = input->maximums[i-1][j-1];;
    }
  }

  for (j = 1; j <= nsc; j++) {
    capacities[nst+j][nst+nsc+1] = input->quotas[j-1];
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
    while (active_node< nst+nsc+1 && excess[active_node] < 0.000001) {
      active_node++;
    }
    if (active_node < nst+nsc+1) {

      /*      printf("The active_node is %d.\n", active_node); */
      
      if (relabel_is_valid(active_node, capacities, preflows, excess, labels, nst, nsc)) {
	
	/* printf("We should be relabelling.\n"); */
	
	relabel(active_node, capacities, preflows, labels, nst, nsc);
      }
      else {
	done = 0;
	k = 0;
	while (!done) {
	  if (push_is_valid(active_node, k, capacities, preflows, excess, labels)) {
	    done = 1;
	  }
	  else {
	    k++;
	  }
	}
	
	/* printf("We should be pushing.\n"); */
	
	push(active_node, k, capacities, preflows, excess);
      }

      /*
      printf("After an operation labels is\n ");
      for (k = 0; k <= nst+nsc+1; k++) {
	printf("%d      ", labels[k]);
      }
      printf("\n\n");
      printf("and the excess is \n");
      for (k = 0; k <= nst+nsc+1; k++) {
	printf("%1.2f   ", excess[k]);
      }
      printf("\n\n");
      */
      
    }
    if (active_node < nst+nsc+1) {
      active_node = 1;
    }
  }

  /* Report the result */

  max_flow_alloc->no_students = nst;
  max_flow_alloc->no_schools = nsc;
  max_flow_alloc->allocations = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    max_flow_alloc->allocations[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      max_flow_alloc->allocations[i-1][j-1] = preflows[i][nst+j]; 
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
	alpha_subset.subset_size++;
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

void initialize_theta(int** theta, struct partial_alloc* final_alloc,
		      struct partial_alloc* feasible_guide, int* favorites) {
  int i, j, done;
  
  int nst = final_alloc->no_students;
  int nsc = final_alloc->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      theta[i-1][j-1] = 0;
    }
    if (final_alloc->allocations[i-1][favorites[i-1]-1] ==
	feasible_guide->allocations[i-1][favorites[i-1]-1]) {
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

  printf("Coming into revise_theta, P_increments is\n");
  print_unordered_list(P_increments);
  printf("\nand J_increments is\n");
  print_unordered_list(J_increments);
  printf("\n");

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

  printf("We have J_students[0] = %d, P_schools[0] = %d, and P_schools[1] = %d.\n",
	 J_students[0],P_schools[0],P_schools[1]);

  for (g = 1; g <= h; g++) {
    theta[J_students[g-1]-1][P_schools[g-1]-1]--;
    theta[J_students[g-1]-1][P_schools[g]-1]++;
  }
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

  *P_subset = singleton_subset(sch, nsc);
  *J_subset = nullset(nst);

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
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;
  
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

    printf("After revision, theta is\n");
    for (int i = 1; i <= nst; i++) {
      for (int j = 1; j <= nsc; j++) {
	printf("%d   ",theta[i-1][j-1]);
      }
      printf("\n");
    }

    destroy_subset(*J_subset);
    destroy_subset(*P_subset);

    *J_subset = nullset(nsc);
    *P_subset = nullset(nst);
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

  int count = 0;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;

  done = 0;
  j = 1;
  while (!done) {

    count++;
    if (count > 10) {
      exit(0);
    }
    
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

      printf("We are about to descend with done = %d and j = %d.\n",done,j);
      
      mas_theta_or_find_crit_pair_for_sch(j, theta, P_subset, J_subset, working_cee,
					  feasible_guide, alpha, favorites);

      printf("We got out after j = %d with done = %d.\n",j,done);
      
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
      mnm = max(mnm,little_min);
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
	temp_min = feasible_guide->allocations[i-1][j-1]/slope;
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
				   int** theta, int* favorites, struct index* alpha,
				   struct frac_scp* working_scp,
				   struct partial_alloc* feasible_guide) {
  int i;
  
  destroy_subset(*P_subset);
  destroy_subset(*J_subset);

  for (i = 1; i <= nst; i++) {
    free(theta[i-1]);
  }
  free(theta);
  free(favorites);
  for (i = 1; i <= nst; i++) {
    destroy_index(alpha[i-1]);
  }
  free(alpha);
  destroy_frac_scp(*working_scp);
  destroy_partial_alloc(*feasible_guide);
}

struct partial_alloc GCPS_allocation(struct frac_scp* input) {
  int i, j;
  
  int nst = (input->cee).no_students;
  int nsc = (input->cee).no_schools;  

  struct partial_alloc final_alloc;
  struct frac_scp working_scp;
  struct partial_alloc feasible_guide;
  final_alloc = zero_alloc_for_frac(&(input->cee));
  copy_frac_scp(input, &working_scp);
  feasible_guide = zero_alloc_for_frac(&(input->cee));

  push_relabel(&(input->cee), &feasible_guide);

  printf("We did push_relabel, obtaining\n");

  print_partial_alloc(&feasible_guide);

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
  }

  int before_split = 1;  
  while (before_split) {
    
    if ((working_scp.cee).time_remaining < 0.000001) {
      destroy_GCPS_allocation_stuff(nst, &P_subset, &J_subset, theta, favorites, alpha,
				    &working_scp, &feasible_guide);
      return final_alloc;
    }

    get_alpha(&(working_scp.cee), alpha);

    get_favorites(&(working_scp.cee), input->preferences, favorites);
    initialize_theta(theta, &final_alloc, &feasible_guide, favorites);

    printf("We initialized alpha, favorites, and theta. Now theta is\n");
    for (i = 1; i <= nst; i++) {
      for (j = 1; j <= nsc; j++) {
	printf("%d   ",theta[i-1][j-1]);
      }
      printf("\n");
    }

    massage_theta_or_find_critical_pair(theta, &P_subset, &J_subset, &(working_scp.cee),
					&feasible_guide, alpha, favorites);

    printf("We massaged theta, or found a critical pair.\n");
    
    if (!is_nullset(&P_subset)) {
      before_split = 0;
    }
    else {
      double delta = time_until_trajectory_change(theta, favorites,
						  &(working_scp.cee),
						  &feasible_guide);

	/* (working_scp.cee).time_remaining -
	time_remaining_after_further_alloc(theta, favorites, &(working_scp.cee), &final_alloc,
	&feasible_guide); */
      
      augment_partial_alloc(&final_alloc, favorites, delta);
      adjust_feasible_guide(&feasible_guide, theta, delta);
      decrement_working_cee(&(working_scp.cee), favorites, delta);

      printf("We should have done some allocating.\n");
      exit(0);
    }
  }

  struct index J_index, P_index;

  if (J_subset.subset_size > 0) {
    struct frac_scp left_scp = critical_sub_frac_scp(&working_scp, &J_subset);
    struct partial_alloc left_increment = GCPS_allocation(&left_scp);
    J_index = index_of_subset(&J_subset);
    P_index = index_of_fullset(nsc);
    increment_partial_alloc(&final_alloc, &left_increment, &J_index, &P_index);
    destroy_frac_scp(left_scp);
    destroy_partial_alloc(left_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }

  if (J_subset.subset_size < J_subset.large_set_size) {
    struct frac_scp right_scp = crit_compl_sub_frac_scp(&working_scp, &J_subset, &P_subset);
    struct partial_alloc right_increment = GCPS_allocation(&right_scp);
    J_index = index_of_complement(&J_subset);
    P_index = index_of_complement(&P_subset);
    increment_partial_alloc(&final_alloc, &right_increment, &J_index, &P_index);
    destroy_frac_scp(right_scp);
    destroy_partial_alloc(right_increment);
    destroy_index(J_index);
    destroy_index(P_index);
  }

  destroy_GCPS_allocation_stuff(nst, &P_subset, &J_subset, theta, favorites, alpha,
				&working_scp, &feasible_guide);
  return final_alloc;
}

