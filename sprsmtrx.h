#ifndef SPRSMTRX_H
#define SPRSMTRX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "subset.h"

struct int_sparse_matrix{
  int no_rows;
  int no_cols;
  int* nos_active_cols;
  int** index_of_active_cols;
  int** entries;
};

struct dbl_sparse_matrix{
  int no_rows;
  int no_cols;
  int* nos_active_cols;
  int** index_of_active_cols;
  double** entries;
};

int int_entry(struct int_sparse_matrix* mymat, int row_no, int col_no);

void set_int_entry(struct int_sparse_matrix* mymat, int row_no, int col_no, int val);

void increment_int_entry(struct int_sparse_matrix* mymat, int row_no, int col_no, int incr);


double dbl_entry(struct dbl_sparse_matrix* mymat, int row_no, int col_no);

void set_dbl_entry(struct dbl_sparse_matrix* mymat, int row_no, int col_no, double val);

void increment_dbl_entry(struct dbl_sparse_matrix* mymat, int row_no, int col_no, double incr);

struct int_sparse_matrix copy_of_int_sp_matrix(struct int_sparse_matrix* given);

struct dbl_sparse_matrix copy_of_dbl_sp_mat(struct dbl_sparse_matrix* given);

 
int* new_nos_active_cols(struct dbl_sparse_matrix* mydsp,
			 struct subset* J_subset, struct subset* P_subset);

int** new_index_of_active_cols(struct dbl_sparse_matrix* mydsp,
			       struct subset* J_subset, struct subset* P_subset);

struct dbl_sparse_matrix zero_dbl_sp_mat_for_subsets(struct dbl_sparse_matrix* mydsp,
						     struct subset* J_subset,
						     struct subset* P_subset);

struct int_sparse_matrix zero_int_sp_mat_from_dbl_sp_mat(struct dbl_sparse_matrix* mydsp);

void destroy_int_sp_mat(struct int_sparse_matrix* mymat);

void destroy_dbl_sp_mat(struct dbl_sparse_matrix* mymat);

#endif /* SPRSMTRX_H */
