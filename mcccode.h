#ifndef MARKET_h
#define MARKET_h

#include "normal.h"
#include "schchprob.h"
#include "partalloc.h"

struct partial_alloc MCC_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse);

double naive_eq_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
		       double old_cutoff);

double demand_at_new_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
			    double new_cutoff);

double* excess_demands(struct process_scp* myscp, struct partial_alloc* demands);

struct partial_alloc compute_demands(struct process_scp* myscp, double* cutoffs);

#endif /* MARKET_h */
