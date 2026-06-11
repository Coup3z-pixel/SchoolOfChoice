#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "gcpsacode.h"

int main(int argc, char *argv[]) {
  int ex_ante_stable;
  input_sch_ch_prob input_scp;
  partial_alloc gcpsaeff_alloc;
  process_scp input_process_scp;

  dgraph* graph; 

  if (argc == 1) {
    const char input_file[20] = "schools.scp";
    input_scp = sch_ch_prob_from_file(input_file);
  }
  if (argc == 2 || argc == 3) {
    input_scp = sch_ch_prob_from_file(argv[1]);
  }
  if (argc == 2) {
    ex_ante_stable = 0;
  }
  if (argc == 3) {
    ex_ante_stable = atoi(argv[2]);
  }
  if (argc > 3)  {
    fprintf(stderr, "gcpsaeff invoked with too many (> 3) command line arguments.\n");
    exit(0);
  }
  
  input_process_scp = process_scp_from_input(&input_scp);

  gcpsaeff_alloc = gcpsaeff_allocation(&input_scp, ex_ante_stable);

  if (!allocation_is_efficient(&gcpsaeff_alloc, &input_process_scp, ex_ante_stable)) {
    fprintf(stderr, "The gcpsaeff allocation is inefficient for ex_ante_stable = %i.\n",
	    ex_ante_stable);
  }

  
  print_sparse_partial_alloc(&gcpsaeff_alloc); 

  destroy_partial_alloc(gcpsaeff_alloc);
  destroy_process_scp(input_process_scp);
  destroy_input_sch_ch_prob(input_scp);

  return 0;
}
