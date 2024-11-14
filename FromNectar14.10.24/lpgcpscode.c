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

  final_alloc = LPGCPS_allocation_with_guide(input, &feasible_guide /*, &probe_list */);

  destroy_partial_alloc(feasible_guide);

  return final_alloc;
}

struct partial_alloc LPGCPS_allocation_with_guide(struct process_scp* input,
						  struct partial_alloc* feasible_guide) {
  int nst, nsc;
  
  struct process_scp working_scp;
  struct partial_alloc final_alloc;
  struct tableau tab;
  struct subset P_subset, J_subset;
  double delta;
  int* favorites;
  int* critical_pair_found;
  
  final_alloc = zero_alloc_for_process_scp(input);
  
  copy_process_scp(input, &working_scp);

  tab = linear_program_from_scp(input);

  delta = full_solve(&tab);

  favorites = get_favorites(input);

  augment_partial_alloc(&final_alloc, favorites, delta);
  decrement_working_scp(&working_scp, favorites, delta);

  nst = input->no_students;
  nsc = input->no_schools;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);
  
  critical_pair_found = malloc(sizeof(int));
  *critical_pair_found = 0;

  find_critical_pair(&working_scp, feasible_guide, &P_subset, &J_subset, critical_pair_found);

  if (J_subset.subset_size > 0) {
    LPdescend_to_left_subproblem(&working_scp, &final_alloc, feasible_guide, &J_subset);
  }

  if (J_subset.subset_size < J_subset.large_set_size) {
    LPdescend_to_right_subproblem(&working_scp, &final_alloc, feasible_guide,
				  &P_subset, &J_subset);
  }
  
  free(critical_pair_found);  
  destroy_subset(P_subset);
  destroy_subset(J_subset);
  destroy_process_scp(working_scp);

  return final_alloc;
}

