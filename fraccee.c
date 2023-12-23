#include "fraccee.h"


void destroy_frac_cee(struct frac_cee my_frac) {
  int i;
  free(my_frac.quotas);
  
  for (i = 1; i <= my_frac.no_students; i++) {
    free(my_frac.maximums[i-1]);
  }
  free(my_frac.maximums);
}

void destroy_frac_scp(struct frac_scp my_scp) {
  int i;
  
  int nst = (my_scp.cee).no_students;

  destroy_frac_cee(my_scp.cee);
  
  free(my_scp.no_eligible_schools);
  
  for (i = 1; i <= nst; i++) {
    free(my_scp.preferences[i-1]);
  }
  free(my_scp.preferences);
}

void print_frac_cee(struct frac_cee* my_frac) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");

  printf("The time remaining is %1.2f.\n",my_frac->time_remaining);
  
  printf("There are %d students and %d schools\n", my_frac->no_students, my_frac->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < my_frac->no_schools; i++) {
    printf("%1.2f,",my_frac->quotas[i-1]);
  }
  printf("%1.2f)\n",my_frac->quotas[my_frac->no_schools-1]);
  
  
  printf("The maximums matrix is");
    for (i = 1; i <= my_frac->no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_frac->no_schools; j++) {
      printf("  %1.2f", my_frac->maximums[i-1][j-1]);
    }
  }
    printf("\n");
}

void print_frac_scp(struct frac_scp* my_scp) {
  int i,j;
  print_frac_cee(&my_scp->cee);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= my_scp->cee.no_students; i++) {
    printf("%i:  ", i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= my_scp->no_eligible_schools[i-1]; j++) {
      printf("%i  ",  my_scp->preferences[i-1][j-1]);
    }
    printf("\n");
  }
}

void copy_frac_cee(struct frac_cee* my_cee, struct frac_cee* copy) {
  int i, j;

  int nst = my_cee->no_students;
  int nsc = my_cee->no_schools;

  copy->no_students = my_cee->no_students;
  copy->no_schools = my_cee->no_schools;

  copy->quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    copy->quotas[j-1] = my_cee->quotas[j-1];
  }

  copy->maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    copy->maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      copy->maximums[i-1][j-1] = my_cee->maximums[i-1][j-1];
    }
  }

  copy->time_remaining = my_cee->time_remaining;
}

void copy_frac_scp(struct frac_scp* my_scp, struct frac_scp* copy) {
  int i, j;

  int nst = (my_scp->cee).no_students;
  
  copy_frac_cee(&(my_scp->cee),&(copy->cee));
  
  copy->no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    (copy->no_eligible_schools)[i-1] = (my_scp->no_eligible_schools)[i-1];
  }

  copy->preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    (copy->preferences)[i-1] = malloc((copy->no_eligible_schools)[i-1] * sizeof(int));
    for (j = 1; j <= (my_scp->no_eligible_schools)[i-1]; j++) {
      (copy->preferences)[i-1][j-1] = (my_scp->preferences)[i-1][j-1];
    }
  }  
}

struct frac_cee make_toy_frac_cee() {
  int i,j;
  struct frac_cee my_frac;
  
  my_frac.no_students = 4;
  my_frac.no_schools = 3;

  my_frac.time_remaining = 1.0;
  
  my_frac.quotas = malloc(3 * sizeof(double));
  my_frac.quotas[0] = 1.0;
  my_frac.quotas[1] = 2.0;
  my_frac.quotas[2] = 1.0;
  
  my_frac.maximums = malloc(4 * sizeof(double*));
  for (i = 1; i <= my_frac.no_students; i++) {
  my_frac.maximums[i-1] = malloc(3 * sizeof(double));
    for (j = 1; j <= my_frac.no_schools; j++) {
      my_frac.maximums[i-1][j-1] = 1.0; 
    }
  }

  my_frac.maximums[1][1] = 0.0;
  
  return my_frac;
}

struct frac_scp make_toy_frac_scp() {
  struct frac_scp my_scp;
  my_scp.cee = make_toy_frac_cee();

  my_scp.no_eligible_schools = malloc(4 * sizeof(int));
  my_scp.no_eligible_schools[0] = 3;
  my_scp.no_eligible_schools[1] = 2;
  my_scp.no_eligible_schools[2] = 3;
  my_scp.no_eligible_schools[3] = 3;

