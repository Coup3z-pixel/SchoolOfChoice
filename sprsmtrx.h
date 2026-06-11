#ifndef SPRSMTRX_H
#define SPRSMTRX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "subset.h"

typedef struct Int_Sparse_Matrix{
  int no_rows;
  int no_cols;
  int* nos_active_cols;
  int** index_of_active_cols;
  int** entries;
} int_sparse_matrix;

typedef struct Dbl_Sparse_Matrix{
  int no_rows;
  int no_cols;
  int* nos_active_cols;
  int** index_of_active_cols;
  double** entries;
} dbl_sparse_matrix;

int int_entry(int_sparse_matrix* mymat, int row_no, int col_no);

void set_int_entry(int_sparse_matrix* mymat, int row_no, int col_no, int val);

void increment_int_entry(int_sparse_matrix* mymat, int row_no, int col_no, int incr);


double dbl_entry(dbl_sparse_matrix* mymat, int row_no, int col_no);

void set_dbl_entry(dbl_sparse_matrix* mymat, int row_no, int col_no, double val);

void increment_dbl_entry(dbl_sparse_matrix* mymat, int row_no, int col_no, double incr);

int_sparse_matrix copy_of_int_sp_matrix(int_sparse_matrix* given);

dbl_sparse_matrix copy_of_dbl_sp_mat(dbl_sparse_matrix* given);

 
int* new_nos_active_cols(dbl_sparse_matrix* mydsp,
			 subset* J_subset, subset* P_subset);

int** new_index_of_active_cols(dbl_sparse_matrix* mydsp,
			       subset* J_subset, subset* P_subset);

dbl_sparse_matrix zero_dbl_sp_mat_for_subsets(dbl_sparse_matrix* mydsp,
						     subset* J_subset,
						     subset* P_subset);

int_sparse_matrix zero_int_sp_mat_from_dbl_sp_mat(dbl_sparse_matrix* mydsp);

dbl_sparse_matrix zero_dbl_sp_mat_from_int_sp_mat(int_sparse_matrix* mydsp);

void destroy_int_sp_mat(int_sparse_matrix* mymat);

void destroy_dbl_sp_mat(dbl_sparse_matrix* mymat);

#endif /* SPRSMTRX_H */
