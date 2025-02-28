#ifndef GCPSCODE_H
#define GCPSCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "segment.h"
#include "efficient.h"
#include "defaccep.h"

struct partial_alloc simple_GCPS_alloc(struct process_scp* input); 

/* The next function uses deferred_acceptance to compute a feasible
allocation, then invokes GCPS_allocation_with_guide to compute the
allocation.  probe_list is a list of pivots.  The idea is that
computing pivots directly is onerous, and the pivots are likely to be
the same when we pass from one segment to the next, so we apply all
the valid old pivots before computing new ones.  */

struct partial_alloc GCPS_allocation(struct process_scp* input, int* no_segments, int* no_splits,
				     int* no_new_pivots, int* no_old_pivots, int* h_sum); 

double compute_until_next_critical_pair(struct process_scp* working_scp,
					struct partial_alloc* feasible_guide,
					struct partial_alloc* final_alloc,
					struct pivot_list* probe_list,
					struct subset* P_subset, struct subset* J_subset,
					int* no_segments, int* no_new_pivots, int* no_old_pivots,
					int* h_sum);

/* GCPS_allocation_with_guide allocates each student's favorite school for as long as it
is possible to find a path above the allocation in the set of feasible allocations.  
This can end because time runs out, in which case the computation is complete, or it can end
because the search for a direction in the set of feasible allocations reveals a critical
pair, in which case we descend recursively to the computation of the two subproblems. */

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						struct pivot_list* probe_list,
						int* no_segments, int* no_splits,
						int* no_new_pivots,
						int* no_old_pivots,int* h_sum);

void descend_to_left_subproblem(struct process_scp* working_scp,
				struct partial_alloc* final_alloc,
				struct partial_alloc* feasible_guide, 
				struct pivot_list* probe_list,
				struct subset* P_subset, struct subset* J_subset,
				int* no_segments, int* no_splits, int* no_new_pivots,
				int* no_old_pivots, int* h_sum);

void descend_to_right_subproblem(struct process_scp* working_scp,
				 struct partial_alloc* final_alloc,
				 struct partial_alloc* feasible_guide, 
				 struct pivot_list* probe_list,
				 struct subset* P_subset, struct subset* J_subset,
				 int* no_segments, int* no_splits, int* no_new_pivots,
				 int* no_old_pivots, int* h_sum);

#endif /* GCPSCODE_H */





