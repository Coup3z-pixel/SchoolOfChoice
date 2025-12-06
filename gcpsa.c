#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "gcpsacode.h"

int main(int argc, char *argv[]) {
  struct input_sch_ch_prob input_scp;
  struct partial_alloc gcpsa_alloc;

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

  gcpsa_alloc = gcpsa_allocation(&input_scp);
  
  print_sparse_partial_alloc(&gcpsa_alloc); 

  destroy_partial_alloc(gcpsa_alloc);

  return 0;
}
