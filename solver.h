#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "partalloc.h"
#include "parser.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp);

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp, int level,
					 int* target_level, int depth,
					 struct subset* overallocated_schools,
					 struct subset* new_critical_set,
					 struct subset_list* observed_overallocated_sets,
					 struct subset_list* known_facets,
					 int* exit_status);

#endif /* SOLVER_H */
