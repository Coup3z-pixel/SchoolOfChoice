#ifndef DEFACCEP_H
#define DEFACCEP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "normal.h"
#include "partalloc.h"

/*

struct partial_alloc deferred_acceptance(struct input_sch_ch_prob* myiscp);

*/

struct pure_alloc deferred_acceptance(struct input_sch_ch_prob* myiscp);

void each_student_applies_to_favorite_school(struct input_sch_ch_prob* myiscp,
					     struct index** applicant_lists, int nst, int nsc);
void reject_student(struct input_sch_ch_prob* myiscp, struct index** applicant_lists,
			int i, int j);

void remove_i_from_applicant_list_j(struct index** applicant_lists, int i, int j);

void i_applies_to_next_school(struct input_sch_ch_prob* myiscp, struct index** applicant_lists,
			      int i, int j);

int lowest_priority_student(struct input_sch_ch_prob* myscp, struct index* school_app_list, int j);

struct partial_alloc partial_alloc_from_applicant_lists(struct input_sch_ch_prob* myiscp,
							struct index** applicant_lists);

struct pure_alloc pure_alloc_from_applicant_lists(struct input_sch_ch_prob* myiscp,
						  struct index** applicant_lists);

void destroy_applicant_lists(struct index** applicant_lists, int nsc);


#endif /* DEFACCEP_H */
