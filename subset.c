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

void print_subset(struct subset my_subset) {
  int i;
  printf("(");
  for (i = 1; i < my_subset.large_set_size; i++) {
    printf("%d,",my_subset.indicator[i-1]);
  }
  printf("%d)",my_subset.indicator[my_subset.large_set_size-1]);
}

void destroy_subset(struct subset my_subset) {
  free(my_subset.indicator);
}


void iterate(struct subset *my_subset) {
  int i,j,k;
  int lss = my_subset->large_set_size;
  int ss = my_subset->subset_size;

  if (ss == lss) {
    printf("You are trying to increment the full subset, which is verboten!\n");
    exit(0);
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
}
