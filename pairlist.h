#ifndef PAIRLIST_H
#define PAIRLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Stu_Sch_Pair_List_Node {
  int stu;
  int sch;
  struct Stu_Sch_Pair_List_Node* next;
} stu_sch_pair_list_node;

void add_pair_to_stu_sch_pair_list(stu_sch_pair_list_node** list, int stu, int sch);

void remove_pair_from_stu_sch_pair_list(stu_sch_pair_list_node** list, int i, int j);

int list_contains_pair(stu_sch_pair_list_node* list, int stu, int sch);

int is_a_subset(stu_sch_pair_list_node* poss_subset, stu_sch_pair_list_node* set);

int is_a_proper_subset(stu_sch_pair_list_node* poss_subset, stu_sch_pair_list_node* set);

int length_of_list(stu_sch_pair_list_node* list);

stu_sch_pair_list_node* copy_of_list_element_n(stu_sch_pair_list_node* list, int n);

stu_sch_pair_list_node* concatenate(stu_sch_pair_list_node* first,
				    stu_sch_pair_list_node* second);

stu_sch_pair_list_node* copy_of_stu_sch_pair_list(stu_sch_pair_list_node* list);


void fprint_pair_list(stu_sch_pair_list_node* list);

void destroy_stu_sch_pair_list(stu_sch_pair_list_node** list);

#endif /* PAIRLIST_H */
