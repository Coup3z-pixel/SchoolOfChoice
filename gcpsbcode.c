#include "gcpsbcode.h"

struct partial_alloc gcpsb_allocation(struct process_scp* myscp) {
  int j, nsc;

  int* coarse;
  
  struct partial_alloc gcpsb_alloc;

  struct process_scp red_scp;

  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  gcpsb_alloc = mccb_alloc_plus_coarse_cutoffs(myscp, coarse);

  red_scp = reduced_scp(myscp, coarse);

  gcpsb_alloc = simple_GCPS_alloc(&red_scp);

  free(coarse);
  destroy_process_scp(red_scp);

  return gcpsb_alloc;
}
