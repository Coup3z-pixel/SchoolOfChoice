#include "lpgcpscode.h"

/* Our first function provides a simple answer to the top level. */

struct partial_alloc simple_LPGCPS_alloc(struct process_scp* input) {

  struct partial_alloc answer;

  answer = LPGCPS_allocation(input); 

  /* The following routine can be used during code reorganization to check correctness. */

  /*
  char filename[20] = "allocate.mat";
  struct partial_alloc input_alloc = allocation_from_file(filename);

  if (!partial_allocs_are_same(&answer, &input_alloc)) {
    printf("We seem to have screwed things up.\n");
  }
  else {
    printf("It seems that everything is OK.\n");
  }
  destroy_partial_alloc(input_alloc);
  */

  return answer;
}

struct partial_alloc LPGCPS_allocation(struct process_scp* input) {
  struct partial_alloc feasible_guide;
  struct partial_alloc final_alloc;
  
  feasible_guide = zero_alloc_for_process_scp(input);
  push_relabel(input, &feasible_guide);

  final_alloc = LPGCPS_allocation_with_guide(input);

  destroy_partial_alloc(feasible_guide);

  return final_alloc;
}

struct partial_alloc LPGCPS_allocation_with_guide(struct process_scp* input) {
  int nst, nsc;

  struct process_scp working_scp;
  struct partial_alloc final_alloc;
  struct mixed_tableau tab;
  struct subset P_subset, J_subset;
  double delta;
  int* favorites;
  int* critical_pair_found;
  
  final_alloc = zero_alloc_for_process_scp(input);
  
  copy_process_scp(input, &working_scp);

  delta = 1.0;
  while(delta > 0.000001 && working_scp.time_remaining > 0.000001) {
    tab = linear_program_from_scp(&working_scp);
    delta = solve_mixed_tableau(&tab);
    destroy_mixed_tableau(tab);
    
    if (delta > 0.000001) {
      favorites = get_favorites(&working_scp);      
      augment_partial_alloc(&final_alloc, favorites, delta);
      decrement_working_scp(&working_scp, favorites, delta);
      free(favorites);
    }
  }

  if (working_scp.time_remaining > 0.000001) {
    nst = input->no_students;
    nsc = input->no_schools;
    P_subset = nullset(nsc);
    J_subset = nullset(nst);
  
    critical_pair_found = malloc(sizeof(int));
    *critical_pair_found = 0;

    find_critical_pair(&working_scp, &P_subset, &J_subset, critical_pair_found);

    if (J_subset.subset_size > 0) {
      LPdescend_to_left_subproblem(&working_scp, &final_alloc, &J_subset, &P_subset);
    }

    if (J_subset.subset_size < J_subset.large_set_size) {
      LPdescend_to_right_subproblem(&working_scp, &final_alloc,
				    &P_subset, &J_subset);
    }
  
    free(critical_pair_found);  
    destroy_subset(P_subset);
    destroy_subset(J_subset);
  }

  destroy_process_scp(working_scp);

  return final_alloc;
}

