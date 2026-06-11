#include "subset.h"

subset nullset(int large_set_size) {
  int i;
  subset my_subset;
  my_subset.large_set_size = large_set_size;
  my_subset.subset_size = 0;
  my_subset.indicator = malloc(large_set_size * sizeof(int));
  for (i = 1; i <= large_set_size; i++) {
    my_subset.indicator[i-1] = 0;
  }
  
  return my_subset;
}

subset fullset(int large_set_size) {
  int i;
  subset my_subset;
  my_subset.large_set_size = large_set_size;
  my_subset.subset_size = large_set_size;
  my_subset.indicator = malloc(large_set_size * sizeof(int));
  for (i = 1; i <= large_set_size; i++) {
    my_subset.indicator[i-1] = 1;
  }
  return my_subset;
}

subset singleton_subset(int elmt, int large_size) {
  subset singleton;
  singleton.large_set_size = large_size;
  singleton.subset_size = 1;
  singleton.indicator = malloc(large_size * sizeof(int));
  for (int j = 1; j <= large_size; j++) {
    singleton.indicator[j-1] = 0;
  }
  singleton.indicator[elmt - 1] = 1;
  return singleton;
}

element_list singleton_element_list(int i) {
  element_list answer;
  
  answer.no_elements = 1;
  answer.indices = malloc(sizeof(int));
  answer.indices[0] = i;

  return answer;
}

void becomes_nullset(subset* my_set) {
  my_set->subset_size = 0;
  for (int i = 1; i <= my_set->large_set_size; i++) {
    my_set->indicator[i-1] = 0;
  }
}

void becomes_singleton(subset* my_set, int elmnt) {
  for (int i = 1; i <= my_set->large_set_size; i++) {
    my_set->indicator[i-1] = 0;
  }
  my_set->indicator[elmnt-1] = 1;
}

void copy_subset(subset* given_subset, subset* copy_subset) {
  int i;
  
  copy_subset->subset_size = given_subset->subset_size;
  for (i = 1; i <= given_subset->large_set_size; i++) {
    copy_subset->indicator[i-1] = given_subset->indicator[i-1];
  }
}

subset copy_of_subset(subset* given_subset) {
  int i;

  subset answer;

  answer.large_set_size = given_subset->large_set_size;
  
  answer.subset_size = given_subset->subset_size;
  
  answer.indicator = malloc(given_subset->large_set_size * sizeof(int));
  for (i = 1; i <= given_subset->large_set_size; i++) {
    answer.indicator[i-1] = given_subset->indicator[i-1];
  }

  return answer;
}

subset complement_of_subset(subset* given_subset)  {
  subset complement;
  
  complement.large_set_size = given_subset->large_set_size;
  complement.subset_size = given_subset->large_set_size - given_subset->subset_size;
  complement.indicator = malloc(given_subset->large_set_size * sizeof(int));
  for (int i = 1; i <= given_subset->large_set_size; i++) {
    complement.indicator[i-1] = 1 - given_subset->indicator[i-1];
  }

  return complement;
}

int is_nullset(subset* my_set)  {
  if (my_set->subset_size == 0) {
    return 1;
  }
  else {
    return 0;
  }
}

int is_fullset(subset* my_set)  {
  if (my_set->subset_size == my_set->large_set_size) {
    return 1;
  }
  else {
    return 0;
  }
}

int is_element(subset* my_set, int i) {
  if (1 <= i && i <= my_set->large_set_size && my_set->indicator[i-1] == 1) {
    return 1;
  }
  else {
    return 0;
  }
}

void add_element(subset* my_set, int new_elt) {
  if (!is_element(my_set, new_elt)) {
    my_set->subset_size++;
    my_set->indicator[new_elt-1] = 1;
  }
}

void add_subset(subset* my_set, subset* increment)  {
  int i;
  
  for (i = 1; i <= my_set->large_set_size; i++) {
    if (is_element(increment, i)) {
      add_element(my_set, i);
    }
  }
}

void remove_subset(subset* my_set, subset* decrement)  {
  int i;
  
  for (i = 1; i <= my_set->large_set_size; i++) {
    if (is_element(decrement, i)) {
      remove_element(my_set, i);
    }
  }
}

