#ifndef OLDMCC_h
#define OLDMCC_h

#include "normal.h"
#include "schchprob.h"
#include "vecmatrx.h"
#include "efficient.h"

/****************************** mcca code ****************************/

struct partial_alloc mcca_alloc_OLD(struct process_scp* myscp);

struct partial_alloc mcca_alloc_plus_coarse_cutoffs_OLD(struct process_scp* myscp, int* coarse);

double excess_sum_before_naive_increase_of_cutoffs(struct process_scp* myscp, double* cutoffs);

void naive_increase_of_cutoffs(struct process_scp* myscp, double* cutoffs,
				struct partial_alloc* demands);

double naive_eq_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands);

double* naive_eq_cutoffs(struct process_scp* myscp, struct partial_alloc* demands);

double demand_at_new_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
				double new_cutoff);

double* excess_demands(struct process_scp* myscp, struct partial_alloc* demands);

struct partial_alloc compute_demands(struct process_scp* myscp, double* cutoffs);

int cutoffs_are_an_mcc(struct process_scp* myscp, double* cutoffs);

/****************************** mccb code ****************************/

struct partial_alloc mccb_alloc_OLD(struct process_scp* myscp);

struct partial_alloc mccb_alloc_plus_coarse_cutoffs_OLD(struct process_scp* myscp, int* coarse);

double deficit_sum_before_naive_reduction_of_cutoffs(struct process_scp* myscp, double* cutoffs);

void naive_reduction_of_cutoffs(struct process_scp* myscp, double* cutoffs);

double* demand_deficits(struct process_scp* myscp, struct partial_alloc* demands);

/********************************* utilities **************************/

void print_cutoffs(int nsc, double* cutoffs);

void print_double_list(int nsc, double* list);

#endif /* OLDMCC_h */
