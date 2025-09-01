#include "gcpsbcode.h"

struct partial_alloc gcpsb_allocation(struct process_scp* myscp) {
  int j, nsc;

  int* coarse;
  
  struct partial_alloc mccb_alloc;  
  struct partial_alloc gcpsb_alloc;

  struct process_scp red_scp;

  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  mccb_alloc = mccb_alloc_plus_coarse_cutoffs(myscp, coarse);

  red_scp = reduced_scp(myscp, coarse);

  gcpsb_alloc = simple_GCPS_alloc(&red_scp);

  destroy_partial_alloc(mccb_alloc);

  free(coarse);

  return gcpsb_alloc;
}
