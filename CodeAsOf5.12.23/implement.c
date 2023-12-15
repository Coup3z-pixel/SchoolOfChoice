#include "implement.h"

void destroy_nonintegral_graph(struct nonintegral_graph* my_graph) {
  int i;

  for (i = 1; i <= my_graph->no_students; i++) {
    free(my_graph->stu_sch_edges[i-1]);
  }
  free(my_graph->stu_sch_edges);
  free(my_graph->sch_sink_edges);
}

void destroy_neighbor_lists(struct neighbor_lists* my_lists) {
  int i,j;

  free(my_lists->stu_no_nbrs);
  free(my_lists->sch_no_nbrs);

  for (i = 1; i <= my_lists->no_students; i++) {
    free(my_lists->stu_sch_nbrs[i-1]);
  }
  free(my_lists->stu_sch_nbrs);

  for (j = 1; j <= my_lists->no_schools; j++) {
    free(my_lists->sch_stu_nbrs[j-1]);
  }
  free(my_lists->sch_stu_nbrs);

  free(my_lists->sch_sink_nbrs);

  free(my_lists->sink_sch_nbrs);
}

void destroy_cycle(struct path_node* cycle) {
  struct path_node* trailer = cycle;
  struct path_node* leader = cycle->next;

  while (leader != cycle) {
    free(trailer);
    trailer = leader;
    leader = leader->next;
  }
  
  free(trailer); 
}

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc, double* sch_sums) {
  int i, j;
  int nst = my_alloc->no_students;
  int nsc = my_alloc->no_schools;

  struct nonintegral_graph my_graph;
  my_graph.no_students = nst;
  my_graph.no_schools  = nsc;
  my_graph.no_edges = 0;

  /* First we fill in the edges */
  
  my_graph.stu_sch_edges = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    my_graph.stu_sch_edges[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (my_alloc->allocations[i-1][j-1] > 0.00001 && my_alloc->allocations[i-1][j-1] < 0.99999) {
	my_graph.stu_sch_edges[i-1][j-1] = 1;
	my_graph.no_edges++;
      }
      else {
	my_graph.stu_sch_edges[i-1][j-1] = 0;
      }
    }
  }

  my_graph.sch_sink_edges = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    if (floor(sch_sums[j-1]) + 0.00001 < sch_sums[j-1] &&
	sch_sums[j-1] < ceil(sch_sums[j-1]) - 0.00001) {
      my_graph.sch_sink_edges[j-1] = 1;
	my_graph.no_edges++;
    }
    else {
      my_graph.sch_sink_edges[j-1] = 0;
    }
  }

  return my_graph;
}

struct neighbor_lists neighbor_lists_from_graph(struct nonintegral_graph* my_graph) {
  int i,j,nbr_index;
  int nst = my_graph->no_students;
  int nsc = my_graph->no_schools;
  
  struct neighbor_lists lists;
  lists.no_students = nst;
  lists.no_schools = nsc;
  
