#include "dgraph.h"

graph_node* new_graph_node(int i, int j) {
  graph_node* answer;

  answer = malloc(sizeof(graph_node));

  answer->stu = i;
  answer->sch = j;
  answer->sources = NULL;
  answer->targets = NULL;

  return answer;
}

graph_node* copy_of_graph_node(graph_node* given) {
  graph_node* answer;

  if (given == NULL) {
    answer = NULL;
  }
  else {
    answer = malloc(sizeof(graph_node));
    answer->stu = given->stu;
    answer->sch = given->sch;
    answer->sources = copy_of_stu_sch_pair_list(given->sources);
    answer->targets = copy_of_stu_sch_pair_list(given->targets);
  }

  return answer;
}

graph_node_list_node* copy_of_graph_node_list_node(graph_node_list_node* given) {
  graph_node_list_node* answer;
  graph_node_list_node* cursor;

  if (given == NULL) {
    answer = NULL;
  }
  else {
    answer = malloc(sizeof(graph_node_list_node));
    answer->node = copy_of_graph_node(given->node);
    answer->next = NULL;
    cursor = answer;
    while (given->next != NULL) {
      given = given->next;
      cursor->next = malloc(sizeof(graph_node_list_node));
      cursor = cursor->next;
      cursor->next = NULL;
      cursor->node = copy_of_graph_node(given->node);
    }
  }

  return answer;
}

/********************/

int number_of_nodes(dgraph* graph) {
  int answer;
  
  graph_node_list_node* cursor;

  if (graph == NULL) {
    return 0;
  }

  answer = 0;

  while (graph != NULL) {
    cursor = graph->listptr;
    while (cursor != NULL) {
      answer++;
      
      cursor = cursor->next;
    }

    graph = graph->next;
  }

  return answer;
}

stu_sch_pair_list_node* node_of_number(dgraph* graph, int node_no) {
  int counter;

  stu_sch_pair_list_node* answer;
  
  graph_node_list_node* cursor;

  counter = 0;

  while (graph != NULL) {
    cursor = graph->listptr;
    while (cursor != NULL) {
      counter++;
      if (counter == node_no) {
	answer = malloc(sizeof(stu_sch_pair_list_node));
	answer->stu = cursor->node->stu;
	answer->sch = cursor->node->sch;
	answer->next = NULL;
      }
      
      cursor = cursor->next;
    }

    graph = graph->next;
  }

  return answer;
}

int graph_contains_stu_sch_pair(dgraph* graph, int stu, int sch) {
  graph_node_list_node* cursor;

  while (graph != NULL) {
    cursor = graph->listptr;
    while (cursor != NULL) {
      if (cursor->node->stu == stu && cursor->node->sch == sch) {
	return 1;
      }

      cursor = cursor->next;
    }

    graph = graph->next;
  }

  return 0;
}  

graph_node* node_of_pair(dgraph* graph, int stu, int sch) {
  graph_node_list_node* cursor;

  while (graph != NULL) {
    cursor = graph->listptr;
    while (cursor != NULL) {
      if (cursor->node->stu == stu && cursor->node->sch == sch) {
	return cursor->node;
      }

      cursor = cursor->next;
    }

    graph = graph->next;
  }

  return NULL;
}

int next_student(dgraph* loop, int stu, int sch) {
  graph_node* my_node;

  my_node = node_of_pair(loop, stu, sch);

  return my_node->targets->stu;
}

int next_school(dgraph* loop, int stu, int sch) {
  graph_node* my_node;

  my_node = node_of_pair(loop, stu, sch);

  return my_node->targets->stu;
}

int graph_contains_node(dgraph* graph, graph_node* node) {
  int found;

  graph_node_list_node* listptr;

  if (graph == NULL) {
    found = 0;
  }
  else {
    found = 0;
    while (graph != NULL && !found) {
      listptr = graph->listptr;
      while (listptr != NULL && !found) {
	if (listptr->node->stu == node->stu && listptr->node->sch == node->sch) {
	  found = 1;
	}
	listptr = listptr->next;
      }
      
      graph = graph->next;
    }
  }

  return found;
}

