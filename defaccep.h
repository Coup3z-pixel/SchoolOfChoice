#ifndef DEFACCEP_H
#define DEFACCEP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"

 
struct partial_alloc deferred_acceptance(struct process_scp* myscp);

void reject_student(struct process_scp* myscp, struct index** applicant_lists, int i, int j);

 
struct partial_alloc deferred_acceptance_OLD(struct process_scp* input);

struct index* eligible_student_lists(struct process_scp* input);

void add_student_to_proposer_list(struct index* eligible_students,
				  struct subset* proposer_list, int i, int j);

/*
int satisfies_the_GMC(struct input_sch_ch_prob* input);
*/

#endif /* DEFACCEP_H */
