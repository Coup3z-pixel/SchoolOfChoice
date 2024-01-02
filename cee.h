#ifndef CEE_H
#define CEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subset.h"


struct input_cee {
  int no_students;
  int no_schools;
  int* quotas;
  int** priority;
};

struct process_cee {
  int no_students;
  int no_schools;
  double* quotas;
  double** maximums;
  double time_remaining;
};

void destroy_input_cee(struct input_cee my_cee);

void print_input_cee(struct input_cee* my_cee);

struct input_cee make_toy_cee();


void destroy_process_cee(struct process_cee my_proc);

void print_process_cee(struct process_cee* my_proc);

void copy_process_cee(struct process_cee* my_cee, struct process_cee* copy);

struct process_cee make_toy_process_cee();

struct process_cee process_cee_from_input_cee(struct input_cee* my_cee);

struct process_cee critical_sub_process_cee(struct process_cee* my_cee, struct subset* J_subset);

struct process_cee crit_compl_sub_process_cee(struct process_cee* my_cee, struct subset* J_subset,
					struct subset* P_subset);


#endif /* CEE_H */