int all_arcs_are_in_loops(dgraph* graph) {
  graph_node_list_node* subprobe;
  
    while (graph != NULL) {
      
      subprobe = graph->listptr;
      while (subprobe != NULL) {

	if (subprobe->node->sources == NULL || subprobe->node->targets == NULL) {
	  return 0;
	}

	subprobe = subprobe->next;
      }

      graph = graph->next;
    }

    return 1;
}

int has_only_loops(dgraph* graph) {
  
  graph_node_list_node* listptr;

  while (graph != NULL) {
    listptr = graph->listptr;
    while (listptr != NULL) {
      if (length_of_list(listptr->node->sources) != 1 ||
	  length_of_list(listptr->node->targets) != 1) {

	return 0;
      }
      
      listptr = listptr->next;
    }

    graph = graph->next;
  }

  return 1;
}

int is_a_loop(dgraph* graph) {
  int first_stu, first_sch, current_stu, current_sch, next_stu, next_sch, no_nodes;

  no_nodes = number_of_nodes(graph);

  if (no_nodes == 0){
    return 0;
  }

  if (!has_only_loops(graph)) {
    return 0;
  }

  first_stu = graph->listptr->node->stu;
  first_sch = graph->listptr->node->sch;

  current_stu = first_stu;
  current_sch = first_sch;

  no_nodes--;
  while (no_nodes > 0) {
    next_stu = next_student(graph, current_stu, current_sch);
    next_sch = next_school(graph, current_stu, current_sch);
    if (next_stu == first_stu && next_sch == first_sch) {
      return 0;
    }
    current_stu = next_stu;
    current_sch = next_sch;
    no_nodes--;
  }

  return 1;
}

stu_sch_pair_list_node* random_node(dgraph* graph) {
  int no_nodes, node_no, done;
  double unif_rv;

  no_nodes = number_of_nodes(graph);

  unif_rv = uniform();

  done = 0;
  node_no = 1;
  while (!done) {
    if ((node_no - 1)/no_nodes <= unif_rv && node_no/no_nodes >= unif_rv) {
      done = 1;
    }
    else {
      node_no++;
    }
  }

  return node_of_number(graph, node_no);
}

stu_sch_pair_list_node* random_loop(dgraph* graph) {
  int done, no_cand, cand_no;

  graph_node* tip_node;
  
  stu_sch_pair_list_node* initial_node;
  stu_sch_pair_list_node* tip;
  stu_sch_pair_list_node* answer;
  stu_sch_pair_list_node* next_target;

  initial_node = random_node(graph);
  tip = initial_node;

  done = 0;
  while (!done) {
    tip_node = node_of_pair(graph, tip->stu, tip->sch);
    no_cand = length_of_list(tip_node->targets);
    cand_no = random_integer_in_one_to_max(no_cand);
    
    next_target = copy_of_list_element_n(tip_node->targets, cand_no);
    if (list_contains_pair(initial_node, next_target->stu, next_target->sch)) {
      done = 1;
    }
    tip->next = next_target;
    tip = tip->next;
  }

  answer = initial_node;
  while (!(answer->stu == tip->stu && answer->sch == tip->sch)) {
    answer = answer->next;
    free(initial_node);
    initial_node = answer;
  }

  answer = answer->next;
  free(initial_node);

  return answer;
}

