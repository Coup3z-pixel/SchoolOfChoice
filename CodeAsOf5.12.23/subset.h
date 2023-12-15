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

int subsets_are_disjoint(struct subset* first, struct subset* second);

void copy_subset(struct subset* given_subset, struct subset* copy_subset);

void complement(struct subset* given_subset, struct subset* comp);

struct subset singleton_subset(int elmnt, int large_size);

struct index index_of_subset(struct subset* my_subset);

struct subset subset_of_index(struct index* my_index, int large_set_size);

void subset_of_subset_to_subset(struct subset* new_subset, struct subset* given_subset,
				struct subset* subset_of_subset);

void print_subset(struct subset* my_subset);

void print_index(struct index* my_index);

void print_index_of_subset(struct subset* my_subset);

int indices_are_same(struct index* first, struct index* second);

int first_precedes_second(struct index* first, struct index* second);

int* indices_of_elements(struct subset* my_subset);

struct index* copy_of_index(struct index* given_index);

struct index singleton_index(int j);

struct index index_with_element_added(struct index* my_index, int j);

struct index index_of_subset_from_indices_list(struct index* my_index, int* list_of_indices);

void destroy_subset(struct subset my_subset);

void destroy_index(struct index my_index);


struct square_matrix matrix_of_zeros(int dim);

struct square_matrix matrix_of_ones(int dim);

struct square_matrix submatrix(struct square_matrix* big_matrix, struct subset* my_subset);

int is_connected(struct subset* my_subset, struct square_matrix* related);

void print_square_matrix(struct square_matrix* my_matrix);

void destroy_square_matrix(struct square_matrix my_matrix);


struct subset_list* initialized_subset_list();

int is_empty_list(struct subset_list* my_list);

void destroy_subset_list(struct subset_list* my_list);

void print_subset_list(struct subset_list* my_list);

int maximum_set_size(struct subset_list* my_list);

void add_subset(struct subset_list* my_list, struct index* my_index);

void remove_subset(struct subset_list* my_list, struct index* my_index);

int list_contains_index(struct subset_list* my_list, struct index* my_index);

struct subset_list*  copy_of_subset_list(struct subset_list* my_list);

void add_second_list_to_first(struct subset_list* first, struct subset_list* second);

int first_list_contains_second(struct subset_list* first, struct subset_list* second);

int lists_are_the_same(struct subset_list* first, struct subset_list* second);

void add_subsets_of_subset_to_list(struct subset_list* big_list, struct subset* the_subset,
				   struct subset_list* list_to_add);

int list_contains_index(struct subset_list* my_list, struct index* my_index);

struct subset_list* nonempty_subsets(struct index* my_index);

struct subset_list* reduced_subset_list(struct subset_list* my_list, struct subset* my_subset);

struct subset_list* immediate_supersets(struct index* my_index, struct square_matrix* related);

struct subset_list* immediate_supersets_of_list(struct subset_list* my_list,
						struct square_matrix* related);

struct subset_list* supersets_of_subsets(struct index* my_index, struct square_matrix* related,
					 int depth);

struct subset_list* supersets_of_subsets(struct index* my_index, struct square_matrix* related,
					 int depth);

struct subset_list* expanded_list(struct subset_list* my_list, struct square_matrix* related);

int* properly_ordered_candidate_list(struct subset* my_subset, struct square_matrix* related);

int new_is_qualified(int j, struct square_matrix* related, int* candidate_list, int probe);

int new_first_subset(struct subset* my_subset, struct square_matrix* related, int set_size);

int new_next_subset(struct subset* my_subset, struct square_matrix* related, int set_size);

#endif /* SUBSET_H */
