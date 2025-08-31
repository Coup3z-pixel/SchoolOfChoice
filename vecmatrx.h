#ifndef VECMATRX_H
#define VECMATRX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "subset.h"

struct vector {
  int dimension;
  double* entries;
};

struct square_matrix {
  int dimension;
  double** entries;
};

struct square_int_matrix {
  int dimension;
  int** entries;
};

struct vector zero_vector(int dimension);

void set_vector_entry(struct vector* vec, int index, double entry);

void negate_vector(struct vector* vec);

struct square_matrix zero_matrix(int dimension);

struct square_int_matrix zero_int_matrix(int dimension);

void set_matrix_entry(struct square_matrix* mat, int row, int col, double entry);

struct square_matrix copy_of_square_matrix(struct square_matrix* mat);

struct square_matrix transpose(struct square_matrix* mat);

struct vector matrix_times_vector(struct square_matrix* mat, struct vector* vec);

struct vector int_matrix_times_vector(struct square_int_matrix* mat, struct vector* vec);

void multiply_row_by_factor(struct square_matrix* mat, int row, double factor);

void swap_rows(struct square_matrix* mat, int row_one, int row_two);

void add_rwo_times_factor_to_row(struct square_matrix* mat, double factor, int r_one, int r_two);

struct square_matrix dbl_matrix_from_int_matrix(struct square_int_matrix* mat);

struct square_matrix inverse(struct square_matrix* mat);

int is_WDD(struct square_int_matrix* mat);

int** directed_graph(struct square_int_matrix* mat);

struct subset T_subset(struct square_int_matrix* mat); 

struct subset minimum_T_subset(struct subset* T_subset, int** dir_gr);

struct subset minimal_T_subset(struct square_int_matrix* mat);

struct square_int_matrix sub_mat(struct square_int_matrix* given, struct subset* rows_and_cols);

struct vector convex_weights(struct square_int_matrix* mat);

void print_vector(struct vector* vec);

void print_square_matrix(struct square_matrix* mat);

void print_square_int_matrix(struct square_int_matrix* mat);

void destroy_vector(struct vector* vec);

void destroy_square_matrix(struct square_matrix* mat);

void destroy_square_int_matrix(struct square_int_matrix* mat);

#endif /* VECMATRX_H */
