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

void copy_sch_ch_prob(struct sch_ch_prob* my_scp, struct sch_ch_prob* copy) {
  int i,j,k;

  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  (copy->cee).no_students = (my_scp->cee).no_students;
  (copy->cee).no_schools = (my_scp->cee).no_schools;

  copy->cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    copy->cee.quotas[j-1] = my_scp->cee.quotas[j-1];
  }

  copy->cee.priority = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    copy->cee.priority[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      copy->cee.priority[i-1][j-1] = my_scp->cee.priority[i-1][j-1];
    }
  }

  copy->no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    copy->no_eligible_schools[i-1] = my_scp->no_eligible_schools[i-1];
  }

  copy->preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    copy->preferences[i-1] = malloc(copy->no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= copy->no_eligible_schools[i-1]; k++) {
      copy->preferences[i-1][k-1] = my_scp->preferences[i-1][k-1];
    }
  }

  copy->priority_threshold = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    copy->priority_threshold[j-1] = my_scp->priority_threshold[j-1];
  }

  copy->time_remaining = my_scp->time_remaining;
}

void destroy_sch_ch_prob(struct sch_ch_prob my_scp) {
  free(my_scp.priority_threshold);
  
  for (int i = 1; i <= my_scp.cee.no_students; i++) {
    free(my_scp.preferences[i-1]);
  }
  free(my_scp.preferences);
  
  free(my_scp.no_eligible_schools);
  
  destroy_double_cee(my_scp.cee);
}

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_scp) {
  free(my_scp.priority_threshold);
  for (int i = 1; i <= my_scp.cee.no_students; i++) {
    free(my_scp.preferences[i-1]);
  }
  free(my_scp.preferences);
  free(my_scp.no_eligible_schools);
  destroy_int_cee(my_scp.cee);
}

void print_sch_ch_prob(struct sch_ch_prob* my_scp) {
  int i,j;
  print_double_cee(&my_scp->cee);
  
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
  
  printf("The priority thresholds of the schools are\n");
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    printf("%i   ", my_scp->priority_threshold[j-1]);
  }
  printf("\n");

  printf("The time remaining is %1.8f\n",my_scp->time_remaining);
}

void print_input_sch_ch_prob(struct input_sch_ch_prob* my_scp) {
  int i,j;
  print_int_cee(&(my_scp->cee));

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


struct sch_ch_prob sch_ch_prob_from_input(struct input_sch_ch_prob* my_scp) {
  int i, j, k;
  struct sch_ch_prob new_sch_ch_prob;

  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  new_sch_ch_prob.cee.no_students = nst;
  new_sch_ch_prob.cee.no_schools = nsc;

  new_sch_ch_prob.cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.cee.quotas[j-1] = (double)my_scp->cee.quotas[j-1];
  }

  new_sch_ch_prob.cee.priority = malloc(nst *sizeof(int*));
  for (i = 1; i <= nst; i++) {
  new_sch_ch_prob.cee.priority[i-1] = malloc(nsc *sizeof(int));
    for (j = 1; j <= nsc; j++) {
      new_sch_ch_prob.cee.priority[i-1][j-1] = my_scp->cee.priority[i-1][j-1];
    }
  }

  new_sch_ch_prob.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.no_eligible_schools[i-1] = my_scp->no_eligible_schools[i-1];
  }

  new_sch_ch_prob.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.preferences[i-1]=malloc(new_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    for (k = 1; k <= new_sch_ch_prob.no_eligible_schools[i-1]; k++) {
      new_sch_ch_prob.preferences[i-1][k-1] = my_scp->preferences[i-1][k-1];
    }
  }

  new_sch_ch_prob.priority_threshold = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.priority_threshold[j-1] = my_scp->priority_threshold[j-1];
  }

  new_sch_ch_prob.time_remaining = 1.0;

  return new_sch_ch_prob;
}

