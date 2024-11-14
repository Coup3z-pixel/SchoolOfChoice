#include "partalloc.h"

int partial_allocs_are_same(struct partial_alloc* first, struct partial_alloc* second) {
  int i, j;

  int nst = first->no_students;
  int nsc = first->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (first->allocations[i-1][j-1] < second->allocations[i-1][j-1] - 0.000001 ||
	  first->allocations[i-1][j-1] > second->allocations[i-1][j-1] + 0.000001) {
	return 0;
      }
    }
  }

  return 1;
}

struct partial_alloc zero_alloc_for_process_scp(struct process_scp* my_scp) {
  int i,j;
  int nst = my_scp->no_students;
  int nsc = my_scp->no_schools;
  
  struct partial_alloc my_partial_alloc;
  my_partial_alloc.no_students = nst;
  my_partial_alloc.no_schools = nsc;
  
  my_partial_alloc.allocations = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    my_partial_alloc.allocations[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      my_partial_alloc.allocations[i-1][j-1] = 0.0; 
    }
  }
  
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

struct partial_alloc reduced_feasible_guide(struct partial_alloc* feasible_guide,
					    struct subset* J_subset, struct subset* P_subset) {
  int i, j, old_std_no, old_sch_no;

  int new_nst = J_subset->subset_size;
  int new_nsc = P_subset->subset_size;

  struct partial_alloc new_guide;

  new_guide.no_students = new_nst;
  new_guide.no_schools = new_nsc;
  
  new_guide.allocations = malloc(new_nst * sizeof(double*));
  for (i = 1; i <= new_nst; i++) {
    new_guide.allocations[i-1] = malloc(new_nsc * sizeof(double));
  }

  old_std_no = 0;
  for (i = 1; i <= new_nst; i++) {
    old_sch_no = 0;
    old_std_no++;
    while (J_subset->indicator[old_std_no-1] == 0) {
      old_std_no++;
    }
    for (j = 1; j <= new_nsc; j++) {
      old_sch_no++;
      while (P_subset->indicator[old_sch_no-1] == 0) {
	old_sch_no++;
      }
      new_guide.allocations[i-1][j-1] = feasible_guide->allocations[old_std_no-1][old_sch_no-1];
    }
  }

  return new_guide;
}

struct partial_alloc left_feasible_guide(struct partial_alloc* feasible_guide,
					 struct subset* J_subset) {
  struct subset all_schools = fullset(feasible_guide->no_schools);

  struct partial_alloc new_guide = reduced_feasible_guide(feasible_guide, J_subset, &all_schools);

  destroy_subset(all_schools);

  return new_guide;
}

struct partial_alloc right_feasible_guide(struct partial_alloc* feasible_guide,
					  struct subset* J_subset, struct subset* P_subset) {
  struct subset J_compl = complement_of_subset(J_subset);
  struct subset P_compl = complement_of_subset(P_subset);

  struct partial_alloc new_guide = reduced_feasible_guide(feasible_guide, &J_compl, &P_compl);

  destroy_subset(J_compl);
  destroy_subset(P_compl);

  return new_guide;
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

void destroy_pure_alloc(struct pure_alloc my_pure_alloc) {
  int i;

  for (i = 1; i <= my_pure_alloc.no_students; i++) {
    free(my_pure_alloc.allocations[i-1]);
  }
  free(my_pure_alloc.allocations);
}
