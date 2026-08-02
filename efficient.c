#include "efficient.h"

/***************************/

dgraph* initialize_nodes(partial_alloc* myalloc, process_scp* myscp) {
  int i, j, k, nst, nsc, first, hit;

  dgraph* answer;
  dgraph* probe;

  graph_node_list_node* subprobe;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  answer = NULL;
  probe = answer;
  first = 1;

  for (j = 1; j <= nsc; j++) {
    
    hit = 0;
    for (i = 1; i <= nst && !hit; i++) {
      if (student_ranking_of_school(myscp,i,j) > 1 && get_entry(myalloc,i,j) > 0.000001) {
	hit = 1;
	k = i;
      }
    }
    
    if (hit) {
      if (first) {
	probe = malloc(sizeof(dgraph));
	answer = probe;
	first = 0;
      }
      else {
	probe->next = malloc(sizeof(dgraph));
	probe = probe->next;
      }
      
      probe->listptr = malloc(sizeof(graph_node_list_node));
      probe->next = NULL;
    
      subprobe = probe->listptr;
      subprobe->next = NULL;
      subprobe->node = new_graph_node(k, j);

      for (i = k+1; i <= nst; i++) {
	if (student_ranking_of_school(myscp,i,j) > 1 && get_entry(myalloc,i,j) > 0.000001) {
	  
	  subprobe->next = malloc(sizeof(graph_node_list_node));
	  subprobe = subprobe->next;
	  subprobe->next = NULL;
	  subprobe->node = new_graph_node(i, j);
	}
      }
    }
  }

  return answer;
}

int i_is_justifiably_envied_for_l(partial_alloc* myalloc, process_scp* myscp, int i, int l) {
  int k, m, n, nst;

  nst = myalloc->no_students;

  if (get_entry(myalloc, i, l) < 0.000001) {
    return 0;
  }

  for (k = 1; k <= nst; k++) {
    if (k != i) {
      for (m = 1; m <= myscp->no_eligible_schools[k-1]; m++) {
	if (myscp->preferences[k-1][m-1] == l) {
	  if (int_entry(&(myscp->priorities), k, l) > int_entry(&(myscp->priorities), i, l)) {
	    for (n = m + 1; n <= myscp->no_eligible_schools[k-1]; n++) {
	      if (get_entry(myalloc, k, myscp->preferences[k-1][n-1]) > 0.000001) {
		return 1;
	      }
	    }
	  }
	}
      }
    }
  }

  return 0;
}

int is_ex_ante_stable(partial_alloc* myalloc, process_scp* myscp) {
  int i, k, l, nst;

  nst = myscp->no_students;

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      l = myscp->preferences[i-1][k-1];
      if (i_is_justifiably_envied_for_l(myalloc, myscp, i, l)) {
	return 0;
      }
    }
  }

  return 1;
}

void  add_arcs(dgraph* graph, partial_alloc* myalloc, process_scp* myscp, int ex_ante_stable) {
  int i, j, k, l, r;

  dgraph* probe;
  dgraph* subprobe;
  probe = graph;
  
  graph_node_list_node* source_node_list;
  graph_node_list_node* target_node_list;
  
  while  (probe != NULL) {
    
    j = probe->listptr->node->sch;
    source_node_list = probe->listptr;
    while (source_node_list != NULL) {
      
      i = source_node_list->node->stu;
      for (r = 1; r < student_ranking_of_school(myscp, i, j); r++) {
	l = myscp->preferences[i-1][r-1];
	subprobe = graph;
	while (subprobe != NULL) {
	  
	  if (l == subprobe->listptr->node->sch) {
	    if (!ex_ante_stable || !i_is_justifiably_envied_for_l(myalloc, myscp, i, l)) {
	      target_node_list = subprobe->listptr;
	      while (target_node_list != NULL) {
	      
		k = target_node_list->node->stu;

		add_pair_to_stu_sch_pair_list(&(source_node_list->node->targets), k, l);
		add_pair_to_stu_sch_pair_list(&(target_node_list->node->sources), i, j);

	      
		target_node_list = target_node_list->next;
	      }
	    }
	  }

	  subprobe = subprobe->next;
	}
      }
      
      source_node_list = source_node_list->next;
    }
    
    probe = probe->next;
  }
}

