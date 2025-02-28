#include "trade.h"

void trade_until_efficient(struct process_scp* myscp, struct partial_alloc* alloc_to_adjust) {

  fprintf(stderr, "Entering trade_until_efficient.\n");
  
  int i, j, nsc, max_depth;

  struct lists_of_students accepting_students;

  struct stu_list_node* probe;

  claim_until_nonwasteful(myscp, alloc_to_adjust);
  
  nsc = myscp->no_schools;

  max_depth = 1;

  while (!allocation_is_efficient(alloc_to_adjust, myscp)) {

    max_depth++;

    fprintf(stderr, "Now max_depth is %i.\n", max_depth);

    accepting_students = get_accepting_students(alloc_to_adjust, myscp);

    for (j = 1; j <= nsc; j++) {
      probe = accepting_students.lists[j-1];
      while (probe != NULL) {
	i = probe->stu_no;
	process_cycles_at_pair(alloc_to_adjust, myscp, &accepting_students,i, j,
			       max_depth/*, could_be_inefficient*/);
	if (accepting_students.lists[j-1] == NULL) {
	  probe = NULL;
	}
	else {
	  if (accepting_students.lists[j-1]->stu_no != i) {
	    probe = accepting_students.lists[j-1];
	  }
	  else {
	    accepting_students.lists[j-1] = probe->next;
	    free(probe);   
	    probe = accepting_students.lists[j-1];
	  }
	}
      }
    }

    destroy_lists_of_students(&accepting_students);
  }
}

void claim_until_nonwasteful(struct process_scp* myscp, struct partial_alloc* alloc_to_adjust) {
  int i, j, k, l, m, nsc, nst, done;

  fprintf(stderr, "Entering claim_until_nonwasteful.\n");

  double Delta;

  double* unused_capacity;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  unused_capacity = malloc(nsc * sizeof(double));

  for (j = 1; j <= nsc; j++) {
    unused_capacity[j-1] = myscp->quotas[j-1];
    for (i = 1; i <= nst; i++) {
      unused_capacity[j-1] -= dbl_entry(&(alloc_to_adjust->sparse), i, j);
    }
  }

  done = 0;
  while (!done) {
    done = 1;
    for (j = 1; j <= nsc && done; j++) {
      if (unused_capacity[j-1] > 0.000001) {
	for (i = 1; i <= nst && done; i++) {
	  k = 1;
	  while (k < myscp->no_eligible_schools[i-1] && j != myscp->preferences[i-1][k-1]) {
	    k++;
	  }
	  if (k < myscp->no_eligible_schools[i-1]) {
	    l = k + 1;
	    while (l <= myscp->no_eligible_schools[i-1] && done) {
	      m = myscp->preferences[i-1][l-1];
	      if (dbl_entry(&(alloc_to_adjust->sparse), i, m) > 0.000001) {
		done = 0;
		Delta = min(unused_capacity[j-1], dbl_entry(&(alloc_to_adjust->sparse), i, m));
		unused_capacity[j-1] -= Delta;
		unused_capacity[m-1] += Delta;
		increment_dbl_entry(&(alloc_to_adjust->sparse), i, j, Delta);
		increment_dbl_entry(&(alloc_to_adjust->sparse), i, m, -Delta);
	      }
	      l++;
	    }
	  }
	}
      }
    }
  }
  free(unused_capacity);

  fprintf(stderr, "Leaving claim_until_nonwasteful.\n");
  
}

void process_cycles_at_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
				struct lists_of_students* accepting_students, int i, int j,
				int max_depth) {
  int found;
  struct stu_sch_node* cycle;
  struct stu_sch_node* probe;

  found = 1;
  while (found && is_accepting(accepting_students, i, j)) {

    cycle = find_cycle_or_not(myalloc, myscp, accepting_students, i, j,
			      max_depth);
    if (cycle == NULL) {
      found = 0;
    }
    else {
      adjust_partial_alloc_along_cycle(myalloc, cycle);

      probe = cycle;
      while (probe != NULL) {
	reduce_student_acceptances(myalloc, myscp, accepting_students, probe->stuno);
	probe = probe->next;
      }

      destroy_stu_sch_list(cycle);
    }
  }
}

