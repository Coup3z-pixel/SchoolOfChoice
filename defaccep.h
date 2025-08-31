#ifndef DEFACCEP_H
#define DEFACCEP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"

 
struct partial_alloc deferred_acceptance(struct process_scp* myscp);

void each_student_applies_to_favorite_school(struct process_scp* myscp,
					     struct index** applicant_lists, int nst, int nsc);

void reject_student(struct process_scp* myscp, struct index** applicant_lists, int i, int j);

void remove_i_from_applicant_list_j(struct index** applicant_lists, int i, int j);

void i_applies_to_next_school(struct process_scp* myscp, struct index** applicant_lists,
			      int i, int j);

int student_to_reject(struct process_scp* myscp, struct index* student_list, int j);

struct partial_alloc partial_alloc_from_applicant_lists(struct process_scp* myscp,
							struct index** applicant_lists);

void destroy_applicant_lists(struct index** applicant_lists, int nsc);

#endif /* DEFACCEP_H */
