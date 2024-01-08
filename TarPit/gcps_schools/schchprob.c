#include "schchprob.h"

struct input_sch_ch_prob make_toy_sch_ch_prob() {
  int i,j;
  struct input_sch_ch_prob my_scp;
   
  my_scp.cee = make_toy_cee();

  int nst = my_scp.cee.no_students;
  int nsc = my_scp.cee.no_schools;

  my_scp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    my_scp.no_eligible_schools[i-1] = nsc;
  }

  my_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    my_scp.preferences[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      my_scp.preferences[i-1][j-1] = j;
    }
  }

  my_scp.priority_threshold = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    my_scp.priority_threshold[j-1] = 1;
  }

  return my_scp;
}

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_scp) {
  free(my_scp.priority_threshold);
  for (int i = 1; i <= my_scp.cee.no_students; i++) {
    free(my_scp.preferences[i-1]);
  }
  free(my_scp.preferences);
  free(my_scp.no_eligible_schools);
  destroy_input_cee(my_scp.cee);
}

void print_input_sch_ch_prob(struct input_sch_ch_prob* my_scp) {
  int i,j;
  print_input_cee(&(my_scp->cee));

  printf("The students numbers of ranked schools are ");
  printf("(");
  for (i = 1; i <= my_scp->cee.no_students - 1; i++) {
    printf("%i,", my_scp->no_eligible_schools[i-1]);
  }
    printf("%i)\n", my_scp->no_eligible_schools[my_scp->cee.no_students-1]);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= my_scp->cee.no_students; i++) {
    printf("%i:  ", i);
    for (j = 1; j <= my_scp->no_eligible_schools[i-1]; j++) {
      printf("%i  ",  my_scp->preferences[i-1][j-1]);
    }
    printf("\n");
  }
  
  printf("The priority thresholds of the schools are\n");
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    printf("%i   ", my_scp->priority_threshold[j-1]);
  }
  printf("\n");
}

struct process_scp process_scp_from_input(struct input_sch_ch_prob* my_scp) {
  
  struct process_scp new_scp;

  new_scp.cee = process_cee_from_input_cee(&(my_scp->cee));
  
  int i, j, k, count;

  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (my_scp->cee.priority[i-1][j-1] < my_scp->priority_threshold[j-1] ||
	  my_scp->cee.priority[i-1][j-1] == 0) {
	new_scp.cee.maximums[i-1][j-1] = 0.0;
      }
      else {
	new_scp.cee.maximums[i-1][j-1] = 1.0;
      }
    }
  }

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    count = 0;
    for (j = 1; j <= nsc; j++) {
      if (new_scp.cee.maximums[i-1][j-1] > 0.000001) {
	count++;
      }
    }
    new_scp.no_eligible_schools[i-1] = count;
  }
  
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    count = 0;
    for (k = 1; k <= my_scp->no_eligible_schools[i-1]; k++) {
      if (new_scp.cee.maximums[i-1][my_scp->preferences[i-1][k-1]-1] > 0.000001) {
	count++;
	new_scp.preferences[i-1][count-1] = my_scp->preferences[i-1][k-1];
      }
    }
  }

  return new_scp;
} 

void destroy_process_scp(struct process_scp my_scp) {
  int i;
  
  int nst = (my_scp.cee).no_students;

  destroy_process_cee(my_scp.cee);
  
  free(my_scp.no_eligible_schools);
  
  for (i = 1; i <= nst; i++) {
    free(my_scp.preferences[i-1]);
  }
  free(my_scp.preferences);
}

