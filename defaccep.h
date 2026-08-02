#ifndef DEFACCEP_H
#define DEFACCEP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"

pure_alloc deferred_acceptance(input_sch_ch_prob* myiscp);

void refine_priorities(input_sch_ch_prob* myiscp);

void each_student_applies_to_favorite_school(input_sch_ch_prob* myiscp,
					     element_list** applicant_lists, int nst, int nsc);
void reject_student(input_sch_ch_prob* myiscp, element_list** applicant_lists,
			int i, int j);

void remove_i_from_applicant_list_j(element_list** applicant_lists, int i, int j);

void i_applies_to_next_school(input_sch_ch_prob* myiscp, element_list** applicant_lists,
			      int i, int j);

int lowest_priority_student(input_sch_ch_prob* myscp, element_list* school_app_list, int j);

partial_alloc partial_alloc_from_applicant_lists(input_sch_ch_prob* myiscp,
							element_list** applicant_lists);

pure_alloc pure_alloc_from_applicant_lists(input_sch_ch_prob* myiscp,
						  element_list** applicant_lists);

void destroy_applicant_lists(element_list** applicant_lists, int nsc);


#endif /* DEFACCEP_H */
