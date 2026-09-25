#include "endpoint.h"

double NEW_time_until_some_requirement_filled(double* eating_speeds, process_scp* working_scp) {
  int i, nst;

  double min_time, req, eatsp, temp_min;
   
  nst = working_scp->no_students;
  min_time = working_scp->time_remaining;

  for (i = 1; i <= nst; i++) {
    req = working_scp->requirements[i-1];
    eatsp = eating_speeds[i-1];
    if (req > 0.00001 && eatsp > 0.00001) {
      temp_min = req/eatsp;
      min_time = min(min_time, temp_min);
    }
  }

  return min_time;
}

/*
double time_until_some_school_exhausted(int* favorites, process_scp* working_scp) {
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
*/

double NEW_time_until_some_school_exhausted(int* favorites, double* eating_speeds,
					    process_scp* working_scp) {
  int i, j;

  double little_min, minimum;
  
  int nst = working_scp->no_students;
  int nsc = working_scp->no_schools;

  double* student_flow = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    student_flow[j-1] = 0.0;
  }

  for (i = 1; i <= nst; i++) {
    student_flow[favorites[i-1]-1] += eating_speeds[i-1];
  }
  
  minimum = working_scp->time_remaining;
  for (j = 1; j <= nsc; j++) {
    if (student_flow[j-1] > 0.0) {
      little_min = working_scp->quotas[j-1]/student_flow[j-1];
      minimum = min(minimum,little_min);
    }
  }

  free(student_flow);
  
  return minimum;
}

/*
double time_until_feasible_guide_not_above_alloc(int_sparse_matrix* theta,
						     int* favorites,
						     process_scp* working_scp,
						     partial_alloc* feasible_guide) {
  int i, j, k, slope;
  
  int nst = working_scp->no_students;

  double lower_bound_min,temp_min;

  lower_bound_min = working_scp->time_remaining;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= working_scp->no_eligible_schools[i-1]; k++) {
      j = working_scp->preferences[i-1][k-1];;
      slope = int_entry(theta, i, j);
      if (favorites[i-1] == j) {
	slope--;
      }
      if (slope < 0) {
	temp_min = - get_entry(feasible_guide, i, j)/slope;
	lower_bound_min = min(lower_bound_min, temp_min);
      }
    }
  }

  return lower_bound_min;
}
*/

double NEW_time_until_feasible_guide_not_above_alloc(int_sparse_matrix* theta,
						     int* favorites,
						     double* eating_speeds,
						     process_scp* working_scp,
						     partial_alloc* feasible_guide) {
  int i, j, k, nst;

  double min_time, theta_slope, favorites_slope, slope, temp_min;
  
  nst = working_scp->no_students;
  min_time = working_scp->time_remaining;
  
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= working_scp->no_eligible_schools[i-1]; k++) {
      j = working_scp->preferences[i-1][k-1];
      
      if (favorites[i-1] == j) {
	favorites_slope = eating_speeds[i-1];
      }
      else {
	favorites_slope = 0.0;
      }
      theta_slope = (double)int_entry(theta, i, j);
      slope = favorites_slope - theta_slope;
      
      if (slope > 0) {
	temp_min = get_entry(feasible_guide, i, j)/slope;
	min_time = min(min_time, temp_min);
      }
    }
  }

  return min_time;;
}

