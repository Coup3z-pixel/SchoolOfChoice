#include "defaccep.h"

 
struct partial_alloc deferred_acceptance(struct process_scp* myscp) {
  int i, j, k, nst, nsc, done;
  
  struct index** applicant_lists;

  struct partial_alloc answer;
  struct partial_alloc OLD_answer;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  applicant_lists = malloc(nsc * sizeof(struct index*));
  for (j = 1; j <= nsc; j++) {
    applicant_lists[j-1] = NULL;
  }

  for (i = 1; i <= nst; i++) {
    j = myscp->preferences[i-1][0];
    if (applicant_lists[j-1] == NULL) {
      applicant_lists[j-1] = malloc(sizeof(struct index*));
      *applicant_lists[j-1] = singleton_index(i);
    }
    else {
      add_element_to_index(applicant_lists[j-1],i);
    }
  }


  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc; j++) {
      if (applicant_lists[j-1]->no_elements > myscp->quotas[j-1]) {
	done = 0;
	reject_student(myscp, applicant_lists, i, j);
      }
    }
  }

  answer = zero_alloc_for_process_scp(myscp);

  for (j = 1; j <= nsc; j++) {
    for (k = 1; k <= applicant_lists[j-1]->no_elements; k++) {
      i = applicant_lists[j-1]->indices[k-1];
      set_entry(&answer, i, j, 1.0);
    }
  }

  for (j = 1; j <= nsc; j++) {
    if (applicant_lists[j-1] != NULL) {
      destroy_index(*(applicant_lists[j-1]));
      free(applicant_lists[j-1]);
    }
  }
  free(applicant_lists);

  OLD_answer = deferred_acceptance_OLD(myscp);
  if (!partial_allocs_are_same(&answer, &OLD_answer)) {
    fprintf(stderr, "deferred_acceptance and deferred_acceptance_OLD are different!\n");
    exit(0);
  }
  destroy_partial_alloc(OLD_answer);

  return answer;  
}

void reject_student(struct process_scp* myscp, struct index** applicant_lists, int i, int j) {
  int k, hit, new_school;

  if (applicant_lists[j-1]->no_elements > 1) {
    remove_element_from_index(applicant_lists[j-1], i);
  }
  else {
    destroy_index(*(applicant_lists[j-1]));
    free(applicant_lists[j-1]);
    applicant_lists[j-1] = NULL;
  }

  hit = 0;
  for (k = 1; k < myscp->no_eligible_schools[i-1] && !hit; k++) {
    if (myscp->preferences[i-1][k-1] == j) {
      hit = 1;
      new_school = myscp->preferences[i-1][k];
    }
  }
  add_element_to_index(applicant_lists[new_school-1], i);
}

 
struct partial_alloc deferred_acceptance_OLD(struct process_scp* myscp) {
  int i, j, k, l, nst, nsc, done, found_reject;
  
  struct partial_alloc answer;

  struct index* eligible_students;

  int* proposal_rank;

  struct subset* proposer_list;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  eligible_students = eligible_student_lists(myscp);

  proposal_rank = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    proposal_rank[i-1] = 1;
  }

  proposer_list = malloc(nsc * sizeof(struct subset));
  for (j = 1; j <= nsc; j++) {
    proposer_list[j-1] = nullset(eligible_students[j-1].no_elements);
  }

  for (i = 1; i <= nst; i++) {
    j = myscp->preferences[i-1][0];
    add_student_to_proposer_list(eligible_students, proposer_list, i, j);
  }

  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc; j++) {
      if (proposer_list[j-1].subset_size > myscp->quotas[j-1]) {
	done = 0;
	k = eligible_students[j-1].no_elements + 1;
	while (proposer_list[j-1].subset_size > myscp->quotas[j-1]) {

	  proposer_list[j-1].subset_size--;

	  found_reject = 0;
	  while (!found_reject) {
	    k--;
	    
	    i = eligible_students[j-1].indices[k-1];
	    if (proposer_list[j-1].indicator[k-1] == 1 &&
		proposal_rank[i-1] < myscp->no_eligible_schools[i-1]) {
	      found_reject = 1;
	    }
	  }
	  
	  proposer_list[j-1].indicator[k-1] = 0;
	  proposal_rank[i-1]++;
	  l = myscp->preferences[i-1][proposal_rank[i-1]-1];
	  add_student_to_proposer_list(eligible_students, proposer_list, i, l);
	}
      }
    }
  }

  /* Report the result */

  answer = zero_alloc_for_process_scp(myscp);
  
  for (i = 1; i <= nst; i++) {
    set_entry(&answer, i, myscp->preferences[i-1][proposal_rank[i-1]-1], 1.0);
  }

  for (j = 1; j <= nsc; j++) {
    destroy_index(eligible_students[j-1]);
    destroy_subset(proposer_list[j-1]);
  }    
  free(eligible_students);
  free(proposer_list);
  free(proposal_rank);

  return answer;
}

struct index* eligible_student_lists(struct process_scp* myscp) {
  int i, j, k, nst, nsc;

  struct index* eligible_students;

  int* no_eligible_students;

  int* cursor;

  nst = myscp->no_students;
  nsc = myscp->no_schools;
  
  no_eligible_students = malloc(nsc * sizeof(j));
  for (j = 1; j <= nsc; j++) {
    no_eligible_students[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      no_eligible_students[myscp->preferences[i-1][k-1]-1]++;
    }
  }

  eligible_students = malloc(nsc * sizeof(struct index));
  for (j = 1; j <= nsc; j++) {
    eligible_students[j-1].no_elements = no_eligible_students[j-1];
    eligible_students[j-1].indices = malloc(no_eligible_students[j-1] * sizeof(int));
  }

  cursor = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    cursor[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      j = myscp->preferences[i-1][k-1];
      cursor[j-1]++;
      eligible_students[j-1].indices[cursor[j-1]-1] = i;
    }
  }

  free(no_eligible_students);
  free(cursor);

  return eligible_students;
}

void add_student_to_proposer_list(struct index* eligible_students,
				  struct subset* proposer_list, int i, int j) {
  int k;
  
  proposer_list[j-1].subset_size++;
  k = 1;
  while (eligible_students[j-1].indices[k-1] != i) {
    k++;
  }
  proposer_list[j-1].indicator[k-1] = 1;
}
