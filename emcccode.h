#ifndef ENHANCED_h
#define ENHANCED_h

#include "mcccode.h"

struct stu_sch_node {
  int stuno;
  int schno;
  struct stu_sch_node* next;
};

struct partial_alloc EMCC_allocation(struct process_scp* myscp);

int** get_active_pairs(struct process_scp* myscp, struct partial_alloc* alloc_to_adjust);

struct stu_sch_node*** get_envy_graph(struct process_scp* myscp,
				      struct partial_alloc* alloc_to_adjust, int* coarse);

struct stu_sch_node*** get_envied_graph(struct stu_sch_node*** envygr, int nst, int nsc);

void process_cycles_at_pair(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
			    struct stu_sch_node*** chart, struct partial_alloc* alloc_to_adjust,
			    int i, int j);

struct stu_sch_node* get_new_layer(struct stu_sch_node*** envygr, struct stu_sch_node*** chart,
				   struct stu_sch_node* last_layer, int* found_cycle, int i, int j);

void process_cycle(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
		   struct stu_sch_node*** chart, struct partial_alloc* alloc_to_adjust,
		   int i, int j);

struct stu_sch_node* extract_cycle(struct stu_sch_node*** chart, int i, int j);

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle,
				      struct stu_sch_node** list);

void clean_up_chart_and_nodes(struct stu_sch_node*** chart, struct stu_sch_node* found_list);

void remove_pair_from_list(struct stu_sch_node** list, int k, int l);

void remove_pair_from_envy_and_envied_graphs(struct stu_sch_node*** envygr,
					     struct stu_sch_node*** enviedgr, int k, int l);

int stu_sch_list_contains_pair(struct stu_sch_node* stu_sch, int i, int j);

void append_node_to_stu_sch_list(struct stu_sch_node** stu_sch, int i, int j);

/* Remaining functions are simple and self explanatory */

int stu_sch_list_contains_pair(struct stu_sch_node* list, int i, int j);

struct stu_sch_node*  create_stu_sch_node(int i, int j);

struct stu_sch_node* copy_of_list(struct stu_sch_node* given);

/*  Some functions used during debugging
void print_graph(struct stu_sch_node*** graph, int nst, int nsc);

void print_stu_sch_list(struct stu_sch_node* node);

int envygr_and_enviedgr_are_consistent(struct stu_sch_node*** envygr,
					     struct stu_sch_node*** enviedgr,
					     int nst, int nsc);
*/

void destroy_stu_sch_list(struct stu_sch_node* node);

void destroy_matrix_of_lists(struct stu_sch_node*** envygr, int nst, int nsc);

#endif /* ENHANCED_H */