void report_on_the_directed_graph(dgraph* graph) {
  int i, j, new, swap, school_count, both_count;

  int* index;
  
  dgraph* probe;
  graph_node_list_node* subprobe;

  stu_sch_pair_list_node* targ;

  if (!all_arcs_are_in_loops(graph)) {
    fprintf(stderr, "A bad one entering rotdg.\n");
    exit(0);
  }
  
  school_count = 0;
  both_count = 0;

  probe = graph;
  while (probe != NULL) {
    school_count++;

    if (probe->listptr != NULL) {
      subprobe = probe->listptr;
      while (subprobe != NULL) {
	if (subprobe->node->sources != NULL && subprobe->node->targets != NULL) {
	  both_count++;
	}

	subprobe = subprobe->next;
      }
    }

    probe = probe->next;
  }
  fprintf(stderr, "There are %i active schools and %i nodes.\n\n",
	  school_count, both_count);

  probe = graph;
  index = malloc((school_count + 1) * sizeof(int));
  
  while (probe != NULL) {
    subprobe = probe->listptr;
    while (subprobe != NULL) {
      targ = subprobe->node->targets;
      fprintf(stderr, "(%i,%i) |-> ", subprobe->node->stu, subprobe->node->sch);
      for (i = 0; i <= school_count; i++) {
	index[i] = 0;
      }
      while (targ != NULL) {
	new = 1;
	for (j = 1; j <= index[0]; j++) {
	  if (targ->sch == index[j]) {
	    new = 0;
	  }
	}
	if (new) {
	  index[0]++;
	  index[index[0]] = targ->sch;
	}
	targ = targ->next;
      }
      
      i = 1;
      while (i < index[0]) {
	if (index[i] > index[i+1]) {
	  swap = index[i];
	  index[i] = index[i+1];
	  index[i+1] = swap;
	}
	else {
	  i++;
	}
      }
      
      for (i = 1; i <= index[0]; i++) {
	fprintf(stderr, "%i", index[i]);
	if (i < index[0]) {
	  fprintf(stderr, ", ");
	}
      }
      fprintf(stderr, "\n");

      subprobe = subprobe->next;
    }
    fprintf(stderr, "\n");

    probe = probe->next;
  }
  free(index);
}

/*******************************/

dgraph* subgraph_of_listed_pairs(dgraph* graph, stu_sch_pair_list_node* list_of_pairs) {
  dgraph* answer;
  dgraph* cursor1;
  graph_node_list_node* cursor2;
  
  answer = copy_of_graph(graph);
  cursor1 = answer;
  while (cursor1 != NULL) {
    cursor2 = cursor1->listptr;
    while (cursor2 != NULL) {
      if (!list_contains_pair(list_of_pairs, cursor2->node->stu, cursor2->node->sch)) {
	destroy_stu_sch_pair_list(&(cursor2->node->sources));
	destroy_stu_sch_pair_list(&(cursor2->node->targets));
      }

      cursor2 = cursor2->next;
    }
    cursor1 = cursor1->next;
  }

  remove_nodes_with_no_arcs(&answer);

  return answer;
}

/****************************/

stu_sch_pair_list_node* pairs_connected_to_pair(dgraph* graph, int stu, int sch) {
  int done;
  
  stu_sch_pair_list_node* pair_list;
  
  stu_sch_pair_list_node* cursor;
  stu_sch_pair_list_node* cursor1; 
  stu_sch_pair_list_node* cursor2;
  
  graph_node* pairs_node;

  pair_list = malloc(sizeof(stu_sch_pair_list_node));
  pair_list->stu = stu;
  pair_list->sch = sch;
  pair_list->next = NULL;
			
  done = 0;
  while (!done) {
    done = 1;
    cursor = pair_list;
    while (cursor != NULL) {
      pairs_node = node_of_pair(graph, cursor->stu, cursor->sch);

      cursor1 = pairs_node->sources;
      while (cursor1 != NULL) {
	if (!list_contains_pair(pair_list, cursor1->stu, cursor1->sch)) {
	  done = 0;
	  add_pair_to_stu_sch_pair_list(&pair_list, cursor1->stu, cursor1->sch);
	}
	
	cursor1 = cursor1->next;		   
      }
      
      cursor2 = pairs_node->targets;
      while (cursor2 != NULL) {
	if (!list_contains_pair(pair_list, cursor2->stu, cursor2->sch)) {
	  done = 0;
	  add_pair_to_stu_sch_pair_list(&pair_list, cursor2->stu, cursor2->sch);
	}
	
	cursor2 = cursor2->next;		   
      }

      cursor = cursor->next;
    } 
  }

  return pair_list;
}

dgraph* component_of_pair(dgraph*
							graph, int stu, int sch) {
  dgraph* answer;
  stu_sch_pair_list_node* list_of_pairs;

  list_of_pairs = pairs_connected_to_pair(graph, stu, sch);

  answer = subgraph_of_listed_pairs(graph, list_of_pairs);

  destroy_stu_sch_pair_list(&list_of_pairs);

  return answer;
}

