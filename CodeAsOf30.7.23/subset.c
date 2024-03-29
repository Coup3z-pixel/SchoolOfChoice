#include "subset.h"

void print_vector_of_ints(int* vector, int dim) {
  printf("(");
  for (int i = 1; i <= dim-1; i++) {
    printf("%d,",vector[i-1]);
  }
  printf("%d)",vector[dim-1]);
}

struct subset nullset(int large_set_size) {
  int i;
  struct subset my_subset;
  my_subset.large_set_size = large_set_size;
  my_subset.subset_size = 0;
  my_subset.indicator = malloc(large_set_size * sizeof(int));
  for (i = 1; i <= large_set_size; i++) {
    my_subset.indicator[i-1] = 0;
  }
  
  return my_subset;
}

struct subset fullset(int large_set_size) {
  int i;
  struct subset my_subset;
  my_subset.large_set_size = large_set_size;
  my_subset.subset_size = large_set_size;
  my_subset.indicator = malloc(large_set_size * sizeof(int));
  for (i = 1; i <= large_set_size; i++) {
    my_subset.indicator[i-1] = 1;
  }
  return my_subset;
}

struct subset singleton_subset(int elmt, int large_size) {
  struct subset singleton;
  singleton.large_set_size = large_size;
  singleton.subset_size = 1;
  singleton.indicator = malloc(large_size * sizeof(int));
  for (int j = 1; j <= large_size; j++) {
    singleton.indicator[j-1] = 0;
  }
  singleton.indicator[elmt - 1] = 1;
  return singleton;
}

struct square_matrix matrix_of_zeros(int dim) {  
  int j, k;
  struct square_matrix zero_matrix;
  
  zero_matrix.dimension = dim;
  zero_matrix.entries = malloc(dim * sizeof(int*));
  for (j = 1; j <= dim; j++) {
    zero_matrix.entries[j-1] = malloc(dim * sizeof(int));
    for (k = 1; k <= dim; k++) {
      zero_matrix.entries[j-1][k-1] = 0;
    }
  }
  
  return zero_matrix;
}

struct square_matrix matrix_of_ones(int dim) {
  int j, k;
  struct square_matrix one_matrix;
  one_matrix.dimension = dim;
  one_matrix.entries = malloc(dim * sizeof(int*));
  for (j = 1; j <= dim; j++) {
    one_matrix.entries[j-1] = malloc(dim * sizeof(int));
    for (k = 1; k <= dim; k++) {
      one_matrix.entries[j-1][k-1] = 1;
    }
  }
  return one_matrix;
}

void print_subset(struct subset* my_subset) {
  int i;
  printf("(");
  for (i = 1; i < my_subset->large_set_size; i++) {
    printf("%d,",my_subset->indicator[i-1]);
  }
  printf("%d)",my_subset->indicator[my_subset->large_set_size-1]);
}



void print_index(struct index* my_index) {
  int i;
  
  printf("(");
  
  for (i = 1; i < my_index->no_elements; i++) {
    printf("%d,",my_index->indices[i-1]);
  }
  printf("%d)",my_index->indices[my_index->no_elements-1]);
}

void print_index_of_subset(struct subset* my_subset) {
  struct index the_index = index_of_subset(my_subset);
  print_index(&the_index);
  destroy_index(the_index);
}


void print_square_matrix(struct square_matrix* my_matrix) {
  int j, k;
  for (j = 1; j <= my_matrix->dimension; j++) {
    for (k = 1; k <= my_matrix->dimension; k++) {
      printf("   %i",my_matrix->entries[j-1][k-1]);
    }
    printf("\n");
  }
}

void destroy_subset(struct subset my_subset) {
  free(my_subset.indicator);
}

void destroy_index(struct index my_index) {
  free(my_index.indices);
}

void destroy_square_matrix(struct square_matrix my_matrix) {
  int j;
  for (j = 1; j <= my_matrix.dimension; j++) {
    free(my_matrix.entries[j-1]);
  }
  free(my_matrix.entries);
}

void copy_subset(struct subset* given_subset, struct subset* copy_subset) {
  int i;
  if (given_subset->large_set_size != copy_subset->large_set_size) {
    printf("ERROR: attempt to copy subsets of different size sets.\n");
    exit(0);
  }
  copy_subset->subset_size = given_subset->subset_size;
  for (i = 1; i <= given_subset->large_set_size; i++) {
    copy_subset->indicator[i-1] = given_subset->indicator[i-1];
  }
}

int subsets_are_same(struct subset* first, struct subset* second) {
  if (first->large_set_size != second->large_set_size) {
    return 0;
  }
  for (int i = 1; i <= first->large_set_size; i++) {
    if (first->indicator[i-1] != second->indicator[i-1]) {
      return 0;
    }
  }
  return 1;
}

int subsets_are_disjoint(struct subset* first, struct subset* second) {
  for (int j = 1; j <= first->large_set_size; j++) {
    if (first->indicator[j-1] == 1 && second->indicator[j-1] == 1) {
      return 0;
    }
  }
  return 1;
}


