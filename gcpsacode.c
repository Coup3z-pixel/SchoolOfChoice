#include "gcpsacode.h"

struct partial_alloc gcpsa_allocation(struct input_sch_ch_prob* myiscp) {
  int j, nsc;

  int* coarse;

  struct process_scp pr_scp;
  struct input_sch_ch_prob red_scp;
  struct partial_alloc mcca_alloc;
  struct partial_alloc gcpsa_alloc;

  nsc = myiscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }
  
  pr_scp = process_scp_from_input(myiscp);

  mcca_alloc = mcca_alloc_plus_coarse_cutoffs(&pr_scp, coarse);

  destroy_process_scp(pr_scp);

  red_scp = reduced_input_scp(myiscp, coarse);

  gcpsa_alloc = simple_GCPS_alloc_with_guide(&red_scp,&mcca_alloc);
  
  destroy_input_sch_ch_prob(*myiscp);
  destroy_input_sch_ch_prob(red_scp);

  free(coarse);

  return gcpsa_alloc;
}
