#ifndef PIVOT_H
#define PIVOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "push_relabel.h"

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

void destroy_pivot(struct pivot my_pivot);

void destroy_pivot_ptr(struct pivot* my_pivot);

void destroy_pivot_list_node(struct pivot_list_node my_pivot_list_node);

void destroy_pivot_list(struct pivot_list my_pivot_list);

struct pivot initialized_pivot(int h);

struct pivot_list void_pivot_list();

struct pivot* copy_of_pivot(struct pivot* my_pivot);

int pivot_list_is_void(struct pivot_list* my_list);

void add_pivot_to_list(struct pivot_list* my_list, struct pivot* new_pivot);

void concatenate_pivot_lists(struct pivot_list* target, struct pivot_list* addition);

int student_qualified_for_school(int i, int j, struct partial_alloc* feasible_guide,int** theta,
				 int* favorites);

int school_qualified_for_student(int i, int j,
				 struct partial_alloc* feasible_guide,
				 int** theta,
				 struct process_cee* working_cee);

int pivot_is_valid(struct pivot* my_pivot, int** theta, int* theta_sums, struct index* alpha,
		   struct partial_alloc* feasible_guide, struct process_cee* working_cee,
		   int* favorites, int* fully_allocated);

void execute_pivot(struct pivot* my_pivot, int** theta, int* theta_sums);

void reuse_prior_pivots(struct pivot_list* old_list, struct pivot_list* new_list,
		       int** theta, int* theta_sums, struct index* alpha,
		       struct partial_alloc* feasible_guide, struct process_cee* working_cee,
			int* favorites, int* fully_allocated, int* no_old_pivots, int* h_sum);

struct pivot_list reduced_pivot_list(struct pivot_list* given_list,
				     struct subset* J_subset, struct subset* P_subset);

struct pivot_list left_reduced_pivot_list(struct pivot_list* given_list,
					  struct subset* J_subset, struct subset* P_subset);

struct pivot_list right_reduced_pivot_list(struct pivot_list* given_list,
					   struct subset* J_subset, struct subset* P_subset);

#endif /* PIVOT_H */
