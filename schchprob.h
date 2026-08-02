#ifndef SCHCHPROB_H
#define SCHCHPROB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "sprsmtrx.h" 

typedef struct Input_Sch_Ch_Prob {
  int no_students;
  int no_schools;
  int* quotas;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int** priorities; /* each schools's priorities of eligible students */
} input_sch_ch_prob;

typedef struct Process_Scp {
  int no_students;
  int no_schools;
  double* quotas;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int_sparse_matrix priorities;  /* each schools's priorities of students */
  double time_remaining;
} process_scp;

int get_input_priority(input_sch_ch_prob* myiscp, int i, int j); 

int get_priority(process_scp* myscp, int i, int j);

int maximum_priority(process_scp* myscp);

/* favorites[i-1] is i's favorite among schools i is eligible for */

int* get_favorites(process_scp* working_scp);

int is_eligible(process_scp* myscp, int i, int j);

int is_eligible_input(input_sch_ch_prob* myiscp, int i, int j);

int inp_safe_school(input_sch_ch_prob* myiscp, int i);

int safe_school(process_scp* myscp, int i);

int two_iscp_are_same(input_sch_ch_prob* myiscp1, input_sch_ch_prob* myiscp2);

int j_is_only_school_for_i(process_scp* myscp, int i, int j);

int a_school_has_too_many_only_students(process_scp* myscp);

int student_ranking_of_school(process_scp* myscp, int i, int j);

int safe_schools_are_safe(input_sch_ch_prob* myscp);

int_sparse_matrix new_int_sp_mat(process_scp* myscp);

dbl_sparse_matrix new_dbl_sp_mat_for_process(process_scp* myscp);

dbl_sparse_matrix new_dbl_sp_mat_for_input(input_sch_ch_prob* myscp);

int_sparse_matrix new_int_sp_mat_for_input(input_sch_ch_prob* myscp);

int_sparse_matrix sparse_priorities(input_sch_ch_prob* myscp);

process_scp process_scp_from_input(input_sch_ch_prob* myscp);

process_scp left_sub_process_scp(process_scp* myscp,subset* J_subset, subset* P_subset);

process_scp right_sub_process_scp(process_scp* myscp, subset* J_subset, subset* P_subset);

process_scp reduced_scp(process_scp* myscp, int* coarse_cutoffs);

input_sch_ch_prob reduced_input_scp(input_sch_ch_prob* myiscp, int* coarse_cutoffs);

input_sch_ch_prob copy_of_input_scp(input_sch_ch_prob* myiscp);

input_sch_ch_prob stu_no_priority_scp(input_sch_ch_prob* myiscp);

input_sch_ch_prob make_toy_sch_ch_prob();

process_scp make_toy_process_scp();

int J_P_split_OK(process_scp* myscp, subset* J_subset, subset* P_subset);

void print_input_scp_body(input_sch_ch_prob* myscp);

void fprint_input_scp_body(input_sch_ch_prob* myscp);

void print_input_sch_ch_prob(input_sch_ch_prob* myscp);

void print_process_scp(process_scp* my_proc);

void fprint_process_scp(process_scp* my_proc);

void destroy_input_sch_ch_prob(input_sch_ch_prob myscp);

void destroy_process_scp(process_scp myscp);

#endif /* SCHCHPROB_H */
