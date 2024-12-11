#ifndef EFFICIENT_H
#define EFFICIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "partalloc.h"

struct stu_sch_node {
  int stuno;
  int schno;
  struct stu_sch_node* next;
};

int allocation_is_efficient(struct partial_alloc* myalloc, struct process_scp* myscp);

int process_graphs_pair(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
			 struct stu_sch_node*** chart, int i, int j);

int** get_pairs_that_can_envy(struct process_scp* myscp, struct partial_alloc* my_alloc);

struct stu_sch_node*  create_stu_sch_node(int i, int j);

int stu_sch_list_contains_pair(struct stu_sch_node* stu_sch, int i, int j);

void append_node_to_stu_sch_list(struct stu_sch_node** stu_sch, int i, int j);

struct stu_sch_node* copy_of_list(struct stu_sch_node* given);

struct stu_sch_node*** get_simple_envy_graph(struct process_scp* myscp,
					      struct partial_alloc* alloc_to_adjust);

struct stu_sch_node*** get_envied_graph(struct stu_sch_node*** envygr, int nst, int nsc);

struct stu_sch_node* get_new_layer(struct stu_sch_node*** envygr, struct stu_sch_node*** chart,
				   struct stu_sch_node* last_layer, int* found_cycle,
				   int i, int j);

void remove_pair_from_list(struct stu_sch_node** list, int k, int l);

void remove_pair_from_envy_and_envied_graphs(struct stu_sch_node*** envygr,
					     struct stu_sch_node*** enviedgr, int k, int l);

void clean_up_chart_and_nodes(struct stu_sch_node*** chart, struct stu_sch_node* found_list);

void destroy_stu_sch_list(struct stu_sch_node* node);

void destroy_matrix_of_lists(struct stu_sch_node*** envygr, int nst, int nsc);

#endif /* EFFICIENT_H */
