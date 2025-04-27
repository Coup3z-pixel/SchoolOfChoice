#ifndef SCHCHPROB_H
#define SCHCHPROB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "sprsmtrx.h" 

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
  int** preferences; /* each student's list of eligible schools, from best to worst */
  struct int_sparse_matrix priorities;  /* each schools's priorities of students */
  double time_remaining;
};

int get_input_priority(struct input_sch_ch_prob* my_scp, int i, int j); 

int get_priority(struct process_scp* my_scp, int i, int j);

int max_priority(struct process_scp* my_scp);

/* favorites[i-1] is i's favorite among schools i is eligible for */

int* get_favorites(struct process_scp* working_scp);

int is_eligible(struct process_scp* myscp, int i, int j);

int student_ranking_of_school(struct process_scp* myscp, int i, int j);

int safe_schools_are_safe(struct input_sch_ch_prob* my_scp);


struct int_sparse_matrix new_int_sp_mat(struct process_scp* myscp);

struct dbl_sparse_matrix new_dbl_sp_mat_for_process(struct process_scp* myscp);

struct dbl_sparse_matrix new_dbl_sp_mat_for_input(struct input_sch_ch_prob* myscp);

struct int_sparse_matrix sparse_priorities(struct input_sch_ch_prob* my_scp);

struct process_scp process_scp_from_input(struct input_sch_ch_prob* my_scp);

struct process_scp left_sub_process_scp(struct process_scp* my_scp,
					struct subset* J_subset, struct subset* P_subset);

struct process_scp right_sub_process_scp(struct process_scp* my_scp,
					 struct subset* J_subset, struct subset* P_subset);

struct process_scp reduced_scp(struct process_scp* my_scp, int* coarse_cutoffs);

struct process_scp copy_of_process_scp(struct process_scp* my_scp);


struct input_sch_ch_prob make_toy_sch_ch_prob();

struct process_scp make_toy_process_scp();

void print_input_scp_body(struct input_sch_ch_prob* my_scp);

void print_input_sch_ch_prob(struct input_sch_ch_prob* my_scp);

void print_process_scp(struct process_scp* my_proc);

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_scp);

void destroy_process_scp(struct process_scp my_scp);

#endif /* SCHCHPROB_H */
