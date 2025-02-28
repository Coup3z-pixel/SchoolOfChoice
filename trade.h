#ifndef TRADE_h
#define TRADE_h

#include "normal.h"
#include "schchprob.h"
#include "efficient.h"

void trade_until_efficient(struct process_scp* myscp, struct partial_alloc* alloc_to_adjust);

void claim_until_nonwasteful(struct process_scp* myscp, struct partial_alloc* alloc_to_adjust);

void process_cycles_at_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
				struct lists_of_students* accepting_students, int i, int j,
			    int max_depth);

struct stu_sch_node* find_cycle_or_not(struct partial_alloc* myalloc,
					   struct process_scp* myscp,
					   struct lists_of_students* accepting_students,
					   int i, int j, int max_depth);

struct stu_sch_node* cycle_or_not(struct partial_alloc* myalloc, struct process_scp* myscp,
				      struct lists_of_students* accepting_students,
				      struct stu_sch_node* all_so_far,
				      struct stu_sch_node* latest,
				      int i, int j, int max_depth);

struct stu_sch_node* predecessor_in_latest(struct partial_alloc* myalloc,
					   struct process_scp* myscp,
					   struct lists_of_students* accepting_students,
					   struct stu_sch_node* latest,
					   struct stu_sch_node* next_link);

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle);

void reduce_student_acceptances(struct partial_alloc* myalloc, struct process_scp* myscp,
				struct lists_of_students* accepting_students, int i);

#endif /* TRADE_H */
