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

struct nonintegral_graph {
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
};

struct path_node {
  int type; /* 1 = student, 2 = school, 3 = sink */
  int index;
  struct path_node* next;
};

int graph_has_a_leaf(struct nonintegral_graph* graph);


struct pure_alloc random_pure_allocation(struct partial_alloc* my_alloc);

void transform_to_random_floating_point_pure_allocation(struct partial_alloc* my_alloc);

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc);

int graph_is_nonempty(struct nonintegral_graph* my_graph);

struct path_node* find_cyclic_path(struct nonintegral_graph* my_graph);

double bound_of_cycle(struct partial_alloc* my_alloc, double* sch_sums, int up,
		      struct path_node* my_cycle);

void student_edge_removal(struct nonintegral_graph* my_graph, int i, int j);

void sink_edge_removal(struct nonintegral_graph* my_graph, int j);

void cycle_adjustment_of_allocation(struct partial_alloc* my_alloc, double* sch_sums, int up,
				    double adjustment, struct path_node* my_cycle);

void cycle_adjustment_of_graph(struct partial_alloc* my_alloc, double* sch_sums,
			       struct nonintegral_graph* my_graph, struct path_node* my_cycle);

void cycle_adjustment(struct partial_alloc* my_alloc, double* sch_sums,
		      struct nonintegral_graph* my_graph, int up,
		      double adjustment, struct path_node* my_cycle);

int* list_with_element_removed(int* old_list, int old_no_elements, int elt); 

void destroy_nonintegral_graph(struct nonintegral_graph* my_graph);

void destroy_cycle(struct path_node* cycle);


int alloc_and_sch_sums_are_consistent(double* sch_sums, struct partial_alloc* my_alloc);

int graph_and_alloc_are_consistent(struct nonintegral_graph* my_graph, double* sch_sums,
				   struct partial_alloc* my_alloc);

#endif /* PURIFYCODE_H */
