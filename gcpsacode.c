#include "gcpsbcode.h"

partial_alloc gcpsa_allocation(input_sch_ch_prob* myiscp) {
  int j, nsc, max_slack_school;

  int* coarse;

  process_scp pr_scp;
  input_sch_ch_prob red_scp;
  partial_alloc mcca_alloc;
  partial_alloc feas_guide;
  partial_alloc red_scp_alloc;
  partial_alloc gcpsa_alloc;

  nsc = myiscp->no_schools;
  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }
  
  pr_scp = process_scp_from_input(myiscp);
  
  mcca_alloc = mcca_alloc_plus_coarse_cutoffs(&pr_scp, coarse);
  red_scp = reduced_input_scp(myiscp, coarse);
  feas_guide = translate_alloc_for_input_scp(&red_scp, &mcca_alloc);
  destroy_partial_alloc(mcca_alloc);
  red_scp_alloc = simple_GCPS_alloc_with_guide(&red_scp, &feas_guide);;
  gcpsa_alloc = translate_alloc_for_input_scp(myiscp, &red_scp_alloc);
  destroy_partial_alloc(red_scp_alloc);
  
  max_slack_school = allocation_is_wasteful(&gcpsa_alloc, &pr_scp);

  while (max_slack_school > 0) {
    fprintf(stderr, "We have a wasteful allocation.\n");
    
    destroy_input_sch_ch_prob(red_scp);
    
    coarse[max_slack_school-1]--;
    
    red_scp = reduced_input_scp(myiscp, coarse);
    feas_guide = translate_alloc_for_input_scp(&red_scp, &gcpsa_alloc);
    destroy_partial_alloc(gcpsa_alloc);
    red_scp_alloc = simple_GCPS_alloc_with_guide(&red_scp, &feas_guide);
    gcpsa_alloc = translate_alloc_for_input_scp(myiscp, &red_scp_alloc);
    destroy_partial_alloc(red_scp_alloc);

    max_slack_school = allocation_is_wasteful(&gcpsa_alloc, &pr_scp);
  }

  free(coarse);
  destroy_input_sch_ch_prob(red_scp);
  destroy_process_scp(pr_scp);  

  return gcpsa_alloc;
}