void complement(struct subset* given_subset, struct subset* comp) {
  int i;
  comp->large_set_size = given_subset->large_set_size;
  comp->subset_size = given_subset->large_set_size - given_subset->subset_size;
  for (i = 1; i <= given_subset->large_set_size; i++) {
    comp->indicator[i-1] = 1 - given_subset->indicator[i-1];
  }
}

struct index index_of_subset(struct subset* my_subset) {
  int i, k;
  struct index my_index;

  if (my_subset->subset_size == 0) {
    printf("Taking the index of an emptyset is not permitted.\n");
    exit(0);	    
  }
  
  my_index.no_elements = my_subset->subset_size;
  
  my_index.indices = malloc(my_subset->subset_size * sizeof(int));
  k = 0;
  for (i = 1; i <= my_subset->subset_size; i++) {
    k++;
    while (my_subset->indicator[k-1] == 0) {
      k++;
    }
    my_index.indices[i-1] = k;
  }
  
  return my_index;
}

struct subset subset_of_index(struct index* my_index, int large_set_size) {
  struct subset answer = nullset(large_set_size);
  
  answer.subset_size = my_index->no_elements;
  
  for (int k = 1; k <= my_index->no_elements; k++) {
    answer.indicator[my_index->indices[k-1]-1] = 1;
  }

  return answer;
}

void subset_of_subset_to_subset(struct subset* new_subset, struct subset* given_subset,
				struct subset* subset_of_subset) {
  int k,l;
  
  struct index index_of_given = index_of_subset(given_subset);

  new_subset->subset_size = 0;
  for (k = 1; k <= new_subset->large_set_size; k++) {
    new_subset->indicator[k-1] = 0;
  }
  
  for (l = 1; l <= given_subset->subset_size; l++) {
    if (subset_of_subset->indicator[l-1] == 1) {
      new_subset->subset_size++;
      new_subset->indicator[index_of_given.indices[l-1]-1] = 1;
    }
  }
}

int* indices_of_elements(struct subset* my_subset) {
  int j;
  int* list_of_indices = malloc(my_subset->large_set_size * sizeof(int));
  int no_elements = 0;
  for (j = 1; j <= my_subset->large_set_size; j++) {
    if (my_subset->indicator[j-1] == 1) {
      no_elements++;
      list_of_indices[j-1] = no_elements;
    }
    else {
      list_of_indices[j-1] = 0;
    }
  }
  return list_of_indices;
}

int indices_are_same(struct index* first, struct index* second) {
  if (first->no_elements != second->no_elements) {
    return 0;
  }
  for (int j = 1; j <= first->no_elements; j++) {
    if (first->indices[j-1] != second->indices[j-1]) {
      return 0;
    }
  }
  return 1;
}

int first_precedes_second(struct index* first, struct index* second) {
  if (first->no_elements < second->no_elements) {
    return 1;
  }
  if (first->no_elements > second->no_elements) {
    return 0;
  }
  
  for (int j = 1; j <= first->no_elements; j++) {
    if (first->indices[j-1] < second->indices[j-1]) {
      return 1;
    }
    if (first->indices[j-1] > second->indices[j-1]) {
      return 0;
    }
  }
  return 0;
}

void copy_index(struct index* given_index, struct index* copy_of_index) {
  int i;
  copy_of_index->no_elements = given_index->no_elements;
  
  copy_of_index->indices = malloc(copy_of_index->no_elements * sizeof(int));
  
  for (i = 1; i <= given_index->no_elements; i++) {
    copy_of_index->indices[i-1] = given_index->indices[i-1];
  }
}

struct index singleton_index(int j) {
  struct index singleton;
  singleton.no_elements = 1;
  singleton.indices = malloc(sizeof(int));
  singleton.indices[0] = j;
  return singleton;
}

struct index index_with_element_added(struct index* my_index, int j) {
  int k, inserted_already;
  struct index new_index;
  new_index.no_elements = my_index->no_elements + 1;
  new_index.indices = malloc(new_index.no_elements * sizeof(int));

  inserted_already = 0;
  for (k = 1; k <= new_index.no_elements; k++) {
    if (inserted_already) {
      new_index.indices[k-1] = my_index->indices[k-2];
    }
    if (!inserted_already) {
      if (k == new_index.no_elements) {
	new_index.indices[k-1] = j;
      }
      else {
	if (my_index->indices[k-1] < j) {
	  new_index.indices[k-1] = my_index->indices[k-1];	
	}
	else {
	  new_index.indices[k-1] = j;
	  inserted_already = 1;
	}
      }
    }
  }
  return new_index;
}

struct index index_of_subset_from_indices_list(struct index* my_index, int* list_of_indices) {
  int j;

  struct index new_index;
  new_index.no_elements = my_index->no_elements;
  new_index.indices = malloc(my_index->no_elements * sizeof(int));
  for (j = 1; j <= my_index->no_elements; j++) {
    new_index.indices[j-1] = list_of_indices[my_index->indices[j-1]-1];
  }

  return new_index;
}

