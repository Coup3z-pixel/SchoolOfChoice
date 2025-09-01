#include "gcpsacode.h"

struct partial_alloc gcpsa_allocation(struct process_scp* myscp) {
  int j, nsc;

  int* coarse;
  
  struct partial_alloc mcca_alloc;
  struct partial_alloc gcpsa_alloc;

  struct process_scp red_scp;

  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  mcca_alloc = mcca_alloc_plus_coarse_cutoffs(myscp, coarse);

  destroy_partial_alloc(mcca_alloc);

  red_scp = reduced_scp(myscp, coarse);

  gcpsa_alloc = simple_GCPS_alloc(&red_scp);

  free(coarse);

  return gcpsa_alloc;
}
