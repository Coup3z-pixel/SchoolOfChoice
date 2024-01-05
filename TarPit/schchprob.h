#ifndef SCHCHPROB_H
#define SCHCHPROB_H

#include "cee.h"

struct input_sch_ch_prob {
  struct input_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int* priority_threshold; /* the schools' minimum priorities */
};

struct process_scp {
  struct process_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
};


void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_scp);

void destroy_process_scp(struct process_scp my_scp);

void print_input_sch_ch_prob(struct input_sch_ch_prob* my_scp);

void print_process_scp(struct process_scp* my_proc);

void copy_process_scp(struct process_scp* my_scp, struct process_scp* copy);

struct input_sch_ch_prob make_toy_sch_ch_prob();

struct process_scp make_toy_process_scp();

struct process_scp process_scp_from_input(struct input_sch_ch_prob* my_scp);

struct process_scp critical_sub_process_scp(struct process_scp* my_scp, struct subset* J_subset);

struct process_scp crit_compl_sub_process_scp(struct process_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset);

#endif /* SCHCHPROB_H */
