#include "purifycode.h"

int graph_has_a_leaf(struct nonintegral_graph* graph) {
  int i, j;

  for (i = 1; i <= graph->no_students; i++) {
    if (graph->stu_no_nbrs[i-1] == 1) {

      fprintf(stderr, "The graph has a student leaf.\n");
      
      return 1;
    }
  }

  for (j = 1; j <= graph->no_schools; j++) {
    if (graph->sch_no_nbrs[j-1] == 1) {

      fprintf(stderr, "The graph has a school leaf.\n");
      
      return 1;
    }
  }

  if (graph->sink_no_nbrs == 1) {

    fprintf(stderr, "The graph has a sink leaf.\n");
      
    return 1;
  }

  return 0;
}

struct pure_alloc random_pure_allocation(struct partial_alloc* my_alloc) {

  transform_to_random_floating_point_pure_allocation(my_alloc);

  return pure_allocation_from_partial(my_alloc);
}

void transform_to_random_floating_point_pure_allocation(struct partial_alloc* my_alloc) {
  int up, down;
  
  up = 1;
  down = 0;

  double alpha;
  double beta;
  double uniform_rv;

  struct path_node* cycle;
  double* sch_sums;
  struct nonintegral_graph my_graph;
  
  sch_sums = school_sums(my_alloc);
  my_graph = graph_from_alloc(my_alloc);

  while(graph_is_nonempty(&my_graph)) {
    
    cycle = find_cyclic_path(&my_graph);

    alpha = bound_of_cycle(my_alloc,sch_sums,up,cycle);
    beta  = bound_of_cycle(my_alloc,sch_sums,down,cycle);
    uniform_rv = uniform();

    if (uniform_rv <= alpha/(alpha + beta)) {
      cycle_adjustment(my_alloc, sch_sums, &my_graph, up, alpha, cycle);
    }
    else {
      cycle_adjustment(my_alloc, sch_sums, &my_graph, down, beta, cycle);	
    }
      
    destroy_cycle(cycle);       
  }

  free(sch_sums);
  destroy_nonintegral_graph(&my_graph);
}

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc) {
  int i, j, cursor;
  int nst = my_alloc->no_students;
  int nsc = my_alloc->no_schools;

  double* sch_sums;

  sch_sums = school_sums(my_alloc);

  struct nonintegral_graph my_graph;
  my_graph.no_students = nst;
  my_graph.no_schools  = nsc;

  /* First we fill in the edges */
  
  my_graph.stu_sch_edges = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    my_graph.stu_sch_edges[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (!is_integer(get_entry(my_alloc, i, j))) {
	my_graph.stu_sch_edges[i-1][j-1] = 1;
      }
      else {
	my_graph.stu_sch_edges[i-1][j-1] = 0;
      }
    }
  }

  my_graph.sch_sink_edges = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    if (!is_integer(sch_sums[j-1])) {
      my_graph.sch_sink_edges[j-1] = 1;
    }
    else {
      my_graph.sch_sink_edges[j-1] = 0;
    }
  }

  my_graph.stu_no_nbrs = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    my_graph.stu_no_nbrs[i-1] = 0;
    for (j = 1; j <= nsc; j++) {
      if (my_graph.stu_sch_edges[i-1][j-1]) {
	my_graph.stu_no_nbrs[i-1]++;
      }
    }
  }

  my_graph.sch_no_nbrs = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    my_graph.sch_no_nbrs[j-1] = 0;
    if (my_graph.sch_sink_edges[j-1]) {
      my_graph.sch_no_nbrs[j-1]++;
    }
    for (i = 1; i <= nst; i++) {
      if (my_graph.stu_sch_edges[i-1][j-1]) {
	my_graph.sch_no_nbrs[j-1]++;
      }
    }
  }

  my_graph.sink_no_nbrs = 0;
  for (j = 1; j <= nsc; j++) {
    if (my_graph.sch_sink_edges[j-1]) {
      my_graph.sink_no_nbrs++;
    }
  }
    
  
  my_graph.stu_nbrs = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    my_graph.stu_nbrs[i-1] = malloc((1 + my_graph.stu_no_nbrs[i-1]) * sizeof(int));
    my_graph.stu_nbrs[i-1][0] = 0;
    cursor = 0;
    for (j = 1; j <= nsc; j++) {
      if (my_graph.stu_sch_edges[i-1][j-1]) {
	cursor++;
	my_graph.stu_nbrs[i-1][cursor] = j;
      }
    }
  }
  
  my_graph.sch_nbrs = malloc(nsc * sizeof(int*));
  for (j = 1; j <= nsc; j++) {
    my_graph.sch_nbrs[j-1] = malloc((1 + my_graph.sch_no_nbrs[j-1]) * sizeof(int));
    my_graph.sch_nbrs[j-1][0] = 0;
    cursor = 0;
    if (my_graph.sch_sink_edges[j-1]) {
      cursor++;
      my_graph.sch_nbrs[j-1][cursor] = 0;
    }
    for (i = 1; i <= nst; i++) {
      if (my_graph.stu_sch_edges[i-1][j-1]) {
	cursor++;
	my_graph.sch_nbrs[j-1][cursor] = i;
      }
    }
  }
 
  my_graph.sink_nbrs = malloc((1 + my_graph.sink_no_nbrs) * sizeof(int));
  my_graph.sink_nbrs[0] = 0;
  cursor = 0;
  for (j = 1; j <= nsc; j++) {
    if (my_graph.sch_sink_edges[j-1]) {
      cursor++;
      my_graph.sink_nbrs[cursor] = j;
    }
  }

  free(sch_sums);

  return my_graph;
}