struct stu_sch_node* find_cycle_or_not(struct partial_alloc* myalloc,
					   struct process_scp* myscp,
					   struct lists_of_students* accepting_students,
					   int i, int j, int max_depth) {
  struct stu_sch_node* answer;
  struct stu_sch_node* all_so_far;
  struct stu_sch_node* latest;
  
  all_so_far = create_stu_sch_node(i,j);
  latest = possible_recipients(myalloc, myscp, accepting_students, i, j);

  answer = cycle_or_not(myalloc, myscp, accepting_students, all_so_far, latest, i, j,
			    max_depth);
  
  destroy_stu_sch_list(all_so_far);
  destroy_stu_sch_list(latest);

  return answer;
}

struct stu_sch_node* cycle_or_not(struct partial_alloc* myalloc, struct process_scp* myscp,
				      struct lists_of_students* accepting_students,
				      struct stu_sch_node* all_so_far,
				      struct stu_sch_node* latest,
				  int i, int j, int max_depth) {
  int found;
  
  struct stu_sch_node* new_nodes;
  struct stu_sch_node* answer;
  struct stu_sch_node* probe;
  struct stu_sch_node* next_link;

  new_nodes = simple_new_layer(myalloc, myscp, accepting_students, all_so_far, latest);

  if (new_nodes == NULL) {
    answer = NULL;
  }
  else {
    found = 0;
    probe = new_nodes;
    while (!found && probe != NULL) {
      if (probe->stuno == i && probe->schno == j) {
	found = 1;
	/*
	*could_be_inefficient = 1;
	*/
	next_link = create_stu_sch_node(i,j);
	answer = predecessor_in_latest(myalloc, myscp, accepting_students, latest, next_link);
      }
      probe = probe->next;
    }

    if (!found) {
      append_copy_of_listB_to_listA(all_so_far, latest);

      if (max_depth == 1) {
	/*
	*could_be_inefficient = 1;
	*/
	answer = NULL;
      }
      else {
	next_link = cycle_or_not(myalloc, myscp, accepting_students, all_so_far, new_nodes,
				 i, j, max_depth - 1);
	if (next_link == NULL) {
	  answer = NULL;
	}
	else {
	  answer = predecessor_in_latest(myalloc, myscp, accepting_students, latest, next_link);
	}
      }
    }
  }

  destroy_stu_sch_list(new_nodes);

  return answer;
}

struct stu_sch_node* predecessor_in_latest(struct partial_alloc* myalloc,
					   struct process_scp* myscp,
					   struct lists_of_students* accepting_students,
					   struct stu_sch_node* latest,
					   struct stu_sch_node* next_link) {
  int found;
  struct stu_sch_node* answer;
  struct stu_sch_node* recipients;
  struct stu_sch_node* probe;
  
  found = 0;
  probe = latest;
  while (!found) {
    recipients = possible_recipients(myalloc, myscp, accepting_students,
				     probe->stuno, probe->schno);
    if (stu_sch_list_contains_pair(recipients, next_link->stuno, next_link->schno)) {
      found = 1;
      answer = create_stu_sch_node(probe->stuno,probe->schno);
    }
    destroy_stu_sch_list(recipients);
    probe = probe->next;
  }

  answer->next = next_link;

  return answer;
}

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle) {
  double Delta;
  struct stu_sch_node* probe;

  Delta = 1;

  probe = cycle;
  while (probe->next != NULL) {
    Delta = min(Delta, get_entry(alloc, probe->stuno, (probe->next)->schno));
    probe = probe->next;
  }
  Delta = min(Delta, get_entry(alloc, probe->stuno, cycle->schno));
  
  probe = cycle;
  while (probe != NULL) {
    increment_entry(alloc, probe->stuno, probe->schno, Delta);
    probe = probe->next;
  }

  probe = cycle;
  while (probe->next != NULL) {
    increment_entry(alloc, probe->stuno, probe->next->schno, -Delta);
    probe = probe->next;
  }
  increment_entry(alloc, probe->stuno, cycle->schno, -Delta);
} 

void reduce_student_acceptances(struct partial_alloc* myalloc, struct process_scp* myscp,
				struct lists_of_students* accepting_students, int i) {
  int k, l, worst;

  worst = 1;
  k = 2;
  while (k <= myscp->no_eligible_schools[i-1]) {
    if (get_entry(myalloc, i, myscp->preferences[i-1][k-1]) > 0.00001) {
      worst = k;
    }
    k++;
  }

  for (k = worst; k <= myscp->no_eligible_schools[i-1]; k++) {
    l = myscp->preferences[i-1][k-1];    
    remove_student_from_list(&(accepting_students->lists[l-1]), i);
  }
}
