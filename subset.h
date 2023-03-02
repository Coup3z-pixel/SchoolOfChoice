#include <stdio.h>
#include <stdlib.h>

struct subset {
  int large_set_size;
  int subset_size;
  int* indicator;
};

struct subset nullset(int large_set_size);

void print_subset(struct subset my_subset);

void iterate(struct subset *my_subset); /* We go from smallest to largest */

int is_element_of(int element, struct subset my_subset);
