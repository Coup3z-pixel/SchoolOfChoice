#ifndef PURIFYCODE_H
#define PURIFYCODE_H

#include <stdio.h>
#include <math.h>

#include "partalloc.h"
#include "normal.h"

/* We are given a partalloc that assigns total probability one to each
   student.  Our goal is to generate a random pure allocation whose
   average is the given partalloc.  We do this by repeatedly finding
   cycles in the graph of nonintegral assignments, then randomizing
   over the two possible adjustments of the allocation that give
   additional integral entries. */

typedef struct Nonintegral_Graph {
  int no_students;
  int no_schools;
  int** stu_sch_edges;
  int* sch_sink_edges;
  int* stu_no_nbrs;
  int* sch_no_nbrs;
  int sink_no_nbrs;
  int** stu_nbrs;
  int** sch_nbrs;
  int* sink_nbrs;
} nonintegral_graph;

typedef struct Path_Node {
  int type; /* 1 = student, 2 = school, 3 = sink */
  int index;
  struct Path_Node* next;
} path_node;

int graph_has_a_leaf(nonintegral_graph* graph);


void repeatedly_remove_leaves_from_graph(nonintegral_graph* graph);

pure_alloc random_pure_allocation(partial_alloc* my_alloc);

void transform_to_random_floating_point_pure_allocation(partial_alloc* my_alloc);

nonintegral_graph graph_from_alloc(partial_alloc* my_alloc);

int graph_is_nonempty(nonintegral_graph* my_graph);

path_node* find_cyclic_path(nonintegral_graph* my_graph);

double bound_of_cycle(partial_alloc* my_alloc, double* sch_sums, int up,
		      path_node* my_cycle);

void student_edge_removal(nonintegral_graph* my_graph, int i, int j);

void sink_edge_removal(nonintegral_graph* my_graph, int j);

void cycle_adjustment_of_allocation(partial_alloc* my_alloc, double* sch_sums, int up,
				    double adjustment, path_node* my_cycle);

void cycle_adjustment_of_graph(partial_alloc* my_alloc, double* sch_sums,
			       nonintegral_graph* my_graph, path_node* my_cycle);

void cycle_adjustment(partial_alloc* my_alloc, double* sch_sums,
		      nonintegral_graph* my_graph, int up,
		      double adjustment, path_node* my_cycle);

int* list_with_element_removed(int* old_list, int old_no_elements, int elt); 

void destroy_nonintegral_graph(nonintegral_graph* my_graph);

void destroy_cycle(path_node* cycle);


int alloc_and_sch_sums_are_consistent(double* sch_sums, partial_alloc* my_alloc);

int graph_and_alloc_are_consistent(nonintegral_graph* my_graph, double* sch_sums,
				   partial_alloc* my_alloc);

#endif /* PURIFYCODE_H */