void remove_element(subset* my_set, int old_elt) {
  if (is_element(my_set, old_elt)) {
    my_set->subset_size--;
    my_set->indicator[old_elt-1] = 0;
  }
}

void remove_all_elements(subset* my_set) {
  my_set->subset_size = 0;
  for (int i = 1; i <= my_set->large_set_size; i++) {
    my_set->indicator[i-1] = 0;
  }
}

int subsets_are_same(subset* first, subset* second) {
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

int subsets_are_disjoint(subset* first, subset* second) {
  for (int j = 1; j <= first->large_set_size; j++) {
    if (first->indicator[j-1] == 1 && second->indicator[j-1] == 1) {
      return 0;
    }
  }
  return 1;
}


void complement(subset* given_subset, subset* comp) {
  int i;
  comp->large_set_size = given_subset->large_set_size;
  comp->subset_size = given_subset->large_set_size - given_subset->subset_size;
  for (i = 1; i <= given_subset->large_set_size; i++) {
    comp->indicator[i-1] = 1 - given_subset->indicator[i-1];
  }
}

int* subset_element_numbers(subset* my_subset) {
  int i;
  int* numbers = malloc(my_subset->large_set_size * sizeof(int));
  int counter = 0;
  for (i = 1; i <= my_subset->large_set_size; i++) {
    if (my_subset->indicator[i-1]) {
      counter++;
      numbers[i-1] = counter;
    }
    else {
      numbers[i-1] = 0;
    }
  }

  return numbers;
}

element_list element_list_of_subset(subset* my_subset) {
  int i, k;
  element_list my_index;

  if (my_subset->subset_size == 0) {
    fprintf(stderr, "Taking the index of an emptyset is not permitted.\n");
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

element_list element_list_of_complement(subset* my_subset) {
  int i, k;
  element_list my_index;

  if (my_subset->subset_size == my_subset->large_set_size) {
    fprintf(stderr, "Taking the index of an emptyset is not permitted.\n");
    exit(0);	    
  }
  
  my_index.no_elements = my_subset->large_set_size - my_subset->subset_size;
  
  my_index.indices = malloc(my_index.no_elements * sizeof(int));
  k = 0;
  for (i = 1; i <= my_index.no_elements; i++) {
    k++;
    while (my_subset->indicator[k-1] == 1) {
      k++;
    }
    my_index.indices[i-1] = k;
  }
  
  return my_index;
}

subset subset_of_element_list(element_list* my_index, int large_set_size) {
  subset answer = nullset(large_set_size);
  
  answer.subset_size = my_index->no_elements;
  
  for (int k = 1; k <= my_index->no_elements; k++) {
    answer.indicator[my_index->indices[k-1]-1] = 1;
  }

  return answer;
}

void subset_of_subset_to_subset(subset* new_subset, subset* given_subset,
				subset* subset_of_subset) {
  int k,l;
  
  element_list element_list_of_given = element_list_of_subset(given_subset);

  new_subset->subset_size = 0;
  for (k = 1; k <= new_subset->large_set_size; k++) {
    new_subset->indicator[k-1] = 0;
  }
  
  for (l = 1; l <= given_subset->subset_size; l++) {
    if (subset_of_subset->indicator[l-1] == 1) {
      new_subset->subset_size++;
      new_subset->indicator[element_list_of_given.indices[l-1]-1] = 1;
    }
  }
}

int* indices_of_elements(subset* my_subset) {
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

int indices_are_same(element_list* first, element_list* second) {
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

int element_list_has_element(element_list* my_ind, int elt) {
  if (my_ind->no_elements == 0) {
    return 0;
  }
  for (int j = 1; j <= my_ind->no_elements; j++) {
    if (my_ind->indices[j-1] == elt) {
      return 1;
    }
  }
  return 0;

}


element_list* copy_of_element_list(element_list* given_index) {
  element_list* copy = malloc(sizeof(element_list));
  int no_elts = given_index->no_elements;
  
  copy->no_elements = no_elts;

  copy->indices = malloc(no_elts * sizeof(int));
  for (int i = 1; i <= no_elts; i++) {
    copy->indices[i-1] = given_index->indices[i-1];
  }

  return copy;
}

element_list element_list_of_fullset(int large_set_size) {
  element_list full_index;
  full_index.no_elements = large_set_size;
  full_index.indices = malloc(large_set_size * sizeof(int));
  for (int i = 1; i <= large_set_size; i++) {
    full_index.indices[i-1] = i;
  }

  return full_index;
}

void add_element_to_element_list(element_list* index_ptr, int elt) {
  int m, n, hit;
  int* new_indices;
  
  n = index_ptr->no_elements;

  new_indices = malloc((n+1) * sizeof(int));
  hit = 0;
  for (m = 1; m <= n; m++) {
    if (!hit && index_ptr->indices[m-1] > elt) {
      new_indices[m-1] = elt;
      hit = 1;
    }
    if (!hit) {
      new_indices[m-1] = index_ptr->indices[m-1];
    }
    else {
      new_indices[m] = index_ptr->indices[m-1];
    }
  }
  if (!hit) {
    new_indices[n] = elt;
  }

  free(index_ptr->indices);

  index_ptr->no_elements = n+1;
  index_ptr->indices = new_indices;  
}

void add_element_to_possibly_NULL_element_list(element_list** index_ptr, int elt) {
  if (*index_ptr == NULL) {
    *index_ptr = malloc(sizeof(element_list));
    (*index_ptr)->no_elements = 1;
    (*index_ptr)->indices = malloc(sizeof(int));
    (*index_ptr)->indices[0] = elt;    
  }
  else {
    add_element_to_element_list(*index_ptr,elt);
  }
}

void remove_element_from_element_list(element_list* index_ptr, int elt) {
  int m, n, hit, present;
  int* new_indices;
  
  n = index_ptr->no_elements;

  present = 0;
  for (m = 1; m <= n; m++) {
    if (index_ptr->indices[m-1] == elt) {
      present = 1;
    }
  }
  if (!present) {
    fprintf(stderr, "We are trying to delete an element that is not present.\n");
    exit(0);
  }

  new_indices = malloc((n-1) * sizeof(int));
  
  hit = 0;
  for (m = 1; m <= n; m++) {
    if (!hit && index_ptr->indices[m-1] != elt) {
      new_indices[m-1] = index_ptr->indices[m-1];
    }
    if (!hit && index_ptr->indices[m-1] == elt) {
      hit = 1;
    }
    if (hit && index_ptr->indices[m-1] != elt) {
      new_indices[m-2] = index_ptr->indices[m-1];
    }
  }

  free(index_ptr->indices);

  index_ptr->no_elements = n-1;
  index_ptr->indices = new_indices;  
}

void add_element_to_element_list_ptr(element_list** index_ptr, int elt) {
  if (*index_ptr == NULL) {
    *index_ptr = malloc(sizeof(element_list));
    (*index_ptr)->no_elements = 1;
    (*index_ptr)->indices = malloc(sizeof(int));
    (*index_ptr)->indices[0] = elt;
  }
  else {
    add_element_to_element_list(*index_ptr, elt);
  }
}

element_list element_list_with_element_added(element_list* my_index, int j) {
  int k, inserted_already;
  element_list new_index;
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

element_list* reverse_of_correspondence_graph(element_list* my_list,
					      element_list* active_range_elts,
					      int no_domain_elts) {
  int i, k;
  
  subset next_subset = nullset(no_domain_elts);
  
  element_list* new_list = malloc(active_range_elts->no_elements * sizeof(element_list));

  for (k = 1; k <= active_range_elts->no_elements; k++) {
    becomes_nullset(&next_subset);
    for (i = 1; i <= no_domain_elts; i++) {
      if (element_list_has_element(&(my_list[i-1]),active_range_elts->indices[k-1])
	  && next_subset.indicator[i-1] == 0) {
	next_subset.subset_size++;
	next_subset.indicator[i-1] = 1;
      }
    }
    
    new_list[k-1] = element_list_of_subset(&next_subset);
  }

  destroy_subset(next_subset);

  return new_list;
}

list_of_elt_lists* initialized_list_of_elt_lists() {
  list_of_elt_lists* new_list = malloc(sizeof(list_of_elt_lists));
  new_list->node_index = NULL;
  new_list->next = NULL;
  return new_list;
}

int list_of_elt_lists_is_empty(list_of_elt_lists* my_list) {
  if (my_list->node_index == NULL && my_list-> next == NULL) {
    return 1;
  }
  else {
    return 0;
  }
}

int length_of_list_of_elt_lists(list_of_elt_lists* my_list) {
  int length = 0;
  if (my_list->node_index == NULL) {
    length = 0;
  }
  else {
    length = 1;
  }
  list_of_elt_lists* probe = my_list;
  while (probe->next != NULL) {
    length++;
    probe = probe->next;
  }
  return length;
}

int element_of_list_set(list_of_elt_lists* my_list, int set_no, int elt_no) {
  int k = 1;

  list_of_elt_lists* probe = my_list;
  while (k < set_no) {
    probe = probe->next;
    k++;
  }

  return (probe->node_index)->indices[elt_no-1];
}

void add_element_list_to_list_of_elt_lists(list_of_elt_lists* my_list, element_list* my_index) {
  if (my_list->node_index == NULL) {
    my_list->node_index = copy_of_element_list(my_index);    
  }
  else {
    list_of_elt_lists* probe = my_list;
    while (probe->next != NULL) {
      probe = probe->next;
    }
    probe->next = malloc(sizeof(list_of_elt_lists));
    (probe->next)->next = NULL;
    (probe->next)->node_index = copy_of_element_list(my_index);
  }
}

void add_list_of_set_to_list_of_elt_lists(list_of_elt_lists* my_list, subset* my_subset) {
  element_list my_elt_list;

  my_elt_list = element_list_of_subset(my_subset);
  add_element_list_to_list_of_elt_lists(my_list, &my_elt_list);
  destroy_element_list(my_elt_list);
}

void print_vector_of_ints(int* vector, int dim) {
  fprintf(stderr, "(");
  for (int i = 1; i <= dim-1; i++) {
    fprintf(stderr, "%d,",vector[i-1]);
  }
  fprintf(stderr, "%d)",vector[dim-1]);
}

void fprint_subset(subset* my_subset) {
  int i;
  fprintf(stderr, "(");
  for (i = 1; i < my_subset->large_set_size; i++) {
    fprintf(stderr, "%d,",my_subset->indicator[i-1]);
  }
  fprintf(stderr, "%d)",my_subset->indicator[my_subset->large_set_size-1]);
}

void print_element_list(element_list* my_index) {
  int i;
  
  fprintf(stderr, "(");
  
  for (i = 1; i < my_index->no_elements; i++) {
    fprintf(stderr, "%d,",my_index->indices[i-1]);
  }
  fprintf(stderr, "%d)",my_index->indices[my_index->no_elements-1]);
}

void print_list_of_elt_lists(list_of_elt_lists* my_list) {
  if (list_of_elt_lists_is_empty(my_list)) {
    fprintf(stderr, "null_list");
  }
  else {
    list_of_elt_lists* probe = my_list;
    print_element_list(probe->node_index); 
    while (probe->next != NULL) {
      print_element_list(probe->next->node_index); 
      probe = probe->next;
    }
  }
}

void print_element_list_of_subset(subset* my_subset) {
  element_list the_index = element_list_of_subset(my_subset);
  print_element_list(&the_index);
  destroy_element_list(the_index);
}

void destroy_subset(subset my_subset) {
  free(my_subset.indicator);
}

void destroy_element_list(element_list my_index) {
  free(my_index.indices);
}

void destroy_list_of_elt_lists(list_of_elt_lists* my_list) {
  list_of_elt_lists* probe = my_list;
  while (probe->next != NULL) {
    list_of_elt_lists* next_probe = probe->next;
    if (probe->node_index != NULL) {
      destroy_element_list(*(probe->node_index));
      free(probe->node_index);
    }
    free(probe);
    probe = next_probe;
  }
  if (probe->node_index != NULL) {
    destroy_element_list(*(probe->node_index));
    free(probe->node_index);
  }
  free(probe);
}

