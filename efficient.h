#ifndef EFFICIENT_H
#define EFFICIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "partalloc.h"

int allocation_is_efficient(struct partial_alloc* myalloc, struct process_scp* myscp);

struct lists_of_students get_accepting_students(struct partial_alloc* myalloc,
						struct process_scp* myscp);

int student_can_accept_school(struct partial_alloc* myalloc,struct process_scp* myscp,
			      int i, int j);

int is_accepting(struct lists_of_students* accepting_students, int i, int j);

int process_initial_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
			 struct lists_of_students* accepting_students,
			 int i, int j);

struct stu_sch_node* NEW_get_new_layer(struct partial_alloc* myalloc, struct process_scp* myscp,
				       struct lists_of_students* accepting_students,
				       struct stu_sch_node* all_so_far, 
				       struct stu_sch_node* last_layer,
				       int* found_cycle, int* something_new,
				       int i, int j);

struct stu_sch_node* simple_new_layer(struct partial_alloc* myalloc, struct process_scp* myscp,
				      struct lists_of_students* accepting_students,
				      struct stu_sch_node* all_so_far, 
				      struct stu_sch_node* last_layer);

struct stu_sch_node* possible_recipients(struct partial_alloc* myalloc, struct process_scp* myscp,
					 struct lists_of_students* accepting_students,
					 int k, int l);

struct stu_sch_node {
  int stuno;
  int schno;
  struct stu_sch_node* next;
};

struct stu_sch_node*  create_stu_sch_node(int i, int j);

int stu_sch_list_contains_pair(struct stu_sch_node* stu_sch, int i, int j);

int stu_sch_list_contains_pair_noninitially(struct stu_sch_node* stu_sch, int i, int j);

void append_node_to_stu_sch_list(struct stu_sch_node** stu_sch, int i, int j);

struct stu_sch_node* copy_of_list(struct stu_sch_node* given);

void remove_pair_from_list(struct stu_sch_node** list, int k, int l);

struct stu_sch_node* copy_of_stu_sch_list(struct stu_sch_node* node);

void print_stu_sch_list(struct stu_sch_node* node);

void destroy_stu_sch_list(struct stu_sch_node* node);


struct stu_list_node {
  int stu_no;
  struct stu_list_node* next;
};

void add_student_to_list(struct stu_list_node** list, int i);

void remove_student_from_list(struct stu_list_node** list, int i);

void destroy_student_list(struct stu_list_node** list);


struct lists_of_students {
  int no_students;
  int no_schools;
  struct stu_list_node** lists;
};


void destroy_accepting_students(struct stu_list_node** array_of_lists, int nsc);

void destroy_lists_of_students(struct lists_of_students* student_lists);

#endif /* EFFICIENT_H */
