#include "emcccode.h"

struct partial_alloc EMCC_allocation(struct process_scp* myscp) {
  int nst = myscp->no_students;
  int nsc = myscp->no_schools;

  struct stu_sch_node* cycle;
  struct stu_sch_node*** cmatrix;

  int* coarse = malloc(nsc * sizeof(int));
  struct partial_alloc alloc_to_adjust = MCC_alloc_plus_coarse_cutoffs(myscp, coarse);

  cmatrix = get_cycle_matrix(myscp, &alloc_to_adjust, coarse);
  while (matrix_has_cycle(cmatrix,nst,nsc)) {
    cycle = random_cycle(cmatrix,nst,nsc);    
    adjust_partial_alloc_along_cycle(&alloc_to_adjust, cycle, myscp);
    destroy_matrix_of_lists(cmatrix, nst, nsc);
    cmatrix = get_cycle_matrix(myscp, &alloc_to_adjust, coarse);
  }

  destroy_matrix_of_lists(cmatrix, nst, nsc);
  free(coarse);

  return alloc_to_adjust;
}

struct stu_sch_node*** get_cycle_matrix(struct process_scp* myscp,
					struct partial_alloc* alloc_to_adjust, int* coarse) {
  int i, j;
  
  int nst = myscp->no_students;
  int nsc = myscp->no_schools;

  struct stu_sch_node*** egraph = get_envy_graph(myscp, alloc_to_adjust, coarse);
  
  struct stu_sch_node*** cmatrix = malloc(nst * sizeof(struct stu_sch_node**));
  for (i = 1; i <= nst; i++) {
    cmatrix[i-1] = malloc(nsc * sizeof(struct stu_sch_node*));
    for (j = 1; j <= nsc; j++) {
      if (egraph[i-1][j-1] != NULL) {
	cmatrix[i-1][j-1] = get_cycle(egraph, nst, i, j);
      }
      else {
	cmatrix[i-1][j-1] = NULL;
      }
    }
  }

  destroy_matrix_of_lists(egraph, nst, nsc);

  return cmatrix;
}

struct stu_sch_node*** get_envy_graph(struct process_scp* myscp,
				      struct partial_alloc* alloc_to_adjust, int* coarse) {
  int h, i, j, k, l;
  
  int nst = myscp->no_students;
  int nsc = myscp->no_schools;

  struct stu_sch_node*** egraph = malloc(nst * sizeof(struct stu_sch_node**));
  for (i = 1; i <= nst; i++) {
    egraph[i-1] = malloc(nsc * sizeof(struct stu_sch_node*));
  }

  int** active = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    active[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (alloc_to_adjust->allocations[i-1][j-1] > 0.00001 && j != myscp->preferences[i-1][0]) {
	active[i-1][j-1] = 1;
      }
      else {
	active[i-1][j-1] = 0;
      }
      
      egraph[i-1][j-1] = NULL;
    }
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (active[i-1][j-1] == 1) {
	k = 1;
	while (myscp->preferences[i-1][k-1] != j) {
	  l = myscp->preferences[i-1][k-1];
	  if (alloc_to_adjust->allocations[i-1][l-1] <
	      myscp->eligible[i-1][l-1] * myscp->time_remaining - 0.000001 &&
	      coarse[l-1] <= myscp->priorities[i-1][l-1]) {
	    for (h = 1; h <= nst; h++) {
	      if (h != i && active[h-1][l-1] == 1) {
		append_node_to_stu_sch_list(&(egraph[i-1][j-1]), h, l);
	      }
	    }
	  }
	  k++;
	}
      }
    }
  }

  for (i = 1; i <= nst; i++) {
    free(active[i-1]);
  }
  free(active);
  
  return egraph;
}

struct stu_sch_node* get_cycle(struct stu_sch_node*** egraph, int nst, int i, int j) {
  int h, k, l, found_cycle, done, level, something_new;
  struct stu_sch_node* answer;
  struct stu_sch_node* probe;
  struct stu_sch_node* subprobe;

