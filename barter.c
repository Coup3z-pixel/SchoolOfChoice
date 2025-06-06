#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "bartercode.h"
#include "defaccep.h"

int main(int argc, char *argv[]) {
  struct input_sch_ch_prob input_scp;
  struct process_scp pr_scp;

  if (argc == 1) {
    const char input_file[20] = "schools.scp";
    input_scp = sch_ch_prob_from_file(input_file);
  }
  if (argc == 2) {
    input_scp = sch_ch_prob_from_file(argv[1]);
  }
  if (argc > 2)  {
    fprintf(stderr, "emcc invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }

  pr_scp = process_scp_from_input(&input_scp);
  

  struct partial_alloc barter_alloc = BARTER_allocation(&pr_scp);
  
  destroy_input_sch_ch_prob(input_scp);
  
  print_sparse_partial_alloc(&barter_alloc); 

  destroy_partial_alloc(barter_alloc);
  destroy_process_scp(pr_scp);

  return 0;
}
