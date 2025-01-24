#include "emcccode.h"

struct partial_alloc EMCC_allocation(struct process_scp* myscp) {
  int i, j, nst, nsc;

  int* coarse;

  struct stu_sch_node*** chart;
  struct stu_sch_node*** envygr;
  struct stu_sch_node*** enviedgr;
  
  struct partial_alloc alloc_to_adjust;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  alloc_to_adjust = MCC_alloc_plus_coarse_cutoffs(myscp, coarse);

  envygr = get_envy_graph(myscp, &alloc_to_adjust, coarse);
  enviedgr = get_envied_graph(envygr, nst, nsc);

  chart = malloc(nst * sizeof(struct stu_sch_node**));
  for (i = 1; i <= nst; i++) {
    chart[i-1]  = malloc(nsc * sizeof(struct stu_sch_node*));
    for (j = 1; j <= nsc; j++) {
      chart[i-1][j-1] = NULL;
    }
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (envygr[i-1][j-1] != NULL || enviedgr[i-1][j-1] != NULL) {
	if (envygr[i-1][j-1] == NULL || enviedgr[i-1][j-1] == NULL) {
	  remove_pair_from_envy_and_envied_graphs(envygr, enviedgr, i, j);
	}
	else {
	  process_cycles_at_pair(envygr, enviedgr, chart, &alloc_to_adjust, i, j);
	}
      }
    }
  }

  free(coarse);
  destroy_matrix_of_lists(envygr, nst, nsc);
  destroy_matrix_of_lists(enviedgr, nst, nsc);
  destroy_matrix_of_lists(chart, nst, nsc);

  return alloc_to_adjust;
}

