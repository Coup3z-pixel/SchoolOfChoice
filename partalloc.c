#include "cee.h"
#include "partalloc.h"

struct partial_alloc zero_partial_alloc(struct double_cee my_cee) {
  int i,j;
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = my_cee.no_students;
  my_partial_alloc.no_schools = my_cee.no_schools;
  
  my_partial_alloc.allocations = malloc(my_cee.no_students * sizeof(double*));
  my_partial_alloc.allocations[i-1] = malloc(my_cee.no_students * sizeof(double));
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      my_partial_alloc.allocations[i-1][j-1] = 0.0; 
    }
  }
  
  return my_partial_alloc;
}

void destroy_partial_alloc(struct partial_alloc my_partial_alloc) {
  free(my_partial_alloc.allocations);
}

void print_partial_alloc(struct partial_alloc my_partial_alloc) {
  int i,j;
  printf("The allocation matrix is:");
    for (i = 1; i <= my_partial_alloc.no_students; i++) {
      printf("\n");
      for (j = 1; j <= my_partial_alloc.no_schools; j++) {
	printf("     %2.2f", my_partial_alloc.allocations[i-1][j-1]);
      }
    }
    printf("\n");
}

void increment_partial_alloc(struct partial_alloc base, struct partial_alloc increment) {
  int i,j;
  if (base.no_students != increment.no_students || base.no_schools != increment.no_schools) {
    printf("ERROR: incrementing a partial allocation by an incompatible increment.\n");
    exit(0);
  }
  for (i = 1; i <= base.no_students; i++) {
    for (j = 1; j <= base.no_students; j++) {
      base.allocations[i-1][j-1] += increment.allocations[i-1][j-1];
    }
  }
  
}

