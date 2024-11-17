#ifndef ENHANCED_h
#define ENHANCED_h

#include "mcccode.h"

struct stu_sch_node {
  int stuno;
  int schno;
  struct stu_sch_node* next;
};

struct partial_alloc EMCC_allocation(struct process_scp* myscp);

struct stu_sch_node*** get_cycle_matrix(struct process_scp* myscp,
				      struct partial_alloc* alloc_to_adjust, int* coarse);

/* The next functions occur in get_cycle_matrix */

struct stu_sch_node*** get_envy_graph(struct process_scp* myscp,
				      struct partial_alloc* alloc_to_adjust, int* coarse);

struct stu_sch_node* get_cycle(struct stu_sch_node*** egraph, int nst, int i, int j);

int already_seen(struct stu_sch_node** previously_found, int level, int k, int l);

struct stu_sch_node* extract_cycle(struct stu_sch_node*** egraph,
				   struct stu_sch_node** previously_found,
				   int level, int i, int j);

/* The following three functions are applied in EMCC_allocation */
  
int matrix_has_cycle(struct stu_sch_node*** cmatrix, int nst, int nsc);

struct stu_sch_node* random_cycle(struct stu_sch_node*** cmatrix, int nst, int nsc);

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle,
				      struct process_scp* myscp);

/* Remaining functions are simple and self explanatory */

int stu_sch_list_contains_pair(struct stu_sch_node* stu_sch, int i, int j);

void append_node_to_stu_sch_list(struct stu_sch_node** stu_sch, int i, int j);

void destroy_stu_sch_list(struct stu_sch_node* node);

void destroy_list_of_lists(struct stu_sch_node** previously_found, int level);

void destroy_matrix_of_lists(struct stu_sch_node*** egraph, int nst, int nsc);

#endif /* ENHANCED_H */
