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

void augment_subset_sizes(int* subset_sizes, struct subset* overallocated_schools) {
  int j, min, nsc;

  nsc = overallocated_schools->large_set_size;
  min = 0;
  for (j = 1; j <= nsc; j++) {
    if (overallocated_schools->indicator[j-1] && min < subset_sizes[j-1]) {
      min = subset_sizes[j-1];
    }
  }
  
  for (j = 1; j <= nsc; j++) {
    if (overallocated_schools->indicator[j-1] && subset_sizes[j-1] == min) {
      subset_sizes[j-1]++;
    }
  }
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

int subset_has_right_subset_size(struct subset* my_subset) {
  int no_elements = 0;

  for (int i = 1; i <= my_subset->large_set_size; i++) {
    if (my_subset->indicator[i-1] == 1) {
      no_elements++;
    }
  }

  if (no_elements < my_subset->subset_size) {
    printf("The set has fewer than subset_size actual elements.\n");
    return 0;
  }

  if (no_elements > my_subset->subset_size) {
    printf("The set has more than subset_size actual elements.\n");
    return 0;
  }

  return 1;
}

int subset_is_connected(struct subset* my_subset, struct square_matrix* related) {
  int j,k;
  
  int size = my_subset->subset_size;

  if (size <= 1) {
    return 1;
  }
  
  struct index the_index = index_of_subset(my_subset);
  int* connected_nodes = malloc(size * sizeof(int));
  connected_nodes[0] = 1;
  for (j = 2; j <= size; j++) {
    connected_nodes[j-1] = 0;
  }
  
  int done = 0;
  while (!done) {
    done = 1;
    for (j = 2; j <= size; j++) {
      if (connected_nodes[j-1] == 0) {
	for (k = 1; k <= size; k++) {
	  if (connected_nodes[k-1] == 1 && related->entries[the_index.indices[j-1]-1][the_index.indices[k-1]-1] == 1) {
	    done = 0;
	    connected_nodes[j-1] = 1;
	  }
	}
      }
    }
  }
  
  int answer = 1;
  for (j = 2; j <= size; j++) {
    if (connected_nodes[j-1] == 0) {
      answer = 0;
    }
  }

  destroy_index(the_index);
  free(connected_nodes);

  return answer;
}

int is_qualified(int j, struct square_matrix* related, int* subset_sizes, int* point_school,
		 int set_size, int* candidate_list, int probe) {
  int k,l;

  if (subset_sizes[j-1] == 0) {
    return 0;
  }

  if (j == *point_school) {
    return 0;
  }

  if (j < *point_school && set_size <= subset_sizes[j-1]) {
    return 0;
  }

  for (k = 1; k < probe; k++) {
    if (j == candidate_list[k-1]) {
      return 0;
    }
  }
  
  if (related->entries[j-1][*point_school-1] == 1) {
    for (l = 1; l < probe; l++) {
      if (j < candidate_list[l-1]) {
	return 0;
      }
    }
    return 1;
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

int next_subset(struct subset* my_subset, struct square_matrix* related, int* subset_sizes,
		int* point_school) {

  if (!subset_has_right_subset_size(my_subset)) {
    printf("We have a bad subset_size.\n");
    exit(0);
  }

  if (!subset_is_connected(my_subset,related)) {
    printf("Whoa, we got a disconnected subset.\n");
  }
  
  int j,k,l;
  int nsc = my_subset->large_set_size;
  int set_size = my_subset->subset_size;

  /*
  if (my_subset->subset_size > 0) {
    printf("There are %d schools and the subset is\n",nsc);
    struct index the_index = index_of_subset(my_subset);
    print_index(&the_index);
    destroy_index(the_index);
    printf(".\n");
  }
  */

  if (set_size == 0) {
    *point_school = 1;
    while (subset_sizes[*point_school - 1] == 0) {
      (*point_school)++;
    }
    if (*point_school <= nsc) {
      my_subset->indicator[*point_school - 1] = 1;
      my_subset->subset_size = 1;
  
      return 1;
    }
    else {
  
      return 0;
    }
  }

  int probe;
  int swap_school;
  int max;
  int qualified;
  int fill_number;

  /* To begin with we get the subset as the point_school + candidate_list. */
    
  int* candidate_list = malloc((set_size - 1) * sizeof(int));
  get_candidate_list(candidate_list,related,my_subset,point_school);

   /* We now try to turn the dial on the odometer. */
      
  fill_number = set_size - 1;
  probe = fill_number;
  
  while (fill_number > 0 && probe < set_size) {
    j = candidate_list[probe-1]+1;
    
    while (j <= nsc && !is_qualified(j,related,subset_sizes,point_school,set_size,
				     candidate_list,probe)) {
      j++;
    }
    
    if (j <= nsc) {      
      candidate_list[probe-1] = j;
      probe++;
    }
    else {
      if (probe == fill_number) {
	fill_number--;
      }
      candidate_list[probe-1] = 0;
      probe--;
    }
  } /* while (fill_number > 0 && probe < set_size) { */

  if (probe >= set_size) {
    for (j = 1; j <= nsc; j++) {
      my_subset->indicator[j-1] = 0;
    }
    my_subset->indicator[(*point_school)-1] = 1;
    
    for (k = 1; k <= set_size-1; k++) {      
      my_subset->indicator[candidate_list[k-1]-1] = 1;      
    }
  
    return 1;
  }
  
  /* The next thing to try is increasing the size of the subset. */

  if (set_size < subset_sizes[*point_school-1]) {    
    set_size++;
    my_subset->subset_size++;
    free(candidate_list);
    candidate_list = malloc((set_size-1) * sizeof(int));
    for (k = 1; k < set_size; k++) {
      candidate_list[k-1] = 0;
    }

    /* We try again to turn the dial on the odometer. */
    /* This is basically the same code as above. */
      
    fill_number = 1;
    probe = fill_number;
    while (fill_number > 0 && probe < set_size) {
      j = candidate_list[probe-1]+1;
      
      while (j <= nsc && !is_qualified(j,related,subset_sizes,point_school,set_size,
				       candidate_list,probe)) {
	j++;
      }
      if (j <= nsc) {
	candidate_list[probe-1] = j;
	probe++;
      }
      else {
	if (probe == fill_number) {
	  fill_number--;
	}
	candidate_list[probe-1] = 0;
	probe--;
      }
    }

    if (probe >= set_size) {
      for (j = 1; j <= nsc; j++) {
	my_subset->indicator[j-1] = 0;
      }
      my_subset->indicator[(*point_school)-1] = 1;            
      for (k = 1; k <= set_size-1; k++) {
	my_subset->indicator[candidate_list[k-1]-1] = 1;      
      }
  
      return 1;
    }
  }

  free(candidate_list);

  /* The final thing to try is to increase the point_school. */

  if (*point_school < nsc) {
    (*point_school)++;
    my_subset->subset_size = 1;
    for (j = 1; j <= nsc; j++) {
      if (j == *point_school) {
	my_subset->indicator[j-1] = 1;
      }
      else {
	my_subset->indicator[j-1] = 0;
      }
    }
  
    return 1;
  }
  
  return 0;
}
