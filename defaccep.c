#include "defaccep.h"

struct pure_alloc deferred_acceptance(struct input_sch_ch_prob* myiscp) {
  int j, nst, nsc, rejectee, done;
  
  struct index** applicant_lists;

  struct pure_alloc answer;

  nst = myiscp->no_students;
  nsc = myiscp->no_schools;

  applicant_lists = malloc(nsc * sizeof(struct index*));
  each_student_applies_to_favorite_school(myiscp, applicant_lists, nst, nsc);  

  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc; j++) {
      if (applicant_lists[j-1] != NULL) {
	if (applicant_lists[j-1]->no_elements > myiscp->quotas[j-1]) {
	  done = 0;
	  rejectee = lowest_priority_student(myiscp, applicant_lists[j-1], j);;
	  reject_student(myiscp, applicant_lists, rejectee, j);
	}
      }
    }
  }

  answer = pure_alloc_from_applicant_lists(myiscp, applicant_lists);

  destroy_applicant_lists(applicant_lists, nsc);

  return answer;  
}


void each_student_applies_to_favorite_school(struct input_sch_ch_prob* myiscp,
					     struct index** applicant_lists, int nst, int nsc) {
  int i, j;
  
  for (j = 1; j <= nsc; j++) {
    applicant_lists[j-1] = NULL;
  }

  for (i = 1; i <= nst; i++) {
    j = myiscp->preferences[i-1][0];
    add_element_to_possibly_NULL_index(&(applicant_lists[j-1]), i);
  }
}

void reject_student(struct input_sch_ch_prob* myiscp, struct index** applicant_lists,
			int i, int j) {
  remove_i_from_applicant_list_j(applicant_lists, i, j);
  i_applies_to_next_school(myiscp, applicant_lists, i, j);
}

void remove_i_from_applicant_list_j(struct index** applicant_lists, int i, int j) {
  if (applicant_lists[j-1]->no_elements > 1) {
    remove_element_from_index(applicant_lists[j-1], i);
  }
  else {
    destroy_index(*(applicant_lists[j-1]));
    free(applicant_lists[j-1]);
    applicant_lists[j-1] = NULL;
  } 
}

void i_applies_to_next_school(struct input_sch_ch_prob* myiscp, struct index** applicant_lists,
			      int i, int j) {
  int k, hit, new_school, no_elig;

  no_elig = myiscp->no_eligible_schools[i-1];

  hit = 0;
  for (k = 1; k < no_elig && !hit; k++) {
    if (myiscp->preferences[i-1][k-1] == j) {
      hit = 1;
      new_school = myiscp->preferences[i-1][k];
    }
  }
  
  add_element_to_possibly_NULL_index(&(applicant_lists[new_school-1]), i);
}

int lowest_priority_student(struct input_sch_ch_prob* myiscp, struct index* school_app_list,
			    int j) {
  int i, cand_worst_pr, cand_worst_st, cand_st, cand_pr;

  cand_worst_st = school_app_list->indices[0];
  
  cand_worst_pr = get_input_priority(myiscp, cand_worst_st, j);

  for (i = 2; i <= school_app_list->no_elements; i++) {
    cand_st = school_app_list->indices[i-1];  
    cand_pr = get_input_priority(myiscp, cand_st, j);
    if (cand_pr < cand_worst_pr) {
      cand_worst_pr = cand_pr;
      cand_worst_st = cand_st;
    }
  }

  return cand_worst_st;
}

struct partial_alloc partial_alloc_from_applicant_lists(struct input_sch_ch_prob* myiscp,
							struct index** applicant_lists) {
  int i, j, k, nsc;

  struct partial_alloc answer;

  nsc = myiscp->no_schools;

  answer = zero_alloc_for_input_scp(myiscp);

  for (j = 1; j <= nsc; j++) {
    if (applicant_lists[j-1] != NULL) {
      for (k = 1; k <= applicant_lists[j-1]->no_elements; k++) {
	i = applicant_lists[j-1]->indices[k-1];
	set_entry(&answer, i, j, 1.0);
      }
    }
  }

  return answer;
}

struct pure_alloc pure_alloc_from_applicant_lists(struct input_sch_ch_prob* myiscp,
						  struct index** applicant_lists) {
  int i, j, k, nsc;

  struct pure_alloc answer;

  nsc = myiscp->no_schools;

  answer = zero_pure_alloc_for_input_scp(myiscp);

  for (j = 1; j <= nsc; j++) {
    if (applicant_lists[j-1] != NULL) {
      for (k = 1; k <= applicant_lists[j-1]->no_elements; k++) {
	i = applicant_lists[j-1]->indices[k-1];
	set_integer_entry(&answer, i, j, 1);
      }
    }
  }

  return answer;
}

void destroy_applicant_lists(struct index** applicant_lists, int nsc) {
  int j;
  
  for (j = 1; j <= nsc; j++) {
    if (applicant_lists[j-1] != NULL) {
      destroy_index(*(applicant_lists[j-1]));
      free(applicant_lists[j-1]);
    }
  }
  free(applicant_lists);
}

/* New Deferred Acceptance - We now take priority into account */
