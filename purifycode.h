#ifndef IMPLEMENT_H
#define IMPLEMENT_H

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
  int no_edges;
  int** stu_sch_edges;
  int* sch_sink_edges;
};

struct neighbor_lists {
  int no_students;
  int no_schools;
  int* stu_no_nbrs;
  int* sch_no_nbrs; /* only student neighbors here */
  int sink_no_nbrs;
  int** stu_sch_nbrs;
  int** sch_stu_nbrs;
  int* sch_sink_nbrs; /* sch_sink_nbrs[j-1] = 1 if j and sink are neighbors, 0 otherwise. */
  int* sink_sch_nbrs;
};

struct path_node {
  int type; /* 1 = student, 2 = school, 3 = sink */
  int index;
  struct path_node* next;
};

struct pure_alloc random_pure_allocation(struct partial_alloc* my_alloc);

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc, double* sch_sums);

struct neighbor_lists neighbor_lists_from_graph(struct nonintegral_graph* my_graph);

int graph_is_nonempty(struct neighbor_lists* my_lists);

struct path_node* find_cyclic_path(struct neighbor_lists* my_lists);

double bound_of_cycle(struct partial_alloc* my_alloc, double* sch_sums, int up,
		      struct path_node* my_cycle);

void student_edge_removal(struct neighbor_lists* my_lists, int i, int j);

void sink_edge_removal(struct neighbor_lists* my_lists, int j);

void cycle_adjustment(struct partial_alloc* my_alloc, double* sch_sums,
		      struct neighbor_lists* my_lists, int up,
		      double adjustment, struct path_node* my_cycle);

/* At the end we need to pass from a partial_alloc whose values (which
   are doubles) are all close to 0 and 1, to the corresponding pure
   allocation, whose values are in {0,1}. */

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc);


void destroy_nonintegral_graph(struct nonintegral_graph* my_graph);

void destroy_neighbor_lists(struct neighbor_lists* my_lists);

void destroy_cycle(struct path_node* cycle);

#endif /* IMPLEMENT_H */
