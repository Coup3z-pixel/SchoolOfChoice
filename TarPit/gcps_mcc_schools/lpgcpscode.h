#ifndef LPGCPSCODE_H
#define LPGCPSCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "critpair.h"

struct partial_alloc simple_LPGCPS_alloc(struct process_scp* input); 

struct partial_alloc LPGCPS_allocation(struct process_scp* input); 

struct partial_alloc LPGCPS_allocation_with_guide(struct process_scp* input);

void find_critical_pair(struct process_scp* working_scp, 
			struct subset* P_subset, struct subset* J_subset,
			int* critical_pair_found);

void LPdescend_to_left_subproblem(struct process_scp* working_scp,
				  struct partial_alloc* final_alloc,
				  struct subset* J_subset);

void LPdescend_to_right_subproblem(struct process_scp* working_scp,
				   struct partial_alloc* final_alloc,
				   struct subset* P_subset, struct subset* J_subset);

struct mixed_tableau linear_program_from_scp(struct process_scp* working_scp);

#endif /* LPGCPSCODE_H */
