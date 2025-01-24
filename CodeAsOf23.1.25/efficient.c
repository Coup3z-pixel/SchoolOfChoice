#include "efficient.h"

int allocation_is_efficient(struct partial_alloc* myalloc, struct process_scp* myscp) {
  int old_eff, new_eff; 

  old_eff = OLD_allocation_is_efficient(myalloc, myscp);
  new_eff = NEW_allocation_is_efficient(myalloc, myscp); 

  if (old_eff && !new_eff) {
    fprintf(stderr, "The allocation is efficient, but NEW_eff says it isn't.\n");
    exit(0);
  }

  if (!old_eff && new_eff) {
    fprintf(stderr, "The allocation is inefficient, but NEW_eff says it's inefficient.\n");
    exit(0);
  }

  /*
  if (!old_eff) {
    fprintf(stderr, "The allocation is inefficient.\n");
    exit(0);
  }
  */

  return old_eff;
}

int OLD_allocation_is_efficient(struct partial_alloc* myalloc, struct process_scp* myscp) {
  int i, j, nst, nsc, inefficient;
  
  struct stu_sch_node*** chart;
  struct stu_sch_node*** envygr;
  struct stu_sch_node*** enviedgr;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  envygr = get_simple_envy_graph(myscp, myalloc);
  enviedgr = get_envied_graph(envygr, nst, nsc);

  chart = malloc(nst * sizeof(struct stu_sch_node**));
  for (i = 1; i <= nst; i++) {
    chart[i-1]  = malloc(nsc * sizeof(struct stu_sch_node*));
    for (j = 1; j <= nsc; j++) {
      chart[i-1][j-1] = NULL;
    }
  }

  inefficient = 0;
  i = 1;
  while (!inefficient && i <= nst) {
    j = 1;
    while (!inefficient && j <= nsc) {
      if (envygr[i-1][j-1] != NULL || enviedgr[i-1][j-1] != NULL) {
	if (envygr[i-1][j-1] == NULL || enviedgr[i-1][j-1] == NULL) {
	  remove_pair_from_envy_and_envied_graphs(envygr, enviedgr, i, j);
	}
	else {
	  inefficient = process_graphs_pair(envygr, enviedgr, chart, i, j);
	}
      }
      j++;
    }
    i++;
  }

  destroy_matrix_of_lists(envygr, nst, nsc);
  destroy_matrix_of_lists(enviedgr, nst, nsc);
  destroy_matrix_of_lists(chart, nst, nsc);

  return 1 - inefficient;
}

int process_graphs_pair(struct stu_sch_node*** envygr, struct stu_sch_node*** enviedgr,
			 struct stu_sch_node*** chart, int i, int j) {
  int new_layer_nonempty, cycle_found;
  int* found_cycle;

  struct stu_sch_node* all_nodes_reached;
  struct stu_sch_node* all_nodes_reached_tip;
  struct stu_sch_node* last_layer;
  struct stu_sch_node* new_layer;
  struct stu_sch_node* probe;

  found_cycle = malloc(sizeof(int));
  *found_cycle = 0;
  new_layer_nonempty = 1;
  all_nodes_reached = NULL;
  all_nodes_reached_tip = NULL;
  
  while (!(*found_cycle) && new_layer_nonempty) {
      
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

  clean_up_chart_and_nodes(chart, all_nodes_reached);
  remove_pair_from_envy_and_envied_graphs(envygr, enviedgr, i, j);

  cycle_found = *found_cycle;
  free(found_cycle);

  return cycle_found;
}

int** get_pairs_that_can_envy(struct process_scp* myscp, struct partial_alloc* my_alloc) {
  int i, j, nst, nsc;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  int** answer = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (get_entry(my_alloc, i, j) > 0.00001 && j != myscp->preferences[i-1][0]) {
	answer[i-1][j-1] = 1;
      }
      else {
	answer[i-1][j-1] = 0;
      }
    }
  }

  return answer;
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
  struct stu_sch_node* probe;

  probe = list;
  
  while (probe != NULL) {
    if (probe->stuno == i && probe->schno == j) {
      return 1;
    }
    probe = probe->next;
  }

  return 0;
}