list_of_dgraphs* components(dgraph* graph) {
  int started;
  
  list_of_dgraphs* answer;
  list_of_dgraphs* component_cursor;

  
  stu_sch_pair_list_node* all_nodes;
  stu_sch_pair_list_node* current_nodes;
  stu_sch_pair_list_node* node_cursor;

  started = 0;
  answer = NULL;
  
  all_nodes = nodes_in_graph(graph);
  
  current_nodes = NULL;
  node_cursor = all_nodes;
  while (node_cursor != NULL) {
    if (!list_contains_pair(current_nodes, node_cursor->stu, node_cursor->sch)) {
      if (!started) {	
	answer = malloc(sizeof(list_of_dgraphs));
	answer->next = NULL;
	answer->component = component_of_pair(graph, node_cursor->stu, node_cursor->sch);

	if (!all_arcs_are_in_loops(answer->component)) {
	  fprintf(stderr, "We found a bad one in the first component.\n");
	  exit(0);
	}

	current_nodes = nodes_in_graph(answer->component);
	component_cursor = answer;
	started = 1;
      }
      else {
	component_cursor->next = malloc(sizeof(list_of_dgraphs));
	component_cursor = component_cursor->next;
	component_cursor->next = NULL;
	component_cursor->component = component_of_pair(graph, node_cursor->stu, node_cursor->sch);

	if (!all_arcs_are_in_loops(component_cursor->component)) {
	  fprintf(stderr, "We found a bad one.\n");
	  exit(0);
	}
	
	current_nodes = concatenate(current_nodes, nodes_in_graph(component_cursor->component));
      }
    }

    node_cursor = node_cursor->next;
  }

  destroy_stu_sch_pair_list(&all_nodes);
  destroy_stu_sch_pair_list(&current_nodes);

  return answer;
}

int number_of_components(list_of_dgraphs* graph) {
  int count;

  count = 0;
  while (graph != NULL) {
    count++;
    graph = graph->next;
  }

  return count;
} 

void report_on_the_components(dgraph* graph) {
  int no_comp, comp_no;
  
  list_of_dgraphs* list_of_components;
  list_of_dgraphs* cursor;

  list_of_components = components(graph);
  no_comp = number_of_components(list_of_components);
  if (no_comp == 1) {
    fprintf(stderr, "\nThere is 1 component:\n");
  }
  else  {
    fprintf(stderr, "\nThere are %i components:\n", no_comp);
  }
  
  cursor = list_of_components;

  comp_no = 0;
  while (cursor != NULL) {
    comp_no++;
    fprintf(stderr, "\nComponent %i:\n", comp_no);
    report_on_the_directed_graph(cursor->component);
    cursor = cursor->next;
  }

  destroy_list_of_dgraphs(list_of_components);
}

/***********************************/

stu_sch_pair_list_node* indirect_desire_set_of_pair(dgraph* graph,
							   int stu, int sch) {
  int done;
  graph_node* pairs_node;
  
  stu_sch_pair_list_node* answer; 
  stu_sch_pair_list_node* cursor; 
  stu_sch_pair_list_node* cursor1;

  answer = malloc(sizeof(stu_sch_pair_list_node));
  answer->stu = stu;
  answer->sch = sch;
  answer->next = NULL;

  done = 0;
  while (!done) {
    done = 1;
    cursor = answer;
    while (cursor != NULL) {
      pairs_node = node_of_pair(graph, cursor->stu, cursor->sch);
      cursor1 = pairs_node->targets;
      while (cursor1 != NULL) {
	if (!list_contains_pair(answer, cursor1->stu, cursor1->sch)) {
	  done = 0;
	  add_pair_to_stu_sch_pair_list(&answer, cursor1->stu, cursor1->sch);
	}
	
	cursor1 = cursor1->next;		   
      }
      
      cursor = cursor->next;
    }
  }

  return answer;
}

stu_sch_pair_list_node* a_minimum_trading_set_list(dgraph* graph) {
  int stu, sch, done;

  stu_sch_pair_list_node* answer;
  stu_sch_pair_list_node* candidate;
  stu_sch_pair_list_node* cursor;

  if (graph == NULL) {
    return NULL;
  }

  stu = graph->listptr->node->stu;
  sch = graph->listptr->node->sch;
  
  answer = indirect_desire_set_of_pair(graph, stu, sch);

  done = 0;
  while (!done) {
    done = 1;
    cursor = answer;
    while (cursor != NULL) {
      candidate = indirect_desire_set_of_pair(graph, cursor->stu, cursor->sch);
      if (is_a_proper_subset(candidate, answer)) {
	done = 0;
	destroy_stu_sch_pair_list(&answer);
	answer = candidate;
      }
      
      cursor = cursor->next;	   
    }
  }

  return answer;
}

