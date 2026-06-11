#include "gcpscode.h" 

partial_alloc simple_GCPS_alloc(input_sch_ch_prob* input) {
  run_data* data;

  partial_alloc answer;

  data = malloc(sizeof(run_data));
  data->no_segments = 0;
  data->no_splits = 0;
  data->no_new_pivots = 0;
  data->no_old_pivots = 0;
  data->h_sum = 0;

  answer = GCPS_allocation(input, data); 
  
  /*
  printf("There were %d segments, %d splits, %d new pivots, %d old
   pivots, and h_sum was %d.\n", data->no_segments, data->no_splits,
   data->no_new_pivots, data->no_old_pivots, data->h_sum);
   */

  free(data);

  return answer;
}

partial_alloc GCPS_allocation(input_sch_ch_prob* input, run_data* data) {
  partial_alloc feasible_guide;
  pivot_list probe_list;
  input_sch_ch_prob stu_no_pr_scp;
  process_scp input_pr_scp;
  pure_alloc pure_da;
  
  stu_no_pr_scp = stu_no_priority_scp(input);
  input_pr_scp = process_scp_from_input(input);
  pure_da = deferred_acceptance(&stu_no_pr_scp); 
  feasible_guide = partial_allocation_from_pure(&pure_da);
  
  destroy_input_sch_ch_prob(stu_no_pr_scp);
  destroy_pure_alloc(pure_da);  
  destroy_input_sch_ch_prob(*input);
  
  probe_list = void_pivot_list();

  return GCPS_allocation_with_guide(&input_pr_scp, &feasible_guide, &probe_list, data);
}

partial_alloc simple_GCPS_alloc_with_guide(input_sch_ch_prob* input,
					   partial_alloc* feasible_guide) {
  pivot_list probe_list;

  process_scp input_pr_scp;

  partial_alloc answer;

  run_data* data;
  data = malloc(sizeof(run_data));
  data->no_segments = 0;
  data->no_splits = 0;
  data->no_new_pivots = 0;
  data->no_old_pivots = 0;
  data->h_sum = 0;

  probe_list = void_pivot_list();

  input_pr_scp = process_scp_from_input(input);

  answer = GCPS_allocation_with_guide(&input_pr_scp, feasible_guide, &probe_list, data);

  free(data);

  return answer;
}

partial_alloc GCPS_allocation_with_guide(process_scp* input, partial_alloc* feasible_guide,
					 pivot_list* probe_list, run_data* data) {  
  int nst, nsc;

  double time_left; 
  
  partial_alloc final_alloc;
  subset P_subset, J_subset;
  
  final_alloc = zero_alloc_for_process_scp(input);
  
  nst = input->no_students;
  J_subset = nullset(nst);
  nsc = input->no_schools;
  P_subset = nullset(nsc);

  time_left = compute_until_next_critical_pair(input, feasible_guide, &final_alloc, probe_list,
					       &P_subset, &J_subset, data);

  if (time_left < 0.0000001) {  
    destroy_subset(P_subset);
  
    destroy_subset(J_subset);
    destroy_process_scp(*input); 
    destroy_partial_alloc(*feasible_guide);
    destroy_pivot_list(*probe_list);
      
    return final_alloc;
  }
  
  data->no_splits++;

  descend_to_left_subproblem(input, &final_alloc, feasible_guide, probe_list,
			     &P_subset, &J_subset, data);
  
  descend_to_right_subproblem(input, &final_alloc, feasible_guide, probe_list,
			      &P_subset, &J_subset, data);
  
  destroy_subset(P_subset);
  destroy_subset(J_subset);

  return final_alloc;
}

double compute_until_next_critical_pair(process_scp* working_scp,
					partial_alloc* feasible_guide,
					partial_alloc* final_alloc,
					pivot_list* probe_list,
					subset* P_subset, subset* J_subset,
					run_data* data) {
  int critical_pair_found;

  critical_pair_found = 0;

  while (!critical_pair_found && working_scp->time_remaining > 0.0000001) {

    critical_pair_found = compute_next_path_segment_or_find_critical_pair(working_scp,
									  feasible_guide,
									  final_alloc,
									  probe_list,
									  P_subset, J_subset,
									  data);
  }
  
  return working_scp->time_remaining;
}

void descend_to_left_subproblem(process_scp* working_scp, partial_alloc* final_alloc,
				partial_alloc* feasible_guide, 
				pivot_list* probe_list,
				subset* P_subset, subset* J_subset,
				run_data* data) {  
  process_scp left_scp;
  partial_alloc left_feas_guide;
  partial_alloc left_increment;
  pivot_list left_list;
  element_list J_index, P_index;
  
  left_scp = left_sub_process_scp(working_scp, J_subset, P_subset);    
  left_feas_guide = left_sub_process_feasible_guide(feasible_guide, J_subset, P_subset);  
    
  left_list = left_reduced_pivot_list(probe_list, J_subset, P_subset);
  
  left_increment = GCPS_allocation_with_guide(&left_scp, &left_feas_guide, &left_list, data);
    
  J_index = element_list_of_subset(J_subset);
  P_index = element_list_of_subset(P_subset);
  
  increment_partial_alloc(final_alloc, &left_increment, &J_index, &P_index);

  destroy_partial_alloc(left_increment);
  destroy_element_list(J_index);
  destroy_element_list(P_index);
}

void descend_to_right_subproblem(process_scp* working_scp,
				 partial_alloc* final_alloc,
				 partial_alloc* feasible_guide, 
				 pivot_list* probe_list,
				 subset* P_subset, subset* J_subset,
				 run_data* data) {
  process_scp right_scp;
  partial_alloc right_feas_guide;
  partial_alloc right_increment;
  pivot_list right_list;
  element_list J_index, P_index;

  right_scp = right_sub_process_scp(working_scp, J_subset, P_subset);
  destroy_process_scp(*working_scp); 
  
  right_feas_guide = right_sub_process_feasible_guide(feasible_guide, J_subset, P_subset);
  destroy_partial_alloc(*feasible_guide);
  
  right_list = right_reduced_pivot_list(probe_list, J_subset, P_subset);
  destroy_pivot_list(*probe_list);

  right_increment = GCPS_allocation_with_guide(&right_scp, &right_feas_guide, &right_list, data);
  
  J_index = element_list_of_complement(J_subset);    
  P_index = element_list_of_complement(P_subset);
  
  increment_partial_alloc(final_alloc, &right_increment, &J_index, &P_index);

  destroy_partial_alloc(right_increment);
  destroy_element_list(J_index);
  destroy_element_list(P_index);
}
