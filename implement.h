#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include <stdio.h>
#include <math.h>

#include "partalloc.h"

/* We are given a partalloc that assigns total probability one to each
   student.  Our goal is to generate a random pure allocation whose
   average is the given partalloc.  We do this by repeatedly finding
   cycles in the graph of nonintegral assignments, then randomizing
   over the two possible adjustments of the allocation that give
   additional integral entries. */

/* A cycle in the graph of nonintegral assignments is a linked list. */

struct list_node {
  int is_student;
  int is_school;
  int is_sink;
  int index; /* student index, school index, or 1 for sink */
  struct list_node* next;
};

/* We create and maintain a graph whose edges are nonintegral entries
   of the allocation, and the edges from schools to the sink whose
   total flow is not integral. The lists of neighbors of each node is
   a convenience in applying the graph that has a maintenance cost. */

struct nonintegral_graph {
  int no_students;
  int no_schools;
  int no_edges;
  int** stu_sch_edges;
  int* sch_sink_edges;
  struct list_node* stu_nbrs; /* list of neighbors of student, starting with the node itself */
  struct list_node* sch_nbrs; /* ditto for school */
  struct list_node sink_nbrs; /* ditto for sink */
};

void destroy_nonintegral_graph(struct nonintegral_graph* my_graph);

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc, double* sch_sums);

/* While maintaining the graph, we need to remove edges that become integral. */

void remove_student_edge(struct nonintegral_graph* my_graph, int i, int j);

void remove_sink_edge(struct nonintegral_graph* my_graph, int j);

/* To find a cycle we start at a node and go to a neighbor, a neighbor of that neighbor, etc. */

int find_cycle(struct nonintegral_graph* my_graph, struct list_node* cycle_node);

/* We compute the maximum adjustment of the nonintegral entries, in
   either of the two directions, that keeps all entries in [0,1]. */

double cycle_bound(struct partial_alloc* my_alloc, double* sch_sums, int up,
		   struct list_node* my_cycle);

/* We adjust the allocation and the nonintegral graph. */

void adjust_cycle(struct nonintegral_graph* my_graph, struct partial_alloc* my_alloc,
		  double* sch_sums, int up, double adjustment, struct list_node* my_cycle);

/* At the end we need to pass from a partial_alloc whose values (which
   are doubles) are all close to 0 and 1, to the corresponding pure
   allocation, whose values are in {0,1}. */

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc);

/* The following is the master function that puts everything together. */

struct pure_alloc random_pure_allocation(struct partial_alloc* my_alloc);

#endif /* IMPLEMENT_H */
