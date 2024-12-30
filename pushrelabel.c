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
  
  struct network_flow cap = set_capacities(input);

  /* A preflow specifies a preflow along each arc that is the negative
     of the preflow along the opposite arc and that does not exceed the
     arc's capacity, such that for each node (other than the source
     and sink) the total preflow in is at least as large as the total
     preflow out.  The initial preflow is time_remaining for the arc
     from the source to a student, - time_remaining from the arc from
     a student to the source, and 0 for all other arcs. */
  
  struct network_flow pre = initialize_preflows(input);

  /* The excess of a preflow at a node (other than the source) is the
     total preflow in minus the total preflow out.  For the initial
     preflow the excess is time_remaining at each student and zero 
     at each school and the sink. */

  double* excess = initialize_excess(input, nst, nsc);

  /* Run the algorithm */

  done = 0;
  while (!done) {

    /*
    for (int k = 0; k <= nst + nsc + 1; k++) {
      for (int l = 0; l <= nst + nsc + 1; l++) {
	if (preflow(&pre, k, l) >= 0.0) {
	  printf(" ");
	}
	if (fabs(preflow(&pre, k, l)) < 0.000001) {
	  printf("0.00  ");
	}
	else {
	  printf("%1.2f  ", preflow(&pre, k, l));
	}
      }
      printf("\n");
    }
    printf("\n");
    */
    
    done = 1;
    pivot_node = 1;
    found_active = 0;
    while (pivot_node < nst+nsc+1 && !found_active) {
      if (excess[pivot_node] > 0.000000001) {
	found_active = 1;
	done = 0;
      }
      else {
	pivot_node++;
      }
    }
    if (found_active) {
      if (relabel_is_valid(pivot_node, &cap, &pre, labels)) {
		relabel(pivot_node, &cap, &pre, labels);
      }
      else {
	found_target = 0;
	target_node = 1;
	while (!found_target) {
	  if (push_is_valid(pivot_node, target_node, &cap, &pre, excess, labels)) {
	    found_target = 1;
	  }
	  else {
	    target_node++;
	  }
	}
	push(pivot_node, target_node, &cap, &pre, excess);
      } 
    }
  }

  /* Report the result */
  
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      set_entry(max_flow_alloc, i, j, preflow(&pre, i, nst+j));
    }
  }

  destroy_network_flow(cap);
  destroy_network_flow(pre);
  free(labels);
  free(excess);
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
      total_flow += get_entry(&max_flow_alloc, i, j);
    }
  }

  destroy_partial_alloc(max_flow_alloc);

  if (total_flow < nst * input->time_remaining - 0.000000001) {
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

struct network_flow set_capacities(struct process_scp* input) {
  int i, j, nst, nsc;

  nst = input->no_students;
  nsc = input->no_schools;

  struct network_flow answer;

  answer.flow_from_source = malloc(nst * sizeof(double));
  for (i = 1; i <= nst; i++) {
    answer.flow_from_source[i-1] = input->time_remaining;
  }

  answer.flows = zero_alloc_for_process_scp(input);
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      set_entry(&(answer.flows), i, j, input->eligible[i-1][j-1] * input->time_remaining);
    }
  }

  answer.flow_to_sink = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer.flow_to_sink[j-1] = input->quotas[j-1];
  }

  return answer;
}

double capacity(struct network_flow* cap, int arc_tail, int arc_head) {
  int nst, nsc;
  
  nst = (cap->flows).no_students;
  nsc = (cap->flows).no_schools;

  if (arc_tail == 0 && 1 <= arc_head && arc_head <= nst) {
    return cap->flow_from_source[arc_head-1];
  }

  if (1 <= arc_tail && arc_tail <= nst && nst + 1 <= arc_head && arc_head <= nst + nsc) {
    return get_entry(&(cap->flows), arc_tail, arc_head - nst);
  }

  if (nst + 1 <= arc_tail && arc_tail <= nst + nsc && arc_head == nst + nsc + 1) {
    return cap->flow_to_sink[arc_tail-nst-1];
  }

  return 0.0;
}

struct network_flow initialize_preflows(struct process_scp* input) {
  int i, j, nst, nsc;

  nst = input->no_students;
  nsc = input->no_schools;

  struct network_flow answer;

  answer.flow_from_source = malloc(nst * sizeof(double));
  for (i = 1; i <= nst; i++) {
    answer.flow_from_source[i-1] = input->time_remaining;
  }

  answer.flows = zero_alloc_for_process_scp(input);

  answer.flow_to_sink = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer.flow_to_sink[j-1] = 0.0;
  }

  return answer;
}

