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

struct vector zero_vector(int dimension);

void set_vector_entry(struct vector* vec, int index, double entry);

struct square_matrix zero_matrix(int dimension);

void set_matrix_entry(struct square_matrix* mat, int row, int col, double entry);

struct vector matrix_times_vector(struct square_matrix* mat, struct vector* vec);

void swap_rows(int dimension, double** mat, int row_one, int row_two);

struct square_matrix inverse(struct square_matrix* mat);


void destroy_vector(struct vector* vec);

void destroy_square_matrix(struct square_matrix* mat);

#endif /* VECMATRX_H */