struct square_matrix submatrix(struct square_matrix* big_matrix, struct subset* my_subset) {
  int j, k;
  if (big_matrix->dimension != my_subset->large_set_size) {
    printf("ERROR: taking a submatrix with incompatible subset.\n");
    exit(0);
  }
  struct index my_index = index_of_subset(my_subset);
  struct square_matrix  my_matrix;
  my_matrix.dimension = my_subset->subset_size;
  my_matrix.entries = malloc(my_subset->subset_size * sizeof(int*));
  for (j = 1; j <= my_subset->subset_size; j++) {
    my_matrix.entries[j-1] = malloc(my_subset->subset_size * sizeof(int));
    for (k = 1; k <= my_subset->subset_size; k++) {
      my_matrix.entries[j-1][k-1] =
	big_matrix->entries[my_index.indices[j-1]-1][my_index.indices[k-1]-1];
    }
  }
  destroy_index(my_index);
  return my_matrix;
}

int is_connected(struct subset* my_subset, struct square_matrix* related) {
  int k, l, m;
  
  int size = my_subset->subset_size;

  if (size <= 1) {
    return 1;
  }
  
  int* component = malloc(size * sizeof(int));
  component[0] = 1;
  for (k = 2; k <= size; k++) {
    component[k-1] = 0;
  }
  
  struct index the_index = index_of_subset(my_subset);

  for (k = 2; k <= size; k++) {
    for (l = 1; l <= size; l++) {
      for (m = 2; m <= size; m++) {
	if (component[l-1] == 1 && component[m-1] == 0 &&
	    related->entries[the_index.indices[l-1]-1][the_index.indices[m-1]-1] == 1) {
	  component[m-1] = 1;
	}
      }
    }
  }

  for (k = 2; k <= size; k++) {
    if (component[k-1] == 0) {
      free(component);
      destroy_index(the_index);
      return 0;
    }
  }

  free(component);
  destroy_index(the_index);
  return 1;
}

void get_candidate_list(int* candidate_list, struct square_matrix* related,
			struct subset* my_subset, int* point_school) {
  int j, k, nsc, set_size, list_index, fill_number, connected, probe, swap_school;

  nsc = my_subset->large_set_size;
  set_size = my_subset->subset_size;
  
  list_index = 0;
  for (j = 1; j <= nsc; j++) {
    if (my_subset->indicator[j-1] == 1 && j != *point_school) {
      list_index++;
      candidate_list[list_index - 1] = j;
    }
  }

  /*
  printf("We did the first part of get_candidate_list.\n");
  printf("There are %d schools and the subset is\n",nsc);
  struct index the_index = index_of_subset(my_subset);
  print_index(&the_index);
  destroy_index(the_index);
  printf(".\n");
  
  print_square_matrix(related);
  printf(".\n");
  */

  /* We need to reorder the candidate_list according to order to join,
     where a school is not allowed to join the list prior to the time
     that it is connected to some member of the list. */

  fill_number = 1;
  while (fill_number < set_size) {
    connected = 0;
    probe = fill_number-1;
    
    while (!connected) {
      probe++;
      
      if (related->entries[*point_school-1][candidate_list[probe-1]-1] == 1) {
	connected = 1;
      }
      
      for (k = 1; k < fill_number; k++) {
	if (related->entries[candidate_list[k-1]-1][candidate_list[probe-1]-1]) {
	  connected = 1;
	}
      }
    }
    
    swap_school = candidate_list[probe-1];
    for (k = probe; k > fill_number; k--) {
      candidate_list[k-1] = candidate_list[k-2];
    }
    candidate_list[fill_number-1] = swap_school;
    fill_number++;
  }
}

struct subset_list* supersets_of_subsets(struct index* my_index, struct square_matrix* related, int depth) {
  struct subset_list* list_of_supersets = initialized_subset_list();

  /*
  printf("We are given my_index = ");
  print_index(my_index);
  printf(" whose list of nonempty subsets is ");
  print_subset_list(nonempty_subsets(my_index));
  printf(".\n");
  */
  
  add_second_list_to_first(list_of_supersets,nonempty_subsets(my_index));

  /*
  printf("After adding nonempty_subsets(my_index), list_of_supersets is ");
  print_subset_list(list_of_supersets);
  printf(".\n");
  */

  int local_depth = depth;
  while (local_depth > 0) {
    local_depth--;
    add_second_list_to_first(list_of_supersets,immediate_supersets_of_list(list_of_supersets, related));
  }

  /*
  printf("At the end of supersets_of_subsets, list_of_supersets is ");
  print_subset_list(list_of_supersets);
  printf(".\n");
  */

  return list_of_supersets;
}


struct subset_list* initialized_subset_list() {
  struct subset_list* new_list = malloc(sizeof(struct subset_list));
  new_list->node_index = NULL;
  new_list->next = NULL;
  return new_list;
}

int is_empty_list(struct subset_list* my_list) {
  if (my_list->node_index == NULL && my_list-> next == NULL) {
    return 1;
  }
  else {
    return 0;
  }
}

