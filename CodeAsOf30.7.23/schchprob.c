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

  /*
  printf("\nWe are now entering sub_sch_ch_prob with %i students and %i schools.\n",
	 stu_subset->subset_size, sch_subset->subset_size);  
  printf("The entry sch_ch_prob is\n");
  print_sch_ch_prob(my_scp);
  printf("The student subset is ");
  print_subset(stu_subset);
  printf(".\nThe school subset is ");
  print_subset(sch_subset);
  printf(".\n");
  printf("The stu_index is ");
  print_index(&stu_index);
  printf("\nThe sch_index is ");
  print_index(&sch_index);
  printf("\n");
  */
  
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

double time_remaining_of_gmc_equality(struct sch_ch_prob* my_scp, struct subset* school_subset,
				                                  struct subset* eating_students,
				                                  struct subset* captive_students,
				      struct subset* overallocated_schools) {
  int i,j;
  int nst = my_scp->cee.no_students; /* my_scp and school_subset are inputs. */
  int nsc = my_scp->cee.no_schools;  /* captive_students and the return value are outputs. */

  /*
  printf("The school subset is ");
  print_subset(school_subset);
  printf(".\n");
  */

  destroy_subset(*captive_students);
  *captive_students = fullset(nst);
  for (i = 1; i <= nst; i++) {
    j = 1;
    while (j <= nsc && captive_students->indicator[i-1] == 1) {
      if (school_subset->indicator[j-1] == 0 && my_scp->cee.priority[i-1][j-1] > 0) {
	captive_students->subset_size--;
	captive_students->indicator[i-1] = 0;
      }
      j++;
    }
  }
  
  destroy_subset(*eating_students);
  *eating_students = nullset(nst);
  for (i = 1; i <= nst; i++) {
    if (school_subset->indicator[my_scp->preferences[i-1][0] - 1] == 1) {
      eating_students->subset_size++;
      eating_students->indicator[i-1] = 1;
    }
  }
  
  double total_quota = 0.0;
  for (j = 1; j <= nsc; j++) {
    if (school_subset->indicator[j-1] == 1) {
      total_quota += my_scp->cee.quotas[j-1];
    }
  }
  
  if (total_quota - captive_students->subset_size * my_scp->time_remaining < -0.000001) {

    /*
    printf("\nAt over_allocation we have total quota = %1.3f, subset_size = %d, and time_remaining = %1.3f.\n",total_quota,
	   captive_students->subset_size, my_scp->time_remaining);

    struct index captive_index = index_of_subset(captive_students);
    printf("The captive students are ");
    print_index(&captive_index);
    destroy_index(captive_index);
    printf(".\n");
    */
    
    copy_subset(school_subset,overallocated_schools);
    return 0.0;
  }
  
  if (eating_students->subset_size == captive_students->subset_size) {
    return 0.0;
  }
    
  double delta_t = (total_quota - captive_students->subset_size * my_scp->time_remaining) /
    (eating_students->subset_size - captive_students->subset_size);
    
  if (delta_t <= my_scp->time_remaining) {
    return my_scp->time_remaining - delta_t;
  }
  else {
    return 0.0;
  }
}

