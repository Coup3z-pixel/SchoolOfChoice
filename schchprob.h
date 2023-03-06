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

void destroy_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob);

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_sch_ch_prob);

void print_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob);

void print_input_sch_ch_prob(struct input_sch_ch_prob my_sch_ch_prob);

/* The next function converts the obvious ints to doubles, and sets 
   time_remaining to 1.0. */

struct sch_ch_prob sch_ch_prob_from_input(struct input_sch_ch_prob my_sch_ch_prob);

/* The next function sets the priority of student i at school j to 0
   if the input priority is 0 or less than the school's priority
   threshold, and 1 otherwise.  It resets each school's priority
   threshold to 1. */

struct sch_ch_prob reduced_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob);

#endif /* SCHCHPROB_H */