double time_until_feasible_guide_not_feasible(int_sparse_matrix* theta,
					      process_scp* working_scp,
					      partial_alloc* feasible_guide) {
  int i, j, k, slope;
  
  int nst = working_scp->no_students;
  int nsc = working_scp->no_schools;

  double individual_min, school_quota_min, temp_min, final_min;

  individual_min = working_scp->time_remaining;
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= working_scp->no_eligible_schools[i-1]; k++) {
      j = working_scp->preferences[i-1][k-1];
      slope = int_entry(theta, i, j);
      if (slope < 0) {	
	temp_min = - get_entry(feasible_guide, i, j)/slope;
      }
      if (slope > 0) {
	temp_min = (is_eligible(working_scp, i, j) * working_scp->time_remaining)/slope;
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
    for (k = 1; k <= working_scp->no_eligible_schools[i-1]; k++) {
      j = working_scp->preferences[i-1][k-1];
      unalloc_quota[j-1] -= get_entry(feasible_guide, i, j);
      slopes[j-1] += int_entry(theta, i, j);
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

/*
double time_until_trajectory_change(int_sparse_matrix* theta, 
					int* favorites, process_scp* working_scp,
					partial_alloc* feasible_guide) {
  double final_min, time_til_some_school_exhaustion, time_til_feasible_guide_not_above,
    time_til_feasible_guide_not_feasible;

  final_min = working_scp->time_remaining;

  time_til_some_school_exhaustion = time_until_some_school_exhausted(favorites, working_scp);
  final_min = min(final_min,time_til_some_school_exhaustion);

  time_til_feasible_guide_not_above = time_until_feasible_guide_not_above_alloc(theta, 
										favorites,
										working_scp,
										feasible_guide);
  final_min = min(final_min,time_til_feasible_guide_not_above);

  time_til_feasible_guide_not_feasible = time_until_feasible_guide_not_feasible(theta, 
										working_scp,
										feasible_guide);
  
  final_min = min(final_min,time_til_feasible_guide_not_feasible);

  return final_min;
}
*/

double NEW_time_until_trajectory_change(int_sparse_matrix* theta, int* favorites,
					double* eating_speeds, process_scp* working_scp,
					partial_alloc* feasible_guide) {
  double wstr, tusrf, tusse,tufgnaa, tufgnf;

  wstr = working_scp->time_remaining;

  tusrf = NEW_time_until_some_requirement_filled(eating_speeds, working_scp);

  tusse = NEW_time_until_some_school_exhausted(favorites, eating_speeds, working_scp);

  tufgnaa = NEW_time_until_feasible_guide_not_above_alloc(theta, favorites, eating_speeds,
							  working_scp, feasible_guide);

  tufgnf = time_until_feasible_guide_not_feasible(theta, working_scp, feasible_guide);
  
  return min(min(min(min(wstr, tusrf), tusse), tufgnaa), tufgnf);
}

/*
void augment_partial_alloc(partial_alloc* my_alloc, int* favorites, double delta) {
  int i;

  int nst = my_alloc->no_students;

  for (i = 1; i <= nst; i++) {
    increment_entry(my_alloc, i, favorites[i-1], delta);
  }
}
*/

void NEW_augment_partial_alloc(partial_alloc* my_alloc, int* favorites, double* eating_speeds,
			       double delta) {
  int i;

  int nst = my_alloc->no_students;

  for (i = 1; i <= nst; i++) {
    increment_entry(my_alloc, i, favorites[i-1], eating_speeds[i-1] * delta);
  }
}

/*
void adjust_feasible_guide(partial_alloc* feasible_guide, process_scp* myscp,
			   int_sparse_matrix* theta,
			   int* favorites, double delta) {
  int i, j, nst, nsc;

  nst = feasible_guide->no_students;
  nsc = feasible_guide->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (is_eligible(myscp, i, j)) {
	increment_entry(feasible_guide, i, j, int_entry(theta, i, j) * delta);
      }
    }
  }
  
  for (i = 1; i <= nst; i++) { 
    increment_entry(feasible_guide, i, favorites[i-1], -delta);
  }
}
*/

void NEW_adjust_feasible_guide(partial_alloc* feasible_guide, process_scp* myscp,
			       int_sparse_matrix* theta,
			       int* favorites, double* eating_speeds, double delta) {
  int i, j, nst, nsc;

  nst = feasible_guide->no_students;
  nsc = feasible_guide->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (is_eligible(myscp, i, j)) {
	increment_entry(feasible_guide, i, j, int_entry(theta, i, j) * delta);
      }
    }
  }
  
  for (i = 1; i <= nst; i++) { 
    increment_entry(feasible_guide, i, favorites[i-1], -delta * eating_speeds[i-1]);
  }
}

/*
void decrement_working_scp(process_scp* working_scp, int* favorites, double delta) {
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
*/

void NEW_decrement_working_scp(process_scp* working_scp, int* favorites, double* eating_speeds,
			       double delta) {
  int i, j, k, nst, nsc, no_elig, cursor;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  for (i = 1; i <= nst; i++ ) {
    working_scp->requirements[i-1] -= delta * eating_speeds[i-1];
    working_scp->quotas[favorites[i-1]-1] -= delta * eating_speeds[i-1];
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

/*
void move_to_endpoint_of_segment(int_sparse_matrix* theta, 
				 int* favorites, 
				 process_scp* working_scp,
				 partial_alloc* feasible_guide,
				 partial_alloc* final_alloc) {
  double delta;
  
  delta = time_until_trajectory_change(theta, favorites, working_scp,
						  feasible_guide);
  
  augment_partial_alloc(final_alloc, favorites, delta);
  adjust_feasible_guide(feasible_guide, working_scp, theta, favorites, delta);
  decrement_working_scp(working_scp, favorites, delta);
}
*/

void NEW_move_to_endpoint_of_segment(int_sparse_matrix* theta, 
				     int* favorites, double* eating_speeds,
				     process_scp* working_scp,
				     partial_alloc* feasible_guide,
				     partial_alloc* final_alloc) {
  double delta;
  
  delta = NEW_time_until_trajectory_change(theta, favorites, eating_speeds, working_scp,
						  feasible_guide);
  
  NEW_augment_partial_alloc(final_alloc, favorites, eating_speeds, delta);
  NEW_adjust_feasible_guide(feasible_guide, working_scp, theta, favorites, eating_speeds, delta);
  NEW_decrement_working_scp(working_scp, favorites, eating_speeds, delta);
}