void destroy_subset_list(struct subset_list* my_list) {
  struct subset_list* probe = my_list;
  while (probe->next != NULL) {
    struct subset_list* next_probe = probe->next;
    if (probe->node_index != NULL) {
      destroy_index(*(probe->node_index));
    }
    free(probe);
    probe = next_probe;
  }
  if (probe->node_index != NULL) {
    destroy_index(*(probe->node_index));
  }
  free(probe);
}

void print_subset_list(struct subset_list* my_list) {
  if (is_empty_list(my_list)) {
    printf("null_list");
  }
  else {
    struct subset_list* probe = my_list;
    print_index(probe->node_index); 
    while (probe->next != NULL) {
      print_index(probe->next->node_index); 
      probe = probe->next;
    }
  }
}

int maximum_set_size(struct subset_list* my_list) {
  if (is_empty_list(my_list)) {
    return 0;
  }
  
  struct subset_list* probe = my_list;
  int answer = 0;
  while (probe != NULL) {
    if ((probe->node_index)->no_elements > answer) {
      answer = (probe->node_index)->no_elements;
    }
    probe = probe->next;
  }
  
  return answer;
}

void add_subset(struct subset_list* my_list, struct index* my_index) {
    
  if (is_empty_list(my_list)) {
    my_list->node_index = malloc(sizeof(struct index));
    copy_index(my_index,my_list->node_index);
  }
  else {
    if (my_list->next == NULL) {
      if (!indices_are_same(my_index,my_list->node_index)) {
	struct subset_list* new_node = malloc(sizeof(struct subset_list));
	my_list->next = new_node;
	new_node->next = NULL;
	new_node->node_index = malloc(sizeof(struct index));
	
	if (first_precedes_second(my_index,my_list->node_index)) {
	  copy_index(my_list->node_index,new_node->node_index);
	  copy_index(my_index,my_list->node_index);
	}
	else {
	  copy_index(my_index,new_node->node_index);
	}
      }
    }
    else {
      if (first_precedes_second(my_index,my_list->node_index)) {
	  struct subset_list* new_node = malloc(sizeof(struct subset_list));
	  new_node->node_index = malloc(sizeof(struct index));
	  new_node->next = my_list->next;
	  my_list->next = new_node;
	  copy_index(my_list->node_index,new_node->node_index);
	  copy_index(my_index,my_list->node_index);
      }
      else {
	struct subset_list* probe = my_list;
	while ((probe->next)->next != NULL && !first_precedes_second(my_index,(probe->next)->node_index) &&
	       !indices_are_same(my_index,probe->node_index)) {
	  probe = probe->next;
	}
	
	if (!indices_are_same(my_index,probe->node_index) && !indices_are_same(my_index,(probe->next)->node_index)) {
	  struct subset_list* new_node = malloc(sizeof(struct subset_list));
	  new_node->node_index = malloc(sizeof(struct index));
	  copy_index(my_index,new_node->node_index);
	  
	  if (first_precedes_second(my_index,(probe->next)->node_index)) {
	    new_node->next = probe->next;
	    probe->next = new_node;
	  }
	  
	  if (first_precedes_second((probe->next)->node_index,my_index)) {
	    new_node->next = (probe->next)->next;
	    (probe->next)->next = new_node;
	  }
	}
      }
    }
  }
}

struct subset_list*  copy_of_subset_list(struct subset_list* my_list) {
  struct subset_list* copy = initialized_subset_list();

  if (!is_empty_list(my_list)) {
    add_subset(copy,my_list->node_index);
    struct subset_list* probe = my_list;
    while (probe->next != NULL) {
      probe = probe->next;
      add_subset(copy,probe->node_index);      
    }
  }

  return copy;
}

void add_second_list_to_first(struct subset_list* first, struct subset_list* second) {
  if (!is_empty_list(second)) {
    struct subset_list* probe = second;
    add_subset(first,probe->node_index);
    while (probe->next != NULL) {
      probe = probe->next;
      add_subset(first,probe->node_index);
    }
  }
}

int first_list_contains_second(struct subset_list* first, struct subset_list* second) {
  if (is_empty_list(second)) {
    return 1;
  }
  
  struct subset_list* probe = second;
  while (probe != NULL) {
    if (list_contains_index(first,probe->node_index)) {
      return 1;
    }
    probe = probe->next;
  }
  
  return 0;
}

void add_subsets_of_subset_to_list(struct subset_list* big_list, struct subset* the_subset,
				   struct subset_list* list_to_add) {
  if (!is_empty_list(list_to_add)) {
    struct subset_list* probe = list_to_add;
    
    struct subset next_subset_of_subset = subset_of_index(probe->node_index,
							  the_subset->subset_size);
    struct subset next_subset = nullset(the_subset->large_set_size);
    subset_of_subset_to_subset(&next_subset,the_subset,&next_subset_of_subset);
    
    struct index next_index = index_of_subset(&next_subset);
    add_subset(big_list,&next_index);
    destroy_index(next_index);
    destroy_subset(next_subset);
    destroy_subset(next_subset_of_subset);
    
    while (probe->next != NULL) {
      probe = probe->next;
      
      struct subset next_subset_of_subset = subset_of_index(probe->node_index,
							    the_subset->large_set_size);
      struct subset next_subset = nullset(the_subset->large_set_size);
      subset_of_subset_to_subset(&next_subset,the_subset,&next_subset_of_subset);

    if (next_subset.subset_size == 0) {
      printf("We have a problem later in the list.\n");
      exit(0);
    }
    
      struct index next_index = index_of_subset(&next_subset);
      add_subset(big_list,&next_index);
      destroy_index(next_index);
      destroy_subset(next_subset);
      destroy_subset(next_subset_of_subset);
    }
  }

}

