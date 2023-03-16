#include "partalloc.h"

struct partial_alloc zero_partial_alloc(struct sch_ch_prob* my_scp) {
  int i,j;
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = my_scp->cee.no_students;
  my_partial_alloc.no_schools = my_scp->cee.no_schools;
  
  my_partial_alloc.allocations = malloc(my_scp->cee.no_students * sizeof(double*));
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    my_partial_alloc.allocations[i-1] = malloc(my_scp->cee.no_schools * sizeof(double));
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      my_partial_alloc.allocations[i-1][j-1] = 0.0; 
    }
  }
  
  return my_partial_alloc;
}

struct partial_alloc allocate_until_new_time(struct sch_ch_prob* my_scp,
					     double new_time_remaining) {
  int i,j;
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = my_scp->cee.no_students;
  my_partial_alloc.no_schools = my_scp->cee.no_schools;
  
  my_partial_alloc.allocations = malloc(my_scp->cee.no_students * sizeof(double*));
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    my_partial_alloc.allocations[i-1] = malloc(my_scp->cee.no_schools * sizeof(double));
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      if (my_scp->preferences[i-1][0] == j) {
	my_partial_alloc.allocations[i-1][j-1] = my_scp->time_remaining - new_time_remaining;
	my_scp->cee.quotas[j-1] -= (my_scp->time_remaining - new_time_remaining);
      }
      else {
	my_partial_alloc.allocations[i-1][j-1] = 0.0;
      }
    }
  }

  my_scp->time_remaining = new_time_remaining;
  
  return my_partial_alloc;
}

void destroy_partial_alloc(struct partial_alloc* my_partial_alloc) {
  free(my_partial_alloc->allocations);
}

void print_partial_alloc(struct partial_alloc my_partial_alloc) {
  int i,j;
  printf(" ");
  for (j = 1; j <= my_partial_alloc.no_schools; j++) {
    printf("       %i:", j);
  }
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    printf("\n%i:",i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      printf("     %2.2f", my_partial_alloc.allocations[i-1][j-1]);
    }
  }
  printf("\n");
}

void increment_partial_alloc(struct partial_alloc* base, struct partial_alloc* increment,
			     struct index* stu_index,struct index* sch_index) {
  int i,j;
  for (i = 1; i <= increment->no_students; i++) {
    for (j = 1; j <= increment->no_schools; j++) {
      base->allocations[stu_index->indices[i-1]-1][sch_index->indices[j-1]-1] +=
	increment->allocations[i-1][j-1]; 
    }
  }
  
}

