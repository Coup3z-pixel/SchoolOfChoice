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

struct square_matrix matrix_of_ones(int dim) {
  int j, k;
  struct square_matrix my_matrix;
  my_matrix.dimension = dim;
  my_matrix.entries = malloc(dim * sizeof(int*));
  for (j = 1; j <= dim; j++) {
    my_matrix.entries[j-1] = malloc(dim * sizeof(int));
    for (k = 1; k <= dim; k++) {
      my_matrix.entries[j-1][k-1] = 1;
    }
  }
  return my_matrix;
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

void destroy_subset(struct subset* my_subset) {
  free(my_subset->indicator);
}

void destroy_index(struct index* my_index) {
  free(my_index->indices);
}

void destroy_square_matrix(struct square_matrix* my_matrix) {
  int j;
  for (j = 1; j <= my_matrix->dimension; j++) {
    free(my_matrix->entries[j-1]);
  }
  free(my_matrix->entries);
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
  destroy_index(&my_index);
  return my_matrix;
}

int iterate(struct subset *my_subset) {
  int i,j,k;
  int lss = my_subset->large_set_size;
  int ss = my_subset->subset_size;

  if (ss == lss) {
    return 0;
  }
  
  int enlarge = 1;    
  for (i = 1; i <= ss; i++) {
    if (my_subset->indicator[i - 1] == 0) {
      enlarge = 0;    
    }
  }
  if (enlarge) {
    ss++;
    my_subset->subset_size++;
    for (i = 1; i <= lss - ss; i++) {
      my_subset->indicator[i-1] = 0;
    }
    for (i = lss - ss + 1; i <= lss; i++) {
      my_subset->indicator[i-1] = 1;
    }
  }
  else {
    if (my_subset->indicator[lss - 1] == 1) {
      i = lss;
      while (my_subset->indicator[i - 1] == 1) {
	i--;
      }
      my_subset->indicator[i-1] = 1;
      my_subset->indicator[i] = 0;
    }
    else {
      i = lss - 1;
      while (my_subset->indicator[i - 1] == 0) {
	i--;
      }
      j = i;
      while (my_subset->indicator[j - 1] == 1) {
	j--;
      }
      my_subset->indicator[j - 1] = 1;
      for (k = j+1; k <= lss - i + j + 1; k++) {
	my_subset->indicator[k - 1] = 0;
      }
      for (k = lss - i + j + 2; k <= lss; k++) {
	my_subset->indicator[k - 1] = 1;
      }
    }
  }
  return 1;
}

int next_subset(struct subset* my_subset, struct square_matrix* related) {
  int i;
  int have_leveled_up = 0;
  int no_needed = 0;
  int change_from;

  /* Sometimes we may want to look at particular cases.
  int debug = 0;
  if (my_subset->large_set_size == 4 && my_subset->indicator[0] == 1
                                     && my_subset->indicator[1] == 0
                                     && my_subset->indicator[2] == 0
                                     && my_subset->indicator[3] == 1) {
    debug = 1;
  }
  */

  /* Get rid of trivial special case */  
  if (my_subset->subset_size == 0) {
    my_subset->subset_size = 1;
    my_subset->indicator[0] = 1;
    return 1;
  }

  /* Set initial state */
  change_from = my_subset->large_set_size;
  while (my_subset->indicator[change_from - 1] == 0) {
    change_from--;
  }
  
  if (change_from < my_subset->large_set_size) {
    my_subset->indicator[change_from-1] = 0;
    my_subset->indicator[change_from] = 1;
    change_from++;
  }
  else {
    while (my_subset->indicator[change_from-1] == 1 && no_needed < my_subset->subset_size) {
      my_subset->indicator[change_from-1] = 0;
      change_from--;
      no_needed++;
    }
    if (no_needed == my_subset->subset_size) {
      if (have_leveled_up) {
	return 0;
      }
      else {
	have_leveled_up = 1;
	no_needed = my_subset->subset_size;
	my_subset->subset_size++;
	if (my_subset->subset_size > my_subset->large_set_size) {
	  return 0;
	}
	change_from = 1;
	my_subset->indicator[0] = 1;
	for (i = 2; i <= my_subset->large_set_size; i++) {
	  my_subset->indicator[i-1] = 0;
	}
      }
    }
    else {
      while (my_subset->indicator[change_from-1] == 0) {
	change_from--;
      }
      my_subset->indicator[change_from-1] = 0;
      change_from++;
      my_subset->indicator[change_from-1] = 1;
    }
  }

  /* We cycle through states until exiting.  At a state my_subset->indicator[change_from-1] is 
     one, all subsequent indicators are zero, and the set given by the prior indicators is 
     a clique for the related matrix.  We first determine whether the union with change_from
     is also a clique, and then move to the appropriate new state, exiting if we find the next
     subset or determine that there isn't one. */
  
  int done = 0;
  while (!done) {
    int related_fail = 0;
    for (i = 1; i < change_from; i++) {
      if (my_subset->indicator[i-1] == 1 && related->entries[i-1][change_from-1] == 0) {
	related_fail = 1;
      }
    }

    if (!related_fail) {
      if (no_needed == 0) {
	return 1;
      }
      else {
	change_from++;
	no_needed--;
	my_subset->indicator[change_from-1] = 1;
      }
    }
    else {
      if (change_from + no_needed < my_subset->large_set_size) {
	my_subset->indicator[change_from-1] = 0;
	change_from++;
	my_subset->indicator[change_from-1] = 1;
      }
      else {
	while (my_subset->indicator[change_from-1] == 1 && no_needed < my_subset->subset_size) {
	  my_subset->indicator[change_from-1] = 0;
	  change_from--;
	  no_needed++;
	}
	if (no_needed < my_subset->subset_size) {
	  while (my_subset->indicator[change_from-1] == 0) {
	    change_from--;
	  }
	  my_subset->indicator[change_from-1] = 0;
	  change_from++;
	  my_subset->indicator[change_from-1] = 1;
	}
	else {
	  if (have_leveled_up) {
	    return 0;
	  }
	  else  {
	    have_leveled_up = 1;
	    no_needed = my_subset->subset_size;
	    my_subset->subset_size++;
	    change_from = 1;
	    my_subset->indicator[0] = 1;
	    for (i = 2; i <= my_subset->large_set_size; i++) {
	      my_subset->indicator[i-1] = 0;
	    }
	  }
	}
      }
    }
  }
  return 0;
}
