#ifndef ENHANCED_h
#define ENHANCED_h

#include "mcccode.h"

struct partial_alloc EMCC_allocation(struct process_scp* myscp);

struct stu_sch_node*** get_envy_graph(struct process_scp* myscp,
				      struct partial_alloc* alloc_to_adjust, int* coarse);

void process_cycles_at_pair(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
			    struct stu_sch_node*** chart, struct partial_alloc* alloc_to_adjust,
			    int i, int j);

void process_cycle(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
		   struct stu_sch_node*** chart, struct partial_alloc* alloc_to_adjust,
		   int i, int j);

struct stu_sch_node* extract_cycle(struct stu_sch_node*** chart, int i, int j);

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle,
				      struct stu_sch_node** list);

/* Remaining functions are simple and self explanatory */

int stu_sch_list_contains_pair(struct stu_sch_node* list, int i, int j);

/*  Some functions used during debugging
void print_graph(struct stu_sch_node*** graph, int nst, int nsc);

void print_stu_sch_list(struct stu_sch_node* node);

int envygr_and_enviedgr_are_consistent(struct stu_sch_node*** envygr,
					     struct stu_sch_node*** enviedgr,
					     int nst, int nsc);
*/

#endif /* ENHANCED_H */
