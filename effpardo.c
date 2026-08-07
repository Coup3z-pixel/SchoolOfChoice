#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "efficient.h"

int main(int argc, char *argv[]) {
  int ex_ante_stable;
  
  input_sch_ch_prob input_scp;
  process_scp input_process_scp;
  partial_alloc input_alloc;

  if (argc < 4)  {
    fprintf(stderr, "effpardo invoked with too few (< 4) command line arguments.\n");
    exit(0);
  }
  if (argc == 4) {
    input_alloc = allocation_from_file(argv[1]);
    input_scp = sch_ch_prob_from_file(argv[2]);
    ex_ante_stable = atoi(argv[3]);
  }
  if (argc > 4)  {
    fprintf(stderr, "effpardo invoked with too many (> 4) command line arguments.\n");
    exit(0);
  }

  if (!is_feasible_for_input_scp(&input_alloc, &input_scp)) {
    fprintf(stderr, "effpardo was called on an alloc that is not feasible.");
    exit(0);
  }
  
  input_process_scp = process_scp_from_input(&input_scp);

  if (ex_ante_stable == 1) {
    if (!is_ex_ante_stable(&input_alloc, &input_process_scp)) {
      fprintf(stderr, "The ex ante stable version of effpardo was called on an alloc that is not ex ante stable.");
      exit(0);
    }
  }
  
  make_alloc_eff_and_par_dominant(&input_alloc, &input_process_scp, ex_ante_stable);
  
  print_sparse_partial_alloc(&input_alloc); 

  destroy_process_scp(input_process_scp);
  destroy_input_sch_ch_prob(input_scp);
  destroy_partial_alloc(input_alloc);

  return 0;
}