  lists.stu_no_nbrs = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    lists.stu_no_nbrs[i-1] = 0;
    for (j = 1; j <= nsc; j++) {
      if (my_graph->stu_sch_edges[i-1][j-1]) {
	lists.stu_no_nbrs[i-1]++;
      }
    }
  }
  
  lists.sch_no_nbrs = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    lists.sch_no_nbrs[j-1] = 0;
    for (i = 1; i <= nst; i++) {
      if (my_graph->stu_sch_edges[i-1][j-1]) {
	lists.sch_no_nbrs[j-1]++;
      }
    }
  }

  lists.sink_no_nbrs = 0;
  for (j = 1; j <= nsc; j++) {
    if (my_graph->sch_sink_edges[j-1]) {
      lists.sink_no_nbrs++;
    }
  }
  
  lists.stu_sch_nbrs = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    lists.stu_sch_nbrs[i-1] = malloc((1 + lists.stu_no_nbrs[i-1]) * sizeof(int));
    lists.stu_sch_nbrs[i-1][0] = 0;
    nbr_index = 0;
    for (j = 1; j <= nsc; j++) {
      if (my_graph->stu_sch_edges[i-1][j-1]) {
	nbr_index++;
	lists.stu_sch_nbrs[i-1][nbr_index] = j;
      }
    }
  }
  
  lists.sch_stu_nbrs = malloc(nsc * sizeof(int*));
  for (j = 1; j <= nsc; j++) {
    lists.sch_stu_nbrs[j-1] = malloc((1 + lists.sch_no_nbrs[j-1]) * sizeof(int));
    lists.sch_stu_nbrs[j-1][0] = 0;
    nbr_index = 0;
    for (i = 1; i <= nst; i++) {
      if (my_graph->stu_sch_edges[i-1][j-1]) {
	nbr_index++;
	lists.sch_stu_nbrs[j-1][nbr_index] = i;
      }
    }
  }

  lists.sch_sink_nbrs = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++)  {
    if (my_graph->sch_sink_edges[j-1]) {
      lists.sch_sink_nbrs[j-1] = 1;
    }
    else  {
      lists.sch_sink_nbrs[j-1] = 0;
    }
  }

  lists.sink_sch_nbrs = malloc((1 + lists.sink_no_nbrs) * sizeof(int));
  lists.sink_sch_nbrs[0] = 0;
  nbr_index = 0;
  for (j = 1; j <= nsc; j++) {
    if (my_graph->sch_sink_edges[j-1]) {
      nbr_index++;
      lists.sink_sch_nbrs[nbr_index] = j;
    }
  }

  return lists;
}

int graph_is_nonempty(struct neighbor_lists* my_lists) {
  int i;

  for (i = 1; i <= my_lists->no_students; i++) {
    if (my_lists->stu_no_nbrs[i-1] > 0) {
      return 1;
    }
  }

  return 0;
}

struct path_node* find_cyclic_path(struct neighbor_lists* my_lists) {
  int i,done;

  struct path_node* start;
  struct path_node* first_step; 
  struct path_node* previous; 
  struct path_node* current; 
  struct path_node* new_node; 
  struct path_node* probe; 
  
  start = malloc(sizeof(struct path_node));
  start->type = 1;
  i = 1;
  while (my_lists->stu_no_nbrs[i-1] == 0) {
    i++;
  }
  start->index = i;
  
  first_step = malloc(sizeof(struct path_node));
  first_step->type = 2;
  first_step->index = my_lists->stu_sch_nbrs[i-1][1];

	if (first_step->index == 0) {
	  printf("I mean, just crap!\n");
	}
	
  first_step->next = NULL;
  start->next = first_step;

  previous = start;
  current = first_step;

  done = 0;
  while (!done) {
    new_node = malloc(sizeof(struct path_node));
    new_node->next = NULL;
    
    if (current->type == 1) {
      new_node->type = 2;
      if (my_lists->stu_sch_nbrs[current->index-1][1] == previous->index) {
	new_node->index = my_lists->stu_sch_nbrs[current->index-1][2];
      }
      else  {
	new_node->index = my_lists->stu_sch_nbrs[current->index-1][1];
      }
    }
    
    if (current->type == 2) {
      if (previous->type == 3) { 
	new_node->type = 1;
	new_node->index = my_lists->sch_stu_nbrs[current->index - 1][1];
      }
      else { /* now previous->type == 1 */
	if (my_lists->sch_sink_nbrs[current->index - 1] == 1) {
	  new_node->type = 3;
	  new_node->index = 1;
	}
	else {
	  new_node->type = 1;
	  if (previous->index == my_lists->sch_stu_nbrs[current->index - 1][1]) {
	    new_node->index = my_lists->sch_stu_nbrs[current->index - 1][2];

	  }
	  else {
	    new_node->index = my_lists->sch_stu_nbrs[current->index - 1][1];
	  }
	}
      }
    }
        
    if (current->type == 3) {
      new_node->type = 2;
      if (my_lists->sink_sch_nbrs[1] == previous->index) {
	new_node->index = my_lists->sink_sch_nbrs[2];
      }
      else  {
	new_node->index = my_lists->sink_sch_nbrs[1];
      }
    }

    current->next = new_node;
    previous = current;
    current = new_node;

    probe = first_step;
    while (probe->next != NULL && !done) {
      if (probe->type != current->type || probe->index != current->index) {
	probe = probe->next;
      }
      else {
	if (probe->next != NULL) {
	  done = 1;
	  previous->next = probe;
	  free(current);
	  while (start != probe) {
	    free(start);
	    start = first_step;
	    first_step = first_step->next;
	  }
	}
      }
    }
  }

