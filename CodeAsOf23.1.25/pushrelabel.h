#ifndef PUSHRELABEL_H
#define PUSHRELABEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"

struct labelling {
  int source;
  int* stu;
  int* sch;
  int sink;
};

struct network_flow {
  double* flow_from_source;
  struct partial_alloc flows;
  double* flow_to_sink;
};
 
void push_relabel(struct process_scp* input, struct partial_alloc* max_flow_alloc);

int satisfies_the_GMC(struct process_scp* input);

int* initialize_labels(int nst, int nsc);

struct network_flow set_capacities(struct process_scp* input);

double capacity(struct network_flow* cap, int arc_tail, int arc_head);

struct network_flow initialize_preflows(struct process_scp* input);

double preflow(struct network_flow* pre, int arc_tail, int arc_head);

void increment_preflow(struct network_flow* pre, int arc_tail, int arc_head, double incr);

double* initialize_excess(struct process_scp* input, int nst, int nsc);

int push_is_valid(int arc_tail, int arc_head, struct network_flow* cap,
		      struct network_flow* pre, double* excess, int* labels);

int relabel_is_valid(int node, struct network_flow* cap, struct network_flow* pre,
			 int* labels);

void push(int arc_tail, int arc_head, struct network_flow* cap, struct network_flow* pre,
	      double* excess);

void relabel(int node, struct network_flow* cap, struct network_flow* pre, int* labels);

void destroy_network_flow(struct network_flow flow);

#endif /* PUSHRELABEL_H */
