#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "gcpsbcode.h"

int main(int argc, char *argv[]) {
  input_sch_ch_prob input_scp;
  partial_alloc gcpsb_alloc;

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

  gcpsb_alloc = gcpsb_allocation(&input_scp);
  
  print_partial_alloc(&gcpsb_alloc); 

  destroy_input_sch_ch_prob(input_scp);
  destroy_partial_alloc(gcpsb_alloc);

  return 0;
}
