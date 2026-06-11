#ifndef SUBSET_H
#define SUBSET_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Subset {
  int large_set_size;
  int subset_size;
  int* indicator;
} subset;

typedef struct Element_List {
  int no_elements;
  int* indices;
} element_list;

typedef struct List_of_Element_Lists {
  element_list* node_index;
  struct List_of_Element_Lists* next;
} list_of_elt_lists;

subset nullset(int large_set_size);

subset fullset(int large_set_size);

int is_nullset(subset* my_set);

int is_fullset(subset* my_set);

int is_element(subset* my_set, int i);

void add_element(subset* my_set, int new_elt);

void add_subset(subset* my_set, subset* increment);

void remove_subset(subset* my_set, subset* decrement);

void remove_element(subset* my_set, int old_elt);

void remove_all_elements(subset* my_set);

int subsets_are_same(subset* first, subset* second);

int subsets_are_disjoint(subset* first, subset* second);

void copy_subset(subset* given_subset, subset* copy_subset);

subset copy_of_subset(subset* given_subset);

subset complement_of_subset(subset* given_subset);

void complement(subset* given_subset, subset* comp);

subset singleton_subset(int elmnt, int large_size);

void becomes_nullset(subset* my_set);

void becomes_singleton(subset* my_set, int elmnt);

int* subset_element_numbers(subset* my_subset);

element_list singleton_element_list(int i);

element_list element_list_of_subset(subset* my_subset);

element_list element_list_of_complement(subset* my_subset);

subset subset_of_element_list(element_list* my_index, int large_set_size);

void subset_of_subset_to_subset(subset* new_subset, subset* given_subset,
				subset* subset_of_subset);

int indices_are_same(element_list* first, element_list* second);

int element_list_has_element(element_list* my_ind, int elt);

int* indices_of_elements(subset* my_subset);

void add_element_to_element_list(element_list* index_ptr, int elt);

void add_element_to_possibly_NULL_element_list(element_list** index_ptr, int elt);

void remove_element_from_element_list(element_list* index_ptr, int elt);

void add_element_to_element_list_ptr(element_list** index_ptr, int elt);

element_list* copy_of_element_list(element_list* given_index);

element_list element_list_of_fullset(int large_set_size);

element_list singleton_element_list(int j);

element_list element_list_with_element_added(element_list* my_index, int j);

element_list* reverse_of_correspondence_graph(element_list* my_list,
					      element_list* active_range_elts,
					      int no_domain_elts);

list_of_elt_lists* initialized_list_of_elt_lists(); 

int list_of_elt_lists_is_empty(list_of_elt_lists* my_list);

int length_of_list_of_elt_lists(list_of_elt_lists* my_list);

int element_of_list_set(list_of_elt_lists* my_list, int set_no, int elt_no);

void add_element_list_to_list_of_elt_lists(list_of_elt_lists* my_list, element_list* my_index);

void add_list_of_set_to_list_of_elt_lists(list_of_elt_lists* my_list, subset* my_subset);

void list_of_set_to_list_of_elt_lists(list_of_elt_lists* my_list, subset* my_subset);

void print_vector_of_ints(int* vector, int dim);

void fprint_subset(subset* my_subset);

void print_element_list(element_list* my_index);

void print_element_list_of_subset(subset* my_subset);

void print_list_of_elt_lists(list_of_elt_lists* my_list);

void destroy_subset(subset my_subset);

void destroy_element_list(element_list my_index);

void destroy_list_of_elt_lists(list_of_elt_lists* my_list);

#endif /* SUBSET_H */
