#include "defaccep.h"
 
struct partial_alloc deferred_acceptance(struct process_scp* input) {
  int i, j, k, l, nst, nsc, done, found_reject;
  
  struct partial_alloc answer;

  struct index* eligible_students;

  struct subset* proposer_list;

  int* proposal_rank;

  nst = input->no_students;
  nsc = input->no_schools;

  eligible_students = eligible_student_lists(input);

  proposal_rank = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    proposal_rank[i-1] = 1;
  }

  proposer_list = malloc(nsc * sizeof(struct subset));
  for (j = 1; j <= nsc; j++) {
    proposer_list[j-1] = nullset(eligible_students[j-1].no_elements);
  }

  for (i = 1; i <= nst; i++) {
    j = input->preferences[i-1][0];
    add_student_to_proposer_list(eligible_students, proposer_list, i, j);
  }

  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc; j++) {
      if (proposer_list[j-1].subset_size > input->quotas[j-1]) {
	done = 0;
	k = eligible_students[j-1].no_elements + 1;
	while (proposer_list[j-1].subset_size > input->quotas[j-1]) {

	  proposer_list[j-1].subset_size--;

	  found_reject = 0;
	  while (!found_reject) {
	    k--;

	    if (k < 1) {
	      fprintf(stderr, "We have been given an infeasible scp and are quitting.\n");
	      exit(0);
	    }
	    
	    i = eligible_students[j-1].indices[k-1];
	    if (proposer_list[j-1].indicator[k-1] == 1 &&
		proposal_rank[i-1] < input->no_eligible_schools[i-1]) {
	      found_reject = 1;
	    }
	  }
	  
	  proposer_list[j-1].indicator[k-1] = 0;
	  proposal_rank[i-1]++;
	  l = input->preferences[i-1][proposal_rank[i-1]-1];
	  add_student_to_proposer_list(eligible_students, proposer_list, i, l);
	}
      }
    }
  }

  /* Report the result */

  answer = zero_alloc_for_process_scp(input);
  
  for (i = 1; i <= nst; i++) {
    set_entry(&answer, i, input->preferences[i-1][proposal_rank[i-1]-1], 1.0);
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

struct index* eligible_student_lists(struct process_scp* input) {
  int i, j, k, nst, nsc;

  struct index* eligible_students;

  int* no_eligible_students;

  int* cursor;

  nst = input->no_students;
  nsc = input->no_schools;
  
  no_eligible_students = malloc(nsc * sizeof(j));
  for (j = 1; j <= nsc; j++) {
    no_eligible_students[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= input->no_eligible_schools[i-1]; k++) {
      no_eligible_students[input->preferences[i-1][k-1]-1]++;
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
    for (k = 1; k <= input->no_eligible_schools[i-1]; k++) {
      j = input->preferences[i-1][k-1];
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
