#ifndef SCHCHPROB_H
#define SCHCHPROB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subset.h"

struct input_sch_ch_prob {
  int no_students;
  int no_schools;
  int* quotas;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int** priorities; /* each schools's priorities of eligible students */
};

struct process_scp {
  int no_students;
  int no_schools;
  double* quotas;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** eligible; /* eligible[i-1][j-1] = 1 if i is eligible for j, 0 otherwise */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int** priorities; /* each schools's priorities of students */
  double time_remaining;
};

void compute_eligibility_matrix(struct process_scp* my_scp);

void copy_process_scp(struct process_scp* my_scp, struct process_scp* copy);

struct process_scp process_scp_from_input(struct input_sch_ch_prob* my_scp);

struct process_scp left_sub_process_scp(struct process_scp* my_scp, struct subset* J_subset,
					    struct subset* P_subset);

struct process_scp right_sub_process_scp(struct process_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset);


/* favorites[i-1] is i's favorite among schools allowed to i in working cee */

int* get_favorites(struct process_scp* working_scp);

struct input_sch_ch_prob make_toy_sch_ch_prob();

struct process_scp make_toy_process_scp();

int safe_schools_are_safe(struct input_sch_ch_prob* my_scp);

void print_generic_header();

void print_input_scp_body(struct input_sch_ch_prob* my_scp);

void print_input_sch_ch_prob(struct input_sch_ch_prob* my_scp);

void print_process_scp(struct process_scp* my_proc);

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_scp);

void destroy_process_scp(struct process_scp my_scp);

#endif /* SCHCHPROB_H */
