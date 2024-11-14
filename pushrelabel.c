#include "pushrelabel.h"
 
void push_relabel(struct process_scp* input, struct partial_alloc* max_flow_alloc) {
  int i, j, nst, nsc, done, found_active, found_target, pivot_node, target_node;
  
  nst = input->no_students;
  nsc = input->no_schools;

  /* The nodes are represented by the integers 0,...,nst+nsc+1, where 0 is the
     source, 1,...nst are the students, nst+1,...,nst+nsc are the schools, and
     nst+nsc+1 is the sink. */

  /* Initialize labels to 2 * nsc + 2 for the source and 0 for all other nodes */
  
  int* labels = initialize_labels(nst, nsc);

  /* Set capacities to time_remaining for the arc from the
     source to a school, the maximum for an arc from a student to a
     school, the quota for an arc from a school to the sink, and 0 for
     all other arcs. */
  
  double** capacities = set_capacities(input, nst, nsc);

  /* A preflow specifies a preflow along each arc that is the negative
     of the preflow along the opposite arc and that does not exceed the
     arc's capacity, such that for each node (other than the source
     and sink) the total preflow in is at least as large as the total
     preflow out.  The initial preflow is time_remaining for the arc
     from the source to a student, - time_remaining from the arc from
     a student to the source, and 0 for all other arcs. */
  
  double** preflows = initialize_preflows(input, nst, nsc);
    
  /* The excess of a preflow at a node (other than the source) is the
     total preflow in minus the total preflow out.  For the initial
     preflow the excess is time_remaining at each student and zero 
     at each school and the sink. */

  double* excess = initialize_excess(input, nst, nsc);

  /* Run the algorithm */

  done = 0;
  while (!done) {
    done = 1;
    pivot_node = 1;
    found_active = 0;
    while (pivot_node < nst+nsc+1 && !found_active) {
      if (excess[pivot_node] > 0.000001) {
	found_active = 1;
	done = 0;
      }
      else {
	pivot_node++;
      }
    }
    if (found_active) {
      if (relabel_is_valid(pivot_node, capacities, preflows, excess, labels, nst, nsc)) {	
	relabel(pivot_node, capacities, preflows, labels, nst, nsc);
      }
      else {
	found_target = 0;
	target_node = 0;
	while (!found_target) {
	  if (push_is_valid(pivot_node, target_node, capacities, preflows, excess, labels)) {
	    found_target = 1;
	  }
	  else {
	    target_node++;
	  }
	}
	push(pivot_node, target_node, capacities, preflows, excess);
      } 
    }
  }

  /* Report the result */
  
  for (i = 1; i <= nst; i++) {
      for (j = 1; j <= nsc; j++) {
      max_flow_alloc->allocations[i-1][j-1] = preflows[i][nst+j]; 
    }
  }
  
  destroy_pointers(labels, capacities, preflows, excess, nst, nsc); 
}

int satisfies_the_GMC(struct process_scp* input) {
  int i, j, nst, nsc;
  double total_flow;
  
  nst = input->no_students;
  nsc = input->no_schools;
  
  struct partial_alloc max_flow_alloc = zero_alloc_for_process_scp(input);
  push_relabel(input, &max_flow_alloc);

  total_flow = 0.0;
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      total_flow += max_flow_alloc.allocations[i-1][j-1];
    }
  }

  destroy_partial_alloc(max_flow_alloc);

  if (total_flow < nst * input->time_remaining - 0.000001) {
    return 0;
  }
  else {
    return 1;
  }
}


int* initialize_labels(int nst, int nsc) {
  int k;
  
  int*  answer = malloc((nst + nsc + 2) * sizeof(int));
  answer[0] = 2 * nsc + 2;
  for (k = 1; k <= nst+nsc+1; k++) {
    answer[k] = 0;
  }

  return answer;
}

double** set_capacities(struct process_scp* input, int nst, int nsc) {
  int i, j, k, l;
  
  double** answer = malloc((nst+nsc+2) * sizeof(double*));
  for (k = 0; k <= nst+nsc+1; k++) {
    answer[k] = malloc((nst+nsc+2) * sizeof(double));
    for (l = 0; l <= nst+nsc+1; l++) {
      answer[k][l] = 0.0;
    }
  }

  for (i = 1; i <= nst; i++) {
    answer[0][i] = input->time_remaining;
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      answer[i][nst+j] = input->eligible[i-1][j-1] * input->time_remaining;
    }
  }

  for (j = 1; j <= nsc; j++) {
    answer[nst+j][nst+nsc+1] = input->quotas[j-1];
  }

  return answer;
}

double** initialize_preflows(struct process_scp* input, int nst, int nsc) {
  int i, k , l;

  double** answer = malloc((nst+nsc+2) * sizeof(double*));
  for (k = 0; k <= nst+nsc+1; k++) {
    answer[k] = malloc((nst+nsc+2) * sizeof(double));
    for (l = 0; l <= nst+nsc+1; l++) {
      answer[k][l] = 0.0;
    }
  }

  for (i = 1; i <= nst; i++) {
    answer[0][i] = input->time_remaining;
    answer[i][0] = - input->time_remaining;
  }

  return answer;
}

double* initialize_excess(struct process_scp* input, int nst, int nsc) {
  int i, j;
  
  double* answer = malloc((nst+nsc+2) * sizeof(double));
  answer[0] = - nst * input->time_remaining;
  for (i = 1; i <= nst; i++) {
    answer[i] = input->time_remaining;
  }
  for (j = 1; j <= nsc; j++) {
    answer[nst+j] = 0.0;
  }
  answer[nst+nsc+1] = 0.0;

  return answer;
}

int push_is_valid(int arc_tail, int arc_head,
		  double** capacities, double** preflows, double* excess, int* labels) {
  if (excess[arc_tail] > 0.000001 &&
      capacities[arc_tail][arc_head] - preflows[arc_tail][arc_head] > 0.000001 &&
      labels[arc_head] == labels[arc_tail] - 1) {
    return 1;
  }
  
  return 0;
}

void push(int arc_tail, int arc_head, double** capacities, double** preflows, double* excess) {
  
  double res_cap, delta;

  res_cap = capacities[arc_tail][arc_head] - preflows[arc_tail][arc_head];
  if (excess[arc_tail] <= res_cap) {
    delta = excess[arc_tail];
  }
  else {
    delta = res_cap;
  }

  preflows[arc_tail][arc_head]+=delta;
  preflows[arc_head][arc_tail]-=delta;
  excess[arc_tail]-=delta;
  excess[arc_head]+=delta;
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

void destroy_pointers(int* labels, double** capacities, double** preflows, double* excess,
		      int nst, int nsc) {
  int k;
  
  free(labels);
  for (k = 0; k <= nst+nsc+1; k++) {
    free(capacities[k]);
  }
  free(capacities);
  for (k = 0; k <= nst+nsc+1; k++) {
    free(preflows[k]);
  }
  free(preflows);
  free(excess);
}
