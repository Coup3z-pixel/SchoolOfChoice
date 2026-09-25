#include "mcccode.h"
#include "purifycode.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
  int j, nsc;

  int* coarse;
  
  input_sch_ch_prob input_scp;
  process_scp input_process_scp;

  if (argc == 1) {
    const char input_file[20] = "schools.scp";
    input_scp = sch_ch_prob_from_file(input_file);
  }
  if (argc == 2) {
    input_scp = sch_ch_prob_from_file(argv[1]);
  }
  if (argc > 2)  {
    fprintf(stderr, "gcps invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }

  input_process_scp = process_scp_from_input(&input_scp);

  nsc = input_scp.no_schools;
  coarse = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = 0;
  }
  
  partial_alloc mcca_alloc;
  mcca_alloc = mcca_alloc_plus_coarse_cutoffs(&input_process_scp, coarse);

  if (!is_a_feasible_allocation(&mcca_alloc, &input_process_scp)) {
    fprintf(stderr, "mcca has produced a nonallocation!!\n");
     exit(0);
  }

  print_partial_alloc(&mcca_alloc); 

  destroy_input_sch_ch_prob(input_scp);
  destroy_process_scp(input_process_scp); 
  destroy_partial_alloc(mcca_alloc);
  free(coarse);

  return 0;
}