dgraph* a_minimum_trading_graph(dgraph* graph) {
  stu_sch_pair_list_node* node_list;
  dgraph* answer;
  
  node_list = a_minimum_trading_set_list(graph);
  answer = subgraph_of_stu_sch_list(graph, node_list);

  return answer;
}

list_of_dgraphs* trading_set_decomposition(dgraph* graph) {
  dgraph* copy_of_dgraph;
  list_of_dgraphs* answer;
  list_of_dgraphs* ans_copy;
  stu_sch_pair_list_node* nodes_in_component;

  copy_of_dgraph = copy_of_graph(graph);
  
  if (copy_of_dgraph == NULL) {
    return NULL;
  }

  answer = malloc(sizeof(list_of_dgraphs));
  answer->component = a_minimum_trading_graph(copy_of_dgraph);
  answer->next = NULL;
  nodes_in_component = nodes_in_graph(answer->component);
  remove_nodes_of_list_from_graph(&copy_of_dgraph, nodes_in_component);
  destroy_stu_sch_pair_list(&nodes_in_component);
  
  ans_copy = answer;
  while (copy_of_dgraph != NULL) {
    ans_copy->next = malloc(sizeof(list_of_dgraphs));
    ans_copy = ans_copy->next;
    ans_copy->component = a_minimum_trading_graph(copy_of_dgraph);
    ans_copy->next = NULL;
    nodes_in_component = nodes_in_graph(ans_copy->component);
    remove_nodes_of_list_from_graph(&copy_of_dgraph, nodes_in_component);
    destroy_stu_sch_pair_list(&nodes_in_component);
  }

  return answer;
}

void report_on_trade_set_decomp(dgraph* graph)  {
  int no_comp, comp_no;
  
  list_of_dgraphs* list_of_components;
  list_of_dgraphs* cursor;

  list_of_components = components(graph);
  no_comp = number_of_components(list_of_components);
  if (no_comp == 1) {
    fprintf(stderr, "There is 1 trading set:\n\n");
  }
  else  {
    fprintf(stderr, "There are %i trading sets:\n\n", no_comp);
  }
  
  cursor = list_of_components;

  comp_no = 0;
  while (cursor != NULL) {
    comp_no++;
    fprintf(stderr, "\nTrading Set %i:\n", comp_no);
    report_on_the_directed_graph(cursor->component);
    cursor = cursor->next;
  }

  destroy_list_of_dgraphs(list_of_components);
}

/******************************/

dgraph* copy_of_graph(dgraph* given) {
  int started;
  
  dgraph* answer;
  dgraph* cursor;

  if (given == NULL) {
    answer = NULL;
  }
  else {
    started = 0;
    while (given != NULL) {
      if (!started) {
	answer = malloc(sizeof(dgraph));
	answer->listptr = copy_of_graph_node_list_node(given->listptr);
	answer->next = NULL;
	cursor = answer;
	started = 1;
	given = given->next;
      }
      else {
	cursor->next  = malloc(sizeof(dgraph));
	cursor = cursor->next;
	cursor->listptr = copy_of_graph_node_list_node(given->listptr);
	cursor->next = NULL;
	given = given->next;
      }
    }
  }

  return answer;
}

dgraph* subgraph_of_stu_sch_list(dgraph* given, stu_sch_pair_list_node* list)
{
  dgraph* answer;
  stu_sch_pair_list_node* list_of_all_nodes;
  stu_sch_pair_list_node* copy_of_list;
  
  answer = copy_of_graph(given);
  list_of_all_nodes = nodes_in_graph(answer);
  copy_of_list = list_of_all_nodes;
  while (copy_of_list != NULL) {
    if (!list_contains_pair(list, copy_of_list->stu, copy_of_list->sch)) {
      remove_node_of_pair_from_graph(&answer, copy_of_list->stu, copy_of_list->sch);
    }
    copy_of_list = copy_of_list->next;
  }

  destroy_stu_sch_pair_list(&list_of_all_nodes);

  return answer;
}