  struct stu_sch_node** previously_found = malloc(nst * sizeof(struct stu_sch_node*));
  for (h = 1; h <= nst; h++) {
    previously_found[h-1] = NULL;
  }

  probe = egraph[i-1][j-1];
  while (probe != NULL) {
    append_node_to_stu_sch_list(&(previously_found[0]), probe->stuno, probe->schno);
    probe = probe->next;
  }

  found_cycle = 0;
  done = 0;
  level = 1;

  while (!done) {
    something_new = 0;
    probe = previously_found[level-1];
    while (!done && probe != NULL) {
      subprobe = egraph[probe->stuno-1][probe->schno-1];
      while (!done && subprobe != NULL) {
	k = subprobe->stuno;
	l = subprobe->stuno;
	
	if (k == i && l == j) {
	  done = 1;
	  found_cycle = 1;
	}

	if (!done & !already_seen(previously_found,level-1,k,l)) {
	  something_new = 1;
	  append_node_to_stu_sch_list(&(previously_found[level-1]), k, l);
	}
	
	subprobe = subprobe->next;
      }
      probe = probe->next;
    }

    if (!something_new) {
      done = 1;
    }

    if (!done) {
      level++;
    }
  }

  if (found_cycle) {
    answer = extract_cycle(egraph, previously_found, level, i, j);
  }

  destroy_list_of_lists(previously_found, level);

  if (found_cycle) {
    return answer;
  }
  else {
    return NULL;
  }
}

int already_seen(struct stu_sch_node** previously_found, int level, int k, int l) {
  int h;
  struct stu_sch_node* probe;

  for (h = 1; h <= level; h++) {
    probe = previously_found[h-1];
    while (probe != NULL) {
      if (probe->stuno == k && probe->schno == l) {
	return 1;
      }
      else {
	probe = probe->next;
      }
    }
  }

  return 0;
}

struct stu_sch_node* extract_cycle(struct stu_sch_node*** egraph,
				   struct stu_sch_node** previously_found,
				   int level, int i, int j) {
  int l, target_stu, target_sch, done;
  
  l = level;
  target_stu = i;
  target_sch = j;
  struct stu_sch_node* old;
  struct stu_sch_node* new;
  struct stu_sch_node* probe;
  
  old = malloc(sizeof(struct stu_sch_node));
  old->stuno = target_stu;
  old->schno = target_sch;
  old->next = NULL;

  while (l >= 1) {
    done = 0;
    probe = previously_found[l-1];
    while (!done) {
      if (stu_sch_list_contains_pair(egraph[probe->stuno-1][probe->schno-1],
				     target_stu, target_sch)) {
	done = 1;
	
	target_stu = probe->stuno;
	target_sch = probe->schno;

	new = malloc(sizeof(struct stu_sch_node));
	new->stuno = target_stu;
	new->schno = target_sch;
	new->next = old;
	old = new;
      }
    }
    
    l--;
  }

  return old;
}

int matrix_has_cycle(struct stu_sch_node*** cmatrix, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (cmatrix[i-1][j-1] != NULL) {
	return 1;
      }
    }
  }

  return 0;
}

struct stu_sch_node* random_cycle(struct stu_sch_node*** cmatrix, int nst, int nsc) {
  int i, j;

