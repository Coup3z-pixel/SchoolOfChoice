#ifndef MCCCODE_h
#define MCCCODE_h

#include "normal.h"
#include "schchprob.h"
#include "efficient.h"

struct partial_alloc mcca_alloc(struct process_scp* myscp);

struct partial_alloc mcca_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse);

double minimum_eq_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
		       double old_cutoff);

double demand_at_new_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
			    double new_cutoff);

double* excess_demands(struct process_scp* myscp, struct partial_alloc* demands);

struct partial_alloc compute_demands(struct process_scp* myscp, double* cutoffs);

#endif /* MCCCODE_h */
