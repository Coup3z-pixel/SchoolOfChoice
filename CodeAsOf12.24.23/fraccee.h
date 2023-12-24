#ifndef FRACCEE_H
#define FRACCEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "subset.h"
#include "partalloc.h"

struct frac_cee {
  int no_students;
  int no_schools;
  double* quotas;
  double** maximums;
  double time_remaining;
};

struct frac_scp {
  struct frac_cee cee;
  int* no_eligible_schools; /* for each student, the number of eligible schools */
  int** preferences; /* each student's list of eligible schools, from best to worst */
};

void destroy_frac_cee(struct frac_cee my_frac);

void destroy_frac_scp(struct frac_scp my_scp);

void print_frac_cee(struct frac_cee* my_frac);

void print_frac_scp(struct frac_scp* my_frac);

void copy_frac_cee(struct frac_cee* my_cee, struct frac_cee* copy);

void copy_frac_scp(struct frac_scp* my_scp, struct frac_scp* copy);

struct frac_cee make_toy_frac_cee();

struct frac_scp make_toy_frac_scp();

struct partial_alloc zero_alloc_for_frac(struct frac_cee* my_cee);

struct frac_cee critical_sub_frac_cee(struct frac_cee* my_cee, struct subset* J_subset);

struct frac_scp critical_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset);

struct frac_cee crit_compl_sub_frac_cee(struct frac_cee* my_cee, struct subset* J_subset,
					struct subset* P_subset);

struct frac_scp crit_compl_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset);

#endif /* FRACCEE_H */
