#include "emcccode.h"

struct partial_alloc EMCC_allocation(struct process_scp* myscp) {
  int j, nsc;

  int* coarse;
  
  struct partial_alloc alloc_to_adjust;

  struct process_scp red_scp;

  nsc = myscp->no_schools;

  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }

  alloc_to_adjust = MCC_alloc_plus_coarse_cutoffs(myscp, coarse);

  red_scp = reduced_scp(myscp, coarse);

  trade_until_efficient(&red_scp, &alloc_to_adjust);

  free(coarse);
  destroy_process_scp(red_scp);

  return alloc_to_adjust;
}
