#ifndef PIVOT_H
#define PIVOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sprsmtrx.h"

typedef struct Pivot {
  int h;
  int* schools;
  int* students;
} pivot;

typedef struct Pivot_List_Node {
  pivot* the_pivot;
  struct Pivot_List_Node* next;
} pivot_list_node;

typedef struct Pivot_List {
  pivot_list_node* first_node;
  pivot_list_node* last_node;
} pivot_list;

pivot initialized_pivot(int h);

pivot* copy_of_pivot(pivot* my_pivot);

void execute_pivot(pivot* my_pivot, int_sparse_matrix* theta, int* theta_sums);


int pivot_list_node_has_NULL_the_pivot(pivot_list_node* my_node);

int pivot_list_has_NULL_the_pivot(pivot_list* my_list);

pivot_list void_pivot_list();

int pivot_list_is_void(pivot_list* my_list);

void add_pivot_to_list(pivot_list* my_list, pivot* new_pivot);

void concatenate_pivot_lists(pivot_list* target, pivot_list* addition);

pivot_list reduced_pivot_list(pivot_list* given_list,
				     subset* J_subset, subset* P_subset);

pivot_list left_reduced_pivot_list(pivot_list* given_list,
					  subset* J_subset, subset* P_subset);

pivot_list right_reduced_pivot_list(pivot_list* given_list,
					   subset* J_subset, subset* P_subset);


void print_pivot(pivot* my_pivot);

void print_pivot_list(pivot_list* my_list);

void destroy_pivot(pivot my_pivot);

void destroy_pivot_ptr(pivot* my_pivot);

void destroy_pivot_list_node(pivot_list_node my_pivot_list_node);

void destroy_pivot_list(pivot_list my_pivot_list);

#endif /* PIVOT_H */
