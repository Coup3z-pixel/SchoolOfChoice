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

struct subset_list {
  struct index* node_index;
  struct subset_list* next;
};

void print_vector_of_ints(int* vector, int dim);

struct subset nullset(int large_set_size);

struct subset fullset(int large_set_size);

int subsets_are_same(struct subset* first, struct subset* second);

void copy_subset(struct subset* given_subset, struct subset* copy_subset);

void complement(struct subset* given_subset, struct subset* comp);

struct index index_of_subset(struct subset* my_subset);

struct subset subset_of_index(struct index* my_index, int large_set_size);

void print_subset(struct subset* my_subset);

void print_index(struct index* my_index);

void print_index_of_subset(struct subset* my_subset);

int indices_are_same(struct index* first, struct index* second);

int first_precedes_second(struct index* first, struct index* second);

int* indices_of_elements(struct subset* my_subset);

void copy_index(struct index* given_index, struct index* copy_index);

struct index singleton_index(int j);

struct index index_with_element_added(struct index* my_index, int j);

struct index index_of_subset_from_indices_list(struct index* my_index, int* list_of_indices);

void destroy_subset(struct subset my_subset);

void destroy_index(struct index my_index);


struct square_matrix matrix_of_zeros(int dim);

struct square_matrix matrix_of_ones(int dim);

struct square_matrix submatrix(struct square_matrix* big_matrix, struct subset* my_subset); 

void print_square_matrix(struct square_matrix* my_matrix);

void destroy_square_matrix(struct square_matrix my_matrix);


struct subset_list* initialized_subset_list();

int is_empty_list(struct subset_list* my_list);

void destroy_subset_list(struct subset_list* my_list);

void print_subset_list(struct subset_list* my_list);

void add_subset(struct subset_list* my_list, struct index* my_index);

struct subset_list*  copy_of_subset_list(struct subset_list* my_list);

void add_second_list_to_first(struct subset_list* first, struct subset_list* second);

int list_contains_index(struct subset_list* my_list, struct index* my_index);

struct subset_list* nonempty_subsets(struct index* my_index);

struct subset_list* reversed_subset_list(struct subset_list* my_list);

struct subset_list* reduced_subset_list(struct subset_list* my_list, struct subset* my_subset);

struct subset_list* immediate_supersets(struct index* my_index, struct square_matrix* related, int* popular);

struct subset_list* immediate_supersets_of_list(struct subset_list* my_list, struct square_matrix* related, int* popular);

struct subset_list* supersets_of_subsets(struct index* my_index, struct square_matrix* related, int* popular, int depth);

void add_supersets_of_subsets_to_list(struct subset_list* my_list, struct index* my_index,
				      struct square_matrix* related, int* popular, int depth);

struct subset_list* expanded_list(struct subset_list* my_list, struct square_matrix* related, int* popular);

#endif /* SUBSET_H */
