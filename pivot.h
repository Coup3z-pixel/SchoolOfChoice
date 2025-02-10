#ifndef PIVOT_H
#define PIVOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pushrelabel.h"
#include "sprsmtrx.h"

struct pivot {
  int h;
  int* schools;
  int* students;
};

struct pivot_list_node {
  struct pivot* the_pivot;
  struct pivot_list_node* next;
};

struct pivot_list {
  struct pivot_list_node* first_node;
  struct pivot_list_node* last_node;
};

struct pivot initialized_pivot(int h);

struct pivot* copy_of_pivot(struct pivot* my_pivot);

void execute_pivot(struct pivot* my_pivot, struct int_sparse_matrix* theta, int* theta_sums);


int pivot_list_node_has_NULL_the_pivot(struct pivot_list_node* my_node);

int pivot_list_has_NULL_the_pivot(struct pivot_list* my_list);

struct pivot_list void_pivot_list();

int pivot_list_is_void(struct pivot_list* my_list);

void add_pivot_to_list(struct pivot_list* my_list, struct pivot* new_pivot);

void concatenate_pivot_lists(struct pivot_list* target, struct pivot_list* addition);

struct pivot_list reduced_pivot_list(struct pivot_list* given_list,
				     struct subset* J_subset, struct subset* P_subset);

struct pivot_list left_reduced_pivot_list(struct pivot_list* given_list,
					  struct subset* J_subset, struct subset* P_subset);

struct pivot_list right_reduced_pivot_list(struct pivot_list* given_list,
					   struct subset* J_subset, struct subset* P_subset);


void print_pivot(struct pivot* my_pivot);

void print_pivot_list(struct pivot_list* my_list);

void destroy_pivot(struct pivot my_pivot);

void destroy_pivot_ptr(struct pivot* my_pivot);

void destroy_pivot_list_node(struct pivot_list_node my_pivot_list_node);

void destroy_pivot_list(struct pivot_list my_pivot_list);

#endif /* PIVOT_H */
