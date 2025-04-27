#include "gcpsacode.h"

struct partial_alloc gcpsa_allocation(struct process_scp* myscp) {
  int j, nsc;

  int* coarse;
  
  struct partial_alloc alloc_to_adjust;

  struct process_scp red_scp;

  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  alloc_to_adjust = mcca_alloc_plus_coarse_cutoffs(myscp, coarse);

  red_scp = reduced_scp(myscp, coarse);

  alloc_to_adjust = simple_GCPS_alloc(&red_scp);

  free(coarse);
  destroy_process_scp(red_scp);

  return alloc_to_adjust;
}