void LPdescend_to_left_subproblem(struct process_scp* working_scp,
				  struct partial_alloc* final_alloc,
				  struct subset* J_subset, struct subset* P_subset) {  
  struct process_scp left_scp;
  struct partial_alloc left_increment;
  struct index J_index, P_index;

  int nsc = working_scp->no_schools;
  
  left_scp = left_sub_process_scp(working_scp, J_subset, P_subset);
    
  left_increment = LPGCPS_allocation_with_guide(&left_scp);
    
  J_index = index_of_subset(J_subset);
  P_index = index_of_fullset(nsc);
  increment_partial_alloc(final_alloc, &left_increment, &J_index, &P_index);

  destroy_process_scp(left_scp);
  destroy_partial_alloc(left_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

void LPdescend_to_right_subproblem(struct process_scp* working_scp,
				   struct partial_alloc* final_alloc,
				   struct subset* P_subset, struct subset* J_subset) {
  struct process_scp right_scp;
  struct partial_alloc right_increment;
  struct index J_index, P_index;

  right_scp = right_sub_process_scp(working_scp, J_subset, P_subset);

  right_increment = LPGCPS_allocation_with_guide(&right_scp);
    
  J_index = index_of_complement(J_subset);    
  P_index = index_of_complement(P_subset);
  increment_partial_alloc(final_alloc, &right_increment, &J_index, &P_index);
    
  destroy_process_scp(right_scp);
  destroy_partial_alloc(right_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

void find_critical_pair(struct process_scp* working_scp, 
			struct subset* P_subset, struct subset* J_subset,
			int* critical_pair_found) {
  int nst, nsc;

  struct index* alpha;
  struct subset active_schools;
  struct index active_school_index;
  struct index* omega;
  int* fully_allocated;
  int* favorites;
  /* int** theta; */
  struct int_sparse_matrix* theta;
  int* theta_sums;
  struct partial_alloc my_feasible_guide;
  
  my_feasible_guide = zero_alloc_for_process_scp(working_scp);
  push_relabel(working_scp, &my_feasible_guide);

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  /* For each student i, alpha[i-1] is the set of schools that are possible for i.
     The active_schools are those that are possible for some student.  For each active
     school j, omega[j-1] is the set of students who might receive more j. */

  alpha = malloc(nst * sizeof(struct index));
  get_alpha(working_scp, alpha);
  active_schools = union_of_indices(alpha, nsc, nst);
  active_school_index = index_of_subset(&active_schools);  
  omega = reverse_of_correspondence_graph(alpha, &active_school_index, nst);

  /* fully_allocated[j-1] == 1 if j is fully allocated in feasible_guide, 0 otherwise. */
  
  fully_allocated = compute_fully_allocated(working_scp,&my_feasible_guide);
  favorites = get_favorites(working_scp);
  /* theta = initialize_theta(&my_feasible_guide, favorites); */
  theta = initialize_theta(working_scp, &my_feasible_guide, favorites);
  /* theta_sums = initialize_theta_sums(theta,nst,nsc);	*/
  theta_sums = initialize_theta_sums(theta,nst,nsc); 

  int* no_new_pivots = malloc(sizeof(int));
  int* h_sum = malloc(sizeof(int));
  *no_new_pivots = 0;
  *h_sum = 0;
  struct pivot_list fake_list;
  fake_list = void_pivot_list();
    
  massage_theta_or_find_critical_pair(theta, theta_sums, P_subset, J_subset, working_scp,
				      &my_feasible_guide, alpha, &active_school_index, omega,
				      favorites, fully_allocated, &fake_list,
				      critical_pair_found,
				      no_new_pivots, h_sum);

  free(no_new_pivots);
  free(h_sum);
  destroy_pivot_list(fake_list);
  destroy_subset(active_schools);  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_index(active_school_index);  
  free(fully_allocated);  
  /* destroy_theta(theta, nst); */
  destroy_int_sp_mat(theta);
  free(theta);
  free(theta_sums);
  destroy_alpha_or_omega(alpha, nst);
  free(favorites);
  destroy_partial_alloc(my_feasible_guide);
}

struct mixed_tableau linear_program_from_scp(struct process_scp* working_scp) {
  int nst, nsc, no_variables, i, j, k, l;
  int* no_elig;
  int* prior_sum;
  
  struct mixed_tableau answer;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  answer.no_eq = nst;
  answer.no_ineq = nsc + nst;

  no_variables = 1;

  no_elig = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    no_elig[i-1] = working_scp->no_eligible_schools[i-1];
  }

  for (i = 1; i <= nst; i++) {
    no_variables += no_elig[i-1];
  }
  answer.no_var = no_variables;

  answer.obj_fnc = malloc((no_variables + 1) * sizeof(double));
  answer.obj_fnc[0] = 1.0;
  for (k = 1; k <= no_variables; k++) {
    answer.obj_fnc[k] = 0.0;    
  }

  prior_sum = malloc(nst * sizeof(int));
  prior_sum[0] = 0;
  for (i = 2; i <= nst; i++) {
    prior_sum[i-1] = prior_sum[i-2] + no_elig[i-2];
  }

  answer.eq_coeff = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    answer.eq_coeff[i-1] = malloc((no_variables + 1) * sizeof(double));
    for (k = 1; k <= no_variables; k++) {
      answer.eq_coeff[i-1][k-1] = 0.0;
    }
    for (k = 1; k <= no_elig[i-1]; k++) {
      answer.eq_coeff[i-1][prior_sum[i-1] + k] = 1.0;
    }
    answer.eq_coeff[i-1][no_variables] = working_scp->time_remaining;
  }

  answer.ineq_coeff = malloc((nsc + nst) * sizeof(double*));

  for (k = 1; k <= nsc + nst; k++) {
    answer.ineq_coeff[k-1] = malloc((no_variables + 1) * sizeof(double));
    for (l = 1; l <= no_variables + 1; l++) {
      answer.ineq_coeff[k-1][l-1] = 0.0;
    }
  }

  for (j = 1; j <= nsc; j++) {
    for (i = 1; i <= nst; i++) {
      for (k = 1; k <= no_elig[i-1]; k++) {
	if (working_scp->preferences[i-1][k-1] == j) {
	    answer.ineq_coeff[j - 1][prior_sum[i-1] + k] = 1.0;
	}
      }
    }
    answer.ineq_coeff[j-1][no_variables] = working_scp->quotas[j-1];
  }

  for (i = 1; i <= nst; i++) {
    answer.ineq_coeff[nsc + i - 1][0] = 1.0;
    answer.ineq_coeff[nsc + i - 1][prior_sum[i-1] + 1] = -1.0;
  }

  free(no_elig);
  free(prior_sum);

  return answer;
}
