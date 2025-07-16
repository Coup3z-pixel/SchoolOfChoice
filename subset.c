#include "subset.h"

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

struct index singleton_index(int i) {
  struct index answer;
  
  answer.no_elements = 1;
  answer.indices = malloc(sizeof(int));
  answer.indices[0] = i;

  return answer;
}

void becomes_nullset(struct subset* my_set) {
  my_set->subset_size = 0;
  for (int i = 1; i <= my_set->large_set_size; i++) {
    my_set->indicator[i-1] = 0;
  }
}

void becomes_singleton(struct subset* my_set, int elmnt) {
  for (int i = 1; i <= my_set->large_set_size; i++) {
    my_set->indicator[i-1] = 0;
  }
  my_set->indicator[elmnt-1] = 1;
}

void copy_subset(struct subset* given_subset, struct subset* copy_subset) {
  int i;
  
  copy_subset->subset_size = given_subset->subset_size;
  for (i = 1; i <= given_subset->large_set_size; i++) {
    copy_subset->indicator[i-1] = given_subset->indicator[i-1];
  }
}

struct subset copy_of_subset(struct subset* given_subset) {
  int i;

  struct subset answer;

  answer.large_set_size = given_subset->large_set_size;
  
  answer.subset_size = given_subset->subset_size;
  
  answer.indicator = malloc(given_subset->large_set_size * sizeof(int));
  for (i = 1; i <= given_subset->large_set_size; i++) {
    answer.indicator[i-1] = given_subset->indicator[i-1];
  }

  return answer;
}

struct subset complement_of_subset(struct subset* given_subset)  {
  struct subset complement;
  
  complement.large_set_size = given_subset->large_set_size;
  complement.subset_size = given_subset->large_set_size - given_subset->subset_size;
  complement.indicator = malloc(given_subset->large_set_size * sizeof(int));
  for (int i = 1; i <= given_subset->large_set_size; i++) {
    complement.indicator[i-1] = 1 - given_subset->indicator[i-1];
  }

  return complement;
}

int is_nullset(struct subset* my_set)  {
  if (my_set->subset_size == 0) {
    return 1;
  }
  else {
    return 0;
  }
}

int is_fullset(struct subset* my_set)  {
  if (my_set->subset_size == my_set->large_set_size) {
    return 1;
  }
  else {
    return 0;
  }
}

int is_element(struct subset* my_set, int i) {
  if (1 <= i && i <= my_set->large_set_size && my_set->indicator[i-1] == 1) {
    return 1;
  }
  else {
    return 0;
  }
}

void add_element(struct subset* my_set, int new_elt) {
  if (new_elt < 0 || new_elt > my_set->large_set_size) {
    fprintf(stderr, "Attempt to add out-of-range element to set.");
    exit(0);
  }
  if (my_set->indicator[new_elt-1] == 1) {
    fprintf(stderr, "Attempt to add existing element to set.\n");
    exit(0);
  }
  my_set->subset_size++;
  my_set->indicator[new_elt-1] = 1;
}

void remove_element(struct subset* my_set, int old_elt) {
  if (old_elt < 0 || old_elt > my_set->large_set_size) {
    fprintf(stderr, "Attempt to remove out-of-range element from set.");
    exit(0);
  }
  if (my_set->indicator[old_elt-1] == 0) {
    fprintf(stderr, "Attempt to remove nonelement from set.\n");
    exit(0);
  }
  my_set->subset_size--;
  my_set->indicator[old_elt-1] = 0;
}

