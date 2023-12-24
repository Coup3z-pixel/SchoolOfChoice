#ifndef SCHCHPROB_H
#define SCHCHPROB_H

#include "cee.h"

struct input_sch_ch_prob {
  struct int_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int* priority_threshold; /* the schools' minimum priorities */
};

struct sch_ch_prob {
  struct double_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int* priority_threshold; /* the schools' minimum priorities */
  double time_remaining;
};

struct input_sch_ch_prob make_toy_sch_ch_prob();

void copy_sch_ch_prob(struct sch_ch_prob* my_scp, struct sch_ch_prob* copy);

void destroy_sch_ch_prob(struct sch_ch_prob my_scp);

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_scp);

void print_sch_ch_prob(struct sch_ch_prob* my_scp);

void print_input_sch_ch_prob(struct input_sch_ch_prob* my_scp);

/* The next function converts the obvious ints to doubles, and sets 
   time_remaining to 1.0. */

struct sch_ch_prob sch_ch_prob_from_input(struct input_sch_ch_prob* my_scp);

/* The next function sets the priority of student i at school j to 0
   if the input priority is 0 or less than the school's priority
   threshold, and 1 otherwise.  It resets each school's priority
   threshold to 1. */

struct sch_ch_prob reduced_sch_ch_prob(struct sch_ch_prob* my_scp);

struct sch_ch_prob sub_sch_ch_prob(struct sch_ch_prob* given_scp, 
				   struct subset* stu_subset, struct subset* sch_subset);

double time_remaining_of_gmc_equality(struct sch_ch_prob* my_scp,struct subset* school_subset,
				      int level, int* target_level,
				      struct subset* eating_students,
				      struct subset* captive_students,
				      struct subset* overallocated_schools,
				      int* exit_status);

double time_rem_after_first_gmc_eq(struct sch_ch_prob* my_scp, 
				   int level, int* target_level, int depth,
				   struct subset* crit_stu_subset,
				   struct subset* crit_sch_subset,
				   struct subset* overallocated_schools,
				   struct subset_list* observed_overallocated_sets,
				   struct subset_list* known_facets,
				   int* exit_status);

#endif /* SCHCHPROB_H */
