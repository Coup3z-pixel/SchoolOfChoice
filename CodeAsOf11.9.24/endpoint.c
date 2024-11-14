#include "endpoint.h"

double time_until_some_max_exhaustion(int* favorites, struct process_scp* working_scp) {
  int i;
  
  int nst = working_scp->no_students;
  
  double mnm = working_scp->time_remaining;;
  for (i = 1; i <= nst; i++) {
    mnm = min(mnm,working_scp->eligible[i-1][favorites[i-1]-1] * working_scp->time_remaining);
  }

  return mnm;
}

double time_until_some_school_exhaustion(int* favorites, struct process_scp* working_scp) {
  int i, j;

  double little_min, minimum;
  
  int nst = working_scp->no_students;
  int nsc = working_scp->no_schools;

  int* student_count = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    student_count[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    student_count[favorites[i-1]-1]++;
  }
  
  minimum = working_scp->time_remaining;
  for (j = 1; j <= nsc; j++) {
    if (student_count[j-1] > 0) {
      little_min = working_scp->quotas[j-1]/student_count[j-1];
      minimum = min(minimum,little_min);
    }
  }

  free(student_count);
  
  return minimum;
}

double time_until_feasible_guide_not_feasible(int** theta, struct index* alpha,
					      struct process_scp* working_scp,
					      struct partial_alloc* feasible_guide)  {
  int i, j, k, slope;
  
  int nst = working_scp->no_students;
  int nsc = working_scp->no_schools;

  double individual_min, school_quota_min, temp_min, final_min;

  individual_min = working_scp->time_remaining;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      slope = theta[i-1][j-1];
      if (slope < 0) {
	temp_min = - feasible_guide->allocations[i-1][j-1]/slope;
      }
      if (slope > 0) {
	temp_min = (working_scp->eligible[i-1][j-1] * working_scp->time_remaining)/slope;
      }
      if (slope != 0) {
	individual_min = min(individual_min, temp_min);
      }
    }
  }

  double* unalloc_quota = malloc(nsc * sizeof(double));
  int* slopes = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    unalloc_quota[j-1] = working_scp->quotas[j-1];
    slopes[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      unalloc_quota[j-1] -= feasible_guide->allocations[i-1][j-1];
      slopes[j-1] += theta[i-1][j-1];
    }
  }

  school_quota_min = working_scp->time_remaining;
  for (j = 1; j <= nsc; j++) {
    if (slopes[j-1] > 0) {
      temp_min = unalloc_quota[j-1]/slopes[j-1];      
      school_quota_min = min(school_quota_min, temp_min);
    }
  }

  free(unalloc_quota);
  free(slopes);
  final_min = min(individual_min, school_quota_min);
  
  return final_min;
}

double time_until_feasible_guide_not_above_alloc(int** theta, struct index* alpha, int* favorites,
						 struct process_scp* working_scp,
						 struct partial_alloc* feasible_guide) {
  int i, j, k, slope;
  
  int nst = working_scp->no_students;

  double lower_bound_min,temp_min;

  lower_bound_min = working_scp->time_remaining;;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      slope = theta[i-1][j-1];
      if (favorites[i-1] == j) {
	slope--;
      }
      if (slope < 0) {
	temp_min = - feasible_guide->allocations[i-1][j-1]/slope;
	lower_bound_min = min(lower_bound_min, temp_min);
      }
    }
  }

  return lower_bound_min;
}

double time_until_trajectory_change(int** theta, struct index* alpha, int* favorites,
				    struct process_scp* working_scp,
				    struct partial_alloc* feasible_guide) {
  double final_min, time_til_some_max_exhaustion, time_til_some_school_exhaustion,
    time_til_feasible_guide_not_above, time_til_feasible_guide_not_feasible;
  
  final_min = working_scp->time_remaining;
  
  time_til_some_max_exhaustion = time_until_some_max_exhaustion(favorites, working_scp);
  final_min = min(final_min,time_til_some_max_exhaustion);

  time_til_some_school_exhaustion = time_until_some_school_exhaustion(favorites, working_scp);
  final_min = min(final_min,time_til_some_school_exhaustion);

  time_til_feasible_guide_not_above = time_until_feasible_guide_not_above_alloc(theta, alpha,
										favorites,
										working_scp,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_above);

  time_til_feasible_guide_not_feasible = time_until_feasible_guide_not_feasible(theta, alpha,
										working_scp,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_feasible);

  return final_min;
}

void augment_partial_alloc(struct partial_alloc* my_alloc, int* favorites, double delta) {
  int i;

  int nst = my_alloc->no_students;

  for (i = 1; i <= nst; i++) {
    my_alloc->allocations[i-1][favorites[i-1]-1] += delta;
  }
}

void adjust_feasible_guide(struct partial_alloc* feasible_guide, int** theta, struct index* alpha,
			   int* favorites,
			   double delta) {
  int i, j, k;

  int nst = feasible_guide->no_students;

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      feasible_guide->allocations[i-1][j-1] += theta[i-1][j-1] * delta;
    }
  }
  
  for (i = 1; i <= nst; i++) {
    feasible_guide->allocations[i-1][favorites[i-1]-1] -= delta;
  }
}

void decrement_working_scp(struct process_scp* working_scp, int* favorites, double delta) {
  int i, j, k, nst, nsc, no_elig, cursor;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  for (i = 1; i <= nst; i++ ) {
    working_scp->quotas[favorites[i-1]-1] -= delta;
  }

  working_scp->time_remaining -= delta;

  int* full_schools = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    if (working_scp->quotas[j-1] < 0.000001) {
      full_schools[j-1] = 1;
    }
    else {
      full_schools[j-1] = 0;
    }
  }

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (full_schools[j-1] == 1) {
	working_scp->eligible[i-1][j-1] = 0;
      }
    }
  }

  for (i = 1; i <= nst; i++) {
    no_elig = working_scp->no_eligible_schools[i-1];
    for (k = 1; k <= no_elig; k++) {
      if (full_schools[working_scp->preferences[i-1][k-1]-1] == 1) {
	working_scp->no_eligible_schools[i-1]--;
	working_scp->preferences[i-1][k-1] = 0;
      }
    }
    if (working_scp->no_eligible_schools[i-1] < no_elig) {
      int* temp = malloc(no_elig * sizeof(int));
      for (k = 1; k <= no_elig; k++) {
	temp[k-1] = working_scp->preferences[i-1][k-1];
      }
      free(working_scp->preferences[i-1]);
      working_scp->preferences[i-1] = malloc(working_scp->no_eligible_schools[i-1] * sizeof(int));
      cursor = 0;
      for (k = 1; k <= no_elig; k++) {
	if (temp[k-1] != 0) {
	  cursor++;
	  working_scp->preferences[i-1][cursor-1] = temp[k-1];
	}
      }
      free(temp);
    }
  }

  free(full_schools);
}

void move_to_endpoint_of_segment(int** theta, struct index* alpha, int* favorites,
				 struct process_scp* working_scp,
				 struct partial_alloc* feasible_guide,
				 struct partial_alloc* final_alloc) {
  
  double delta = time_until_trajectory_change(theta, alpha, favorites, working_scp,
					      feasible_guide);
  
  augment_partial_alloc(final_alloc, favorites, delta);
  adjust_feasible_guide(feasible_guide, theta, alpha, favorites, delta);
  decrement_working_scp(working_scp, favorites, delta);
}