  my_scp.preferences = malloc(4 * sizeof(int*));
  my_scp.preferences[0] = malloc(3 * sizeof(int));
  my_scp.preferences[1] = malloc(2 * sizeof(int));
  my_scp.preferences[2] = malloc(3 * sizeof(int));
  my_scp.preferences[3] = malloc(3 * sizeof(int));

  my_scp.preferences[0][0] = 1;
  my_scp.preferences[1][0] = 1;
  my_scp.preferences[2][0] = 1;
  my_scp.preferences[3][0] = 1;

  my_scp.preferences[0][1] = 2;
  my_scp.preferences[1][1] = 3;
  my_scp.preferences[2][1] = 2;
  my_scp.preferences[3][1] = 2;

  my_scp.preferences[0][2] = 3;
  my_scp.preferences[2][2] = 3;
  my_scp.preferences[3][2] = 3;

  return my_scp;
}

struct partial_alloc zero_alloc_for_frac(struct frac_cee* my_cee) {
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

struct frac_scp critical_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset) {
  struct frac_scp new_scp;
  
  int i, j, k;
  struct index J_index;
  J_index = index_of_subset(J_subset);

  int nst = J_subset->subset_size;
  int nsc = my_scp->cee.no_schools;

  new_scp.cee.no_students = nst;
  new_scp.cee.no_students = nsc;

  (new_scp.cee).quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.cee.quotas[j-1] = my_scp->cee.quotas[j-1];
  }

  new_scp.cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_scp.cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      new_scp.cee.maximums[i-1][j-1] = my_scp->cee.maximums[J_index.indices[i-1]-1][j-1];
    }
  }

  new_scp.cee.time_remaining = my_scp->cee.time_remaining;

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = my_scp->no_eligible_schools[J_index.indices[i-1]-1];
  }
  
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= new_scp.no_eligible_schools[i-1]; k++) {
      new_scp.preferences[i-1][k-1] = my_scp->preferences[J_index.indices[i-1]-1][k];
    }
  }

  destroy_index(J_index);
  return new_scp;    
}

struct frac_scp crit_compl_sub_frac_scp(struct frac_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset) {
  struct frac_scp new_scp;
  
  int i, j, k, l, count;

  struct subset J_compl, P_compl;
  J_compl = complement_of_subset(J_subset);
  P_compl = complement_of_subset(P_subset);
  struct index J_index, P_index;
  J_index = index_of_subset(&J_compl);
  P_index = index_of_subset(&P_compl);
  
  int* reverse_P_index = malloc(my_scp->cee.no_schools * sizeof(int));
  count = 0;
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    if (P_compl.indicator[j] == 1) {
      count++;
      reverse_P_index[j-1] = count;
    }
    else {
      reverse_P_index[j-1] = 0;
    }
  }

  int nst = J_compl.subset_size;
  int nsc = P_compl.subset_size;

  new_scp.cee.no_students = nst;
  new_scp.cee.no_students = nsc;

  (new_scp.cee).quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.cee.quotas[j-1] = my_scp->cee.quotas[P_index.indices[j-1]-1];
    for (k = 1; k <= my_scp->cee.no_students; k++) {
      if (J_subset->indicator[k-1] == 1) {
	new_scp.cee.quotas[j-1] -= my_scp->cee.maximums[k][P_index.indices[j-1]-1];
      }
    }
  }

  new_scp.cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_scp.cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      new_scp.cee.maximums[i-1][j-1] =
	my_scp->cee.maximums[J_index.indices[i-1]-1][P_index.indices[j-1]-1];
    }
  }

  new_scp.cee.time_remaining = my_scp->cee.time_remaining;

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = 0;
    for (l = 1; l <= my_scp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.no_eligible_schools[i-1]++;
      }
    }
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int*));
    count = 0;
    for (l = 1; l <= my_scp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.preferences[i-1][count] =
	  reverse_P_index[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1];
	count++;
      }
    }
  }

  destroy_subset(J_compl);
  destroy_subset(P_compl);
  destroy_index(J_index);
  destroy_index(P_index);
  free(reverse_P_index);
  
  return new_scp;    
}

