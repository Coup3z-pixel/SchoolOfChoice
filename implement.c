#include "implement.h"

void destroy_nonintegral_graph(struct nonintegral_graph* my_graph) {
  int i, j;
  struct list_node* swap;

  while (my_graph->sink_nbrs.next != NULL) {
    swap = my_graph->sink_nbrs.next;
    free(my_graph->sink_nbrs.next);
    my_graph->sink_nbrs.next = swap;
  }

  for (j = 1; j <= my_graph->no_schools; j++) {
    while (my_graph->sch_nbrs[j-1].next != NULL) {
      swap = my_graph->sch_nbrs[j-1].next;
      free(my_graph->sch_nbrs[j-1].next);
      my_graph->sch_nbrs[j-1].next = swap;
    }
  }

  for (i = 1; i <= my_graph->no_students; i++) {
    while (my_graph->stu_nbrs[i-1].next != NULL) {
      swap = my_graph->stu_nbrs[i-1].next;
      free(my_graph->stu_nbrs[i-1].next);
      my_graph->stu_nbrs[i-1].next = swap;
    }
  }

  free(my_graph->sch_sink_edges);

  for (i = 1; i <= my_graph->no_students; i++) {
    free(my_graph->stu_sch_edges[i-1]);
  }
  free(my_graph->stu_sch_edges);
}

void initialize_list_node(struct list_node my_node, int type, int index) {
  my_node.is_student = 0;
  my_node.is_school = 0;
  my_node.is_sink = 0;
  my_node.index = index;
  my_node.next = NULL;
  if (type == 1) {
    my_node.is_student = 1;
  }
  if (type == 2) {
    my_node.is_school = 1;
  }
  if (type == 3) {
    my_node.is_sink = 1;
  }
}

struct nonintegral_graph graph_from_alloc(struct partial_alloc* my_alloc, double* sch_sums) {
  int i, j;
  int no_stu = my_alloc->no_students;
  int no_sch = my_alloc->no_schools;

  struct nonintegral_graph my_graph;
  my_graph.no_students = no_stu;
  my_graph.no_schools  = no_sch;
  my_graph.no_edges = 0;

  /* First we fill in the edges */
  
  my_graph.stu_sch_edges = malloc(no_stu * sizeof(int*));
  for (i = 1; i <= no_stu; i++) {
    my_graph.stu_sch_edges[i-1] = malloc(no_sch * sizeof(int));
    for (j = 1; j <= no_sch; j++) {
      if (my_alloc->allocations[i-1][j-1] > 0.00001 && my_alloc->allocations[i-1][j-1] < 0.99999) {
	my_graph.stu_sch_edges[i-1][j-1] = 1;
	my_graph.no_edges++;
      }
      else {
	my_graph.stu_sch_edges[i-1][j-1] = 0;
      }
    }
  }

  my_graph.sch_sink_edges = malloc(no_sch * sizeof(int));
  for (j = 1; j <= no_sch; j++) {
    if (floor(sch_sums[j-1]) + 0.00001 < sch_sums[j-1] &&
	sch_sums[j-1] < ceil(sch_sums[j-1]) - 0.00001) {
      my_graph.sch_sink_edges[j-1] = 1;
	my_graph.no_edges++;
    }
    else {
      my_graph.sch_sink_edges[j-1] = 0;
    }
  }

  /* Now we fill in the lists of neighbors */
   
  my_graph.stu_nbrs = malloc(no_stu * sizeof(struct list_node));
  for (i = 1; i <= no_stu; i++) {
    initialize_list_node(my_graph.stu_nbrs[i-1],1,i);

    struct list_node current = my_graph.stu_nbrs[i-1];
    for (j = 1; j <= no_sch; j++) {
      if (my_graph.sch_sink_edges[j-1] == 1) {
	current.next = malloc(sizeof(struct list_node));
	initialize_list_node(*(current.next),2,j);
	current = *current.next;
      }
    }
  }
   
  my_graph.sch_nbrs = malloc(no_sch * sizeof(struct list_node));
  for (j = 1; j <= no_sch; j++) {
    initialize_list_node(my_graph.sch_nbrs[j-1],2,j);

    struct list_node current = my_graph.sch_nbrs[j-1];
    for (i = 1; i <= no_stu; i++) {
      if (my_graph.stu_sch_edges[i-1][j-1] == 1) {
	current.next = malloc(sizeof(struct list_node));
	initialize_list_node(*(current.next),1,i);
	current = *current.next;
      }
    }
    if (my_graph.sch_sink_edges[j-1] == 1) {
      current.next = malloc(sizeof(struct list_node));
      initialize_list_node(*(current.next),3,1);
    }
  }

