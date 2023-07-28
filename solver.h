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
					 int depth, struct subset_list* watch_list,
					 struct subset_list* found_critical_sets,
					 struct subset_list* found_overalloc_sets,
					 int* we_found_somthing);

#endif /* SOLVER_H */
