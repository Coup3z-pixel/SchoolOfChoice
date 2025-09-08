#ifndef FDAMCC_h
#define FDAMCC_h

#include "normal.h"
#include "schchprob.h"
#include "vecmatrx.h"
#include "efficient.h"

/****************************** mcca code ****************************/

struct partial_alloc mcca_alloc_plus_coarse_cutoffs_FDA(struct process_scp* myscp, int* coarse);

void naive_increase_of_cutoffs(struct process_scp* myscp, double* cutoffs);

struct partial_alloc mcca_alloc_FDA(struct process_scp* myscp);

/****************************** mccb code ****************************/

struct partial_alloc mccb_alloc_plus_coarse_cutoffs_FDA(struct process_scp* myscp, int* coarse);

void naive_reduction_of_cutoffs(struct process_scp* myscp, double* cutoffs);

struct partial_alloc mccb_alloc_FDA(struct process_scp* myscp);

/************************** general functions and utilities **************************/

double student_demand_at_new_cutoff(struct process_scp* myscp, struct partial_alloc* demands,
				    int i, int j, double new_cutoff);

double demand_at_new_cutoff(struct process_scp* myscp, struct partial_alloc* demands, int j, 
				double new_cutoff);

double naive_eq_cutoff(struct process_scp* myscp, struct partial_alloc* demands, int j);

double* naive_eq_cutoffs(struct process_scp* myscp, struct partial_alloc* demands);

int cutoffs_are_an_mcc(struct process_scp* myscp, double* cutoffs);

void print_cutoffs(int nsc, double* cutoffs);

#endif /* FDAMCC_h */
