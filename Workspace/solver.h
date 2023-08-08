#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "partalloc.h"
#include "parser.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp);

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp, struct square_matrix* related,
					 struct subset* target, int depth,
					 struct subset* overallocated_schools, struct subset* new_critical_set,
					 struct subset_list* submission_list, struct subset_list* watch_list,
					 int mode);

#endif /* SOLVER_H */