int stu_sch_list_contains_pair_noninitially(struct stu_sch_node* list, int i, int j) {
  struct stu_sch_node* probe;

  probe = list;

  if (probe == NULL) {
    return 0;
  }

  probe = probe->next;
  
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

struct stu_sch_node*** get_simple_envy_graph(struct process_scp* myscp,
					     struct partial_alloc* my_alloc) {
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

  active = get_pairs_that_can_envy(myscp, my_alloc);

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (active[i-1][j-1] == 1) {
	k = 1;
	while (myscp->preferences[i-1][k-1] != j) {
	  l = myscp->preferences[i-1][k-1];
	  if (get_entry(my_alloc, i, l) <
	      is_eligible(myscp, i, l) * myscp->time_remaining - 0.000001) {
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

struct stu_sch_node* copy_of_stu_sch_list(struct stu_sch_node* node) {
  struct stu_sch_node* probe;
  struct stu_sch_node* answer;
  struct stu_sch_node* answer_tip;

  probe = node;
  if (probe == NULL) {
    answer = NULL;
  }
  else {
    answer = create_stu_sch_node(probe->stuno, probe->schno);
    answer_tip = answer;
    while (probe->next != NULL) {
      probe = probe->next;
      answer_tip->next = create_stu_sch_node(probe->stuno, probe->schno);
      answer_tip = answer_tip->next;
    }
  }

  return answer;
}

void print_stu_sch_list(struct stu_sch_node* node) {
  struct stu_sch_node* probe;

  if (node == NULL) {
    printf("The list is NULL.\n");
  }

  probe = node;
  while (probe != NULL) {
    printf(" (%i,%i)", probe->stuno, probe->schno);
    probe = probe->next;
  }
  printf("\n");
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


/*  Reimplementation */

struct lists_of_students get_accepting_students(struct partial_alloc* myalloc,
						    struct process_scp* myscp) {
  int i, j, nst, nsc;
  
  struct lists_of_students answer;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  answer.no_students = nst;
  answer.no_schools = nsc;
  
  answer.lists = malloc(nsc * sizeof(struct stu_list_node));
  for (j = 1; j <= nsc; j++) {
    answer.lists[j-1] = NULL;
    for (i = 1; i <= nst; i++) {
      if (student_can_accept_school(myalloc, myscp, i, j)) {
	add_student_to_list(&(answer.lists[j-1]), i);

	if (answer.lists[j-1] == NULL) {
	  printf("Somehow we added nothing to the list.\n");
	}
	    
      }
    }
  }

  return answer;  
}

int student_can_accept_school(struct partial_alloc* myalloc, struct process_scp* myscp,
			      int i, int j) {
  int ranking, k;

  ranking = student_ranking_of_school(myscp, i, j);
  if (ranking > 0) {
    for (k = ranking+1; k <= myscp->no_eligible_schools[i-1]; k++) {
      if (get_entry(myalloc, i, myscp->preferences[i-1][k-1]) > 0.000001) {	
	return 1;
      }
    }
  }

  return 0;
}

void add_student_to_list(struct stu_list_node** list, int i) {
  struct stu_list_node* probe;
  struct stu_list_node* new;

  probe = *list;

  if (probe == NULL) {
    probe = malloc(sizeof(struct stu_list_node)); 
    probe->stu_no = i;
    probe->next = NULL;
    *list = probe;
  }
  else {
    while (probe->stu_no < i && probe->next != NULL) {
      probe = probe->next;
    }
    if (probe->stu_no < i) {
      new = malloc(sizeof(struct stu_list_node));
      new->next = NULL;
      probe->next = new;
      new->stu_no = i;
    }
    if (probe->stu_no > i) {
      new = malloc(sizeof(struct stu_list_node));
      new->next = probe->next;
      probe->next = new;
      new->stu_no = probe->stu_no;
      probe->stu_no = i;
    }
  }

  if (*list == NULL) {
    printf("Somehow we failed.\n");
  }
}

void remove_student_from_list(struct stu_list_node** list, int i) {
  int is_in_list;
  
  struct stu_list_node* probe;
  struct stu_list_node* trail;

  /*
  printf("Going in to remove %i we have:\n", i);
  probe = *list;
  while (probe != NULL) {
    printf("The student number is %i.\n", probe->stu_no);
    probe = probe->next;
  }
  printf("\n");
  */

  probe = *list;

  is_in_list = 0;

  while (probe != NULL) {
    if (probe->stu_no == i) {
      is_in_list = 1;
    }
    probe = probe->next;
  }

  if (is_in_list) {
    probe = *list;
    if (probe->stu_no == i) {
      *list = probe->next;
      free(probe);
    }
    else {
      trail = *list;
      probe = (*list)->next;
      while (probe->stu_no != i) {
	probe = probe->next;
	trail = trail->next;
      }
      trail->next = probe->next;
      free(probe);
    }
  }

  /*
  printf("On the way out we have:\n");
  probe = *list;
  while (probe != NULL) {
    printf("The student number is %i.\n", probe->stu_no);
    probe = probe->next;
  }
  printf("\n");
  */
}

int is_accepting(struct lists_of_students* accepting_students, int i, int j) {
  struct stu_list_node* probe;

  probe = accepting_students->lists[j-1];

  while (probe != NULL) {
    if (probe->stu_no == i) {
      return 1;
    }
    probe = probe->next;
  }
  return 0;
}

struct stu_sch_node* possible_recipients(struct partial_alloc* myalloc, struct process_scp* myscp,
					 struct lists_of_students* accepting_students,
					 int k, int l) {
  int i, j, r, m;
  
  struct stu_sch_node* answer;
  struct stu_sch_node* probe;
  struct stu_list_node* subprobe;

  answer = NULL;
  probe = NULL;
  
  r = student_ranking_of_school(myscp, k, l);
  for (m = r+1; m <= myscp->no_eligible_schools[k-1]; m++) {
    j = myscp->preferences[k-1][m-1];
    if (get_entry(myalloc, k, j) > 0.000001) {
      subprobe = accepting_students->lists[j-1];
      while (subprobe != NULL) {
	i = subprobe->stu_no;
	if (i != k) {
	  if (probe == NULL) {
	    probe = create_stu_sch_node(i,j);
	    answer = probe;
	  }
	  else {
	    probe->next = create_stu_sch_node(i,j);
	    probe = probe->next;
	  }
	}
	subprobe = subprobe->next;
      }
    }
  }

  return answer;
}

struct stu_sch_node* simple_new_layer(struct partial_alloc* myalloc, struct process_scp* myscp,
				      struct lists_of_students* accepting_students,
				      struct stu_sch_node* all_so_far, 
				      struct stu_sch_node* last_layer)  {
  int k, l, p, q;
  
  struct stu_sch_node* answer;
  struct stu_sch_node* answer_tip;
  struct stu_sch_node* recipients;
  struct stu_sch_node* probe;
  struct stu_sch_node* subprobe;

  answer = NULL;
  answer_tip = NULL;

  probe = last_layer;
  while (probe != NULL) {
    k = probe->stuno;
    l = probe->schno;
    recipients = possible_recipients(myalloc, myscp, accepting_students, k, l);
    subprobe = recipients;
    while (subprobe != NULL) {
      p = subprobe->stuno;
      q = subprobe->schno;
      if (!stu_sch_list_contains_pair_noninitially(all_so_far, p, q) &&
	  !stu_sch_list_contains_pair(answer, p, q)) {
	if (answer == NULL) {
	  answer = create_stu_sch_node(p, q);
	  answer_tip = answer;
	}
	else {
	  answer_tip->next = create_stu_sch_node(p, q);
	  answer_tip = answer_tip->next;
	}
      }
      subprobe = subprobe->next;
    }
    
    destroy_stu_sch_list(recipients);
    probe = probe->next;
  }

  return answer;
}

struct stu_sch_node* NEW_get_new_layer(struct partial_alloc* myalloc, struct process_scp* myscp,
				       struct lists_of_students* accepting_students,
				       struct stu_sch_node* all_so_far, 
				       struct stu_sch_node* last_layer,
				       int* found_cycle, int* something_new,
				       int i, int j)  {
  struct stu_sch_node* answer;
  struct stu_sch_node* probe;

  answer = simple_new_layer(myalloc, myscp, accepting_students, all_so_far, last_layer);

  if (answer == NULL) {
    *something_new = 0;
  }
  else {
    *something_new = 1;
  }

  *found_cycle = 0;
  probe = answer;
  while (probe != NULL) {
    if (probe->stuno == i && probe->schno == j) {
      *found_cycle = 1;

      /*
      fprintf(stderr, "The cycle involves the pair (%i,%i).\n", i, j);
      */
      
    }
    probe = probe->next;
  }

  return answer;
}

int process_initial_pair(struct partial_alloc* myalloc, struct process_scp* myscp,
			     struct lists_of_students* accepting_students,
			     int i, int j) {
  int cycle_found;
  
  int* found_cycle;
  int* something_new;

  struct stu_sch_node* all_nodes_reached;
  struct stu_sch_node* all_nodes_reached_tip;
  struct stu_sch_node* last_layer;
  struct stu_sch_node* new_layer;

  found_cycle = malloc(sizeof(int));
  *found_cycle = 0;
  something_new = malloc(sizeof(int));
  *something_new = 1;
  all_nodes_reached = create_stu_sch_node(i,j);
  all_nodes_reached_tip = all_nodes_reached;
  last_layer = all_nodes_reached;
  
  while (!(*found_cycle) && *something_new) {
    new_layer = NEW_get_new_layer(myalloc, myscp, accepting_students, all_nodes_reached, 
				  last_layer, found_cycle, something_new, i, j);
    if (*something_new) {
      all_nodes_reached_tip->next = new_layer;
      while (all_nodes_reached_tip->next != NULL) {
	all_nodes_reached_tip = all_nodes_reached_tip->next;
      }
    }
    last_layer = new_layer;
  }

  destroy_stu_sch_list(all_nodes_reached);

  cycle_found = *found_cycle;
  free(found_cycle);
  free(something_new);

  return cycle_found;
}

int NEW_allocation_is_efficient(struct partial_alloc* myalloc, struct process_scp* myscp) {
  int j, nsc, inefficient;

  struct stu_list_node* lead;
  struct lists_of_students accepting_students;
  
  nsc = myscp->no_schools;

  accepting_students = get_accepting_students(myalloc, myscp);

  /*  struct stu_list_node* probe; */

  inefficient = 0;
  j = 1;
  while (j <= nsc && !inefficient) {
    lead = accepting_students.lists[j-1];
    while (lead != NULL && !inefficient) {
      inefficient = process_initial_pair(myalloc, myscp, &accepting_students, lead->stu_no, j);
      accepting_students.lists[j-1] = lead->next;
      free(lead);   
      lead = accepting_students.lists[j-1];
    }
    j++;
  }

  destroy_lists_of_students(&accepting_students);

  return 1 - inefficient;
}

void destroy_student_list(struct stu_list_node** list) {
  struct stu_list_node* trail;
  struct stu_list_node* lead;
  
  lead = *list;
  if (lead != NULL) {
    trail = lead;
    while (lead != NULL) {
      lead = lead->next;
      free(trail);
      trail = lead;
    }
  }
}

void destroy_lists_of_students(struct lists_of_students* student_lists) {
  int j, nsc;

  nsc = student_lists->no_schools;
  
  for (j = 1; j <= nsc; j++) {
    destroy_student_list(&(student_lists->lists[j-1]));
  }

  free(student_lists->lists);
}
