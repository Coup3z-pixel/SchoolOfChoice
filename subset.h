#ifndef SUBSET_H
#define SUBSET_H

#include <stdio.h>
#include <stdlib.h>

struct subset {
  int large_set_size;
  int subset_size;
  int* indicator;
};

struct index {
  int no_elements;
  int* indices;
};

struct square_matrix {
  int dimension;
  int** entries;
};

struct subset nullset(int large_set_size);

struct subset fullset(int large_set_size);

int subsets_are_same(struct subset* first, struct subset* second);

void copy_subset(struct subset* given_subset, struct subset* copy_subset);

void complement(struct subset* given_subset, struct subset* comp);

struct index index_of_subset(struct subset* my_subset);

void print_subset(struct subset* my_subset);

void print_index(struct index* my_index);

void destroy_subset(struct subset* my_subset);

void destroy_index(struct index* my_index);


struct square_matrix matrix_of_zeros(int dim);

struct square_matrix matrix_of_ones(int dim);

struct square_matrix submatrix(struct square_matrix* big_matrix, struct subset* my_subset); 

int next_subset(struct subset* my_subset, struct square_matrix* related, int* subset_sizes,
 		int* point_school);

void print_square_matrix(struct square_matrix* my_matrix);

void destroy_square_matrix(struct square_matrix* my_matrix);

#endif /* SUBSET_H */