double time_rem_after_first_gmc_eq(struct sch_ch_prob* my_scp, struct square_matrix* related,
				   int depth,
				   struct subset* crit_stu_subset,
				   struct subset* crit_sch_subset,
				   struct subset* overallocated_schools,
				   struct subset_list* known_facets) {
  int j;
  
  double scan_answer;
  double answer = 0.0;

  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  struct subset eating_students = nullset(nst);
  struct subset crit_eat_students = nullset(nst);
  struct subset captive_students = nullset(nst);

  j = 0;
  while (overallocated_schools->subset_size == 0 && j < nsc) {
    j++;

    struct subset scan_subset = singleton_subset(j,nsc);
    
    scan_answer = time_remaining_of_gmc_equality(my_scp, &scan_subset, &eating_students, &captive_students,
						 overallocated_schools);
    
    if (scan_answer > answer + 0.000001) {
      answer = scan_answer;
      copy_subset(&scan_subset,crit_sch_subset);
      copy_subset(&captive_students,crit_stu_subset);
      copy_subset(&eating_students,&crit_eat_students);
    }

    destroy_subset(scan_subset);  
  }

  if (!is_empty_list(known_facets)) {
    struct subset_list* probe = known_facets;
  
    while (overallocated_schools->subset_size == 0 && probe != NULL) {

      struct subset scan_subset = subset_of_index(probe->node_index,nsc);
    
      scan_answer = time_remaining_of_gmc_equality(my_scp, &scan_subset, &eating_students,
						   &captive_students, overallocated_schools);
    
      if (scan_answer > answer) {
	answer = scan_answer;
	copy_subset(&scan_subset,crit_sch_subset);
	copy_subset(&captive_students,crit_stu_subset);
	copy_subset(&eating_students,&crit_eat_students);
      }

      probe = probe->next; 
      destroy_subset(scan_subset);  
    }
  }

  int subset_size = 2;
  
  while (overallocated_schools->subset_size == 0 && subset_size <= depth) {

    struct subset scan_subset = nullset(nsc);

    /* int count = 0; */
  
    while (overallocated_schools->subset_size == 0 &&
	   next_subset(&scan_subset, related, subset_size)) {

      /* count++; */
      
      scan_answer = time_remaining_of_gmc_equality(my_scp, &scan_subset, &eating_students,
						   &captive_students, overallocated_schools);
    
      if (scan_answer > answer) {
	answer = scan_answer;
	copy_subset(&scan_subset,crit_sch_subset);
	copy_subset(&captive_students,crit_stu_subset);
	copy_subset(&eating_students,&crit_eat_students);
      }
    }

    /*
    if (subset_size >= 2) {
      printf("At subset_size %d there were %d subsets.\n",subset_size,count);
    }
    */
      
    destroy_subset(scan_subset);
    
    subset_size++;
  }

  destroy_subset(eating_students);
  destroy_subset(captive_students);

  return answer;
}

double new_time_rem_after_first_gmc_eq(struct sch_ch_prob* my_scp, struct square_matrix* related,
				       struct subset* target, int depth,
				       struct subset* crit_stu_subset,
				       struct subset* crit_sch_subset,
				       struct subset* overallocated_schools,
				       struct subset_list* known_facets) {
  int j;
  
  double scan_answer;
  double answer = 0.0;

  int nsc = my_scp->cee.no_schools;

  struct subset eating_students = nullset(my_scp->cee.no_students);
  struct subset crit_eat_students = nullset(my_scp->cee.no_students);
  struct subset captive_students = nullset(my_scp->cee.no_students);

  j = 0;
  while (overallocated_schools->subset_size == 0 && j < nsc) {
    j++;

    struct subset scan_subset = singleton_subset(j,nsc);
    
    scan_answer = time_remaining_of_gmc_equality(my_scp, &scan_subset, &eating_students, &captive_students,
						 overallocated_schools);
    
    if (scan_answer > answer + 0.000001) {
      answer = scan_answer;
      copy_subset(&scan_subset,crit_sch_subset);
      copy_subset(&captive_students,crit_stu_subset);
      copy_subset(&eating_students,&crit_eat_students);
    }

    destroy_subset(scan_subset);  
  }

  struct subset_list* probe = known_facets;
  
  while (overallocated_schools->subset_size == 0 && probe != NULL) {

    struct subset scan_subset = subset_of_index(probe->node_index,nsc);
    
    scan_answer = time_remaining_of_gmc_equality(my_scp, &scan_subset, &eating_students, &captive_students,
						 overallocated_schools);
    
    if (scan_answer > answer) {
      answer = scan_answer;
      copy_subset(&scan_subset,crit_sch_subset);
      copy_subset(&captive_students,crit_stu_subset);
      copy_subset(&eating_students,&crit_eat_students);
    }

    probe = probe->next; 
    destroy_subset(scan_subset);  
  }

  int subset_size = 2;
  
  while (overallocated_schools->subset_size == 0 && subset_size <= depth) {

    struct subset scan_subset = nullset(nsc);
  
    while (overallocated_schools->subset_size == 0 && new_next_subset(&scan_subset,target,related,subset_size)) {
    
      scan_answer = time_remaining_of_gmc_equality(my_scp, &scan_subset, &eating_students, &captive_students,
						   overallocated_schools);
    
      if (scan_answer > answer) {
	answer = scan_answer;
	copy_subset(&scan_subset,crit_sch_subset);
	copy_subset(&captive_students,crit_stu_subset);
	copy_subset(&eating_students,&crit_eat_students);
      }
    }
    
    destroy_subset(scan_subset);
    subset_size++;
  }


  destroy_subset(eating_students);
  destroy_subset(captive_students);

  return answer;
}


