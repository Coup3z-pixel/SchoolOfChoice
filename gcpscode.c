#include "gcpscode.h" 

struct partial_alloc simple_GCPS_alloc(struct process_scp* input) {
  int* no_segments;
  int* no_splits;
  int* no_new_pivots;
  int* no_old_pivots;
  int* h_sum;

  struct partial_alloc answer;

  no_segments = malloc(sizeof(int));
  no_splits = malloc(sizeof(int));
  no_new_pivots = malloc(sizeof(int));
  no_old_pivots = malloc(sizeof(int));
  h_sum = malloc(sizeof(int));

  *no_segments = 0;
  *no_splits = 0;
  *no_new_pivots = 0;
  *no_old_pivots = 0;
  *h_sum = 0;

  answer = GCPS_allocation(input, no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum); 
  
  /*
  printf("There were %d segments, %d splits, %d new pivots, %d old pivots, and h_sum was %d.\n",
   *no_segments, *no_splits, *no_new_pivots, *no_old_pivots, *h_sum); 
   */

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
  
  free(no_segments);
  free(no_splits);
  free(no_new_pivots);
  free(no_old_pivots);
  free(h_sum);

  return answer;
}

struct partial_alloc GCPS_allocation(struct process_scp* input, int* no_segments,
				     int* no_splits, int* no_new_pivots,
				     int* no_old_pivots, int* h_sum) {
  struct partial_alloc feasible_guide;
  struct pivot_list probe_list;
  
  feasible_guide = deferred_acceptance(input);
  
  probe_list = void_pivot_list();

  return GCPS_allocation_with_guide(input, &feasible_guide, &probe_list,
				    no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum);
}

double compute_until_next_critical_pair(struct process_scp* working_scp,
					struct partial_alloc* feasible_guide,
					struct partial_alloc* final_alloc,
					struct pivot_list* probe_list,
					struct subset* P_subset, struct subset* J_subset,
					int* no_segments, int* no_new_pivots, int* no_old_pivots,
					int* h_sum) {
  int* critical_pair_found;
  critical_pair_found = malloc(sizeof(int));
  *critical_pair_found = 0;

  while (!*critical_pair_found && working_scp->time_remaining > 0.000000001) {

    compute_next_path_segment_or_find_critical_pair(working_scp, feasible_guide,
						    final_alloc, probe_list,
						    P_subset, J_subset,
						    critical_pair_found,
						    no_segments, no_new_pivots,
						    no_old_pivots, h_sum);
  }
    
  free(critical_pair_found);
  
  return working_scp->time_remaining;
}

struct partial_alloc GCPS_allocation_with_guide(struct process_scp* input,
						struct partial_alloc* feasible_guide,
						struct pivot_list* probe_list,
						int* no_segments, int* no_splits,
						int* no_new_pivots,
						int* no_old_pivots,int* h_sum) {  
  int nst, nsc;

  double time_remaining; 
  
  struct partial_alloc final_alloc;
  struct subset P_subset, J_subset;
  
  final_alloc = zero_alloc_for_process_scp(input);
  
  nst = input->no_students;
  nsc = input->no_schools;
  P_subset = nullset(nsc);
  J_subset = nullset(nst);

  time_remaining = compute_until_next_critical_pair(input, feasible_guide, &final_alloc,
						    probe_list, &P_subset, &J_subset,
						    no_segments, no_new_pivots, no_old_pivots,
						    h_sum);

  if (time_remaining < 0.000000001) {  

    destroy_subset(P_subset);
    destroy_subset(J_subset);
    destroy_process_scp(*input); 
    destroy_partial_alloc(*feasible_guide);
    destroy_pivot_list(*probe_list);
      
    return final_alloc;
  }
  
  (*no_splits)++;

  descend_to_left_subproblem(input, &final_alloc, feasible_guide, probe_list,
			     &P_subset, &J_subset,
			     no_segments, no_splits, no_new_pivots,no_old_pivots, h_sum);
  
  descend_to_right_subproblem(input, &final_alloc, feasible_guide, probe_list,
			      &P_subset, &J_subset,
			      no_segments, no_splits, no_new_pivots, no_old_pivots, h_sum);
  
  destroy_subset(P_subset);
  destroy_subset(J_subset);

  return final_alloc;
}

void descend_to_left_subproblem(struct process_scp* working_scp, struct partial_alloc* final_alloc,
				struct partial_alloc* feasible_guide, 
				struct pivot_list* probe_list,
				struct subset* P_subset, struct subset* J_subset,
				int* no_segments, int* no_splits, int* no_new_pivots,
				int* no_old_pivots, int* h_sum) {  
  struct process_scp left_scp;
  struct partial_alloc left_feas_guide;
  struct partial_alloc left_increment;
  struct pivot_list left_list;
  struct index J_index, P_index;
  
  left_scp = left_sub_process_scp(working_scp, J_subset, P_subset);    
  left_feas_guide = left_sub_process_feasible_guide(feasible_guide, J_subset, P_subset);  
  left_list = left_reduced_pivot_list(probe_list, J_subset, P_subset);
    
  left_increment = GCPS_allocation_with_guide(&left_scp, &left_feas_guide, &left_list,
						no_segments, no_splits, no_new_pivots,
						no_old_pivots, h_sum);
    
  J_index = index_of_subset(J_subset);
  P_index = index_of_subset(P_subset);
  
  increment_partial_alloc(final_alloc, &left_increment, &J_index, &P_index);

  destroy_partial_alloc(left_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}

void descend_to_right_subproblem(struct process_scp* working_scp,
				 struct partial_alloc* final_alloc,
				 struct partial_alloc* feasible_guide, 
				 struct pivot_list* probe_list,
				 struct subset* P_subset, struct subset* J_subset,
				 int* no_segments, int* no_splits, int* no_new_pivots,
				 int* no_old_pivots, int* h_sum) {
  struct process_scp right_scp;
  struct partial_alloc right_feas_guide;
  struct partial_alloc right_increment;
  struct pivot_list right_list;
  struct index J_index, P_index;

  right_scp = right_sub_process_scp(working_scp, J_subset, P_subset);
  destroy_process_scp(*working_scp); 
  
  right_feas_guide = right_sub_process_feasible_guide(feasible_guide, J_subset, P_subset);
  destroy_partial_alloc(*feasible_guide);
  
  right_list = right_reduced_pivot_list(probe_list, J_subset, P_subset);
  destroy_pivot_list(*probe_list);
  

  right_increment = GCPS_allocation_with_guide(&right_scp, &right_feas_guide, &right_list,
					       no_segments, no_splits, no_new_pivots,
					       no_old_pivots, h_sum);    
  J_index = index_of_complement(J_subset);    
  P_index = index_of_complement(P_subset);
  
  increment_partial_alloc(final_alloc, &right_increment, &J_index, &P_index);

  destroy_partial_alloc(right_increment);
  destroy_index(J_index);
  destroy_index(P_index);
}