struct stu_sch_node*** get_envy_graph(struct process_scp* myscp,
				      struct partial_alloc* alloc_to_adjust, int* coarse) {
  int h, i, j, k, l, nst, nsc;

  int** active;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  struct stu_sch_node*** envygr = malloc(nst * sizeof(struct stu_sch_node**));
  for (i = 1; i <= nst; i++) {
    envygr[i-1] = malloc(nsc * sizeof(struct stu_sch_node*));
    for (j = 1; j <= nsc; j++) {
      envygr[i-1][j-1] = NULL;
    }
  }

  active = get_pairs_that_can_envy(myscp, alloc_to_adjust);

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (active[i-1][j-1] == 1) {
	k = 1;
	while (myscp->preferences[i-1][k-1] != j) {
	  l = myscp->preferences[i-1][k-1];
	  if (get_entry(alloc_to_adjust, i, l) <
	      is_eligible(myscp, i, l) * myscp->time_remaining - 0.000001 &&
	      coarse[l-1] <= get_priority(myscp, i, l)) {
	    for (h = 1; h <= nst; h++) {
	      if (h != i && active[h-1][l-1] == 1) {
		append_node_to_stu_sch_list(&(envygr[i-1][j-1]), h, l);
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
  
  return envygr;
}

void process_cycles_at_pair(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
			    struct stu_sch_node*** chart, struct partial_alloc* alloc_to_adjust,
			    int i, int j) {
  int new_layer_nonempty;
  int* found_cycle;

  struct stu_sch_node* all_nodes_reached;
  struct stu_sch_node* all_nodes_reached_tip;
  struct stu_sch_node* last_layer;
  struct stu_sch_node* new_layer;
  struct stu_sch_node* probe;

  found_cycle = malloc(sizeof(int));
  *found_cycle = 1;

  while(*found_cycle && envygr[i-1][j-1] != NULL) {
    *found_cycle = 0;
    new_layer_nonempty = 1;
    all_nodes_reached = NULL;
    all_nodes_reached_tip = NULL;
    
    while (!(*found_cycle) && new_layer_nonempty && envygr[i-1][j-1] != NULL) {
      
      if (all_nodes_reached == NULL) {
	all_nodes_reached = copy_of_list(envygr[i-1][j-1]);
	all_nodes_reached_tip = all_nodes_reached;
	while (all_nodes_reached_tip->next != NULL) {
	  all_nodes_reached_tip = all_nodes_reached_tip->next;
	}
	
	last_layer = all_nodes_reached;
	
	probe = all_nodes_reached;
	while (probe != NULL) {
	  chart[probe->stuno-1][probe->schno-1] = create_stu_sch_node(i,j);
	  probe = probe->next;
	}
      }
      
      else {
	new_layer =  get_new_layer(envygr, chart, last_layer, found_cycle, i, j);
	if (new_layer == NULL) {
	  new_layer_nonempty = 0;
	}
	else {
	  all_nodes_reached_tip->next = new_layer;
	  while (all_nodes_reached_tip->next != NULL) {
	    all_nodes_reached_tip = all_nodes_reached_tip->next;
	  }
	}
	last_layer = new_layer;
      } 
    }

    if (*found_cycle) {

      /* fprintf(stderr, "We actually found an inefficiency!\n"); */
      
      process_cycle(envygr, enviedgr, chart, alloc_to_adjust, i, j);
    }

    clean_up_chart_and_nodes(chart, all_nodes_reached);
  }

  free(found_cycle);
  remove_pair_from_envy_and_envied_graphs(envygr, enviedgr, i, j);
}

void process_cycle(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
		   struct stu_sch_node*** chart, struct partial_alloc* alloc_to_adjust,
		   int i, int j) {
  struct stu_sch_node* cycle;
  struct stu_sch_node* removal_list;
  struct stu_sch_node* probe;
  
  cycle = extract_cycle(chart, i, j);
  removal_list = NULL;
  adjust_partial_alloc_along_cycle(alloc_to_adjust, cycle, &removal_list);

  if (removal_list == NULL) {
    printf("We got a NULL removal list.\n");
    exit(0);
  }
  
  probe = removal_list;
  while (probe != NULL) {
    remove_pair_from_envy_and_envied_graphs(envygr, enviedgr, probe->stuno, probe->schno);
    probe = probe->next;
  }
  
  destroy_stu_sch_list(cycle);
  destroy_stu_sch_list(removal_list);
}

struct stu_sch_node* extract_cycle(struct stu_sch_node*** chart, int i, int j) {
  int g, h, k, l;
  struct stu_sch_node* answer;
  struct stu_sch_node* leader;


  k = chart[i-1][j-1]->stuno;
  l = chart[i-1][j-1]->schno;
  answer = create_stu_sch_node(k, l);
  while (k != i || l != j) {

    if (chart[k-1][l-1] == NULL) {
      printf("Something is wrong with the chart at (k,l) = (%i,%i).\n", k, l);
      exit(0);
    }
    
    g = chart[k-1][l-1]->stuno;
    h = chart[k-1][l-1]->schno;
    leader = create_stu_sch_node(g, h);
    leader->next = answer;
    answer = leader;
    k = g;
    l = h;
  }

  return answer;
}

void adjust_partial_alloc_along_cycle(struct partial_alloc* alloc, struct stu_sch_node* cycle,
				      struct stu_sch_node** removal_list) {
  double Delta;
  struct stu_sch_node* probe;
  struct stu_sch_node* base;
  struct stu_sch_node* new;

  Delta = get_entry(alloc, cycle->stuno, cycle->schno);

  probe = cycle->next;
  while (probe != NULL) {
    Delta = min(Delta, get_entry(alloc, probe->stuno, probe->schno));
    probe = probe->next;
  }
  
  probe = cycle;
  while (probe != NULL) {
    increment_entry(alloc, probe->stuno, probe->schno, -Delta);
    probe = probe->next;
  }

  probe = cycle;
  while (probe->next != NULL) {
    increment_entry(alloc, probe->stuno, probe->next->schno, Delta);
    probe = probe->next;
  }
  increment_entry(alloc, probe->stuno, cycle->schno, Delta);

  probe = cycle;
  while (probe != NULL) {
    if (get_entry(alloc, probe->stuno, probe->schno) < Delta + 0.000001) {
      if (*removal_list == NULL) {
	base = create_stu_sch_node(probe->stuno,probe->schno);
	*removal_list = base;
      }
      else {
	new = create_stu_sch_node(probe->stuno,probe->schno);
	base->next = new;
	base = new;
      }
    }
    probe = probe->next;
  }
}

void NEW_adjust_partial_alloc_along_cycle(struct partial_alloc* alloc,
					  struct stu_sch_node* cycle) {
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

/*
void print_graph(struct stu_sch_node*** graph, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (graph[i-1][j-1] != NULL) {
	printf("(%i,%i): ", i, j);
	print_stu_sch_list(graph[i-1][j-1]);
	printf("\n");
      }
    }
  }
}

void print_stu_sch_list(struct stu_sch_node* node) {
  struct stu_sch_node* probe;

  probe = node;
  
  while (probe != NULL) {
    printf("(%i,%i)",probe->stuno,probe->schno);
    probe = probe->next;
  }
}
*/

/*
int envygr_and_enviedgr_are_consistent(struct stu_sch_node*** envygr,
					     struct stu_sch_node*** enviedgr,
					     int nst, int nsc) {
  int i, j;
  struct stu_sch_node* probe;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      probe = envygr[i-1][j-1];
      while (probe != NULL) {
	if (!stu_sch_list_contains_pair(enviedgr[probe->stuno-1][probe->schno-1], i, j)) {

	  printf("We failed to find (%i,%i) in enviedgr[%i-1][%i-1].\n", i, j, probe->stuno,
		 probe->schno);
	  
	  return 0;
	}
	probe = probe->next;
      }
    }
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      probe = enviedgr[i-1][j-1];
      while (probe != NULL) {
	if (!stu_sch_list_contains_pair(envygr[probe->stuno-1][probe->schno-1], i, j)) {

	  printf("We failed to find (%i,%i) in envygr[%i-1][%i-1].\n", i, j, probe->stuno,
		 probe->schno);
	  
	  return 0;
	}
	probe = probe->next;
      }
    }
  }

  return 1;
}
*/

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

void reduce_accepting_students(struct partial_alloc* myalloc, struct process_scp* myscp,
			       struct lists_of_students* accepting_students,
			       struct stu_sch_node* removal_list) {
  int i, j, k, l, r, need_to_adjust;
  
  struct stu_sch_node* probe;

  probe = removal_list;
  while (probe != NULL) {
    i = probe->stuno;
    j = probe->schno;
    r = student_ranking_of_school(myscp, i, j);
    need_to_adjust = 1;
    for (k = r + 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      if (get_entry(myalloc, i, myscp->preferences[i-1][k-1]) > 0.00001) {
	need_to_adjust = 0;
      }
    }
    if (need_to_adjust) {
      k = r;
      while (need_to_adjust) {
	l = myscp->preferences[i-1][k-1];
	remove_student_from_list(&(accepting_students->lists[l-1]), i);
	if (get_entry(myalloc, i, l) > 0.00001) {
	  need_to_adjust = 0;
	}
	k--;
      }
    }
    probe = probe->next;
  }
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

void NEW_process_cycles_at_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
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

      /*
      printf("With i = %i and j = %i, the cycle is", i, j);
      print_stu_sch_list(cycle);
      printf("\n");
      */
      
      *must_repeat = 1;
      
      NEW_adjust_partial_alloc_along_cycle(myalloc, cycle);

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

struct partial_alloc NEW_EMCC_allocation(struct process_scp* myscp) {
  int i, j, nst, nsc;

  int* must_repeat;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  int* coarse;
  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  struct process_scp red_scp;

  struct lists_of_students accepting_students;
  
  struct partial_alloc alloc_to_adjust;

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
	  NEW_process_cycles_at_pair(&alloc_to_adjust, &red_scp, &accepting_students, i, j,
				     must_repeat);
	}
      }
    }
  }

  if (!allocation_is_efficient(&alloc_to_adjust, &red_scp)) {
    fprintf(stderr, "The emcc allocation is inefficient.\n");
  }
  else {
    fprintf(stderr, "The emcc allocation is efficient.\n");
  }

  free(must_repeat);
  free(coarse);
  destroy_process_scp(red_scp);
  destroy_lists_of_students(&accepting_students);

  return alloc_to_adjust;
}
