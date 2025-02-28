#include "bartercode.h"

struct partial_alloc BARTER_allocation(struct process_scp* input) {
  struct partial_alloc alloc_to_adjust;
  struct process_scp pr_scp;

  alloc_to_adjust = deferred_acceptance(input);

  fprintf(stderr, "Got past deferred_acceptance.\n");

  trade_until_efficient(&pr_scp, &alloc_to_adjust);

  return alloc_to_adjust;
}