  initialize_list_node(my_graph.sink_nbrs,3,1);
  struct list_node current = my_graph.sink_nbrs;
  for (j = 1; j <= no_sch; j++) {
    if (my_graph.sch_sink_edges[j-1] == 1) {
      current.next = malloc(sizeof(struct list_node));
      initialize_list_node(*(current.next),2,j);
      current = *current.next;
    }
  }

  return my_graph;
}

void remove_student_edge(struct nonintegral_graph* my_graph, int i, int j) {
  my_graph->stu_sch_edges[i-1][j-1] = 0;

  /* We now find j in the list of i's neighbors and remove it. */
  
  struct list_node* probe = &my_graph->stu_nbrs[i-1];
  while ((probe->next)->index != j) {
    probe = probe->next;
  }
  struct list_node* target = probe->next;
  probe->next = target->next;
  free(target);

  /* We now find j in the list of i's neighbors and remove it. */
  
  probe = &my_graph->sch_nbrs[j-1];
  while ((probe->next)->index != i) {
    probe = probe->next;
  }
  target = probe->next;
  probe->next = target->next;
  free(target);
}

void remove_sink_edge(struct nonintegral_graph* my_graph, int j) {
  my_graph->sch_sink_edges[j-1] = 0;

  /* We now remove the sink from the list of j's neighbors. */
  
  struct list_node* probe = &my_graph->sch_nbrs[j-1];
  while ((probe->next)->next != NULL) {
    probe = probe->next;  /* If j has a sink neighbor, it is last on the list */
  }
  free(probe->next);
  probe->next = NULL;

  /* We now remove j from the list of the sink's neighbors. */
  
  probe = &my_graph->sink_nbrs;
  while ((probe->next)->index != j) {
    probe = probe->next;  
  }
  struct list_node* target = probe->next;
  probe->next = target->next;
  free(target);
}

void copy_list_node_data(struct list_node* copy,struct list_node* orig) {
  copy->is_student = orig->is_student;
  copy->is_school = orig->is_school;
  copy->is_sink = orig->is_sink;
  copy->index = orig->index;
}

int same_data(struct list_node* first,struct list_node* second) {
  if (first->is_student == second->is_student &&
      first->is_student == second->is_student &&
      first->is_student == second->is_student &&
      first->is_student == second->is_student) {
    return 1;
  }
  else {
    return 0;
  }
}

struct list_node* correct_tmp_ptr(struct list_node* tmp_ptr,struct list_node* anc_ptr) {
  if ((tmp_ptr->is_student && anc_ptr->is_student && tmp_ptr->index == anc_ptr->index) ||
      (tmp_ptr->is_school && anc_ptr->is_school && tmp_ptr->index == anc_ptr->index) ||
      (tmp_ptr->is_sink && anc_ptr->is_sink)) {
    tmp_ptr = tmp_ptr->next;
  }
  return tmp_ptr;
}

struct list_node* get_new_tmp_ptr(struct nonintegral_graph* my_graph, struct list_node* tmp_ptr) {
  if (tmp_ptr->is_student) {
    tmp_ptr = my_graph->stu_nbrs[tmp_ptr->index-1].next;
  }
  if (tmp_ptr->is_school) {
    tmp_ptr = my_graph->sch_nbrs[tmp_ptr->index-1].next;
  }
  if (tmp_ptr->is_sink) {
    tmp_ptr = my_graph->sink_nbrs.next;
  }
  
  return tmp_ptr;
}

int find_cycle(struct nonintegral_graph* my_graph, struct list_node* cycle_node) {
  int i;
  for (i = 1; i <= my_graph->no_students; i++) {
    if (my_graph->stu_nbrs[i-1].next != NULL) {
      struct list_node* first_node;
      struct list_node* old_node;
      struct list_node* new_node;
      struct list_node* tmp_ptr;
      
      first_node = malloc(sizeof(struct list_node));
      initialize_list_node(*first_node,1,i);
      
      new_node = malloc(sizeof(struct list_node));
      first_node->next = new_node;
      old_node = first_node;
      
      tmp_ptr = my_graph->stu_nbrs[i-1].next;
      copy_list_node_data(new_node,tmp_ptr);
      
      int done = 0;
      while (!done) {
	struct list_node* ancient_node = old_node;
	old_node = new_node;
	new_node = malloc(sizeof(struct list_node));
	old_node->next = new_node;
	
	tmp_ptr = correct_tmp_ptr(tmp_ptr,ancient_node);
	copy_list_node_data(new_node,tmp_ptr);
	tmp_ptr = get_new_tmp_ptr(my_graph,tmp_ptr);
	
	struct list_node* search_node = first_node;
	while (!same_data(search_node,new_node) && search_node->next != old_node->next) {
	  search_node = search_node->next;
	}
	if (same_data(search_node,new_node)) {
	  done = 1;
	  new_node->next = search_node;
	  cycle_node = search_node; /* There is now a cycle starting at this node */

	  ancient_node = first_node; /* Free nodes not in cycle */
	  old_node = first_node->next;
	  while (old_node != search_node) {
	    free(ancient_node);
	    ancient_node = old_node;
	    old_node = old_node->next;
	  }
	  free(ancient_node);
	}
      }

      return 1;
    }
  }

  return 0;
}

