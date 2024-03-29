#include "partalloc.h"

struct partial_alloc zero_partial_alloc(struct double_cee* my_cee) {
  int i,j;
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = my_cee->no_students;
  my_partial_alloc.no_schools = my_cee->no_schools;
  
  my_partial_alloc.allocations = malloc(my_cee->no_students * sizeof(double*));
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    my_partial_alloc.allocations[i-1] = malloc(my_cee->no_schools * sizeof(double));
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      my_partial_alloc.allocations[i-1][j-1] = 0.0; 
    }
  }
  
  return my_partial_alloc;
}

void destroy_pure_alloc(struct pure_alloc my_pure_alloc) {
  int i;

  for (i = 1; i <= my_pure_alloc.no_students; i++) {
    free(my_pure_alloc.allocations[i-1]);
  }
  free(my_pure_alloc.allocations);
}

void print_partial_alloc(struct partial_alloc* my_partial_alloc) {
  int i,j;
  int nst = my_partial_alloc->no_students;
  int nsc = my_partial_alloc->no_schools;
  
  printf("/* This is a sample introductory comment. */\n");

  printf("There are %d students and %d schools\n",nst,nsc);
  
  for (j = 1; j <= my_partial_alloc->no_schools; j++) {
    if (j < 10) {
      printf(" ");
    }
    printf("         %i:", j);
  }
  for (i = 1; i <= my_partial_alloc->no_students; i++) {
    printf("\n%i:",i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= my_partial_alloc->no_schools; j++) {
      if (my_partial_alloc->allocations[i-1][j-1] < -0.000001) {
	printf("We have a negative allocation probability.\n");
	exit(0);
      }
      printf("  %2.8f", fabs(my_partial_alloc->allocations[i-1][j-1]));
    }
  }
  printf("\n");
}

void print_pure_alloc(struct pure_alloc my_pure_alloc) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");
  
  printf("      ");
  for (j = 1; j <= my_pure_alloc.no_schools; j++) {
    if (j < 100) {
      printf(" ");
    }
    if (j < 10) {
      printf(" ");
    }
    printf(" %i:", j);
  }
  for (i = 1; i <= my_pure_alloc.no_students; i++) {
    printf("\n");
    if (i < 1000) {
      printf(" ");
    }
    if (i < 100) {
      printf(" ");
    }
    if (i < 10) {
      printf(" ");
    }
    printf("%i:",i);
    for (j = 1; j <= my_pure_alloc.no_schools; j++) {
      printf("    %i", my_pure_alloc.allocations[i-1][j-1]);
    }
  }
  printf("\n");
}

void destroy_partial_alloc(struct partial_alloc my_partial_alloc) {
  int i;
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    free(my_partial_alloc.allocations[i-1]);
  }
  free(my_partial_alloc.allocations);
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

double* school_sums(struct partial_alloc* my_alloc) {
  int i, j;
  double* sums = malloc(my_alloc->no_schools * sizeof(double));
  for (j = 1; j <= my_alloc->no_schools; j++) {
    sums[j-1] = 0.0;
    for (i = 1; i <= my_alloc->no_students; i++) {
      sums[j-1] += my_alloc->allocations[i-1][j-1];
    }
  }
  return sums;
}

int is_feasible_allocation(struct sch_ch_prob* my_scp, struct partial_alloc* my_alloc) {
  int i, j, answer;
  double st_total, sc_total;

  int nst = my_alloc->no_students;
  int nsc = my_alloc->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++ ) {
      if (my_alloc->allocations[i-1][j-1] < -0.000001) {
	printf("We have a negative allocation.\n");
	return 0;
      }
      if (my_alloc->allocations[i-1][j-1] > 1.000001) {
	printf("We have an allocation above 1.\n");
	return 0;
      }
    }
  }

  answer = 1;
  for (i = 1; i <= nst; i++) {
    st_total = 0.0;
    for (j = 0; j <= nsc; j++) {
      st_total += my_alloc->allocations[i-1][j-1];
    }
    if (st_total < my_scp->time_remaining - 0.000001) {
      printf("Student %d is underallocated.\n",i);
      answer = 0;
      return 0;
    }
    if (st_total > my_scp->time_remaining + 0.000001) {
      printf("Student %d is overallocated.\n",i);
      answer = 0;
    }
  }
  if (answer == 0) {
    return 0;
  }

  for (j = 1; j <= nsc; j++) {
    sc_total = 0.0;
    for (i = 1; i <= nst; i++) {
      sc_total += my_alloc->allocations[i-1][j-1];
    }
    if (sc_total > (my_scp->cee).quotas[j-1]) {
      printf("We have an overallocated school.\n");
      return 0;
    }
  }

  return 1;
}

