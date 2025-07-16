#include "gcpsacode.h"

struct partial_alloc gcpsa_allocation(struct process_scp* myscp) {
  int j, nsc;

  int* coarse;
  
  struct partial_alloc gcpsa_alloc;

  struct process_scp red_scp;

  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  gcpsa_alloc = mcca_alloc_plus_coarse_cutoffs(myscp, coarse);

  red_scp = reduced_scp(myscp, coarse);

  gcpsa_alloc = simple_GCPS_alloc(&red_scp);

  free(coarse);
  destroy_process_scp(red_scp);

  return gcpsa_alloc;
}
