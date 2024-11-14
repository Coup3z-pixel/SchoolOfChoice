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

void push_relabel(struct process_scp* input, struct partial_alloc* max_flow_alloc);

int satisfies_the_GMC(struct process_scp* input);

int* initialize_labels(int nst, int nsc);

double** set_capacities(struct process_scp* input, int nst, int nsc);

double** initialize_preflows(struct process_scp* input, int nst, int nsc);

double* initialize_excess(struct process_scp* input, int nst, int nsc);

int push_is_valid(int source, int target,
		  double** capacities, double** preflows, double* excess, int* labels);

void push(int source, int target, double** capacities, double** preflows, double* excess);

int relabel_is_valid(int node, double** capacities, double** preflows,
		     double* excess, int* labels, int nst, int nsc);

void relabel(int node, double** capacities, double** preflows, int* labels, int nst, int nsc);

void destroy_pointers(int* labels, double** capacities, double** preflows, double* excess,
		      int nst, int nsc);

#endif /* PUSHRELABEL_H */