double cycle_bound(struct partial_alloc* my_alloc, double* sch_sums, int up,
			 struct list_node* my_cycle) {
  double max = 1.0;
  struct list_node* probe = my_cycle;

  int done = 0;
  while (!done) {
    double gap;
    
    if (probe->is_student) {
      if (up) {
	gap = 1.0 - my_alloc->allocations[probe->index-1][(probe->next)->index-1];
      }
      else {
	gap = my_alloc->allocations[probe->index-1][(probe->next)->index-1];
      }
    }
    
    if (probe->is_school) {
      if ((probe->next)->is_student) {
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
    
    if (probe->is_sink) {
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

void adjust_cycle(struct nonintegral_graph* my_graph, struct partial_alloc* my_alloc,
		  double* sch_sums, int up, double adjustment, struct list_node* my_cycle) {
  struct list_node* probe = my_cycle;

  int done = 0;
  while (!done) {
    double gap;
    
    if (probe->is_student) {
      if (up) {
	my_alloc->allocations[probe->index-1][(probe->next)->index-1] += adjustment;
      }
      else {
	my_alloc->allocations[probe->index-1][(probe->next)->index-1] -= adjustment;
      }
      if (is_integer(my_alloc->allocations[probe->index-1][(probe->next)->index-1])) {
	remove_student_edge(my_graph,probe->index,(probe->next)->index);
      }
    }
    
    if (probe->is_school) {
      if ((probe->next)->is_student) {
	if (up) {
	  my_alloc->allocations[(probe->next)->index-1][probe->index-1] += adjustment;
	}
	else {
	  my_alloc->allocations[(probe->next)->index-1][probe->index-1] -= adjustment;
	}
	if (is_integer(my_alloc->allocations[(probe->next)->index-1][probe->index-1])) {
	  remove_student_edge(my_graph,(probe->next)->index,probe->index);
	}
      }
      else {
	if (up) {
	  sch_sums[probe->index-1] += adjustment;
	}
	else {
	  sch_sums[probe->index-1] -= adjustment;
	}
      }
      if (is_integer(sch_sums[probe->index-1])) {
	  remove_sink_edge(my_graph,probe->index);
      }
    }
    
    if (probe->is_sink) {
      if (up) {
	sch_sums[(probe->next)->index-1] += adjustment;	
      }
      else {
	sch_sums[(probe->next)->index-1] -= adjustment;		
      }
      if (is_integer(sch_sums[(probe->next)->index-1])) {
	remove_sink_edge(my_graph,(probe->next)->index);
      }
    }
    
    if (probe->next == my_cycle) {
      done = 1;
    }
    probe = probe->next;
  }
}

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc) {
  int i, j;
  int no_stu = my_alloc->no_students;
  int no_sch = my_alloc->no_schools;
  
  struct pure_alloc my_pure;
  my_pure.no_students = no_stu;
  my_pure.no_schools = no_sch;
  for (i = 1; i <= no_stu; i++) {
    for (j = 1; j <= no_sch; j++) {
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
  int up = 1;
  int down = 0;
  
  double* sch_sums = school_sums(my_alloc);
  struct list_node* cycle_node;
  struct nonintegral_graph the_graph = graph_from_alloc(my_alloc,sch_sums);

  int stu_index = 1;
  while (stu_index <= my_alloc->no_students) {
    while (the_graph.stu_nbrs[stu_index-1].next == NULL) {
      stu_index++;
    }
    if (stu_index <= my_alloc->no_students) {
      while (the_graph.stu_nbrs[stu_index-1].next != NULL) {
	cycle_node = the_graph.stu_nbrs[stu_index-1].next;
	find_cycle(&the_graph,cycle_node);
	double alpha = cycle_bound(my_alloc,sch_sums,up,cycle_node);
	double beta  = cycle_bound(my_alloc,sch_sums,down,cycle_node);
	double uniform_rv = uniform();
	if (uniform_rv <= alpha/(alpha + beta)) {
	  adjust_cycle(&the_graph,my_alloc,sch_sums,up,alpha,cycle_node);
	}
	else {
	  adjust_cycle(&the_graph,my_alloc,sch_sums,down,beta,cycle_node);
	}
      }
    }
    stu_index++;
  }

  free(sch_sums);
  destroy_nonintegral_graph(&the_graph);

  return pure_allocation_from_partial(my_alloc);
}
