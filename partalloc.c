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

void destroy_pure_alloc(struct pure_alloc* my_pure_alloc) {
  int i;

  for (i = 1; i <= my_pure_alloc->no_students; i++) {
    free(my_pure_alloc->allocations[i-1]);
  }
  free(my_pure_alloc->allocations);
}

void print_partial_alloc(struct partial_alloc my_partial_alloc) {
  int i,j;
  int nst = my_partial_alloc.no_students;
  int nsc = my_partial_alloc.no_schools;
  
  printf("/* This is a sample introductory comment. */\n");

  printf("There are %d students and %d schools\n",nst,nsc);
  
  for (j = 1; j <= my_partial_alloc.no_schools; j++) {
    if (j < 10) {
      printf(" ");
    }
    printf("         %i:", j);
  }
  for (i = 1; i <= my_partial_alloc.no_students; i++) {
    printf("\n%i:",i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= my_partial_alloc.no_schools; j++) {
      printf("  %2.8f", my_partial_alloc.allocations[i-1][j-1]);
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
  int i;
  for (i = 1; i <= my_partial_alloc->no_students; i++) {
    free(my_partial_alloc->allocations[i-1]);
  }
  free(my_partial_alloc->allocations);
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

double distance_to_gmc_equality(struct double_cee* my_cee, struct partial_alloc* my_alloc,
				int my_stu, int first_sch, int second_sch,
				struct subset* school_subset) {
  int i,j;
  int no_stu = my_cee->no_students;
  int no_sch = my_cee->no_schools;
  struct subset student_subset; /* those who must eat school_subset */
  student_subset = fullset(no_stu);
  for (i = 1; i <= no_stu; i++) {
    j = 1;
    while (j <= no_sch && student_subset.indicator[i-1] == 1) {
      if (school_subset->indicator[j-1] == 0 && my_cee->priority[i-1][j-1] > 0) {
	student_subset.subset_size--;
	student_subset.indicator[i-1] = 0;
      }
      j++;
    }
  }

  double total_quota = 0.0;
  for (j = 1; j <= no_sch; j++) {
    if (school_subset->indicator[j-1] == 1) {
      total_quota += my_cee->quotas[j-1];
    }
  }

  double excess = total_quota - (double)student_subset.subset_size;
  for (j = 1; j <= no_sch; j++) {
    if (school_subset->indicator[j-1] ==1) {
      for (i = 1; i <= no_stu; i++) {
	if (student_subset.indicator == 0) {
	  excess -= my_alloc->allocations[i-1][j-1];
	}
      }
    }
  }

  if (student_subset.indicator[my_stu-1] == 0 && school_subset->indicator[first_sch-1] == 1
      && school_subset->indicator[second_sch-1] == 0
      && excess < my_alloc->allocations[my_stu-1][second_sch-1]) {
    destroy_subset(school_subset);
    return excess;
  }
  else {
    destroy_subset(school_subset);
    return 2.0;
  }
}

double distance_to_first_gmc_eq(struct double_cee* my_cee, struct partial_alloc* my_alloc,
				int my_stu, int first_sch, int second_sch,
				struct subset* school_subset) {
  struct square_matrix related = matrix_of_ones(my_cee->no_schools);
  int max_clique_size = my_cee->no_schools;

  *school_subset = nullset(my_cee->no_schools);

  /* complete */
  
  return 0.0;
}

struct partial_alloc sample_pure_assignment(struct partial_alloc* my_alloc,
					    struct double_cee* my_cee) {
  return zero_partial_alloc(my_cee);
}