dgraph* reduced_graph(partial_alloc* myalloc, process_scp* myscp, int ex_ante_stable) {
  dgraph* answer;
  answer = initialize_nodes(myalloc, myscp);
  add_arcs(answer, myalloc, myscp, ex_ante_stable);
  remove_nodes_and_arcs_outside_loops(&answer);

  return answer;
}

int someone_would_like_some_j(int j, partial_alloc* myalloc, process_scp* myscp) {
  int i, k, r, s, nst;
  
  nst = myscp->no_students;
  
  for (i = 1; i <= nst; i++) {
    if (is_eligible(myscp, i, j)) {
      r = student_ranking_of_school(myscp, i, j);
      for (k = r+1; k <= myscp->no_eligible_schools[i-1]; k++) {
	s = myscp->preferences[i-1][k-1];
	if (get_entry(myalloc, i, s) > 0.000001) {
	  return 1;
	}
      }
    }
  }

  return 0;
}

int allocation_is_wasteful(partial_alloc* myalloc, process_scp* myscp) {
  int i, j, nst, nsc, max_slack_school;

  double school_sum, slack, max_slack;

  max_slack_school = 0;
  slack = 0.0;
  max_slack = 0.0;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  for (j = 1; j <= nsc; j++) {
    school_sum = 0.0;
    for (i = 1; i <= nst; i++) {
      school_sum += get_entry(myalloc, i, j);
    }
    slack = (double)myscp->quotas[j-1] - school_sum;
    if (slack > max_slack + 0.000001 && someone_would_like_some_j(j, myalloc, myscp)) {
      max_slack = slack;
      max_slack_school = j;
    }
  }

  return max_slack_school;;
}

int allocation_is_efficient(partial_alloc* myalloc, process_scp* myscp, int ex_ante_stable) {
  int answer;
  dgraph* graph;
  
  if (allocation_is_wasteful(myalloc,myscp)) {
    return 0;
  }

  graph = reduced_graph(myalloc, myscp, ex_ante_stable);
 
 if (graph == NULL) {
   answer = 1;
 }
 else {
   answer = 0;
 }
 
 destroy_dgraph(graph);

 return answer;
}

void cyclic_trade(partial_alloc* myalloc, stu_sch_pair_list_node* loop) {
  double max_trade;

  stu_sch_pair_list_node* loop_copy;

  max_trade = 1.0;
  loop_copy = loop;
  while (loop_copy != NULL) {
    max_trade = min(max_trade, get_entry(myalloc, loop_copy->stu, loop_copy->sch));
    loop_copy = loop_copy->next;
  }

  loop_copy = loop;
  while (loop_copy != NULL) {
    increment_entry(myalloc, loop_copy->stu, loop_copy->sch, -max_trade);
    if (loop_copy->next != NULL) {
      increment_entry(myalloc, loop_copy->stu, loop_copy->next->sch, max_trade);
    }
    else {
      increment_entry(myalloc, loop_copy->stu, loop->sch, max_trade);
    }
    loop_copy = loop_copy->next;
  }
}

void make_alloc_eff_and_par_dominant(partial_alloc* my_alloc, process_scp* myscp,
				     int ex_ante_stable) {
  dgraph* graph;
  stu_sch_pair_list_node* loop; 
  
  graph = reduced_graph(my_alloc, myscp, ex_ante_stable);

  while (graph != NULL) {
    loop = random_loop(graph);
    cyclic_trade(my_alloc, loop);
    
    destroy_stu_sch_pair_list(&loop);
    destroy_dgraph(graph);
    
    graph = reduced_graph(my_alloc, myscp, ex_ante_stable);
  }

  destroy_dgraph(graph);
}

/*************************/

void fprint_arc(int i, int j, int k, int l) {
  fprintf(stderr, "(%i,%i)->(%i,%i)", i, j, k, l);
}
