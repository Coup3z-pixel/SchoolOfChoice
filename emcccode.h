#ifndef ENHANCED_h
#define ENHANCED_h

#include "mcccode.h"

struct partial_alloc EMCC_allocation(struct process_scp* myscp);

void process_cycles_at_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
			    struct lists_of_students* accepting_students, int i, int j,
			    int* must_repeat);

struct stu_sch_node* find_cycle_or_show_none_exist(struct partial_alloc* myalloc,
						   struct process_scp* myscp,
						   struct lists_of_students* accepting_students,
						   int i, int j);

struct stu_sch_node* cycle_or_not(struct partial_alloc* myalloc, struct process_scp* myscp,
				  struct lists_of_students* accepting_students,
				  struct stu_sch_node* all_so_far,
				  struct stu_sch_node* latest,
				  int i, int j);

struct stu_sch_node* predecessor_in_latest(struct partial_alloc* myalloc,
					   struct process_scp* myscp,
					   struct lists_of_students* accepting_students,
					   struct stu_sch_node* latest, int i, int j);

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle);

void reduce_student_acceptances(struct partial_alloc* myalloc, struct process_scp* myscp,
				struct lists_of_students* accepting_students,
			        int i);

#endif /* ENHANCED_H */
