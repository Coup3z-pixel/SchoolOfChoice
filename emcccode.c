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

int** get_active_pairs(struct process_scp* myscp, struct partial_alloc* alloc_to_adjust) {
  int i, j, nst, nsc;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  int** answer = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (alloc_to_adjust->allocations[i-1][j-1] > 0.00001 && j != myscp->preferences[i-1][0]) {
	answer[i-1][j-1] = 1;
      }
      else {
	answer[i-1][j-1] = 0;
      }
    }
  }

  return answer;
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

  active = get_active_pairs(myscp, alloc_to_adjust);

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

struct stu_sch_node*** get_envied_graph(struct stu_sch_node*** envygr, int nst, int nsc) {
  int i, j, k, l;
  struct stu_sch_node* probe;

  struct stu_sch_node*** enviedgr = malloc(nst * sizeof(struct stu_sch_node**));
  for (i = 1; i <= nst; i++) {
    enviedgr[i-1] = malloc(nsc * sizeof(struct stu_sch_node*));
    for (j = 1; j <= nsc; j++) {
      enviedgr[i-1][j-1] = NULL;
    }
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      probe = envygr[i-1][j-1];
      while (probe != NULL) {
	k = probe->stuno;
	l = probe->schno;
	append_node_to_stu_sch_list(&(enviedgr[k-1][l-1]), i, j);
	probe = probe->next;
      }
    }
  }
  
  return enviedgr;
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

	if (all_nodes_reached_tip == NULL) {
	  printf("Somehow after initialization all_nodes_reached_tip is NULL.\n");
	  exit(0);
	}
      }
      
      else {

	if (all_nodes_reached != NULL && all_nodes_reached_tip == NULL) {
	  printf("Somehow all_nodes_reached_tip is NULL.\n");
	  exit(0);
	}
	
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

struct stu_sch_node* get_new_layer(struct stu_sch_node*** envygr, struct stu_sch_node*** chart,
				   struct stu_sch_node* last_layer, int* found_cycle, int i, int j) {
  int g, h, k, l;
  struct stu_sch_node* new_layer;
  struct stu_sch_node* new_layer_tip;
  struct stu_sch_node* probe;
  struct stu_sch_node* subprobe;
  
  new_layer = NULL;
  new_layer_tip = NULL;
  probe = last_layer;
  while (probe != NULL) {
    g = probe->stuno;
    h = probe->schno;
    subprobe = envygr[g-1][h-1];
    while (subprobe != NULL) {
     k = subprobe->stuno;
      l = subprobe->schno;
      if (k == i && l == j) {
	*found_cycle = 1;
      }
      if (chart[k-1][l-1] == NULL) {	
	chart[k-1][l-1] = create_stu_sch_node(g, h);
	if (new_layer == NULL) {
	  new_layer = create_stu_sch_node(k, l);
	  new_layer_tip = new_layer;
	}
	else {
	  new_layer_tip->next = create_stu_sch_node(k, l);
	  new_layer_tip = new_layer_tip->next;
	}
      }
      subprobe = subprobe->next;
    }
    probe = probe->next;
  }

  return new_layer;
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

void clean_up_chart_and_nodes(struct stu_sch_node*** chart, struct stu_sch_node* found_list) {
  int g, h;
  struct stu_sch_node* probe;

  probe = found_list;
  while (probe != NULL) {
    g = probe->stuno;
    h = probe->schno;
    free(chart[g-1][h-1]);
    chart[g-1][h-1] = NULL;
    probe = probe->next;
  }

  destroy_stu_sch_list(found_list);
}

void remove_pair_from_list(struct stu_sch_node** list, int k, int l) {
  if (!stu_sch_list_contains_pair(*list, k, l)) {
    printf("We are trying to remove something that isn't there.\n");
    exit(0);
  }
  
  struct stu_sch_node* probe;
  struct stu_sch_node* preprobe;

  probe = *list;
  
  preprobe = *list;
  while (probe->stuno != k || probe->schno != l) {
    
    probe = probe->next;
    
    if (preprobe->next != probe) {
      preprobe = preprobe->next;
    }
  }

  if (probe != *list) {
    preprobe->next = probe->next;
  }
  else {
    *list = probe->next;
  }
  free(probe);
}

void remove_pair_from_envy_and_envied_graphs(struct stu_sch_node*** envygr,
 					     struct stu_sch_node*** enviedgr, int k, int l) {
  struct stu_sch_node* probe;
  struct stu_sch_node* yprobe;
  struct stu_sch_node* iedprobe;

  yprobe = copy_of_list(envygr[k-1][l-1]);
  iedprobe = copy_of_list(enviedgr[k-1][l-1]);

  probe = yprobe;
  while (probe != NULL) {
    remove_pair_from_list(&(enviedgr[probe->stuno-1][probe->schno-1]), k, l);
    probe = probe->next;
  }

  probe = iedprobe;
  probe = enviedgr[k-1][l-1];
  while (probe != NULL) {
    remove_pair_from_list(&(envygr[probe->stuno-1][probe->schno-1]), k, l);
    probe = probe->next;
  }

  destroy_stu_sch_list(yprobe);
  destroy_stu_sch_list(iedprobe);

  destroy_stu_sch_list(envygr[k-1][l-1]);
  destroy_stu_sch_list(enviedgr[k-1][l-1]);
  envygr[k-1][l-1] = NULL;
  enviedgr[k-1][l-1] = NULL;
}

struct stu_sch_node* create_stu_sch_node(int i, int j) {
  struct stu_sch_node* new_node;
  
  new_node = malloc(sizeof(struct stu_sch_node));
  new_node->stuno = i;
  new_node->schno = j;
  new_node->next = NULL;

  return new_node;
}

int stu_sch_list_contains_pair(struct stu_sch_node* list, int i, int j) {
  struct stu_sch_node* probe = list;
  
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
    *stu_sch = create_stu_sch_node(i, j);
  }
  else {
    if (!stu_sch_list_contains_pair(*stu_sch, i, j)) {
      probe = *stu_sch;
      while (probe->next != NULL) {
	probe = probe->next;
      }
      probe->next = create_stu_sch_node(i, j);
    }
  }
}

struct stu_sch_node* copy_of_list(struct stu_sch_node* given) {
  struct stu_sch_node* answer;
  struct stu_sch_node* reader;
  struct stu_sch_node* constructor;
  struct stu_sch_node* new_node;

  if (given == NULL) {
    answer = NULL;
  }
  else {
    answer = create_stu_sch_node(given->stuno,given->schno);
    reader = given;
    constructor = answer;
    while (reader->next != NULL) {
      reader = reader->next;
      new_node = create_stu_sch_node(reader->stuno,reader->schno);
      constructor->next = new_node;
      constructor = constructor->next;
    }
  }

  return answer;
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

void destroy_matrix_of_lists(struct stu_sch_node*** envygr, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      destroy_stu_sch_list(envygr[i-1][j-1]);
    }
  }

  for (i = 1; i <= nst; i++) {
    free(envygr[i-1]);
  }
  free(envygr);
}
