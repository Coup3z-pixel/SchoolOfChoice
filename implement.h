#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include <stdio.h>
#include <math.h>

#include "partalloc.h"

struct list_node {
  int is_student;
  int is_school;
  int is_sink;
  int index; /* student index, school index, or 1 for sink */
  struct list_node* next;
};

struct nonintegral_graph {
  int no_students;
  int no_schools;
  int no_edges;
  int** stu_sch_edges;
  int* sch_sink_edges;
  struct list_node* stu_nbrs; /* list of neighbors of student, starting with the node itself */
  struct list_node* sch_nbrs; /* ditto */
  struct list_node sink_nbrs; /* ditto */
};

void destroy_nonintegral_graph(struct nonintegral_graph* my_graph);

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc, double* sch_sums);

void remove_student_edge(struct nonintegral_graph* my_graph, int i, int j);

void remove_sink_edge(struct nonintegral_graph* my_graph, int j);

int find_cycle(struct nonintegral_graph* my_graph, struct list_node* cycle_node);

double cycle_bound(struct partial_alloc* my_alloc, double* sch_sums, int up,
		   struct list_node* my_cycle);

void adjust_cycle(struct nonintegral_graph* my_graph, struct partial_alloc* my_alloc,
		  double* sch_sums, int up, double adjustment, struct list_node* my_cycle);

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc);

struct pure_alloc random_pure_allocation(struct partial_alloc* my_alloc);

#endif /* IMPLEMENT_H */
