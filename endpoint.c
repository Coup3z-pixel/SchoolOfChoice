#include "endpoint.h"

double time_until_some_max_exhaustion(int* favorites, struct process_cee* working_cee) {
  int i;
  
  int nst = working_cee->no_students;
  
  double mnm = working_cee->time_remaining;;
  for (i = 1; i <= nst; i++) {
    mnm = min(mnm,working_cee->maximums[i-1][favorites[i-1]-1]);
  }

  return mnm;
}

double time_until_some_school_exhaustion(int* favorites, struct process_cee* working_cee) {
  int i, j;

  double little_min, minimum;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  int* student_count = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    student_count[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    student_count[favorites[i-1]-1]++;
  }
  
  minimum = working_cee->time_remaining;
  for (j = 1; j <= nsc; j++) {
    if (student_count[j-1] > 0) {
      little_min = working_cee->quotas[j-1]/student_count[j-1];
      minimum = min(minimum,little_min);
    }
  }

  free(student_count);
  
  return minimum;
}

double time_until_feasible_guide_not_feasible(int** theta, struct index* alpha,
					      struct process_cee* working_cee,
					      struct partial_alloc* feasible_guide)  {
  int i, j, k, slope;
  
  int nst = working_cee->no_students;
  int nsc = working_cee->no_schools;

  double individual_min, school_quota_min, temp_min, final_min;

  individual_min = working_cee->time_remaining;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      slope = theta[i-1][j-1];
      if (slope < 0) {
	temp_min = - feasible_guide->allocations[i-1][j-1]/slope;
      }
      if (slope > 0) {
	temp_min = working_cee->maximums[i-1][j-1]/slope;
      }
      if (slope != 0) {
	individual_min = min(individual_min, temp_min);
      }
    }
  }

  double* unalloc_quota = malloc(nsc * sizeof(double));
  int* slopes = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    unalloc_quota[j-1] = working_cee->quotas[j-1];
    slopes[j-1] = 0;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= alpha[i-1].no_elements; k++) {
      j = alpha[i-1].indices[k-1];
      unalloc_quota[j-1] -= feasible_guide->allocations[i-1][j-1];
      slopes[j-1] += theta[i-1][j-1];
    }
  }

  school_quota_min = working_cee->time_remaining;
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
						 struct process_cee* working_cee,
						 struct partial_alloc* feasible_guide) {
  int i, j, k, slope;
  
  int nst = working_cee->no_students;

  double lower_bound_min,temp_min;

  lower_bound_min = working_cee->time_remaining;;
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
				    struct process_cee* working_cee,
				    struct partial_alloc* feasible_guide) {
  double final_min, time_til_some_max_exhaustion, time_til_some_school_exhaustion,
    time_til_feasible_guide_not_above, time_til_feasible_guide_not_feasible;
  
  final_min = working_cee->time_remaining;
  
  time_til_some_max_exhaustion = time_until_some_max_exhaustion(favorites, working_cee);
  final_min = min(final_min,time_til_some_max_exhaustion);

  time_til_some_school_exhaustion = time_until_some_school_exhaustion(favorites, working_cee);
  final_min = min(final_min,time_til_some_school_exhaustion);

  time_til_feasible_guide_not_above = time_until_feasible_guide_not_above_alloc(theta, alpha,
										favorites,
										working_cee,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_above);

  time_til_feasible_guide_not_feasible = time_until_feasible_guide_not_feasible(theta, alpha,
										working_cee,
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

void decrement_working_cee(struct process_cee* working_cee, int* favorites, double delta) {
  int i;
  
  int nst = working_cee->no_students;

  for (i = 1; i <= nst; i++ ) {
    working_cee->maximums[i-1][favorites[i-1]-1] -= delta; 
    working_cee->quotas[favorites[i-1]-1] -= delta;
  }

  working_cee->time_remaining -= delta;
}

void move_to_endpoint_of_segment(int** theta, struct index* alpha, int* favorites,
				 struct process_scp* working_scp,
				 struct partial_alloc* feasible_guide,
				 struct partial_alloc* final_alloc) {
  
  double delta = time_until_trajectory_change(theta, alpha, favorites,&(working_scp->cee),
					      feasible_guide);
  augment_partial_alloc(final_alloc, favorites, delta);
  adjust_feasible_guide(feasible_guide, theta, alpha, favorites, delta);
  decrement_working_cee(&(working_scp->cee), favorites, delta);
}
