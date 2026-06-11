#include "gcpsbcode.h"

partial_alloc gcpsb_allocation(input_sch_ch_prob* myiscp) {
  int j, nsc, max_slack_school;

  int* coarse;

  process_scp pr_scp;
  input_sch_ch_prob red_scp;
  partial_alloc mccb_alloc;
  partial_alloc gcpsb_alloc;
  partial_alloc gcpsb_copy;

  nsc = myiscp->no_schools;
  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }
  
  pr_scp = process_scp_from_input(myiscp);
  mccb_alloc = mcca_alloc_plus_coarse_cutoffs(&pr_scp, coarse);
  red_scp = reduced_input_scp(myiscp, coarse);
  gcpsb_alloc = simple_GCPS_alloc_with_guide(&red_scp,&mccb_alloc);
  
  max_slack_school = allocation_is_wasteful(&gcpsb_alloc, &pr_scp);

  while (max_slack_school > 0) {
    fprintf(stderr, "We have a wasteful allocation.\n");
    
    destroy_input_sch_ch_prob(red_scp);
    coarse[max_slack_school-1]--;
    red_scp = reduced_input_scp(myiscp, coarse);
    
    gcpsb_copy = copy_of_partial_alloc(&gcpsb_alloc);
    destroy_partial_alloc(gcpsb_alloc);
    gcpsb_alloc = simple_GCPS_alloc_with_guide(&red_scp, &gcpsb_copy);

    max_slack_school = allocation_is_wasteful(&gcpsb_alloc, &pr_scp);
  }

  free(coarse);
  destroy_input_sch_ch_prob(red_scp);
  destroy_process_scp(pr_scp);  

  return gcpsb_alloc;
}

partial_alloc gcpsbeff_allocation(input_sch_ch_prob* myiscp, int ex_ante_stable) {
  partial_alloc gcpsb_alloc;
  process_scp myscp;
  
  myscp = process_scp_from_input(myiscp);
  gcpsb_alloc = gcpsb_allocation(myiscp);
  make_alloc_eff_and_par_dominant(&gcpsb_alloc, &myscp, ex_ante_stable);

  destroy_process_scp(myscp);

  return gcpsb_alloc;
}