dgraph* subgraph_of_pairs_outside_stu_sch_list(dgraph* given,
						      stu_sch_pair_list_node* list) {
  dgraph* answer;
  stu_sch_pair_list_node* list_of_all_nodes;
  stu_sch_pair_list_node* copy_of_list;
  
  answer = copy_of_graph(given);
  list_of_all_nodes = nodes_in_graph(answer);
  copy_of_list = list_of_all_nodes;
  while (copy_of_list != NULL) {
    if (list_contains_pair(list, copy_of_list->stu, copy_of_list->sch)) {
      remove_node_of_pair_from_graph(&answer, copy_of_list->stu, copy_of_list->sch);
    }
    copy_of_list = copy_of_list->next;
  }

  destroy_stu_sch_pair_list(&list_of_all_nodes);

  return answer;
}

/*
graph_node* node_of_pair(dgraph* graph, int i, int j) {
  
  dgraph* probe;
  graph_node_list_node* subprobe;
  graph_node* answer;

  probe = graph;
  while (probe != NULL) {
    if (j == probe->listptr->node->sch) {
      subprobe = probe->listptr;
      while (subprobe != NULL) {
	if (i == subprobe->node->stu) {
	  answer = subprobe->node;
	}

	subprobe = subprobe->next;
      }
    }

    probe = probe->next;
  }
  
  return answer;
}
*/

stu_sch_pair_list_node* nodes_in_graph(dgraph* graph) {
  stu_sch_pair_list_node* answer;

  graph_node_list_node* node_list;

  answer = NULL;
  while (graph != NULL) {
    node_list = graph->listptr;
    while (node_list != NULL) {
      add_pair_to_stu_sch_pair_list(&answer, node_list->node->stu, node_list->node->sch);
      node_list = node_list->next;
    }

    graph = graph->next;
  }

  return answer;
}

/*************************/

void remove_pair_from_source_and_target_lists(dgraph* graph, int stu, int sch) {
  graph_node_list_node* subprobe;

  while (graph != NULL) {
    subprobe = graph->listptr;
    while (subprobe != NULL) {
      remove_pair_from_stu_sch_pair_list(&(subprobe->node->sources), stu, sch);
      remove_pair_from_stu_sch_pair_list(&(subprobe->node->targets), stu, sch);

      subprobe = subprobe->next;
    }

    graph = graph->next;
  }
}

void remove_node_from_graph(dgraph** graph, graph_node* target) {
  int found;
  
  dgraph* probe;
  dgraph* trailer;
  graph_node_list_node* subprobe;
  graph_node_list_node* subtrailer;

  remove_pair_from_source_and_target_lists(*graph, target->stu, target->sch);

  probe = *graph;
  trailer = probe;
  found = 0;

  while (probe != NULL && !found) {
    subprobe = probe->listptr;
    subtrailer = subprobe;
    
    while (subprobe != NULL && !found) {
      if (subprobe->node != target) {
	if (subprobe->next == NULL) {
	  if (trailer != probe) {
	    trailer = probe;
	  }
	  probe = probe->next;
	  subprobe = probe->listptr;
	  subtrailer = subprobe;
	}
	else {
	  if (subtrailer != subprobe) {
	    subtrailer = subprobe;
	  }
	  subprobe = subprobe->next;
	}
      }
      else {
	found = 1;

	/* destroy_graph_node(subprobe->node); */
	
	destroy_stu_sch_pair_list(&(subprobe->node->sources));
	destroy_stu_sch_pair_list(&(subprobe->node->targets));
	free(subprobe->node);

	if (subtrailer == subprobe) {
	  probe->listptr = subprobe->next;
	}
	else {
	  subtrailer->next = subprobe->next;
	};
	free(subprobe);

	if (probe->listptr == NULL) {
	  if (trailer == probe) {
	    *graph = probe->next;
	  }
	  else {
	    trailer->next = probe->next;
	  }
	  free(probe);
	}
      }
    }
  }
}

