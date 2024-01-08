#include "cee.h"


struct input_cee make_toy_cee() {
  int i,j;
  struct input_cee my_cee;
  
  my_cee.no_students = 4;
  my_cee.no_schools = 3;
  
  my_cee.quotas = malloc(3 * sizeof(int));
  my_cee.quotas[0] = 1;
  my_cee.quotas[1] = 2;
  my_cee.quotas[2] = 1;
  
  my_cee.priority = malloc(4 * sizeof(int*));
  for (i = 1; i <= my_cee.no_students; i++) {
  my_cee.priority[i-1] = malloc(3 * sizeof(int));
    for (j = 1; j <= my_cee.no_schools; j++) {
      my_cee.priority[i-1][j-1] = 1; 
    }
  }
  
  return my_cee;
}

void destroy_input_cee(struct input_cee my_cee) {
  int i;
  free(my_cee.quotas);
  for (i = 1; i <= my_cee.no_students; i++) {
    free(my_cee.priority[i-1]);
  }
  free(my_cee.priority);
}

void print_input_cee(struct input_cee* my_cee) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");
  
  printf("There are %d students and %d schools\n", my_cee->no_students, my_cee->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < my_cee->no_schools; i++) {
    printf("%d,",my_cee->quotas[i-1]);
  }
  printf("%d)\n",my_cee->quotas[my_cee->no_schools-1]);
  
  
  printf("The priority matrix is");
    for (i = 1; i <= my_cee->no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_cee->no_schools; j++) {
      printf("  %d", my_cee->priority[i-1][j-1]);
    }
  }
    printf("\n");
}


void destroy_process_cee(struct process_cee my_proc) {
  int i;
  free(my_proc.quotas);
  
  for (i = 1; i <= my_proc.no_students; i++) {
    free(my_proc.maximums[i-1]);
  }
  free(my_proc.maximums);
}


void print_process_cee(struct process_cee* my_proc) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");

  printf("The time remaining is %1.2f.\n",my_proc->time_remaining);
  
  printf("There are %d students and %d schools\n", my_proc->no_students, my_proc->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < my_proc->no_schools; i++) {
    printf("%1.2f,",my_proc->quotas[i-1]);
  }
  printf("%1.2f)\n",my_proc->quotas[my_proc->no_schools-1]);
  
  
  printf("The maximums matrix is");
    for (i = 1; i <= my_proc->no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_proc->no_schools; j++) {
      printf("  %1.2f", my_proc->maximums[i-1][j-1]);
    }
  }
    printf("\n");
}


void copy_process_cee(struct process_cee* my_cee, struct process_cee* copy) {
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


struct process_cee make_toy_process_cee() {
  int i,j;
  struct process_cee my_proc;
  
  my_proc.no_students = 4;
  my_proc.no_schools = 3;

  my_proc.time_remaining = 1.0;
  
  my_proc.quotas = malloc(3 * sizeof(double));
  my_proc.quotas[0] = 1.0;
  my_proc.quotas[1] = 2.0;
  my_proc.quotas[2] = 1.0;
  
  my_proc.maximums = malloc(4 * sizeof(double*));
  for (i = 1; i <= my_proc.no_students; i++) {
  my_proc.maximums[i-1] = malloc(3 * sizeof(double));
    for (j = 1; j <= my_proc.no_schools; j++) {
      my_proc.maximums[i-1][j-1] = 1.0; 
    }
  }

  my_proc.maximums[1][1] = 0.0;
  my_proc.maximums[1][2] = 0.0;
  
  return my_proc;
}

struct subset nonnull_schools(struct process_cee* my_cee) {
  
  int nsc = my_cee->no_schools;

  struct subset nonnull = nullset(nsc);
  for (int j = 1; j <= nsc; j++) {
    if (my_cee->quotas[j-1] > 0.000001) {
      add_element(&nonnull,j);
    }
  }

  return nonnull;
}

struct process_cee cee_without_null_schools(struct process_cee* my_cee, struct subset* nonnull)  {
  int i, j, k, count;
  
  struct process_cee new_cee;

  int nst = my_cee->no_students;
  int nsc = nonnull->subset_size;

  new_cee.no_students = nst;
  new_cee.no_schools = nsc;

  int* old_sch_no = malloc(nsc * sizeof(int));
  count = 0;
  for (j = 1; j <= my_cee->no_schools; j++) {
    if (nonnull->indicator[j-1] == 1) {
      count++;
      old_sch_no[count - 1] = j;
    }
  }

  new_cee.quotas = malloc(nsc * sizeof(double));
  for (k = 1; k <= nsc; k++) {
    new_cee.quotas[k-1] = my_cee->quotas[old_sch_no[k-1]-1];
  }

  new_cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (k = 1; k <= nsc; k++) {
      new_cee.maximums[i-1][k-1] = my_cee->maximums[i-1][old_sch_no[k-1]-1];
    }
  }

  free(old_sch_no);
  
  new_cee.time_remaining = my_cee->time_remaining;

  return new_cee;
}

struct process_cee process_cee_from_input_cee(struct input_cee* my_cee) {
  int i, j;
  
  struct process_cee new_cee;

  int nst = my_cee->no_students;
  int nsc = my_cee->no_schools;

  new_cee.no_students = nst;
  new_cee.no_schools = nsc;

  new_cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_cee.quotas[j-1] = (double)my_cee->quotas[j-1];
  }

  new_cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      if (my_cee->priority[i-1][j-1] > 0) {
	new_cee.maximums[i-1][j-1] = 1.0;
      }
      else {
	new_cee.maximums[i-1][j-1] = 0.0;
      }
    }
  }

  new_cee.time_remaining = 1.0;

  return new_cee;
}



struct process_cee crit_compl_sub_process_cee(struct process_cee* my_cee, struct subset* J_subset,
					struct subset* P_subset) {
  struct process_cee new_cee;
  
  int i, j, k;

  struct subset J_compl, P_compl;
  J_compl = complement_of_subset(J_subset);
  P_compl = complement_of_subset(P_subset);
  struct index J_index, P_index;
  J_index = index_of_subset(&J_compl);
  P_index = index_of_subset(&P_compl);

  int nst = J_compl.subset_size;
  int nsc = P_compl.subset_size;

  new_cee.no_students = nst;
  new_cee.no_schools = nsc;

  (new_cee).quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_cee.quotas[j-1] = my_cee->quotas[P_index.indices[j-1]-1];
    for (k = 1; k <= my_cee->no_students; k++) {
      if (J_subset->indicator[k-1] == 1) {
	new_cee.quotas[j-1] -= my_cee->maximums[k-1][P_index.indices[j-1]-1];
      }
    }
  }

  new_cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      new_cee.maximums[i-1][j-1] =
	my_cee->maximums[J_index.indices[i-1]-1][P_index.indices[j-1]-1];
    }
  }

  new_cee.time_remaining = my_cee->time_remaining;

  destroy_subset(J_compl);
  destroy_subset(P_compl);
  destroy_index(J_index);
  destroy_index(P_index);

  /*  print_process_cee(&(new_cee)); */
  
  return new_cee;    
}