void LPdescend_to_left_subproblem(struct process_scp* working_scp,
				  struct partial_alloc* final_alloc,
				  struct partial_alloc* feasible_guide,
				  struct subset* J_subset) {  
  struct process_scp left_scp;
  struct partial_alloc left_feas_guide;
  struct partial_alloc left_increment;
  struct index J_index, P_index;

  int nsc = working_scp->no_schools;
  
  left_scp = critical_sub_process_scp(working_scp, J_subset);    
  left_feas_guide = left_feasible_guide(feasible_guide, J_subset);
    
  left_increment = LPGCPS_allocation_with_guide(&left_scp, &left_feas_guide);
    
  J_index = index_of_subset(J_subset);
  P_index = index_of_fullset(nsc);
  increment_partial_alloc(final_alloc, &left_increment, &J_index, &P_index);

  destroy_process_scp(left_scp);
  destroy_partial_alloc(left_feas_guide);
  destroy_partial_alloc(left_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

void LPdescend_to_right_subproblem(struct process_scp* working_scp,
				   struct partial_alloc* final_alloc,
				   struct partial_alloc* feasible_guide, 
				   struct subset* P_subset, struct subset* J_subset) {
  struct process_scp right_scp;
  struct partial_alloc right_feas_guide;
  struct partial_alloc right_increment;
  struct index J_index, P_index;

  right_scp = crit_compl_sub_process_scp(working_scp, J_subset, P_subset);
  right_feas_guide = right_feasible_guide(feasible_guide, J_subset, P_subset);

  right_increment = LPGCPS_allocation_with_guide(&right_scp,&right_feas_guide);
    
  J_index = index_of_complement(J_subset);    
  P_index = index_of_complement(P_subset);
  increment_partial_alloc(final_alloc, &right_increment, &J_index, &P_index);
    
  destroy_process_scp(right_scp);
  destroy_partial_alloc(right_feas_guide);  
  destroy_partial_alloc(right_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

void find_critical_pair(struct process_scp* working_scp, struct partial_alloc* feasible_guide,
			struct subset* P_subset, struct subset* J_subset,
			int* critical_pair_found) {
  int nst, nsc;

  struct index* alpha;
  struct subset active_schools;
  struct index active_school_index;
  struct index* omega;
  int* fully_allocated;
  int* favorites;
  int** theta;
  int* theta_sums;

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
  
  fully_allocated = compute_fully_allocated(working_scp,feasible_guide);
  favorites = get_favorites(working_scp);
  theta = initialize_theta(feasible_guide, favorites);
  theta_sums = initialize_theta_sums(theta,nst,nsc);
    
  massage_theta_or_find_critical_pair(theta, theta_sums, P_subset, J_subset, working_scp,
				      feasible_guide, alpha, &active_school_index, omega,
				      favorites, fully_allocated, critical_pair_found);
  
  destroy_subset(active_schools);  
  destroy_alpha_or_omega(omega, active_school_index.no_elements); 
  destroy_index(active_school_index);  
  free(fully_allocated);  
  destroy_theta(theta, nst);
  free(theta_sums);
  destroy_alpha_or_omega(alpha, nst);
  free(favorites);
}

  
void massage_theta_or_find_critical_pair(int** theta, int* theta_sums, struct subset* P_subset,
					 struct subset* J_subset, struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega,int* favorites,
					 int* fully_allocated, int* critical_pair_found) {
  int j, k; 

  k = 1;
  while (!*critical_pair_found && k <= active_schools_index->no_elements) {
    j = active_schools_index->indices[k-1];
    if (fully_allocated[j-1]) {
      while (!*critical_pair_found && theta_sums[j-1] > 0) {
	mas_theta_or_find_crit_pair_for_sch(j, theta, theta_sums, P_subset, J_subset, working_scp,
					    feasible_guide, alpha, active_schools_index, omega,
					    favorites, fully_allocated, critical_pair_found);
      }
    }
    k++;
  }
}


void mas_theta_or_find_crit_pair_for_sch(int sch, int** theta, int* theta_sums,
					 struct subset* P_subset, struct subset* J_subset,
					 struct process_scp* working_scp,
					 struct partial_alloc* feasible_guide,
					 struct index* alpha, struct index* active_schools_index,
					 struct index* omega, int* favorites,
					 int* fully_allocated, int* critical_pair_found) {
  
  int* o_h = malloc(sizeof(int));
  struct pivot* my_pivot;  
  struct index_list* P_increments;
  struct index_list* J_increments;
  
  P_increments = initialized_index_list();
  J_increments = initialized_index_list();

  *o_h = 0;

  compute_increments_and_o_h(J_subset, P_subset, J_increments, P_increments,
			     feasible_guide, working_scp, alpha, active_schools_index,
			     omega, theta, theta_sums, favorites, fully_allocated,
			     sch, o_h, critical_pair_found);
  
  if (!*critical_pair_found) {
    my_pivot = extract_pivot(theta, alpha, *o_h, feasible_guide, working_scp,
			     J_increments, P_increments, favorites);
			
    execute_pivot(my_pivot,theta,theta_sums);

    destroy_pivot_ptr(my_pivot);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(o_h);
  destroy_index_list(J_increments);
  destroy_index_list(P_increments);
}


void compute_increments_and_o_h(struct subset* J_subset,
				struct subset* P_subset,
				struct index_list* J_increments,
				struct index_list* P_increments,
				struct partial_alloc* feasible_guide,
				struct process_scp* working_scp,
				struct index* alpha,
				struct index* active_schools_index,
				struct index* omega, int** theta, int* theta_sums,
				int* favorites,
				int* fully_allocated, int sch, int* o_h,
				int* critical_pair_found) {
  int k, nst, nsc, done, sch_no;
  
  struct subset next_J_subset, next_P_subset;
  struct index first_P_index, next_J_index, next_P_index;

  done = 0;
  
  nst = feasible_guide->no_students;
  nsc = feasible_guide->no_schools;

  add_element(P_subset,sch);

  first_P_index = index_of_subset(P_subset);
  add_index_to_index_list(P_increments,&first_P_index);     
  destroy_index(first_P_index);

  while (!done) {
    next_J_subset = nullset(nst);
    next_J_h(&next_J_subset, J_subset, P_subset, feasible_guide, active_schools_index,
	     omega, theta, favorites);
	   
    if (is_nullset(&next_J_subset)) {
      done = 1;
      destroy_subset(next_J_subset);
    }
    else {
      next_J_index = index_of_subset(&next_J_subset);
	   
      add_index_to_index_list(J_increments,&next_J_index);     
  
      destroy_subset(next_J_subset);
      destroy_index(next_J_index);
    }
    
    if (!done) {
      next_P_subset = nullset(nsc);
      next_P_h(&next_P_subset, J_subset, P_subset, feasible_guide, working_scp, alpha, theta);
      if (is_nullset(&next_P_subset)) {
	done = 1;
	destroy_subset(next_P_subset);
      }
      else {
	next_P_index = index_of_subset(&next_P_subset);
	add_index_to_index_list(P_increments,&next_P_index);     
	destroy_subset(next_P_subset);

	for (k = 1; k <= next_P_index.no_elements; k++) {
	  sch_no = next_P_index.indices[k-1];
	  if (!fully_allocated[sch_no-1] || theta_sums[sch_no-1] < 0) {	    
	    done = 1;
	    *o_h = sch_no;
	  }
	}
	destroy_index(next_P_index);
      }
    }
  }

  if (*o_h == 0) {
    *critical_pair_found = 1;
  }
}


struct tableau linear_program_from_scp(struct process_scp* working_scp) {
  int nst, nsc, no_b, no_n, i, j, k, l;
  int* prior_sum;
  
  struct tableau answer;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  no_b =  nst + nsc;
  no_n = 1;
  for (i = 1; i <= nst; i++) {
    no_n += working_scp->no_eligible_schools[i-1] - 1;
  }
  
  answer.no_basis = no_b;
  answer.no_nonbasis = no_n;

  answer.index = malloc((no_b + no_n) * sizeof(int));
  for (k = 1; k <= no_b + no_n; k++) {
    answer.index[k-1] = k;
  }

  answer.obj_fnc = malloc((no_n + 1) * sizeof(double));
  for (k = 1; k <= no_n - 1; k++) {
    answer.obj_fnc[k-1] = 0.0;
  }
  answer.obj_fnc[no_n - 1] = 1.0;
  answer.obj_fnc[no_n] = 0.0;

  answer.coeff = malloc(no_b * sizeof(double*));
  for (k = 1; k <= no_b; k++) {
    answer.coeff[k] = malloc((no_n + 1) * sizeof(double));
    for (l = 1; l <= no_n + 1; l++) {
      answer.coeff[k-1][l-1] = 0.0;
    }
  }

  prior_sum = malloc(nst * sizeof(int));
  prior_sum[0] = 0;
  for (i = 2; i <= nst; i++) {
    prior_sum[i-1] = prior_sum[i-2] + working_scp->no_eligible_schools[i-2] - 1;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= working_scp->no_eligible_schools[i-1]; k++) {
      answer.coeff[i-1][prior_sum[i-1] + k - 1] = 1.0;
    }
    answer.coeff[i-1][no_n-1] = 1.0;
    answer.coeff[i-1][no_n] = 1.0;
  }

  for (j = 1; j <= nsc; j++) {
    answer.coeff[nst + j - 1][no_n] = working_scp->quotas[j-1];
    for (i = 1; i <= nst; i++) {
      if (working_scp->preferences[i-1][0] == j) {
	answer.coeff[nst + j - 1][no_n] -= 1.0;
	for (k = 2; k <= working_scp->no_eligible_schools[i-1]; k++) {
	  answer.coeff[nst + j - 1][prior_sum[i-1] + k - 2] = -1.0;
	}
      }
      else {
	for (k = 2; k <= working_scp->no_eligible_schools[i-1]; k++) {
	  if (working_scp->preferences[i-1][k-1] == j) {
	    answer.coeff[nst + j - 1][prior_sum[i-1] + k - 2] = 1.0;
	  }
	}
      }
    }
  }

  free(prior_sum);

  return answer;
}
