#include "cee.h"
#include "partalloc.h"

struct partial_alloc zero_partial_alloc(struct double_cee my_cee) {
  int i,j;
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = my_cee.no_students;
  my_partial_alloc.no_schools = my_cee.no_schools;
  
  my_partial_alloc.allocations = malloc(12 * sizeof(double));
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      my_partial_alloc.allocations[(i - 1) * my_partial_alloc.no_schools + j - 1] = 0.0; 
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
	printf("     %2.2f", my_partial_alloc.allocations[(i-1)*my_partial_alloc.no_schools+j-1]);
      }
    }
    printf("\n");
}