int list_contains_index(struct subset_list* my_list, struct index* my_index) {
  if (is_empty_list(my_list)) {
    return 0;
  }
  struct subset_list* probe = my_list;
  while (probe != NULL) {
    if (indices_are_same(probe->node_index,my_index)) {
      return 1;
    }
    probe = probe->next;
  }
  return 0;
}

struct subset_list* nonempty_subsets(struct index* my_index) {
  int k, h, count, cursor;

  int no_elts = my_index-> no_elements;
  
  struct subset_list* answer = initialized_subset_list();

  int* odometer = malloc(no_elts * sizeof(int));  
  for (k = 1; k <= no_elts; k++) {
    odometer[k-1] = 0;
  }

  k = 1;
  while (k <= no_elts) {
    k = 1;
    while (odometer[k-1] == 1) {
      odometer[k-1] = 0;
      k++;
    }
    if (k <= no_elts) {
      odometer[k-1] = 1;
    
      count = 0;
      for (h = k; h <= no_elts; h++) {
	if (odometer[h-1] == 1) {
	  count++;
	}
      }
      
      struct index* next_index = malloc(sizeof(struct index));
      next_index->no_elements = count;
      next_index->indices = malloc(count * sizeof(int));
      
      cursor = 0;
      for (h = k; h <= no_elts; h++) {
	if (odometer[h-1] == 1) {
	  cursor++;
	  next_index->indices[cursor-1] = my_index->indices[h-1];
	}
      }
      add_subset(answer,next_index);
      destroy_index(*next_index);
      free(next_index);
    }
  }
  return answer;
}

struct subset_list* reduced_subset_list(struct subset_list* my_list, struct subset* my_subset) {
  int j, done;

  int* indices_list = indices_of_elements(my_subset);
  
  struct subset_list* reduced_list = initialized_subset_list();
  struct subset_list* probe = my_list;

  done = 0;
  if (is_empty_list(my_list)) {
    done = 1;
  }
  while (!done) {
    int is_subset = 1;
    for (j = 1; j <= probe->node_index->no_elements; j++) {
      if (my_subset->indicator[probe->node_index->indices[j-1]-1] == 0) {
	is_subset = 0;
      }
    }
    if (is_subset) {
      struct index new_index = index_of_subset_from_indices_list(probe->node_index, indices_list);
      add_subset(reduced_list,&new_index);
    }
    if (probe->next == NULL) {
      done = 1;
    }
    else {
      probe = probe->next;
    }
  }

  free(indices_list);

  return reduced_list;
}


struct subset_list* immediate_supersets(struct index* my_index, struct square_matrix* related) {
  int j, k, qualified, nsc;
  
  nsc = related->dimension;
  
  struct subset_list* list_of_supersets = initialized_subset_list();

  for (j = 1; j <= nsc; j++) {
    qualified = 1;
    if (related->entries[j-1][j-1] == 0) {
      qualified = 0;
    }
    if (qualified) {
      for (k = 1; k <= my_index->no_elements; k++) {
	if (my_index->indices[k-1] == j) {
	  qualified = 0;
	}
      }
    }
    if (qualified) {
      qualified = 0;
      for (k = 1; k <= my_index->no_elements; k++) {
	if (related->entries[my_index->indices[k-1]-1][j-1] == 1) {
	  qualified = 1;
	}
      }
    }
    if (qualified) {
      struct index next_superset = index_with_element_added(my_index,j);
      
      /*
      printf("The next_superset is ");
      print_index(&next_superset);
      printf(".\n");
      */
      
      add_subset(list_of_supersets,&next_superset);
    }
  }
  
  return list_of_supersets;
}

struct subset_list* immediate_supersets_of_list(struct subset_list* my_list, struct square_matrix* related) {
  struct subset_list* list_of_supersets = initialized_subset_list();
  if (!is_empty_list(my_list)) {
    struct subset_list* probe = my_list;
    add_second_list_to_first(list_of_supersets,immediate_supersets(probe->node_index,related));
    while (probe->next != NULL) {
      probe = probe->next;
      add_second_list_to_first(list_of_supersets,immediate_supersets(probe->node_index,related));
    }
  }
  return list_of_supersets;
}

struct subset_list* expanded_list(struct subset_list* my_list, struct square_matrix* related) {
  int j, nsc, done;

  nsc = related->dimension;
  
  struct subset_list* expansion = initialized_subset_list();

  int zero_popular = 1;