  int no_active_students = 0;
  int* has_cycle = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    has_cycle[i-1] = 0;
    for (j = 1; j <= nsc && !has_cycle[i-1]; j++) {
      if (cmatrix[i-1][j-1] != NULL) {
        has_cycle[i - 1] = 1;
	no_active_students++;
      }
    }
  }

  int chosen_student_index = 0;
  double random = no_active_students * uniform();
  for (int i = 1; i <= no_active_students && chosen_student_index == 0; i++) {
    if ((double)(i - 1) < random && random <= (double)i) {
      chosen_student_index = i;
    }
  }

  int chosen_student = 0;
  for (i = 1; i <= nst && chosen_student == 0; i++) {
    if (has_cycle[i-1]) {
      chosen_student_index--;
      if (chosen_student_index == 0) {
	chosen_student = i;
      }
    }
  }

  int chosen_school = 0;
  for (j = 1; j <= nsc && chosen_school == 0; j++) {
    if (cmatrix[chosen_student-1][j-1] != NULL) {
      chosen_school = j;
    }
  }

  free(has_cycle);

  return cmatrix[chosen_student-1][chosen_school-1];
}


void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle,
				      struct process_scp* myscp) {
  double Delta;
  struct stu_sch_node* probe;

  Delta = alloc->allocations[cycle->stuno-1][cycle->schno-1];

  probe = cycle->next;
  while (probe != NULL) {
    Delta = min(Delta, alloc->allocations[probe->stuno-1][probe->schno-1]);
    probe = probe->next;
  }

  probe = cycle;
  while (probe->next != NULL) {
    Delta = min(Delta, (myscp->eligible[probe->stuno-1][probe->next->schno-1] *
			myscp->time_remaining)
		- alloc->allocations[probe->stuno-1][probe->next->schno-1]);
    probe = probe->next;
  }
  Delta = min(Delta, (myscp->eligible[probe->stuno-1][cycle->schno-1] * myscp->time_remaining) -
	      alloc->allocations[probe->stuno-1][cycle->schno-1]);

  probe = cycle;
  while (probe != NULL) {
    alloc->allocations[probe->stuno-1][probe->schno-1] -= Delta;
    probe = probe->next;
  }

  probe = cycle;
  while (probe->next != NULL) {
    alloc->allocations[probe->stuno-1][probe->next->schno-1] += Delta;
    probe = probe->next;
  }
  alloc->allocations[probe->stuno-1][cycle->schno-1] += Delta;
}

int stu_sch_list_contains_pair(struct stu_sch_node* stu_sch, int i, int j) {
  struct stu_sch_node* probe = stu_sch;
  
  while (probe != NULL) {
    if (probe->stuno == i && probe->schno == j) {
      return 1;
    }
    probe = probe->next;
  }

  return 0;
}

void append_node_to_stu_sch_list(struct stu_sch_node** stu_sch, int i, int j) {

  struct stu_sch_node* probe;
  
  if (*stu_sch == NULL) {
    *stu_sch = malloc(sizeof(struct stu_sch_node));
    (*stu_sch)->stuno = i;
    (*stu_sch)->schno = j;
    (*stu_sch)->next = NULL;
  }
  else {
    if (!stu_sch_list_contains_pair(*stu_sch, i, j)) {
      probe = *stu_sch;
      while (probe->next != NULL) {
	probe = probe->next;
      }
    probe->next = malloc(sizeof(struct stu_sch_node));
    (probe->next)->stuno = i;
    (probe->next)->schno = j;
    (probe->next)->next = NULL;
    }
  }
}

void destroy_stu_sch_list(struct stu_sch_node* node) {
  struct stu_sch_node* probe;
  
  if (node != NULL) {
    if (node->next != NULL) {
      while (node->next != NULL) {
	probe = node;
	while (probe->next->next != NULL) {
	  probe = probe->next;
	}
	free(probe->next);
	probe->next = NULL;
      }
    }
    
    free(node);
  }
}

void destroy_list_of_lists(struct stu_sch_node** previously_found, int level) {
  int l;

  for (l = 1; l <= level; l++) {
      destroy_stu_sch_list(previously_found[l-1]);
  }
  
  free(previously_found);
}

void destroy_matrix_of_lists(struct stu_sch_node*** egraph, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      destroy_stu_sch_list(egraph[i-1][j-1]);
    }
  }

  for (i = 1; i <= nst; i++) {
    free(egraph[i-1]);
  }
  free(egraph);
}

