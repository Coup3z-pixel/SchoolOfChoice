#ifndef VECMATRX_H
#define VECMATRX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "subset.h"

typedef struct Vector {
  int dimension;
  double* entries;
} vector;

typedef struct Square_Matrix {
  int dimension;
  double** entries;
} square_matrix;

typedef struct Square_Int_Matrix {
  int dimension;
  int** entries;
} square_int_matrix;

vector zero_vector(int dimension);

void set_vector_entry(vector* vec, int index, double entry);

void negate_vector(vector* vec);

square_matrix zero_matrix(int dimension);

square_int_matrix zero_int_matrix(int dimension);

void set_matrix_entry(square_matrix* mat, int row, int col, double entry);

square_matrix copy_of_square_matrix(square_matrix* mat);

square_matrix transpose(square_matrix* mat);

vector matrix_times_vector(square_matrix* mat, vector* vec);

vector int_matrix_times_vector(square_int_matrix* mat, vector* vec);

void multiply_row_by_factor(square_matrix* mat, int row, double factor);

void swap_rows(square_matrix* mat, int row_one, int row_two);

void add_rwo_times_factor_to_row(square_matrix* mat, double factor, int r_one, int r_two);

square_matrix dbl_matrix_from_int_matrix(square_int_matrix* mat);

square_matrix inverse(square_matrix* mat);

int is_WDD(square_int_matrix* mat);

int** directed_graph(square_int_matrix* mat);

subset T_subset(square_int_matrix* mat); 

subset minimum_T_subset(subset* T_subset, int** dir_gr);

subset minimal_T_subset(square_int_matrix* mat);

square_int_matrix sub_mat(square_int_matrix* given, subset* rows_and_cols);

vector convex_weights(square_int_matrix* mat);

void print_vector(vector* vec);

void print_square_matrix(square_matrix* mat);

void print_square_int_matrix(square_int_matrix* mat);

void destroy_vector(vector* vec);

void destroy_square_matrix(square_matrix* mat);

void destroy_square_int_matrix(square_int_matrix* mat);

#endif /* VECMATRX_H */
