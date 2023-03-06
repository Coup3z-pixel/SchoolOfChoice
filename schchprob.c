#include "schchprob.h"

struct input_sch_ch_prob make_toy_sch_ch_prob() {
  int i,j;
  struct input_sch_ch_prob my_sch_ch_prob;
  my_sch_ch_prob.cee = make_toy_cee();

  my_sch_ch_prob.no_eligible_schools = malloc(my_sch_ch_prob.cee.no_students * sizeof(int));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    my_sch_ch_prob.no_eligible_schools[i-1] = my_sch_ch_prob.cee.no_schools;
  }

  my_sch_ch_prob.preferences = malloc(my_sch_ch_prob.cee.no_students * sizeof(int*));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    my_sch_ch_prob.preferences[i-1] = malloc(my_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    for (j = 1; j <= my_sch_ch_prob.no_eligible_schools[i-1]; j++) {
      my_sch_ch_prob.preferences[i-1][j-1] = j;
    }
  }

  my_sch_ch_prob.priority_threshold = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int));
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
    my_sch_ch_prob.priority_threshold[j-1] = 1;
  }

  return my_sch_ch_prob;
}

void destroy_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob) {
  free(my_sch_ch_prob.priority_threshold);
  for (int i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    free(my_sch_ch_prob.preferences[i-1]);
  }
  free(my_sch_ch_prob.preferences);
  free(my_sch_ch_prob.no_eligible_schools);
  destroy_double_cee(my_sch_ch_prob.cee);
}

void destroy_input_sch_ch_prob(struct input_sch_ch_prob my_sch_ch_prob) {
  free(my_sch_ch_prob.priority_threshold);
  for (int i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    free(my_sch_ch_prob.preferences[i-1]);
  }
  free(my_sch_ch_prob.preferences);
  free(my_sch_ch_prob.no_eligible_schools);
  destroy_int_cee(my_sch_ch_prob.cee);
}

void print_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob) {
  int i,j;
  print_double_cee(my_sch_ch_prob.cee);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    printf("%i:  ", i);
    for (j = 1; j <= my_sch_ch_prob.no_eligible_schools[i-1]; j++) {
      printf("%i  ",  my_sch_ch_prob.preferences[i-1][j-1]);
    }
    printf("\n");
  }
  
  printf("The priority thresholds of the schools are\n");
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
    printf("%i   ", my_sch_ch_prob.priority_threshold[j-1]);
  }
  printf("\n");

  printf("The time remaining is %1.2f\n",my_sch_ch_prob.time_remaining);
}

void print_input_sch_ch_prob(struct input_sch_ch_prob my_sch_ch_prob) {
  int i,j;
  print_int_cee(my_sch_ch_prob.cee);

  printf("The students numbers of ranked schools are ");
  printf("(");
  for (i = 1; i <= my_sch_ch_prob.cee.no_students - 1; i++) {
    printf("%i,", my_sch_ch_prob.no_eligible_schools[i-1]);
  }
    printf("%i)\n", my_sch_ch_prob.no_eligible_schools[my_sch_ch_prob.cee.no_students-1]);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    printf("%i:  ", i);
    for (j = 1; j <= my_sch_ch_prob.no_eligible_schools[i-1]; j++) {
      printf("%i  ",  my_sch_ch_prob.preferences[i-1][j-1]);
    }
    printf("\n");
  }
  
  printf("The priority thresholds of the schools are\n");
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
    printf("%i   ", my_sch_ch_prob.priority_threshold[j-1]);
  }
  printf("\n");
}


struct sch_ch_prob sch_ch_prob_from_input(struct input_sch_ch_prob my_sch_ch_prob) {
  int i, j, k;
  struct sch_ch_prob new_sch_ch_prob;

  int nst = my_sch_ch_prob.cee.no_students;
  int nsc = my_sch_ch_prob.cee.no_schools;

  new_sch_ch_prob.cee.no_students = nst;
  new_sch_ch_prob.cee.no_schools = nsc;

  new_sch_ch_prob.cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.cee.quotas[j-1] = (double)my_sch_ch_prob.cee.quotas[j-1];
  }

  new_sch_ch_prob.cee.priority = malloc(nst *sizeof(int*));
  for (i = 1; i <= nst; i++) {
  new_sch_ch_prob.cee.priority[i-1] = malloc(nsc *sizeof(int));
    for (j = 1; j <= nsc; j++) {
      new_sch_ch_prob.cee.priority[i-1][j-1] = my_sch_ch_prob.cee.priority[i-1][j-1];
    }
  }

  new_sch_ch_prob.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.no_eligible_schools[i-1] = my_sch_ch_prob.no_eligible_schools[i-1];
  }

  new_sch_ch_prob.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.preferences[i-1]=malloc(new_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    for (k = 1; k <= new_sch_ch_prob.no_eligible_schools[i-1]; k++) {
      new_sch_ch_prob.preferences[i-1][k-1] = my_sch_ch_prob.preferences[i-1][k-1];
    }
  }

  new_sch_ch_prob.priority_threshold = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.priority_threshold[j-1] = my_sch_ch_prob.priority_threshold[j-1];
  }

  new_sch_ch_prob.time_remaining = 1.0;

  return new_sch_ch_prob;
}

struct sch_ch_prob reduced_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob) {
  int i, j, k, count;
  struct sch_ch_prob new_sch_ch_prob;

  int nst = my_sch_ch_prob.cee.no_students;
  int nsc = my_sch_ch_prob.cee.no_schools;

  new_sch_ch_prob.cee.no_students = nst;
  new_sch_ch_prob.cee.no_schools = nsc;

  new_sch_ch_prob.cee.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_sch_ch_prob.cee.quotas[j-1] = my_sch_ch_prob.cee.quotas[j-1];
  }

  new_sch_ch_prob.cee.priority = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_sch_ch_prob.cee.priority[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (my_sch_ch_prob.cee.priority[i-1][j-1] <
	  my_sch_ch_prob.priority_threshold[j-1] ||
	  my_sch_ch_prob.cee.priority[i-1][j-1] == 0) {
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

  new_sch_ch_prob.time_remaining = my_sch_ch_prob.time_remaining;

  return new_sch_ch_prob;
}
    
