#include "defaccep.h"

pure_alloc deferred_acceptance(input_sch_ch_prob* myiscp) {
  int j, nst, nsc, rejectee, done;

  input_sch_ch_prob myiscp_copy;
  
  element_list** applicant_lists;

  pure_alloc answer;

  if (!safe_schools_are_safe(myiscp)) {
    fprintf(stderr, "Unsafe safe schools in myiscp!\n");
    exit(0);
  }

  myiscp_copy = copy_of_input_scp(myiscp);
  /*
  refine_priorities(&myiscp_copy);
  */

  if (!safe_schools_are_safe(&myiscp_copy)) {
    fprintf(stderr, "Unsafe safe schools in myiscp_copy!\n");
    exit(0);
  }

  nst = myiscp_copy.no_students;
  nsc = myiscp_copy.no_schools;

  applicant_lists = malloc(nsc * sizeof(element_list*));
  each_student_applies_to_favorite_school(&myiscp_copy, applicant_lists, nst, nsc);  

  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc; j++) {
      if (applicant_lists[j-1] != NULL) {
	if (applicant_lists[j-1]->no_elements > myiscp_copy.quotas[j-1]) {
	  done = 0;
	  rejectee = lowest_priority_student(&myiscp_copy, applicant_lists[j-1], j);;
	  reject_student(&myiscp_copy, applicant_lists, rejectee, j);
	}
      }
    }
  }

  answer = pure_alloc_from_applicant_lists(myiscp, applicant_lists);

  destroy_applicant_lists(applicant_lists, nsc);

  destroy_input_sch_ch_prob(myiscp_copy);

  return answer;  
}

void refine_priorities(input_sch_ch_prob* myiscp) {
  int i, j, k, l, nst, nsc, no_eligible, top_priority_class, no_pr_class, cursor, index;

  int* no_with_priority;
  int* list_of_students;
  int* ordering;
  int* new_ordering_of_students;

  nst = myiscp->no_students;
  nsc = myiscp->no_schools;

  for (j = 1; j <= nsc; j++) {
    
    no_eligible = 0;
    top_priority_class = 0; 
    for (i = 1; i <= nst; i++) {
      if (is_eligible_input(myiscp, i, j)) {
	no_eligible++;
      }
      if (myiscp->priorities[i-1][j-1] > top_priority_class) {
	top_priority_class = myiscp->priorities[i-1][j-1];
      }
    }

    new_ordering_of_students = malloc(no_eligible * sizeof(int));

    no_with_priority = malloc((top_priority_class + 1) * sizeof(int));
    for (k = 0; k <= top_priority_class; k++) {
      no_with_priority[k] = 0;
    }
    for (i = 1; i <= nst; i++) {
      if (is_eligible_input(myiscp, i, j)) {
	no_with_priority[myiscp->priorities[i-1][j-1]]++;
      }
    }

    cursor = 0;
    for (k = 0; k <= top_priority_class; k++) {
      
      no_pr_class = no_with_priority[k];
      if (no_pr_class > 0) {
	list_of_students = malloc(no_with_priority[k] * sizeof(int));
      
	index = 0;
	for (i = 1; i <= nst; i++) {
	  if (is_eligible_input(myiscp, i, j)) {
	    if (myiscp->priorities[i-1][j-1] == k) {
	      list_of_students[index] = i;
	      index++;
	    }
	  }
	}

	ordering = random_ordering(no_pr_class);
	
	for (l = 1; l <= no_pr_class; l++) {
	  new_ordering_of_students[cursor + ordering[l-1] - 1] = list_of_students[l-1];
	}
	free(ordering);
	
	cursor += no_pr_class;
      
	free(list_of_students);
      }
    }

    for (k = 1; k <= no_eligible; k++) {
      myiscp->priorities[new_ordering_of_students[k-1]-1][j-1] = k-1;
    }

    free(no_with_priority);
    free(new_ordering_of_students);
  }
}


void each_student_applies_to_favorite_school(input_sch_ch_prob* myiscp,
					     element_list** applicant_lists, int nst, int nsc) {
  int i, j;
  
  for (j = 1; j <= nsc; j++) {
    applicant_lists[j-1] = NULL;
  }

  for (i = 1; i <= nst; i++) {
    j = myiscp->preferences[i-1][0];
    add_element_to_possibly_NULL_element_list(&(applicant_lists[j-1]), i);
  }
}

void reject_student(input_sch_ch_prob* myiscp, element_list** applicant_lists,
			int i, int j) {
  remove_i_from_applicant_list_j(applicant_lists, i, j);
  i_applies_to_next_school(myiscp, applicant_lists, i, j);
}

void remove_i_from_applicant_list_j(element_list** applicant_lists, int i, int j) {
  if (applicant_lists[j-1]->no_elements > 1) {
    remove_element_from_element_list(applicant_lists[j-1], i);
  }
  else {
    destroy_element_list(*(applicant_lists[j-1]));
    free(applicant_lists[j-1]);
    applicant_lists[j-1] = NULL;
  } 
}

void i_applies_to_next_school(input_sch_ch_prob* myiscp, element_list** applicant_lists,
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
  
  add_element_to_possibly_NULL_element_list(&(applicant_lists[new_school-1]), i);
}

int lowest_priority_student(input_sch_ch_prob* myiscp, element_list* school_app_list, int j) {
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

partial_alloc partial_alloc_from_applicant_lists(input_sch_ch_prob* myiscp,
							element_list** applicant_lists) {
  int i, j, k, nsc;

  partial_alloc answer;

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

pure_alloc pure_alloc_from_applicant_lists(input_sch_ch_prob* myiscp,
						  element_list** applicant_lists) {
  int i, j, k, nsc;

  pure_alloc answer;

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

void destroy_applicant_lists(element_list** applicant_lists, int nsc) {
  int j;
  
  for (j = 1; j <= nsc; j++) {
    if (applicant_lists[j-1] != NULL) {
      destroy_element_list(*(applicant_lists[j-1]));
      free(applicant_lists[j-1]);
    }
  }
  free(applicant_lists);
}

/* New Deferred Acceptance - We now take priority into account */
