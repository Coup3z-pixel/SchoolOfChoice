#ifndef FDAMCC_h
#define FDAMCC_h

#include "normal.h"
#include "schchprob.h"
#include "vecmatrx.h"
#include "efficient.h"

/****************************** mcca code ****************************/

partial_alloc mcca_alloc_plus_coarse_cutoffs_FDA(process_scp* myscp, int* coarse);

void naive_increase_of_cutoffs(process_scp* myscp, double* cutoffs);

partial_alloc mcca_alloc_FDA(process_scp* myscp);

/****************************** mccb code ****************************/

partial_alloc mccb_alloc_plus_coarse_cutoffs_FDA(process_scp* myscp, int* coarse);

void naive_reduction_of_cutoffs(process_scp* myscp, double* cutoffs);

partial_alloc mccb_alloc_FDA(process_scp* myscp);

/************************** general functions and utilities **************************/

double student_demand_at_new_cutoff(process_scp* myscp, partial_alloc* demands,
				    int i, int j, double new_cutoff);

double demand_at_new_cutoff(process_scp* myscp, partial_alloc* demands, int j, 
				double new_cutoff);

double naive_eq_cutoff(process_scp* myscp, partial_alloc* demands, int j);

double* naive_eq_cutoffs(process_scp* myscp, partial_alloc* demands);

int cutoffs_are_an_mcc(process_scp* myscp, double* cutoffs);

void print_cutoffs(int nsc, double* cutoffs);

#endif /* FDAMCC_h */
