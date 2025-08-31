#include "fdamcc.h"

/****************************** mcca code ****************************/

struct partial_alloc mcca_alloc_plus_coarse_cutoffs_FDA(struct process_scp* myscp, int* coarse) {
  int j, nsc;
  double excess_sum;
  double* cutoffs;

  struct partial_alloc answer;

  nsc = myscp->no_schools;

  cutoffs = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    cutoffs[j-1] = 0.0;
  }
  
  excess_sum = 1.0;
  while (excess_sum > 0.00000001) {
    naive_increase_of_cutoffs(myscp, cutoffs);
    excess_sum = sum_of_excesses(myscp, cutoffs);
  }

  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = floor(cutoffs[j-1]);
  }
  
  answer = compute_demands(myscp, cutoffs);

  free(cutoffs);

  return answer;
}

void naive_increase_of_cutoffs(struct process_scp* myscp, double* cutoffs) {
  int j, nsc;

  double* excesses;
  struct partial_alloc demands;

  nsc = myscp->no_schools;
  demands = compute_demands(myscp, cutoffs);
  
  excesses = excess_demands(myscp, &demands);

  for (j = 1; j <= nsc; j++) {
    if (excesses[j-1] > 0.00000000001) {
      cutoffs[j-1] = naive_eq_cutoff(myscp, &demands, j);
    }
  }

  free(excesses);
  destroy_partial_alloc(demands);
}

struct partial_alloc mcca_alloc_FDA(struct process_scp* myscp) {
  int nsc;
  int* coarse;
  struct partial_alloc answer;

  nsc = myscp->no_schools;
  coarse = malloc(nsc * sizeof(int));
  answer = mcca_alloc_plus_coarse_cutoffs_FDA(myscp, coarse);

  free(coarse);

  return answer;
}

/****************************** mccb code ****************************/

struct partial_alloc mccb_alloc_plus_coarse_cutoffs_FDA(struct process_scp* myscp, int* coarse) {
  int j, nsc;
  double deficit_sum, max_cutoff;
  double* cutoffs;

  struct partial_alloc answer;

  nsc = myscp->no_schools;

  max_cutoff = 1.0 + (double)maximum_priority(myscp);

  cutoffs = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    cutoffs[j-1] = max_cutoff;
  }

  deficit_sum = 1.0;
  while (deficit_sum > 0.00000001) {
    naive_reduction_of_cutoffs(myscp, cutoffs);
    deficit_sum = sum_of_deficits(myscp, cutoffs);
  }

  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = floor(cutoffs[j-1]);
  }

  answer = compute_demands(myscp, cutoffs);

  if (!cutoffs_are_an_mcc(myscp,cutoffs)) {
    fprintf(stderr, "We are exiting mccb_alloc_plus_coarse_cutoffs_FDA with a nonmcc!\n");
    exit(0);
  }

  free(cutoffs);

  return answer;
}

void naive_reduction_of_cutoffs(struct process_scp* myscp, double* cutoffs) {
  int j, nsc;

  double* deficits;

  struct partial_alloc demands;

  nsc = myscp->no_schools;
  
  demands = compute_demands(myscp, cutoffs);
    
  deficits = demand_deficits(myscp, &demands);

  for (j = 1; j <= nsc; j++) {
    if (cutoffs[j-1] > 0.000001 && deficits[j-1] > 0.00000000001) {
      cutoffs[j-1] = naive_eq_cutoff(myscp, &demands, j);
    }
  }

  destroy_partial_alloc(demands);
  free(deficits);
}

struct partial_alloc mccb_alloc_FDA(struct process_scp* myscp) {
  int nsc;
  int* coarse;
  struct partial_alloc answer;

  nsc = myscp->no_schools;
  coarse = malloc(nsc * sizeof(int));
  answer = mccb_alloc_plus_coarse_cutoffs_FDA(myscp, coarse);
  
  free(coarse);

  return answer;
}

/************************** general functions and utilties ***************************/