struct sch_ch_prob sub_sch_ch_prob(struct sch_ch_prob* my_scp, 
				   struct subset* stu_subset, struct subset* sch_subset) {
  int i, j, k;

  int nst = stu_subset->subset_size;  
  int nsc = sch_subset->subset_size;
  
  struct sch_ch_prob new_sch_ch_prob;

  new_sch_ch_prob.cee.no_students = nst;
  new_sch_ch_prob.cee.no_schools = nsc;

  struct index stu_index;
  stu_index = index_of_subset(stu_subset);
  
  struct index sch_index;
  sch_index = index_of_subset(sch_subset);
  
  new_sch_ch_prob.cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.cee.quotas[j-1] = my_scp->cee.quotas[sch_index.indices[j-1]-1];
  }

  new_sch_ch_prob.cee.priority = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.cee.priority[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      new_sch_ch_prob.cee.priority[i-1][j-1] =
	my_scp->cee.priority[stu_index.indices[i-1]-1][sch_index.indices[j-1]-1];
    }
  }

  new_sch_ch_prob.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    int count = 0;
    for (j = 1; j <= nsc; j++) {
      if (new_sch_ch_prob.cee.priority[i-1][j-1] > 0) {
	count++;
      }
    }
    new_sch_ch_prob.no_eligible_schools[i-1] = count;
  }

  int* inverse_sch_index = malloc(my_scp->cee.no_schools * sizeof(int));
  k = 0;
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    if (sch_subset->indicator[j-1] == 1) {
      k++;
      inverse_sch_index[j-1] = k;
    }
    else {
      inverse_sch_index[j-1] = 0;
    }
  }
  
  new_sch_ch_prob.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.preferences[i-1]=malloc(new_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    int count = 0;
    for (k = 1; k <= my_scp->no_eligible_schools[stu_index.indices[i-1]-1]; k++) {
      if (sch_subset->indicator[my_scp->preferences[stu_index.indices[i-1]-1][k-1]-1] == 1) {
	new_sch_ch_prob.preferences[i-1][count] =
	  inverse_sch_index[my_scp->preferences[stu_index.indices[i-1]-1][k-1]-1];
	count++;
      }
    }
  } 
  free(inverse_sch_index);

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= new_sch_ch_prob.no_eligible_schools[i-1]; k++) {
      if (new_sch_ch_prob.preferences[i-1][k-1] == 0) {
	printf("We have a zero with i = %i and k = %i.\n",i,k);
	exit(0);
      }
    }
  }

  new_sch_ch_prob.priority_threshold = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.priority_threshold[j-1] = my_scp->priority_threshold[sch_index.indices[j-1]-1];
  }

  new_sch_ch_prob.time_remaining = my_scp->time_remaining;

  destroy_index(stu_index);
  destroy_index(sch_index);

  return new_sch_ch_prob;
}

struct sch_ch_prob reduced_sch_ch_prob(struct sch_ch_prob* my_scp) {
  int i, j, k, count;
  struct sch_ch_prob new_sch_ch_prob;

  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  new_sch_ch_prob.cee.no_students = nst;
  new_sch_ch_prob.cee.no_schools = nsc;

  new_sch_ch_prob.cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.cee.quotas[j-1] = my_scp->cee.quotas[j-1];
  }

  new_sch_ch_prob.cee.priority = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.cee.priority[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (my_scp->cee.priority[i-1][j-1] <
	  my_scp->priority_threshold[j-1] ||
	  my_scp->cee.priority[i-1][j-1] == 0) {
	new_sch_ch_prob.cee.priority[i-1][j-1] = 0;
      }
      else {
	new_sch_ch_prob.cee.priority[i-1][j-1] = 1;
      }
    }
  }

  new_sch_ch_prob.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    count = 0;
    for (j = 1; j <= nsc; j++) {
      if (new_sch_ch_prob.cee.priority[i-1][j-1] == 1) {
	count++;
      }
    }
    
    if (count == 0) {
      printf("Student %i has no eligible schools.\n",i);
      exit(0);
    }
    new_sch_ch_prob.no_eligible_schools[i-1] = count;
  }
  
  new_sch_ch_prob.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.preferences[i-1]=malloc(new_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    j = 1;
    for (k = 1; k <= new_sch_ch_prob.no_eligible_schools[i-1]; k++) {
      while (new_sch_ch_prob.cee.priority[i-1][j-1] == 0) {
	j++;
      }
      new_sch_ch_prob.preferences[i-1][k-1] = j;
      j++;
    }
  }

  new_sch_ch_prob.priority_threshold = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.priority_threshold[j-1] = 1;
  }

  new_sch_ch_prob.time_remaining = my_scp->time_remaining;

  return new_sch_ch_prob;
}

