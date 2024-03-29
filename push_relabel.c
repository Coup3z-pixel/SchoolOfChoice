#include "push_relabel.h"

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

int push_relabel(struct process_cee* input, struct partial_alloc* max_flow_alloc) {
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
	  else {
	    k++;
	  }
	}
	
	push(active_node, k, capacities, preflows, excess);
      }
      
    }
    if (active_node < nst+nsc+1) {
      active_node = 1;
    }
  }

  /* Report the result */
  
  for (i = 1; i <= nst; i++) {
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
