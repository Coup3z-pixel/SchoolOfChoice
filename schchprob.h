#ifndef SCHCHPROB_H
#define SCHCHPROB_H

#include "cee.h"

struct sch_ch_prob {
  struct double_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
  int* priority_threshold; /* the schools' minimum priorities */
  double time_remaining;
};

struct sch_ch_prob make_toy_sch_ch_prob();

void destroy_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob);

void print_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob);

#endif /* SCHCHPROB_H */
