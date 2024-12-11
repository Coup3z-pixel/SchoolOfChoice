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
	  if (alloc_to_adjust->allocations[i-1][l-1] <
	      myscp->eligible[i-1][l-1] * myscp->time_remaining - 0.000001 &&
	      coarse[l-1] <= myscp->priorities[i-1][l-1]) {
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

      /*      fprintf(stderr, "We actually found an inefficiency!\n"); */
      
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
				      struct stu_sch_node** list) {
  double Delta;
  struct stu_sch_node* probe;
  struct stu_sch_node* base;
  struct stu_sch_node* new;

  Delta = alloc->allocations[cycle->stuno-1][cycle->schno-1];

  probe = cycle->next;
  while (probe != NULL) {
    Delta = min(Delta, alloc->allocations[probe->stuno-1][probe->schno-1]);
    probe = probe->next;
  }

  probe = cycle;
  while (probe != NULL) {
    if (alloc->allocations[probe->stuno-1][probe->schno-1] < Delta + 0.000001) {
      if (*list == NULL) {
	base = create_stu_sch_node(probe->stuno,probe->schno);
	*list = base;
      }
      else {
	new = create_stu_sch_node(probe->stuno,probe->schno);
	base->next = new;
	base = new;
      }
    }
    probe = probe->next;
  }
  
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