  return probe;
}

double bound_of_cycle(struct partial_alloc* my_alloc, double* sch_sums, int up,
			 struct path_node* my_cycle) {
  double max = 1.0;
  struct path_node* probe = my_cycle;

  int done = 0;
  while (!done) {
    double gap;
    
    if (probe->type == 1) {
      if (up) {
	gap = 1.0 - my_alloc->allocations[probe->index-1][(probe->next)->index-1];
      }
      else {
	gap = my_alloc->allocations[probe->index-1][(probe->next)->index-1];
      }
    }
    
    if (probe->type == 2) {
      if ((probe->next)->type == 1) {
	if (up) {
	  gap = my_alloc->allocations[(probe->next)->index-1][probe->index-1];
	}
	else {
	  gap = 1.0 - my_alloc->allocations[(probe->next)->index-1][probe->index-1];
	}
      }
      else {
	if (up) {
	  gap = ceil(sch_sums[probe->index-1]) - sch_sums[probe->index-1];
	}
	else {
	  gap = sch_sums[probe->index-1] - floor(sch_sums[probe->index-1]);
	}
      }
    }
    
    if (probe->type == 3) {
      if (up) {
	gap = ceil(sch_sums[(probe->next)->index-1]) - sch_sums[(probe->next)->index-1];
      }
      else {
	gap = sch_sums[(probe->next)->index-1] - floor(sch_sums[(probe->next)->index-1]);
      }
    }
    
    max = min(gap,max);
    if (probe->next == my_cycle) {
      done = 1;
    }
    probe = probe->next;
  }
  
  return max;
}

void cycle_adjustment(struct partial_alloc* my_alloc, double* sch_sums,
		      struct neighbor_lists* my_lists, int up,
		      double adjustment, struct path_node* my_cycle) {
  struct path_node* probe = my_cycle;

  int done = 0;
  while (!done) {
    if (probe->type == 1) {
      if (up) {
	my_alloc->allocations[probe->index-1][(probe->next)->index-1] += adjustment;
      }
      else {
	my_alloc->allocations[probe->index-1][(probe->next)->index-1] -= adjustment;
      }
      
      if (is_integer(my_alloc->allocations[probe->index-1][(probe->next)->index-1])) {

	student_edge_removal(my_lists,probe->index,(probe->next)->index);
      }
    }
    
    if (probe->type == 2) {
      if ((probe->next)->type == 1) {
	if (up) {
	  my_alloc->allocations[(probe->next)->index-1][probe->index-1] -= adjustment;
	}
	else {
	  my_alloc->allocations[(probe->next)->index-1][probe->index-1] += adjustment;
	}
	if (is_integer(my_alloc->allocations[(probe->next)->index-1][probe->index-1])) {
	  student_edge_removal(my_lists,(probe->next)->index,probe->index); 
	}
      }
      else {
	if (up) {
	  sch_sums[probe->index-1] += adjustment;
	}
	else {
	  sch_sums[probe->index-1] -= adjustment;
	}
	if (is_integer(sch_sums[probe->index-1])) {
	  sink_edge_removal(my_lists,probe->index);
	}
      }
    }
    
    if (probe->type == 3) {
      if (up) {
	sch_sums[(probe->next)->index-1] -= adjustment;	
      }
      else {
	sch_sums[(probe->next)->index-1] += adjustment;		
      }
      if (is_integer(sch_sums[(probe->next)->index-1])) {
	sink_edge_removal(my_lists,(probe->next)->index);
      }
    }
    
    if (probe->next == my_cycle) {
      done = 1;
    }
    probe = probe->next;
  }

}

