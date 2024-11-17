#include "mcccode.h"

struct partial_alloc MCC_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse) {
  int j;
  double excess_sum;
  
  int nsc = myscp->no_schools;

  double* fine_cutoffs = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    fine_cutoffs[j-1] = 0.0;
  }
  
  excess_sum = 1.0;
  while (excess_sum > 0.000001) {
    
    struct partial_alloc demands;
    demands = compute_demands(myscp, fine_cutoffs);
    
    double* excesses = excess_demands(myscp, &demands);
    
    excess_sum = 0.0;
    for (j = 1; j <= nsc; j++) {
      excess_sum += max(excesses[j-1], 0.0);
    }
      
    for (j = 1; j <= nsc; j++) {
      if (excesses[j-1] > myscp->quotas[j-1] - 0.000001) {
	fine_cutoffs[j-1] = naive_eq_cutoff(myscp, j, &demands, fine_cutoffs[j-1]);
      }
    }

    destroy_partial_alloc(demands);
    free(excesses);
  }

  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = floor(fine_cutoffs[j-1]);
  }

  struct partial_alloc answer = compute_demands(myscp, fine_cutoffs);

  free(fine_cutoffs);

  return answer;
}

double naive_eq_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
		       double old_cutoff) {
  int i;
  double lower_cand, upper_cand, new_cand, lower_dmd, upper_dmd, new_dmd, target;
  
  int nst = myscp->no_students;

  int max_priority = 0;
  for (i = 1; i <= nst; i++) {
    max_priority = max(max_priority, myscp->priorities[i-1][j-1]) ;
  }
  max_priority++;

  target = (double)myscp->quotas[j-1];
  if (demand_at_new_cutoff(myscp, j, demands, old_cutoff) <= target + 0.000000001) {
    return old_cutoff;
  }

  lower_cand = old_cutoff;
  lower_dmd = demand_at_new_cutoff(myscp, j, demands, lower_cand);
  upper_cand = (double)max_priority;
  upper_dmd = demand_at_new_cutoff(myscp, j, demands, upper_cand);
  
  new_dmd = -1.0;
  while (fabs(new_dmd - target) > 0.000000001) {

    new_cand = lower_cand +
               (upper_cand - lower_cand) * (lower_dmd - target)/(lower_dmd - upper_dmd);
    new_dmd = demand_at_new_cutoff(myscp, j, demands, new_cand);
    if (new_dmd > target) {
      lower_cand = new_cand;
      lower_dmd = new_dmd;
    }
    if (new_dmd < target) {
      upper_cand = new_cand;
      upper_dmd = new_dmd;
    }
  }

  return new_cand;
}

double demand_at_new_cutoff(struct process_scp* myscp, int j, struct partial_alloc* demands,
			    double new_cutoff) {
  int i;
  double total_demand;
  
  int nst = myscp->no_students;

  int New_Cutoff = floor(new_cutoff);

  total_demand = 0.0;
  for (i = 1; i <= nst; i++) {
    if (New_Cutoff < myscp->priorities[i-1][j-1]) {
      total_demand += demands->allocations[i-1][j-1];
    }
    else {
      if (New_Cutoff == myscp->priorities[i-1][j-1]) {
	total_demand += min(demands->allocations[i-1][j-1],
			    1.0 - (new_cutoff - (double)(New_Cutoff)));
      }
    }
  }

  return total_demand;
}


double* excess_demands(struct process_scp* myscp, struct partial_alloc* demands) {
  int i, j;
  double total_demand;
  
  int nst = myscp->no_students;
  int nsc = myscp->no_schools;

  double* answer = malloc(nsc * sizeof(double));
  
  for (j = 1; j <= nsc; j++) {
    total_demand = 0.0;
    for (i = 1; i <= nst; i++) {
      total_demand += demands->allocations[i-1][j-1];
    }
    answer[j-1] = total_demand - (double)myscp->quotas[j-1];
  }

  return answer;
}

struct partial_alloc compute_demands(struct process_scp* myscp, double* fine_cutoffs) {
  int i, j, k;
  
  int nst = myscp->no_students;
  int nsc = myscp->no_schools;

  int* coarse_cutoffs = malloc (nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse_cutoffs[j-1] = floor(fine_cutoffs[j-1]);
  }

  struct partial_alloc answer = zero_alloc_for_process_scp(myscp);
  for (i = 1; i <= nst; i++) {
    double unfilled_demand = 1.0;
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      if (unfilled_demand > 0.000001) {
	j = myscp->preferences[i-1][k-1];
	if (myscp->priorities[i-1][j-1] > coarse_cutoffs[j-1]) {
	  answer.allocations[i-1][j-1] = unfilled_demand;
	}
	else if (myscp->priorities[i-1][j-1] == coarse_cutoffs[j-1]) {
	  answer.allocations[i-1][j-1] = min(unfilled_demand,
				       1.0 - (fine_cutoffs[j-1] - (double)coarse_cutoffs[j-1]));  
	}
	unfilled_demand = unfilled_demand - answer.allocations[i-1][j-1];
      }
    }
  }

  free(coarse_cutoffs);

  return answer;
}