double demand_at_new_cutoff(struct process_scp* myscp, struct partial_alloc* demands, int j,
				double new_cutoff) {
  int i, k, l, nst, hit, New_Cutoff;
  double answer, total_preferred;
  
  nst = myscp->no_students;

  New_Cutoff = (int)floor(new_cutoff);

  answer = 0.0;
  for (i = 1; i <= nst; i++) {
    hit = 0;
    total_preferred = 0;
    for (k = 1; k <= myscp->no_eligible_schools[i-1] && !hit; k++) {
      l = myscp->preferences[i-1][k-1];
      if (l == j) {
	hit = 1;
      }
      if (hit == 0) {
	total_preferred += get_entry(demands, i, l);
      }
    }
    if (New_Cutoff < get_priority(myscp, i, j)) {
      answer += 1.0 - total_preferred;
    }
    if (New_Cutoff == get_priority(myscp, i, j)) {
	answer += min(1.0 - total_preferred, 1.0 - (new_cutoff - (double)(New_Cutoff)));
    }
  }

  return answer;
}

double naive_eq_cutoff(struct process_scp* myscp, struct partial_alloc* demands, int j) {
  int max_priority;
  double lower_cand, upper_cand, new_cand, lower_dmd, upper_dmd, new_dmd, midpoint, midpoint_dmd, quota, slope;
  
  max_priority = maximum_priority(myscp);

  quota = myscp->quotas[j-1];
  
  if (demand_at_new_cutoff(myscp, demands, j, 0.0) < quota - 0.000001) {
    return 0.0;
  }

  lower_cand = 0.0;
  lower_dmd = demand_at_new_cutoff(myscp, demands, j, lower_cand);
  upper_cand = (double)max_priority;
  upper_dmd = demand_at_new_cutoff(myscp, demands, j, upper_cand);

  new_dmd = -1.0;
  while (fabs(new_dmd - quota) > 0.00000000001) {

    slope = (upper_dmd - lower_dmd)/(upper_cand - lower_cand);

    new_cand = lower_cand + (quota - lower_dmd)/slope;
    new_dmd = demand_at_new_cutoff(myscp, demands, j, new_cand);

    if (fabs(new_dmd - quota) > 0.00000000001) {
      midpoint = (upper_cand + lower_cand)/2;
      midpoint_dmd = demand_at_new_cutoff(myscp, demands, j, midpoint);
      if (midpoint_dmd < quota) {
	upper_cand = midpoint;
	upper_dmd = midpoint_dmd;
      }
      else {
	lower_cand = midpoint;
	lower_dmd = midpoint_dmd;
      }
    }
  }

  return new_cand;
}

double* naive_eq_cutoffs(struct process_scp* myscp, struct partial_alloc* demands) {
  int j, nsc;
  double* answer;

  nsc = myscp->no_schools;

  answer = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer[j-1] = naive_eq_cutoff(myscp, demands, j);
  }

  return answer;
}

int cutoffs_are_an_mcc(struct process_scp* myscp, double* cutoffs) {
  int i, j, nst, nsc;

  double sum;

  struct partial_alloc demands;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  demands = compute_demands(myscp, cutoffs);

  for (i = 1; i <= nst; i++) {
    sum = get_total_demand_for_student(myscp, &demands, i);
    if (fabs(sum - 1.0) > 0.000001) {

      fprintf(stderr, "Student %i is not exactly allocated.\n", i);
      
      destroy_partial_alloc(demands);
      return 0;
    }
  }

  for (j = 1; j <= nsc; j++) {
    sum = get_total_demand_for_school(&demands, j);
    
    if (sum - myscp->quotas[j-1] > 0.000001 ||
	(cutoffs[j-1] > 0.000001 && fabs(sum - (double)myscp->quotas[j-1]) > 0.000001)) {
      destroy_partial_alloc(demands);
      return 0;
    }
  }

  destroy_partial_alloc(demands);
  return 1;
}

void print_cutoffs(int nsc, double* cutoffs) {
  int j;
  
  fprintf(stderr, "(");
  for (j = 1; j < nsc; j++) {
    fprintf(stderr, "%1.2f, ", cutoffs[j-1]);
  }
  fprintf(stderr, "%1.2f)", cutoffs[nsc-1]);
}
