#ifndef PUSH_RELABEL_H
#define PUSH_RELABEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




#include "normal.h"
#include "subset.h"
#include "cee.h"
#include "schchprob.h"
#include "partalloc.h"

int push_is_valid(int source, int target,
		  double** capacities, double** preflows, double* excess, int* labels);

void push(int source, int target, double** capacities, double** preflows, double* excess);

int relabel_is_valid(int node, double** capacities, double** preflows,
		     double* excess, int* labels, int nst, int nsc);

void relabel(int node, double** capacities, double** preflows, int* labels, int nst, int nsc);

int push_relabel(struct process_cee* input, struct partial_alloc* max_flow_alloc);

#endif /* PUSH_RELABEL_H */
