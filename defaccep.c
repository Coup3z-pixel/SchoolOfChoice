#include "defaccep.h"

struct partial_alloc deferred_acceptance(struct process_scp* myscp) {
  int j, nst, nsc, rejectee, done;
  
  struct index** applicant_lists;

  struct partial_alloc answer;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  applicant_lists = malloc(nsc * sizeof(struct index*));
  each_student_applies_to_favorite_school(myscp, applicant_lists, nst, nsc);  

  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc; j++) {
      if (applicant_lists[j-1] != NULL) {
	if (applicant_lists[j-1]->no_elements > myscp->quotas[j-1]) {
	  done = 0;
	  rejectee = student_to_reject(myscp, applicant_lists[j-1], j);;
	  reject_student(myscp, applicant_lists, rejectee, j);
	}
      }
    }
  }

  answer = partial_alloc_from_applicant_lists(myscp, applicant_lists);

  destroy_applicant_lists(applicant_lists, nsc);

  return answer;  
}

void each_student_applies_to_favorite_school(struct process_scp* myscp,
					     struct index** applicant_lists, int nst, int nsc) {
  int i, j;
  
  for (j = 1; j <= nsc; j++) {
    applicant_lists[j-1] = NULL;
  }

  for (i = 1; i <= nst; i++) {
    j = myscp->preferences[i-1][0];
    add_element_to_possibly_NULL_index(&(applicant_lists[j-1]), i);
  }
}

void reject_student(struct process_scp* myscp, struct index** applicant_lists, int i, int j) {
  remove_i_from_applicant_list_j(applicant_lists, i, j);
  i_applies_to_next_school(myscp, applicant_lists, i, j);
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

void i_applies_to_next_school(struct process_scp* myscp, struct index** applicant_lists,
			      int i, int j) {
  int k, hit, new_school;

  hit = 0;
  for (k = 1; k < myscp->no_eligible_schools[i-1] && !hit; k++) {
    if (myscp->preferences[i-1][k-1] == j) {
      hit = 1;
      new_school = myscp->preferences[i-1][k];
    }
  }
  
  add_element_to_possibly_NULL_index(&(applicant_lists[new_school-1]), i);
}

int student_to_reject(struct process_scp* myscp, struct index* student_list, int j) {
  int hit, rej_index, rej_cand;
  
  hit = 0;
  rej_index = student_list->no_elements;
  rej_cand = student_list->indices[rej_index-1];
  while (!hit) {
    if (safe_school(myscp, rej_cand) != j) {
      hit = 1;
    }
    else {
      rej_index--;
      rej_cand = student_list->indices[rej_index-1];
    }
  }

  return rej_cand;
}


struct partial_alloc partial_alloc_from_applicant_lists(struct process_scp* myscp,
							struct index** applicant_lists) {
  int i, j, k, nsc;

  struct partial_alloc answer;

  nsc = myscp->no_schools;

  answer = zero_alloc_for_process_scp(myscp);

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