void print_process_scp(struct process_scp* my_scp) {
  int i,j;
  print_process_cee(&my_scp->cee);
  
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

void copy_process_scp(struct process_scp* my_scp, struct process_scp* copy) {
  int i, j;

  int nst = (my_scp->cee).no_students;
  
  copy_process_cee(&(my_scp->cee),&(copy->cee));
  
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

struct process_scp make_toy_process_scp() {
  struct process_scp my_scp;
  my_scp.cee = make_toy_process_cee();

  my_scp.no_eligible_schools = malloc(4 * sizeof(int));
  my_scp.no_eligible_schools[0] = 3;
  my_scp.no_eligible_schools[1] = 1;
  my_scp.no_eligible_schools[2] = 3;
  my_scp.no_eligible_schools[3] = 3;

  my_scp.preferences = malloc(4 * sizeof(int*));
  my_scp.preferences[0] = malloc(3 * sizeof(int));
  my_scp.preferences[1] = malloc(1 * sizeof(int));
  my_scp.preferences[2] = malloc(3 * sizeof(int));
  my_scp.preferences[3] = malloc(3 * sizeof(int));

  my_scp.preferences[0][0] = 1;
  my_scp.preferences[1][0] = 1;
  my_scp.preferences[2][0] = 1;
  my_scp.preferences[3][0] = 1;

  my_scp.preferences[0][1] = 2;
  my_scp.preferences[2][1] = 2;
  my_scp.preferences[3][1] = 2;

  my_scp.preferences[0][2] = 3;
  my_scp.preferences[2][2] = 3;
  my_scp.preferences[3][2] = 3;

  return my_scp;
}

struct process_cee critical_sub_process_cee(struct process_cee* my_cee, struct subset* J_subset) {
  struct process_cee new_cee;
  
  int i, j;
  struct index J_index;
  J_index = index_of_subset(J_subset);

  int nst = J_subset->subset_size;
  int nsc = my_cee->no_schools;

  new_cee.no_students = nst;
  new_cee.no_schools = nsc;

  (new_cee).quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_cee.quotas[j-1] = my_cee->quotas[j-1];
  }

  new_cee.maximums = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    new_cee.maximums[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      new_cee.maximums[i-1][j-1] = my_cee->maximums[J_index.indices[i-1]-1][j-1];
    }
  }

  new_cee.time_remaining = my_cee->time_remaining;

  destroy_index(J_index);
  return new_cee;    
}

struct process_scp critical_sub_process_scp(struct process_scp* my_scp, struct subset* J_subset) {
  struct process_scp new_scp;

  new_scp.cee = critical_sub_process_cee(&(my_scp->cee), J_subset);
  
  int i, k;
  struct index J_index;
  J_index = index_of_subset(J_subset);

  int nst = J_subset->subset_size;
  int nsc = my_scp->cee.no_schools;

  new_scp.cee.no_students = nst;
  new_scp.cee.no_schools = nsc;

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = my_scp->no_eligible_schools[J_index.indices[i-1]-1];
  }
  
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= new_scp.no_eligible_schools[i-1]; k++) {
      new_scp.preferences[i-1][k-1] = my_scp->preferences[J_index.indices[i-1]-1][k-1];
    }
  }

  destroy_index(J_index);
  return new_scp;    
}


struct process_scp crit_compl_sub_process_scp(struct process_scp* my_scp, struct subset* J_subset,
					struct subset* P_subset) {
  struct process_scp new_scp;
  
  int i, j, l, count;

  new_scp.cee = crit_compl_sub_process_cee(&(my_scp->cee), J_subset,P_subset);

  struct subset J_compl, P_compl;
  J_compl = complement_of_subset(J_subset);
  P_compl = complement_of_subset(P_subset);
  struct index J_index, P_index;
  J_index = index_of_subset(&J_compl);
  P_index = index_of_subset(&P_compl);

  int* reverse_P_index = malloc(my_scp->cee.no_schools * sizeof(int));
  count = 0;
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    if (P_compl.indicator[j-1] == 1) {
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
  new_scp.cee.no_schools = nsc;

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = 0;
    for (l = 1; l <= my_scp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[my_scp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.no_eligible_schools[i-1]++;
      }
    }
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
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