  for (j = 1; j <= nsc; j++) {
    if (related->entries[j-1][j-1] == 1) {
      zero_popular = 0;
    }
  }

  if (zero_popular == 1) {
    for (j = 1; j <= nsc; j++) {
      struct index new_index = singleton_index(j);
      add_subset(expansion,&new_index);
    }
  }

  else {
    for (j = 1; j <= nsc; j++) {
      if (related->entries[j-1][j-1]) {
	struct index new_index = singleton_index(j);
	add_subset(expansion,&new_index);
      }
    }

    if (!is_empty_list(my_list)) {

      struct subset_list* probe = my_list;
      done = 0;
      while (!done) {
	add_subset(expansion,probe->node_index);
	struct subset_list* list_of_subsets = immediate_supersets(probe->node_index,related);
	add_second_list_to_first(expansion,list_of_subsets);
	destroy_subset_list(list_of_subsets); 
	if (probe->next == NULL) {
	  done = 1;
	}
	else {	  
	  probe = probe->next;
	}
      }
    }
  }

  return expansion;
} 


int reset_probe_and_deficit_from_end(struct subset* my_subset, int* probe_ptr, int* deficit_ptr) {
  
  while (*probe_ptr >= 1 && my_subset->indicator[*probe_ptr - 1] == 1) {
    my_subset->indicator[*probe_ptr - 1] = 0;
    (*probe_ptr)--;
    (*deficit_ptr)++;
  }
  while (*probe_ptr >= 1 && my_subset->indicator[*probe_ptr - 1] == 0) {
    (*probe_ptr)--;
  }
  if (*probe_ptr == 0) {
    return 0;
  }
  else {
    my_subset->indicator[*probe_ptr - 1] = 0;
    (*probe_ptr)++;
    my_subset->indicator[*probe_ptr - 1] = 1;
  }

  return 1;
}

int turn_odometer_until_hit(struct subset* my_subset, struct subset* target,
			    struct square_matrix* related, int* probe_ptr, int* deficit_ptr) {
  int large_size = my_subset->large_set_size;

  while (*deficit_ptr > 0 || !is_connected(my_subset,related) ||
	 subsets_are_disjoint(my_subset,target)) {
    if (*deficit_ptr > 0) {
      if (*probe_ptr == large_size) {
	return 0;
      }
      (*probe_ptr)++;
      my_subset->indicator[*probe_ptr - 1] = 1;
      (*deficit_ptr)--;
    }
    else {
      if (*probe_ptr < large_size) {
	my_subset->indicator[*probe_ptr - 1] = 0;
	(*probe_ptr)++;
	my_subset->indicator[*probe_ptr - 1] = 1;
      }
      else {
	if (!reset_probe_and_deficit_from_end(my_subset,probe_ptr,deficit_ptr)) {
	  return 0;
	}
      }
    }
  }

  return 1;
}

int alternative_targeted_first_subset(struct subset* my_subset, struct subset* target,
				      struct square_matrix* related, int set_size) {

  int probe = 0;
  int deficit = set_size;

  my_subset->subset_size = set_size;

  if (!turn_odometer_until_hit(my_subset,target,related,&probe,&deficit)) {
    return 0;
  }

  return 1;
}

int alternative_targeted_next_subset(struct subset* my_subset, struct subset* target,
				     struct square_matrix* related, int set_size) {
  int large_size = my_subset->large_set_size;

  if (my_subset->subset_size == 0) {
    return alternative_targeted_first_subset(my_subset, target, related, set_size);
  }

  int probe = 0;
  int deficit = set_size;

  while (deficit > 0) {
    probe++;
    if (my_subset->indicator[probe-1] == 1) {
      deficit--;
    }
  }
  
  if (probe < large_size) {
    my_subset->indicator[probe - 1] = 0;
    probe++;
    my_subset->indicator[probe - 1] = 1;
  }
  else {
    while (probe >= 1 && my_subset->indicator[probe - 1] == 1) {
      my_subset->indicator[probe - 1] = 0;
      probe--;
      deficit++;
    }
    while (probe >= 1 && my_subset->indicator[probe - 1] == 0) {
      probe--;
    }
    if (probe == 0) {
      return 0;
    }
    else {
      my_subset->indicator[probe - 1] = 0;
      probe++;
      my_subset->indicator[probe - 1] = 1;
    }
  }

  if (!turn_odometer_until_hit(my_subset,target,related,&probe,&deficit)) {
    return 0;
  }

  return 1;
}

int* targeted_ordered_list_from_subset(struct subset* my_subset, struct subset* target,
			      struct square_matrix* related) {
  int j, k, l, done;
  
  int* ordered_list = malloc(my_subset->subset_size * sizeof(int));

  j = 0;
  done = 0;
  while (!done) {
    j++;
    if (my_subset->indicator[j-1] == 1 && target->indicator[j-1] == 1) {
      done = 1;
      ordered_list[0] = j;
    }
  }

  for (k = 2; k <= my_subset->subset_size; k++) {
    j = 0;
    done = 0;
    while (!done) {
      j++;
      
      if (my_subset->indicator[j-1] == 1) {
	done = 1;
      }
      if (done) {
	for (l = 1; l < k; l++) {
	  if (j == ordered_list[l-1]) {
	    done = 0;
	  }
	}
      }
      if (done) {
	done = 0;
	for (l = 1; l < k; l++) {
	  if (related->entries[j-1][ordered_list[l-1]-1] == 1) {
	    done = 1;
	  }
	}
      }
    }
    ordered_list[k-1] = j;
  }

  return ordered_list;
}