void student_edge_removal(struct neighbor_lists* my_lists, int i, int j) {
  int k, cursor;

  my_lists->stu_no_nbrs[i-1]--;
  my_lists->sch_no_nbrs[j-1]--;

  int* new_list_for_stu = malloc((1 + my_lists->stu_no_nbrs[i-1]) * sizeof(int));

  new_list_for_stu[0] = 0;
  cursor = 0;
  for (k = 1; k <= my_lists->stu_no_nbrs[i-1] + 1; k++) {
    if (my_lists->stu_sch_nbrs[i-1][k] != j) {
      cursor++;
      new_list_for_stu[cursor] = my_lists->stu_sch_nbrs[i-1][k];
    }
  }
  free(my_lists->stu_sch_nbrs[i-1]);
  my_lists->stu_sch_nbrs[i-1] = new_list_for_stu;
  
  int* new_list_for_sch = malloc((1 + my_lists->sch_no_nbrs[j-1]) * sizeof(int));

  new_list_for_sch[0] = 0;
  cursor = 0;
  for (k = 1; k <= my_lists->sch_no_nbrs[j-1] + 1; k++) {
    if (my_lists->sch_stu_nbrs[j-1][k] != i) {
      cursor++;
      new_list_for_sch[cursor] = my_lists->sch_stu_nbrs[j-1][k];
    }
  }
  free(my_lists->sch_stu_nbrs[j-1]);
  my_lists->sch_stu_nbrs[j-1] = new_list_for_sch;
}

void sink_edge_removal(struct neighbor_lists* my_lists, int j) {
  int k, cursor;
  
  my_lists->sink_no_nbrs--;
  my_lists->sch_sink_nbrs[j-1] = 0;

  int* new_list_for_sink = malloc((1 + my_lists->sink_no_nbrs) * sizeof(int));

  new_list_for_sink[0] = 0; 
  
  cursor = 0;
  
  for (k = 1; k <= my_lists->sink_no_nbrs + 1; k++) {
    if (my_lists->sink_sch_nbrs[k] != j) {
      cursor++;
      new_list_for_sink[cursor] = my_lists->sink_sch_nbrs[k];
    }
  }
  free(my_lists->sink_sch_nbrs);
  my_lists->sink_sch_nbrs = new_list_for_sink;  
}

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc) {
  int i, j;
  int nst = my_alloc->no_students;
  int nsc = my_alloc->no_schools;
  
  struct pure_alloc my_pure;
  my_pure.no_students = nst;
  my_pure.no_schools = nsc;
  my_pure.allocations = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    my_pure.allocations[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (my_alloc->allocations[i-1][j-1] > 0.99999) {
	my_pure.allocations[i-1][j-1] = 1;
      }
      else {
	my_pure.allocations[i-1][j-1] = 0;
      }
    }
  }
  
  return my_pure;
} 

struct pure_alloc random_pure_allocation(struct partial_alloc* my_alloc) {
  int i;
  int up = 1;
  int down = 0;

  int nst = my_alloc->no_students;

  struct path_node* cycle;
  
  double* sch_sums = school_sums(my_alloc);
  struct nonintegral_graph graph = graph_from_alloc(my_alloc,sch_sums);
  struct neighbor_lists nbr_lists = neighbor_lists_from_graph(&graph);

  int done = 0;
  while(!done) {
    done = 1;
    for (i = 1; i <= nst; i++) {
      if (nbr_lists.stu_no_nbrs[i-1] > 0) {
	done = 0;
      }
    }
    if (!done) {
      cycle = find_cyclic_path(&nbr_lists);

      double alpha = bound_of_cycle(my_alloc,sch_sums,up,cycle);
      double beta  = bound_of_cycle(my_alloc,sch_sums,down,cycle);
      double uniform_rv = uniform();

      if (uniform_rv <= alpha/(alpha + beta)) {
	cycle_adjustment(my_alloc,sch_sums,&nbr_lists,up,alpha,cycle);
      }
      else {
	cycle_adjustment(my_alloc,sch_sums,&nbr_lists,down,beta,cycle);	
      }
      
      destroy_cycle(cycle); 
    }
  }

  destroy_neighbor_lists(&nbr_lists);
  destroy_nonintegral_graph(&graph);
  free(sch_sums);

  return pure_allocation_from_partial(my_alloc);
}
