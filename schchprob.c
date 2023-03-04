#include "schchprob.h"

struct sch_ch_prob make_toy_sch_ch_prob() {
  int i,j;
  struct sch_ch_prob my_sch_ch_prob;
  my_sch_ch_prob.cee = make_toy_double_cee();

  my_sch_ch_prob.no_eligible_schools = malloc(my_sch_ch_prob.cee.no_students * sizeof(int));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    my_sch_ch_prob.no_eligible_schools[i-1] = my_sch_ch_prob.cee.no_schools;
  }

  my_sch_ch_prob.preferences = malloc(my_sch_ch_prob.cee.no_students * sizeof(int*));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    my_sch_ch_prob.preferences[i-1] = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int));
    for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
      my_sch_ch_prob.preferences[i-1][j-1] = j;
    }
  }

  my_sch_ch_prob.priority_threshold = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int));
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
    my_sch_ch_prob.priority_threshold[j-1] = 1;
  }

  my_sch_ch_prob.time_remaining = 1.0;

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

void print_sch_ch_prob(struct sch_ch_prob my_sch_ch_prob) {
  int i,j;
  printf("The cee is\n");
  print_double_cee(my_sch_ch_prob.cee);
  
  printf("The students' preferences are\n");
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    printf("%i:  ", i);
    for (j = 1; j <= my_sch_ch_prob.no_eligible_schools[i-1]; j++) {
      printf("%i  ",  my_sch_ch_prob.preferences[i-1][j-1]);
    }
    printf("\n");
  }
  
  printf("The schools' priority thresholds are\n");
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
    printf("%i   ", my_sch_ch_prob.priority_threshold[j-1]);
  }
  printf("\n");

  printf("The time remaining is %1.2f.\n",my_sch_ch_prob.time_remaining);
}

    
