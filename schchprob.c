#include "schchprob.h"

struct input_sch_ch_prob make_toy_sch_ch_prob() {
  int i,j;
  struct input_sch_ch_prob my_scp;
  my_scp.cee = make_toy_cee();

  my_scp.no_eligible_schools = malloc(my_scp.cee.no_students * sizeof(int));
  for (i = 1; i <= my_scp.cee.no_students; i++) {
    my_scp.no_eligible_schools[i-1] = my_scp.cee.no_schools;
  }

  my_scp.preferences = malloc(my_scp.cee.no_students * sizeof(int*));
  for (i = 1; i <= my_scp.cee.no_students; i++) {
    my_scp.preferences[i-1] = malloc(my_scp.no_eligible_schools[i-1]*sizeof(int));
    for (j = 1; j <= my_scp.no_eligible_schools[i-1]; j++) {
      my_scp.preferences[i-1][j-1] = j;
    }
  }

  my_scp.priority_threshold = malloc(my_scp.cee.no_schools * sizeof(int));
  for (j = 1; j <= my_scp.cee.no_schools; j++) {
    my_scp.priority_threshold[j-1] = 1;
  }

  return my_scp;
}

void destroy_sch_ch_prob(struct sch_ch_prob* my_scp) {
  free(my_scp->priority_threshold);
  for (int i = 1; i <= my_scp->cee.no_students; i++) {
    free(my_scp->preferences[i-1]);
  }
  free(my_scp->preferences);
  free(my_scp->no_eligible_schools);
  destroy_double_cee(&my_scp->cee);
}

void destroy_input_sch_ch_prob(struct input_sch_ch_prob* my_scp) {
  free(my_scp->priority_threshold);
  for (int i = 1; i <= my_scp->cee.no_students; i++) {
    free(my_scp->preferences[i-1]);
  }
  free(my_scp->preferences);
  free(my_scp->no_eligible_schools);
  destroy_int_cee(&my_scp->cee);
}

void print_sch_ch_prob(struct sch_ch_prob* my_scp) {
  int i,j;
  print_double_cee(&my_scp->cee);
  
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

  printf("The time remaining is %1.2f\n",my_scp->time_remaining);
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
      if (count == 0) {
	printf("Student %i has no eligible schools.\n",i);
	exit(0);
      }
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

double time_remaining_of_gmc_equality(struct sch_ch_prob* my_scp, struct subset* school_subset){
  int i,j;
  struct subset student_subset; /* those who must eat school_subset */
  student_subset = fullset(my_scp->cee.no_students);
  for (i = 1; i <= my_scp->cee.no_students; i++) {
    j = 1;
    while (j <= my_scp->cee.no_schools && student_subset.indicator[i-1] == 1) {
      if (school_subset->indicator[j-1] == 0 && my_scp->cee.priority[i-1][j-1] > 0) {
	student_subset.indicator[i-1] = 0;
      }
    }
  }

  double total_quota = 0.0;
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    if (school_subset->indicator[j-1] == 1) {
      total_quota += my_scp->cee.quotas[j-1];
    }
  }

  int no_eaters = 0;
  for (i = 1; i <= my_scp->cee.no_students; i++) {
    if (school_subset->indicator[my_scp->preferences[i-1][0]] == 1) {
      no_eaters++;
    }
  }

  if (no_eaters == student_subset.subset_size) {
    return -1.0;
  }
  else {
    double delta_t = (total_quota - student_subset.subset_size * my_scp->time_remaining) /
      (no_eaters - student_subset.subset_size);
    destroy_subset(&student_subset);
    if (delta_t <= my_scp->time_remaining) {
      return my_scp->time_remaining - delta_t;
    }
    else {
      return -1.0;
    }
  }
}
    
