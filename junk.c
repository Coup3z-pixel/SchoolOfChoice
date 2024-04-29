

/* A CURRENTLY UNUSED TEST */

/*
int theta_is_valid(int** theta, struct process_cee* working_cee,
		   struct partial_alloc* feasible_guide,
		   struct index* alpha, int* favorites, int* fully_allocated) {
  int i, j;

  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (!index_has_element(&(alpha[i-1]),j) && theta[i-1][j-1] != 0) {
	printf("Nonzero theta of i = %d for ineligible school j = %d.\n",i,j);
	printf("The working_cee is \n");
	print_process_cee(working_cee);
	return 0;
      }
      if (feasible_guide->allocations[i-1][j-1] < 0.000001 && theta[i-1][j-1] < 0) {
	printf("Negative theta of zero allocation.\n");
	return 0;
      }
      if (feasible_guide->allocations[i-1][j-1] < 0.000001 && favorites[i-1] == j &&
	  theta[i-1][j-1] < 1) {
	printf("Nonpositive theta of zero allocation of favorite.\n");
	return 0;
      }
      if (feasible_guide->allocations[i-1][j-1] > working_cee->maximums[i-1][j-1] -0.000001 && 
	  theta[i-1][j-1] > 0) {
	printf("Positive theta in spite of max allocation.\n");
	return 0;
      }
    }
  }

  for (i = 1; i <= nst; i++) {
    int sum = 0;
    for (j = 1; j <= nsc; j++) {
      sum += theta[i-1][j-1];
    }
    if (sum != 0) {
      printf("The sum of theta for agent %d is not zero.\n",i);
    }
  }

  for (j = 1; j <= nsc; j++) {
    int total_theta = 0;
    for (i = 1; i <= nst; i++) {
      total_theta += theta[i-1][j-1];
    }
      
    if (fully_allocated[j-1] && total_theta > 0) {
      printf("Positive theta_sum for fully allocated school %d.\n", j);
      return 0;
    }
  }

  return 1;
}
*/
