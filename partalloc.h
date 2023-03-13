#ifndef PARTALLOC_H
#define PARTALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schchprob.h"

struct partial_alloc {
  int no_students;
  int no_schools;
  double** allocations; 
};

struct partial_alloc zero_partial_alloc(struct sch_ch_prob* my_scp);

struct partial_alloc allocate_until_new_time(struct sch_ch_prob* my_scp,
					     double new_time_remaining);

void destroy_partial_alloc(struct partial_alloc* my_partial_alloc);

void print_partial_alloc(struct partial_alloc my_partial_alloc);

void increment_partial_alloc(struct partial_alloc* base, struct partial_alloc* increment,
			     struct index* stu_index,struct index* sch_index); 

#endif /* PARTALLOC_H */