int targeted_is_qualified(int j, struct subset* target, struct square_matrix* related,
		     int* candidate_list, int probe) {
  int k,l;

  if (probe == 1) {
    if (target->indicator[j-1] == 1) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  for (k = 1; k < probe; k++) {
    if (j == candidate_list[k-1] || (target->indicator[j-1] == 1 && j < candidate_list[k-1])) {
      return 0;
    }
  }
  
  for (k = 1; k < probe; k++) {
    if (related->entries[j-1][candidate_list[k-1]-1] == 1) {
      for (l = k+1; l < probe; l++) {
	if (j < candidate_list[l-1]) {
	  return 0;
	}
      }
      return 1;
    }
  }

  return 0;
}

int targeted_first_subset(struct subset* my_subset, struct subset* target,
		     struct square_matrix* related, int set_size) {

  int j, k, probe;
  int nsc = my_subset->large_set_size;

  int* candidate_list = malloc(set_size * sizeof(int));
  for (j = 1; j <= set_size; j++) {
    candidate_list[j-1] = 0;
  }
      
  probe = 1;
  
  while (probe > 0 && probe <= set_size) {
    j = candidate_list[probe-1]+1;
    
    while (j <= nsc && !targeted_is_qualified(j, target, related, candidate_list, probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      candidate_list[probe-1] = j;      
      probe++;
    }
    else {
      candidate_list[probe-1] = 0;
      probe--;
    }
  }
  

  if (probe > set_size) {
    my_subset->subset_size = set_size;
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    for (k = 1; k <= set_size; k++) {      
      my_subset->indicator[candidate_list[k-1]-1] = 1;      
    }
    free(candidate_list);
    return 1;
  }

  free(candidate_list);
  return 0;
}

int targeted_next_subset(struct subset* my_subset, struct subset* target,
		    struct square_matrix* related, int set_size) {

  if (my_subset->subset_size == 0) {
    return targeted_first_subset(my_subset, target, related, set_size);
  }

  int j, k, probe;
  int nsc = my_subset->large_set_size;

  int* ordered_list = targeted_ordered_list_from_subset(my_subset,target,related);
      
  probe = set_size;
  
  while (probe > 0 && probe <= set_size) {
    j = ordered_list[probe-1]+1;
    
    while (j <= nsc && !targeted_is_qualified(j, target, related, ordered_list, probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      ordered_list[probe-1] = j;
      probe++;
    }
    else {
      ordered_list[probe-1] = 0;
      probe--;
    }
  } 

  if (probe > set_size) {
    my_subset->subset_size = set_size;
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    for (k = 1; k <= set_size; k++) {      
      my_subset->indicator[ordered_list[k-1]-1] = 1;      
    }
    free(ordered_list);

    if (!is_connected(my_subset,related)) {
      printf("We have a disconnected subset coming out of next_subset.\n");
      exit(0);
    }
  
    return 1;
  }

  free(ordered_list);
  return 0;
} 


int* ordered_list_from_subset(struct subset* my_subset, struct square_matrix* related) {
  int j, k, l, done;
  
  int* ordered_list = malloc(my_subset->subset_size * sizeof(int));

  j = 0;
  done = 0;
  while (!done) {
    j++;
    if (my_subset->indicator[j-1] == 1) {
      done = 1;
      ordered_list[0] = j;
    }
  }

  for (k = 2; k <= my_subset->subset_size; k++) {
    j = 0;
    done = 0;
    while (!done) {
      j++;
      
      if (my_subset->indicator[j-1] == 1) {
	done = 1;
      }
      if (done) {
	for (l = 1; l < k; l++) {
	  if (j == ordered_list[l-1]) {
	    done = 0;
	  }
	}
      }
      if (done) {
	done = 0;
	for (l = 1; l < k; l++) {
	  if (related->entries[j-1][ordered_list[l-1]-1] == 1) {
	    done = 1;
	  }
	}
      }
    }
    ordered_list[k-1] = j;
  }

  return ordered_list;
}

int is_qualified(int j, struct square_matrix* related, int* candidate_list, int probe) {
  int k,l;

  if (probe == 1) {
      return 1;
  }
  
  for (k = 1; k < probe; k++) {
    if (j == candidate_list[k-1] || j < candidate_list[k-1]) {
      return 0;
    }
  }
  
  for (k = 1; k < probe; k++) {
    if (related->entries[j-1][candidate_list[k-1]-1] == 1) {
      for (l = k+1; l < probe; l++) {
	if (j < candidate_list[l-1]) {
	  return 0;
	}
      }
      return 1;
    }
  }

  return 0;
}

int first_subset(struct subset* my_subset, struct square_matrix* related, int set_size) {

  int j, k, probe;
  int nsc = my_subset->large_set_size;

  int* candidate_list = malloc(set_size * sizeof(int));
  for (j = 1; j <= set_size; j++) {
    candidate_list[j-1] = 0;
  }
      
  probe = 1;
  
  while (probe > 0 && probe <= set_size) {
    j = candidate_list[probe-1]+1;
    
    while (j <= nsc && !is_qualified(j, related, candidate_list, probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      candidate_list[probe-1] = j;      
      probe++;
    }
    else {
      candidate_list[probe-1] = 0;
      probe--;
    }
  }
  

  if (probe > set_size) {
    my_subset->subset_size = set_size;
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    for (k = 1; k <= set_size; k++) {      
      my_subset->indicator[candidate_list[k-1]-1] = 1;      
    }
    free(candidate_list);
    return 1;
  }

  free(candidate_list);
  return 0;
}

int next_subset(struct subset* my_subset, struct square_matrix* related, int set_size) {

  if (my_subset->subset_size == 0) {
    return first_subset(my_subset, related, set_size);
  }

  int j, k, probe;
  int nsc = my_subset->large_set_size;

  int* ordered_list = ordered_list_from_subset(my_subset,related);
      
  probe = set_size;
  
  while (probe > 0 && probe <= set_size) {
    j = ordered_list[probe-1]+1;
    
    while (j <= nsc && !is_qualified(j, related, ordered_list, probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      ordered_list[probe-1] = j;
      probe++;
    }
    else {
      ordered_list[probe-1] = 0;
      probe--;
    }
  } 

  if (probe > set_size) {
    my_subset->subset_size = set_size;
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    for (k = 1; k <= set_size; k++) {      
      my_subset->indicator[ordered_list[k-1]-1] = 1;      
    }
    free(ordered_list);

    if (!is_connected(my_subset,related)) {
      printf("We have a disconnected subset coming out of next_subset.\n");
      exit(0);
    }
  
    return 1;
  }

  free(ordered_list);
  return 0;
}


int new_is_qualified(int j, struct subset* target, struct square_matrix* related,
		     int* candidate_list, int probe) {
  int k,l;

  if (probe == 1) {
    if (target->indicator[j-1] == 1) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  for (k = 1; k < probe; k++) {
    if (j == candidate_list[k-1] || (target->indicator[j-1] == 1 && j < candidate_list[k-1])) {
      return 0;
    }
  }
  
  for (k = 1; k < probe; k++) {
    if (related->entries[j-1][candidate_list[k-1]-1] == 1) {
      for (l = k+1; l < probe; l++) {
	if (j < candidate_list[l-1]) {
	  return 0;
	}
      }
      return 1;
    }
  }

  return 0;
}


int new_first_subset(struct subset* my_subset, struct subset* target,
		     struct square_matrix* related, int set_size) {

  int j, k, probe;
  int nsc = my_subset->large_set_size;

  int* candidate_list = malloc((set_size - 1) * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    candidate_list[j-1] = 0;
  }
      
  probe = 1;
  
  while (probe > 0 && probe < set_size) {
    j = candidate_list[probe-1]+1;
    
    while (j <= nsc && !new_is_qualified(j, target, related, candidate_list, probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      candidate_list[probe-1] = j;
      probe++;
    }
    else {
      candidate_list[probe-1] = 0;
      probe--;
    }
  } 

  if (probe > set_size) {
    my_subset->subset_size = set_size;
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    for (k = 1; k <= set_size; k++) {      
      my_subset->indicator[candidate_list[k-1]-1] = 1;      
    }
    free(candidate_list);
    return 1;
  }

  free(candidate_list);
  return 0;
}

int new_next_subset(struct subset* my_subset, struct subset* target,
		    struct square_matrix* related, int set_size) {

  if (my_subset->subset_size == 0) {
    return new_first_subset(my_subset, target, related, set_size);
  }

  int j, k, cursor, probe;
  int nsc = my_subset->large_set_size;

  int* candidate_list = malloc((set_size - 1) * sizeof(int));
  cursor = 0;
  for (j = 1; j <= nsc; j++) {
    if (my_subset->indicator[j-1] == 1) {
      cursor++;
      candidate_list[cursor-1] = j;
    }
  }
      
  probe = set_size;
  
  while (probe > 0 && probe < set_size) {
    j = candidate_list[probe-1]+1;
    
    while (j <= nsc && !new_is_qualified(j, target, related, candidate_list, probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      candidate_list[probe-1] = j;
      probe++;
    }
    else {
      candidate_list[probe-1] = 0;
      probe--;
    }
  } 

  if (probe > set_size) {
    my_subset->subset_size = set_size;
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    for (k = 1; k <= set_size; k++) {      
      my_subset->indicator[candidate_list[k-1]-1] = 1;      
    }
    free(candidate_list);
    return 1;
  }

  free(candidate_list);
  return 0;
} 
