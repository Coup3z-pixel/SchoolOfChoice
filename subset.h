#ifndef SUBSET_H
#define SUBSET_H

#include <stdio.h>
#include <stdlib.h>

struct subset {
  int large_set_size;
  int subset_size;
  int* indicator;
};

struct index {
  int no_elements;
  int* indices;
};

struct subset nullset(int large_set_size);

struct subset fullset(int large_set_size);

void print_subset(struct subset* my_subset);

void destroy_subset(struct subset* my_subset);

void copy_subset(struct subset* given_subset, struct subset* copy_subset);

void complement(struct subset* given_subset, struct subset* comp);

struct index index_of_subset(struct subset* my_subset);

void destroy_index(struct index* my_index);

void iterate(struct subset* my_subset); /* We go from smallest to largest */

#endif /* SUBSET_H */
