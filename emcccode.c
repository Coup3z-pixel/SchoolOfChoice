#include "emcccode.h"

struct partial_alloc EMCC_allocation(struct process_scp* myscp) {
  int i, j, nst, nsc;

  int* coarse;
  int* must_repeat;
  
  struct partial_alloc alloc_to_adjust;

  struct process_scp red_scp;

  struct lists_of_students accepting_students;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  alloc_to_adjust = MCC_alloc_plus_coarse_cutoffs(myscp, coarse);

  red_scp = reduced_scp(myscp, coarse);

  accepting_students = get_accepting_students(&alloc_to_adjust, &red_scp);

  must_repeat = malloc(sizeof(int));
  *must_repeat = 1;
  while (*must_repeat == 1) {
    *must_repeat = 0;
    for (i = 1; i <= nst; i++) {
      for (j = 1; j <= nsc; j++) {
	if (student_can_accept_school(&alloc_to_adjust, &red_scp, i, j)) {
	  process_cycles_at_pair(&alloc_to_adjust, &red_scp, &accepting_students, i, j,
				     must_repeat);
	}
      }
    }
  }

  free(must_repeat);
  free(coarse);
  destroy_process_scp(red_scp);
  destroy_lists_of_students(&accepting_students);

  return alloc_to_adjust;
}

void process_cycles_at_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
			    struct lists_of_students* accepting_students, int i, int j,
			    int* must_repeat) {
  int found;
  struct stu_sch_node* cycle;
  struct stu_sch_node* probe;

  found = 1;
  while (found && is_accepting(accepting_students, i, j)) {

    cycle = find_cycle_or_show_none_exist(myalloc, myscp, accepting_students, i, j);
    if (cycle == NULL) {
      found = 0;
    }
    else {
      *must_repeat = 1;
      
      adjust_partial_alloc_along_cycle(myalloc, cycle);

      probe = cycle;
      while (probe != NULL) {
	reduce_student_acceptances(myalloc, myscp, accepting_students, probe->stuno);
	probe = probe->next;
      }

      destroy_stu_sch_list(cycle);
      cycle = NULL;
    }
  }
}

struct stu_sch_node* find_cycle_or_show_none_exist(struct partial_alloc* myalloc,
						   struct process_scp* myscp,
						   struct lists_of_students* accepting_students,
						   int i, int j) {
  struct stu_sch_node* answer;
  struct stu_sch_node* all_so_far;
  struct stu_sch_node* latest;
  
  all_so_far = create_stu_sch_node(i,j);
  latest = possible_recipients(myalloc, myscp, accepting_students, i, j);

  answer = cycle_or_not(myalloc, myscp, accepting_students, all_so_far, latest, i, j);
  
  destroy_stu_sch_list(all_so_far);
  destroy_stu_sch_list(latest);

  return answer;
}

struct stu_sch_node* cycle_or_not(struct partial_alloc* myalloc, struct process_scp* myscp,
				  struct lists_of_students* accepting_students,
				  struct stu_sch_node* all_so_far,
				  struct stu_sch_node* latest,
				  int i, int j) {
  int found;
  
  struct stu_sch_node* new_nodes;
  struct stu_sch_node* answer;
  struct stu_sch_node* probe;
  struct stu_sch_node* next_link;
  struct stu_sch_node* latest_copy;

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
	answer = predecessor_in_latest(myalloc, myscp, accepting_students, latest, i, j);
	answer->next = create_stu_sch_node(i,j);
      }
      probe = probe->next;
    }

    if (!found) {
      probe = all_so_far;
      while (probe->next != NULL) {
	probe = probe->next;
      }
      latest_copy = copy_of_stu_sch_list(latest);
      probe->next = latest_copy;
      next_link = cycle_or_not(myalloc, myscp, accepting_students, all_so_far, new_nodes, i, j);
      if (next_link == NULL) {
	answer = NULL;
      }
      else {
	answer = predecessor_in_latest(myalloc, myscp, accepting_students, latest,
				       next_link->stuno, next_link->schno);
	answer->next = next_link;
      }
    }
  }

  destroy_stu_sch_list(new_nodes);

  return answer;
}

struct stu_sch_node* predecessor_in_latest(struct partial_alloc* myalloc,
					   struct process_scp* myscp,
					   struct lists_of_students* accepting_students,
					   struct stu_sch_node* latest, int i, int j) {
  int found;
  struct stu_sch_node* answer;
  struct stu_sch_node* recipients;
  struct stu_sch_node* probe;
  
  found = 0;
  probe = latest;
  while (!found) {
    recipients = possible_recipients(myalloc, myscp, accepting_students,
				     probe->stuno, probe->schno);
    if (stu_sch_list_contains_pair(recipients, i, j)) {
      found = 1;
      answer = create_stu_sch_node(probe->stuno,probe->schno);
    }
    destroy_stu_sch_list(recipients);
    probe = probe->next;
  }

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
				struct lists_of_students* accepting_students,
			        int i) {
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