void remove_all_elements(struct subset* my_set) {
  my_set->subset_size = 0;
  for (int i = 1; i <= my_set->large_set_size; i++) {
    my_set->indicator[i-1] = 0;
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

int* subset_element_numbers(struct subset* my_subset) {
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

struct index index_of_subset(struct subset* my_subset) {
  int i, k;
  struct index my_index;

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

struct index index_of_complement(struct subset* my_subset) {
  int i, k;
  struct index my_index;

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

int index_has_element(struct index* my_ind, int elt) {
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


struct index* copy_of_index(struct index* given_index) {
  struct index* copy = malloc(sizeof(struct index));
  int no_elts = given_index->no_elements;
  
  copy->no_elements = no_elts;

  copy->indices = malloc(no_elts * sizeof(int));
  for (int i = 1; i <= no_elts; i++) {
    copy->indices[i-1] = given_index->indices[i-1];
  }

  return copy;
}

struct index index_of_fullset(int large_set_size) {
  struct index full_index;
  full_index.no_elements = large_set_size;
  full_index.indices = malloc(large_set_size * sizeof(int));
  for (int i = 1; i <= large_set_size; i++) {
    full_index.indices[i-1] = i;
  }

  return full_index;
}

/*
struct index singleton_index(int j) {
  struct index singleton;
  singleton.no_elements = 1;
  singleton.indices = malloc(sizeof(int));
  singleton.indices[0] = j;
  return singleton;
}
*/

void add_element_to_index(struct index* index_ptr, int elt) {
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

void remove_element_from_index(struct index* index_ptr, int elt) {
  int m, n, hit;
  int* new_indices;
  
  n = index_ptr->no_elements;

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

void add_element_to_index_ptr(struct index** index_ptr, int elt) {
  if (*index_ptr == NULL) {
    *index_ptr = malloc(sizeof(struct index));
    (*index_ptr)->no_elements = 1;
    (*index_ptr)->indices = malloc(sizeof(int));
    (*index_ptr)->indices[0] = elt;
  }
  else {
    add_element_to_index(*index_ptr, elt);
  }
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

struct index* reverse_of_correspondence_graph(struct index* my_list,
					      struct index* active_range_elts,
					      int no_domain_elts) {
  int i, k;
  
  struct subset next_subset = nullset(no_domain_elts);
  
  struct index* new_list = malloc(active_range_elts->no_elements * sizeof(struct index));

  for (k = 1; k <= active_range_elts->no_elements; k++) {
    becomes_nullset(&next_subset);
    for (i = 1; i <= no_domain_elts; i++) {
      if (index_has_element(&(my_list[i-1]),active_range_elts->indices[k-1])
	  && next_subset.indicator[i-1] == 0) {
	next_subset.subset_size++;
	next_subset.indicator[i-1] = 1;
      }
    }
    
    new_list[k-1] = index_of_subset(&next_subset);
  }

  destroy_subset(next_subset);

  return new_list;
}

struct index_list* initialized_index_list() {
  struct index_list* new_list = malloc(sizeof(struct index_list));
  new_list->node_index = NULL;
  new_list->next = NULL;
  return new_list;
}

int index_list_is_empty(struct index_list* my_list) {
  if (my_list->node_index == NULL && my_list-> next == NULL) {
    return 1;
  }
  else {
    return 0;
  }
}

int length_of_index_list(struct index_list* my_list) {
  int length = 0;
  if (my_list->node_index == NULL) {
    length = 0;
  }
  else {
    length = 1;
  }
  struct index_list* probe = my_list;
  while (probe->next != NULL) {
    length++;
    probe = probe->next;
  }
  return length;
}

int element_of_list_set(struct index_list* my_list, int set_no, int elt_no) {
  int k = 1;

  struct index_list* probe = my_list;
  while (k < set_no) {
    probe = probe->next;
    k++;
  }

  return (probe->node_index)->indices[elt_no-1];
}

void add_index_to_index_list(struct index_list* my_list, struct index* my_index) {
  if (my_list->node_index == NULL) {
    my_list->node_index = copy_of_index(my_index);    
  }
  else {
    struct index_list* probe = my_list;
    while (probe->next != NULL) {
      probe = probe->next;
    }
    probe->next = malloc(sizeof(struct index_list));
    (probe->next)->next = NULL;
    (probe->next)->node_index = copy_of_index(my_index);
  }
}

struct subset union_of_indices(struct index* my_list, int large_set_size, int list_length) {
  int i, k;
  
  struct subset my_union = nullset(large_set_size);
  
  for (i = 1; i <= list_length; i++) {
    for (k = 1; k <= my_list[i-1].no_elements; k++) {
      if (my_union.indicator[my_list[i-1].indices[k-1]-1] == 0) {
	add_element(&my_union,my_list[i-1].indices[k-1]);
      }
    }
  }

  return my_union;  
}



void print_vector_of_ints(int* vector, int dim) {
  fprintf(stderr, "(");
  for (int i = 1; i <= dim-1; i++) {
    fprintf(stderr, "%d,",vector[i-1]);
  }
  fprintf(stderr, "%d)",vector[dim-1]);
}

void print_subset(struct subset* my_subset) {
  int i;
  fprintf(stderr, "(");
  for (i = 1; i < my_subset->large_set_size; i++) {
    fprintf(stderr, "%d,",my_subset->indicator[i-1]);
  }
  fprintf(stderr, "%d)",my_subset->indicator[my_subset->large_set_size-1]);
}

void print_index(struct index* my_index) {
  int i;
  
  fprintf(stderr, "(");
  
  for (i = 1; i < my_index->no_elements; i++) {
    fprintf(stderr, "%d,",my_index->indices[i-1]);
  }
  fprintf(stderr, "%d)",my_index->indices[my_index->no_elements-1]);
}

void print_index_list(struct index_list* my_list) {
  if (index_list_is_empty(my_list)) {
    fprintf(stderr, "null_list");
  }
  else {
    struct index_list* probe = my_list;
    print_index(probe->node_index); 
    while (probe->next != NULL) {
      print_index(probe->next->node_index); 
      probe = probe->next;
    }
  }
}

void print_index_of_subset(struct subset* my_subset) {
  struct index the_index = index_of_subset(my_subset);
  print_index(&the_index);
  destroy_index(the_index);
}

void destroy_subset(struct subset my_subset) {
  free(my_subset.indicator);
}

void destroy_index(struct index my_index) {
  free(my_index.indices);
}

void destroy_index_list(struct index_list* my_list) {
  struct index_list* probe = my_list;
  while (probe->next != NULL) {
    struct index_list* next_probe = probe->next;
    if (probe->node_index != NULL) {
      destroy_index(*(probe->node_index));
      free(probe->node_index);
    }
    free(probe);
    probe = next_probe;
  }
  if (probe->node_index != NULL) {
    destroy_index(*(probe->node_index));
    free(probe->node_index);
  }
  free(probe);
}

