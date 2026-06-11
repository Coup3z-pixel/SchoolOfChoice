#ifndef GCPSCODE_H
#define GCPSCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "segment.h"
#include "efficient.h"
#include "defaccep.h"

partial_alloc simple_GCPS_alloc(input_sch_ch_prob* input); 

/* The next function uses deferred_acceptance to compute a feasible
allocation, then invokes GCPS_allocation_with_guide to compute the
allocation.  probe_list is a list of pivots.  The idea is that
computing pivots directly is onerous, and the pivots are likely to be
the same when we pass from one segment to the next, so we apply all
the valid old pivots before computing new ones.  */

partial_alloc GCPS_allocation(input_sch_ch_prob* input, run_data* data); 

/* GCPS_allocation_with_guide allocates each student's favorite school for as long as it
is possible to find a path above the allocation in the set of feasible allocations.  
This can end because time runs out, in which case the computation is complete, or it can end
because the search for a direction in the set of feasible allocations reveals a critical
pair, in which case we descend recursively to the computation of the two subproblems. */

partial_alloc simple_GCPS_alloc_with_guide(input_sch_ch_prob* input,
					   partial_alloc* feasible_guide);

partial_alloc GCPS_allocation_with_guide(process_scp* input,
					 partial_alloc* feasible_guide,
					 pivot_list* probe_list,
					 run_data* data);

double compute_until_next_critical_pair(process_scp* working_scp,
					partial_alloc* feasible_guide,
					partial_alloc* final_alloc,
					pivot_list* probe_list,
					subset* P_subset, subset* J_subset,
					run_data* data);

void descend_to_left_subproblem(process_scp* working_scp,
				partial_alloc* final_alloc,
				partial_alloc* feasible_guide, 
				pivot_list* probe_list,
				subset* P_subset, subset* J_subset,
				run_data* data);

void descend_to_right_subproblem(process_scp* working_scp,
				 partial_alloc* final_alloc,
				 partial_alloc* feasible_guide, 
				 pivot_list* probe_list,
				 subset* P_subset, subset* J_subset,
				 run_data* data);

#endif /* GCPSCODE_H */