double preflow(struct network_flow* pre, int arc_tail, int arc_head) {
  int nst, nsc;
  
  nst = (pre->flows).no_students;
  nsc = (pre->flows).no_schools;
  
  if (arc_tail == 0) {
    if (1 <= arc_head && arc_head <= nst) {
      return pre->flow_from_source[arc_head-1];
    }
    else {
      return 0.0;
    }
  }
  
  if (1 <= arc_tail && arc_tail <= nst) {
    if (arc_head == 0) {
      return -pre->flow_from_source[arc_tail-1];      
    }
    if (nst + 1 <= arc_head && arc_head <= nst + nsc) {
      return get_entry(&(pre->flows), arc_tail, arc_head - nst);
    }
  }
  
  if (nst + 1 <= arc_tail && arc_tail <= nst + nsc) {
    if (1 <= arc_head && arc_head <= nst) {
      return -get_entry(&(pre->flows), arc_head, arc_tail - nst);
    }
    if (arc_head == nst + nsc + 1) {
      return pre->flow_to_sink[arc_tail-nst-1];
    }
  }

  if (arc_tail == nst + nsc + 1) {
    if (nst + 1 <= arc_head && arc_head <= nst + nsc) {
      return -pre->flow_to_sink[arc_head-nst-1];
    }
  }

  return 0.0;
}

void increment_preflow(struct network_flow* pre, int arc_tail, int arc_head, double incr) {
  int nst, nsc;
  
  nst = (pre->flows).no_students;
  nsc = (pre->flows).no_schools;
  
  if (arc_tail == 0) {
    if (1 <= arc_head && arc_head <= nst) {
      pre->flow_from_source[arc_head-1] += incr;
    }
  }

  if (1 <= arc_tail && arc_tail <= nst) {
    if (arc_head == 0) {
      pre->flow_from_source[arc_tail-1] -= incr;      
    }
    if (nst + 1 <= arc_head && arc_head <= nst + nsc) {
      increment_entry(&(pre->flows), arc_tail, arc_head - nst, incr);
    }
  }

  if (nst + 1 <= arc_tail && arc_tail <= nst + nsc) {
    if (1 <= arc_head && arc_head <= nst) {
      increment_entry(&(pre->flows), arc_head, arc_tail - nst, -incr);
    }
    if (arc_head == nst + nsc + 1) {
      pre->flow_to_sink[arc_tail-nst-1] += incr;
    }
  }

  if (arc_tail == nst + nsc + 1) {
    if (nst + 1 <= arc_head && arc_head <= nst + nsc) {
      pre->flow_to_sink[arc_head-nst-1] -= incr;
    }
  }
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

int push_is_valid(int arc_tail, int arc_head, struct network_flow* cap,
		      struct network_flow* pre, double* excess, int* labels) {
  
  if (excess[arc_tail] > 0.000000001 &&
      capacity(cap, arc_tail, arc_head) - preflow(pre, arc_tail, arc_head) > 0.000000001
      && labels[arc_head] == labels[arc_tail] - 1) {
    return 1;
  }
  
  return 0;
}

int relabel_is_valid(int node, struct network_flow* cap, struct network_flow* pre,
			 int* labels) {
  int k, nst, nsc;
  
  nst = (pre->flows).no_students;
  nsc = (pre->flows).no_schools;

  for (k = 0; k <= nst+nsc+1; k++) {    
    if (capacity(cap, node, k) - preflow(pre, node, k) > 0.000000001 &&
	labels[node] > labels[k]) {
      return 0;
    }
  }

  return 1;
}

void push(int arc_tail, int arc_head, struct network_flow* cap, struct network_flow* pre,
	      double* excess) {
  
  double res_cap, delta;

  res_cap = capacity(cap, arc_tail, arc_head) - preflow(pre, arc_tail, arc_head);
  if (excess[arc_tail] <= res_cap) {
    delta = excess[arc_tail];
  }
  else {
    delta = res_cap;
  }

  increment_preflow(pre, arc_tail, arc_head, delta);
  
  excess[arc_tail]-=delta;
  excess[arc_head]+=delta;
}

void relabel(int node, struct network_flow* cap, struct network_flow* pre, int* labels) {
  int k, min, hit, nst, nsc;
  
  nst = (pre->flows).no_students;
  nsc = (pre->flows).no_schools;

  hit = 0;

  for (k = 1; k <= nst+nsc+1; k++) {
    if (capacity(cap, node, k) - preflow(pre, node, k) > 0.000000001) {
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

void destroy_network_flow(struct network_flow flow) {
  free(flow.flow_from_source);
  destroy_partial_alloc(flow.flows);
  free(flow.flow_to_sink);
}