int graph_is_nonempty(struct nonintegral_graph* my_graph) {
  int j;

  for (j = 1; j <= my_graph->no_schools; j++) {
    if (my_graph->sch_no_nbrs[j-1] > 0) {
      return 1;
    }
  }
  
  return 0;
}

struct path_node* find_cyclic_path(struct nonintegral_graph* my_graph) {
  int i, j, done;

  struct path_node* start;
  struct path_node* first_step; 
  struct path_node* previous; 
  struct path_node* current; 
  struct path_node* new_node; 
  struct path_node* probe; 

  start = malloc(sizeof(struct path_node));
  start->type = 1;
  i = 1;
  while (my_graph->stu_no_nbrs[i-1] == 0) {
    i++;
  }
  start->index = i;
  
  first_step = malloc(sizeof(struct path_node));
  first_step->type = 2;
  j = 1;
  while (my_graph->stu_sch_edges[i-1][j-1] == 0) {
    j++;
  }
  first_step->index = j;	
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
      if (my_graph->stu_nbrs[current->index-1][1] == previous->index) {
	new_node->index = my_graph->stu_nbrs[current->index-1][2];
      }
      else {
	new_node->index = my_graph->stu_nbrs[current->index-1][1];
      }
    }
    
    if (current->type == 2) {
      if (my_graph->sch_nbrs[current->index-1][1] == previous->index) {
	new_node->index = my_graph->sch_nbrs[current->index-1][2];
      }
      else {
	new_node->index = my_graph->sch_nbrs[current->index-1][1];
      }
      if (new_node->index == 0) {
	new_node->type = 3;
      }
      else {
	new_node->type = 1;
      } 
    }
        
    if (current->type == 3) {
      new_node->type = 2;
      if (my_graph->sink_nbrs[1] == previous->index) {
	new_node->index = my_graph->sink_nbrs[2];
      }
      else {
	new_node->index = my_graph->sink_nbrs[1];
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
	gap = 1.0 - get_entry(my_alloc, probe->index, (probe->next)->index);;
      }
      else {
	gap = get_entry(my_alloc, probe->index, (probe->next)->index);
      }
    }
    
    if (probe->type == 2) {
      if ((probe->next)->type == 1) {
	if (up) {
	  gap = get_entry(my_alloc, (probe->next)->index, probe->index);
	}
	else {
	  gap = 1.0 - get_entry(my_alloc, (probe->next)->index, probe->index);
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

void cycle_adjustment_of_allocation(struct partial_alloc* my_alloc, double* sch_sums, int up,
				    double adjustment, struct path_node* my_cycle) {
  int done;
  struct path_node* probe;
  
  probe = my_cycle;
  done = 0;
  while (!done) {
    if (probe->type == 1) {
      if (up) {
	increment_entry(my_alloc, probe->index, (probe->next)->index, adjustment);
	sch_sums[(probe->next)->index-1] += adjustment;
      }
      else {
	increment_entry(my_alloc, probe->index, (probe->next)->index, -adjustment);
	sch_sums[(probe->next)->index-1] -= adjustment;
      }
    }
    
    if (probe->type == 2) {
      if ((probe->next)->type == 1) {
	if (up) {
	  increment_entry(my_alloc, (probe->next)->index, probe->index, -adjustment);
	  sch_sums[probe->index-1] -= adjustment;
	}
	else {
	  increment_entry(my_alloc, (probe->next)->index, probe->index, adjustment);
	  sch_sums[probe->index-1] += adjustment;
	}
      }
    }
    
    if (probe->next == my_cycle) {
      done = 1;
    }
    
    probe = probe->next;
  }
}

void cycle_adjustment_of_graph(struct partial_alloc* my_alloc, double* sch_sums,
			       struct nonintegral_graph* my_graph, struct path_node* my_cycle) {  
  int done;
  
  struct path_node* probe;
  
  probe = my_cycle;
  done = 0;
  while (!done) {
    if (probe->type == 1) {
      if (is_integer(get_entry(my_alloc, probe->index, (probe->next)->index))) {
	student_edge_removal(my_graph, probe->index, (probe->next)->index);
      }
    }
    
    if (probe->type == 2) {
      if ((probe->next)->type == 1) {
	if (is_integer(get_entry(my_alloc, (probe->next)->index, probe->index))) {
	  student_edge_removal(my_graph, (probe->next)->index, probe->index); 
	}
      }
      else {
	if (is_integer(sch_sums[probe->index-1])) {
	  sink_edge_removal(my_graph, probe->index);
	}
      }
    }
    
    if (probe->type == 3) {
      if (is_integer(sch_sums[(probe->next)->index-1])) {
	sink_edge_removal(my_graph, (probe->next)->index);
      }
    }
    
    if (probe->next == my_cycle) {
      done = 1;
    }
    
    probe = probe->next;
  }
}

void cycle_adjustment(struct partial_alloc* my_alloc, double* sch_sums,
			  struct nonintegral_graph* my_graph, int up,
		      double adjustment, struct path_node* my_cycle) {
    
  cycle_adjustment_of_allocation(my_alloc, sch_sums, up, adjustment, my_cycle);

  cycle_adjustment_of_graph(my_alloc, sch_sums, my_graph, my_cycle);
}

void student_edge_removal(struct nonintegral_graph* my_graph, int i, int j) {
  my_graph->stu_sch_edges[i-1][j-1] = 0;

  my_graph->stu_nbrs[i-1] = list_with_element_removed(my_graph->stu_nbrs[i-1],
						      my_graph->stu_no_nbrs[i-1], j);

  my_graph->sch_nbrs[j-1] = list_with_element_removed(my_graph->sch_nbrs[j-1],
						      my_graph->sch_no_nbrs[j-1], i);

  my_graph->stu_no_nbrs[i-1]--;
  my_graph->sch_no_nbrs[j-1]--;
}

void sink_edge_removal(struct nonintegral_graph* my_graph, int j) {
  my_graph->sch_sink_edges[j-1] = 0;

  my_graph->sch_nbrs[j-1] = list_with_element_removed(my_graph->sch_nbrs[j-1],
						      my_graph->sch_no_nbrs[j-1], 0);

  my_graph->sink_nbrs = list_with_element_removed(my_graph->sink_nbrs,
						  my_graph->sink_no_nbrs, j);

  my_graph->sch_no_nbrs[j-1]--;
  my_graph->sink_no_nbrs--;
}

int* list_with_element_removed(int* old_list, int old_no_elements, int elt) {
  int k, cursor;
  
  int* new_list = malloc(old_no_elements * sizeof(int));
  new_list[0] = 0;
  cursor = 0;
  for (k = 1; k <= old_no_elements; k++) {
    if (old_list[k] != elt) {
      cursor++;
      new_list[cursor] = old_list[k];
    }
  }

  free(old_list);
  return(new_list);
} 

void destroy_nonintegral_graph(struct nonintegral_graph* my_graph) {
  int i, j;

  for (i = 1; i <= my_graph->no_students; i++) {
    free(my_graph->stu_sch_edges[i-1]);
  }
  free(my_graph->stu_sch_edges);
  free(my_graph->sch_sink_edges);
  free(my_graph->stu_no_nbrs);
  free(my_graph->sch_no_nbrs);
  for (i = 1; i <= my_graph->no_students; i++) {
    free(my_graph->stu_nbrs[i-1]);
  }
  free(my_graph->stu_nbrs);
  for (j = 1; j <= my_graph->no_schools; j++) {
    free(my_graph->sch_nbrs[j-1]);
  }
  free(my_graph->sch_nbrs);
  free(my_graph->sink_nbrs);
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

int alloc_and_sch_sums_are_consistent(double* sch_sums, struct partial_alloc* my_alloc) {
  int i, j, nst, nsc;

  double sum;

  nst = my_alloc->no_students;
  nsc = my_alloc->no_schools;

  for (j = 1; j <= nsc; j++) {
    sum = 0.0;
    for (i = 1; i <= nst; i++) {
      sum += get_entry(my_alloc, i, j);
    }
    if (fabs(sum - sch_sums[j-1]) > 0.000001) {
      printf("School %i is inconsistent.\n", j);
      return 0;
    } 
  }

  return 1;
}

int graph_and_alloc_are_consistent(struct nonintegral_graph* my_graph, double* sch_sums,
				   struct partial_alloc* my_alloc) {
  int i, j, nst, nsc, count;

  nst = my_alloc->no_students;
  nsc = my_alloc->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if ((is_integer(get_entry(my_alloc, i, j)) && my_graph->stu_sch_edges[i-1][j-1] == 1)
	  || (!is_integer(get_entry(my_alloc, i, j)) && my_graph->stu_sch_edges[i-1][j-1] == 0)) {
	printf("Student %i and school %i are inconsistent.\n", i, j);
	     return 0;
      }
    } 
  }
  
  for (j = 1; j <= nsc; j++) {
    if (is_integer(sch_sums[j-1]) && my_graph->sch_sink_edges[j-1] == 1) {
      printf("School %i has an integral sch_sums but is nonintegral in the graph.\n", j);
      return 0;
    }
    if (!is_integer(sch_sums[j-1]) && my_graph->sch_sink_edges[j-1] == 0) {
      printf("School %i has a nonintegral sch_sums but is integral in the graph.\n", j);
      return 0;
    }
  }

  count = 0;
  for (j = 1; j <= nsc; j++) {
    if (!is_integer(sch_sums[j-1])) {
      count++;
    }
  }
  if (my_graph->sink_no_nbrs != count) {
    printf("We have my_graph->sink_no_nbrs out of line with sch_sums.\n");
    return 0;
  }

  return 1;
}