void remove_node_from_list_of_graph_node_lists(dgraph** graph,
					       dgraph* target){
  dgraph* probe;
  dgraph* subprobe;

  probe = *graph;

  if (probe != target) {
    subprobe = probe;
    probe = probe->next;
    while (probe != target) {
      probe = probe->next;
      subprobe = subprobe->next;
    }
    subprobe->next = probe->next;
    free(probe);
  }
  else {
    if (probe->next != NULL) {
      *graph = probe->next;
    }
    else {
      *graph = NULL;
    }
    free(probe);
  }
}

void remove_node_of_pair_from_graph(dgraph** graph, int stu, int sch) {
  graph_node* node;

  node = node_of_pair(*graph, stu, sch);
  remove_node_from_graph(graph, node);

  destroy_graph_node(node);
}

void remove_nodes_of_list_from_graph(dgraph** given, stu_sch_pair_list_node* list) {
  while (list != NULL) {
    remove_node_of_pair_from_graph(given, list->stu, list->sch);

    list = list->next;
  }
}

void remove_nodes_and_arcs_outside_loops(dgraph** graph) {
  int done, found_one;
  
  dgraph* probe;
  graph_node_list_node* subprobe;

  /*
          stu_sch_pair_list_node* nodelist;
	  fprintf(stderr, "Initially the nodes in the graph are:\n");
	  nodelist = nodes_in_graph(*graph);
	  fprint_pair_list(nodelist);
	  destroy_stu_sch_pair_list(&nodelist);
	  fprintf(stderr, ".\n");
  */

  done = 0;
  while (!done) {
    done = 1;
    found_one = 0;
    probe = *graph;
    while (probe != NULL && !found_one) {      
      subprobe = probe->listptr;
      while (subprobe != NULL && !found_one) {

	if (subprobe->node->sources == NULL || subprobe->node->targets == NULL) {
	  done = 0;
	  found_one = 1;
	  remove_node_from_graph(graph, subprobe->node);
	}

	if (!found_one) {
	  subprobe = subprobe->next;
	}
      }

      if (!found_one) {
	probe = probe->next;
      }
    }
  }
}

void remove_nodes_with_no_arcs(dgraph** graph) {
  int done;

  dgraph* probe;
  graph_node_list_node* subprobe;
  
  probe = *graph;
  while (probe != NULL) {

    done = 0;
    subprobe = probe->listptr;
    while (probe->listptr != NULL && !done) {
      done = 1;
      subprobe = probe->listptr;
      while (subprobe != NULL && done) {
	if (subprobe->node->sources == NULL && subprobe->node->targets == NULL) {
	  done = 0;
	  remove_node_from_graph(graph, subprobe->node);
	}
	else {
	  subprobe = subprobe->next;
	}
      }
    }
    probe = probe->next;
  }

  done = 0;
  while(!done) {
    done = 1;
    if (*graph != NULL) {
      if ((*graph)->next == NULL && (*graph)->listptr == NULL) {
	free(*graph);
	*graph = NULL;
      }
      else {
	probe = *graph;

	while (probe->listptr != NULL && probe->next != NULL) {
	  probe = probe->next;
	}
	if (probe->listptr == NULL) {
	  done = 0;
	  remove_node_from_list_of_graph_node_lists(graph, probe);
	}
      }
    }
  }
}

/*********************************/



void destroy_graph_node(graph_node* node) {
  destroy_stu_sch_pair_list(&(node->sources));
  destroy_stu_sch_pair_list(&(node->targets));
  free(node);
}

void destroy_graph_node_list(graph_node_list_node* list) {
  graph_node_list_node* trailing_node;

  while (list != NULL) {
    destroy_graph_node(list->node);
    trailing_node = list;
    list = list->next;
    free(trailing_node);
  }
}

void destroy_dgraph(dgraph* graph) {
  dgraph* trailing_node;
  while (graph != NULL) {
    destroy_graph_node_list(graph->listptr);
    trailing_node = graph;
    graph = graph->next;
    free(trailing_node);
  }
}

void destroy_list_of_dgraphs(list_of_dgraphs* list) {
  list_of_dgraphs* trailing_node;
  
  while (list != NULL) {
    destroy_dgraph(list->component);
    trailing_node = list;
    list = list ->next;
    free(trailing_node);
  }
}
