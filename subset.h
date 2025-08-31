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

struct index_list {
  struct index* node_index;
  struct index_list* next;
};

struct subset nullset(int large_set_size);

struct subset fullset(int large_set_size);

int is_nullset(struct subset* my_set);

int is_fullset(struct subset* my_set);

int is_element(struct subset* my_set, int i);

void add_element(struct subset* my_set, int new_elt);

void remove_element(struct subset* my_set, int old_elt);

void remove_all_elements(struct subset* my_set);

int subsets_are_same(struct subset* first, struct subset* second);

int subsets_are_disjoint(struct subset* first, struct subset* second);

void copy_subset(struct subset* given_subset, struct subset* copy_subset);

struct subset copy_of_subset(struct subset* given_subset);

struct subset complement_of_subset(struct subset* given_subset);

void complement(struct subset* given_subset, struct subset* comp);

struct subset singleton_subset(int elmnt, int large_size);

void becomes_nullset(struct subset* my_set);

void becomes_singleton(struct subset* my_set, int elmnt);

int* subset_element_numbers(struct subset* my_subset);

struct index singleton_index(int i);

struct index index_of_subset(struct subset* my_subset);

struct index index_of_complement(struct subset* my_subset);

struct subset subset_of_index(struct index* my_index, int large_set_size);

void subset_of_subset_to_subset(struct subset* new_subset, struct subset* given_subset,
				struct subset* subset_of_subset);

int indices_are_same(struct index* first, struct index* second);

int index_has_element(struct index* my_ind, int elt);

int* indices_of_elements(struct subset* my_subset);

void add_element_to_index(struct index* index_ptr, int elt);

void add_element_to_possibly_NULL_index(struct index** index_ptr, int elt);

void remove_element_from_index(struct index* index_ptr, int elt);

void add_element_to_index_ptr(struct index** index_ptr, int elt);

struct index* copy_of_index(struct index* given_index);

struct index index_of_fullset(int large_set_size);

struct index singleton_index(int j);

struct index index_with_element_added(struct index* my_index, int j);

struct index* reverse_of_correspondence_graph(struct index* my_list,
					      struct index* active_range_elts,
					      int no_domain_elts);


struct index_list* initialized_index_list(); 

int index_list_is_empty(struct index_list* my_list);

int length_of_index_list(struct index_list* my_list);

int element_of_list_set(struct index_list* my_list, int set_no, int elt_no);

void add_index_to_index_list(struct index_list* my_list, struct index* my_index);

struct subset union_of_indices(struct index* my_list, int large_set_size, int list_length);


void print_vector_of_ints(int* vector, int dim);

void print_subset(struct subset* my_subset);

void print_index(struct index* my_index);

void print_index_of_subset(struct subset* my_subset);

void print_index_list(struct index_list* my_list);

void destroy_subset(struct subset my_subset);

void destroy_index(struct index my_index);

void destroy_index_list(struct index_list* my_list);

#endif /* SUBSET_H */
