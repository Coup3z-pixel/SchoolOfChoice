#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "pushrelabel.h"
#include "mcccode.h"

int main(int argc, char *argv[]) {
  struct input_sch_ch_prob input_scp;

  if (argc == 1) {
    const char input_file[20] = "schools.scp";
    input_scp = sch_ch_prob_from_file(input_file);
  }
  if (argc == 2) {
    input_scp = sch_ch_prob_from_file(argv[1]);
  }
  if (argc > 2)  {
    printf("mcc invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }

  struct process_scp pr_scp = process_scp_from_input(&input_scp);

  if (!satisfies_the_GMC(&pr_scp)) {
    printf("The input_scp does not satisfy the GMC.\n");
    exit(0);
  }

  int* coarse = malloc(pr_scp.no_schools * sizeof(int));
  
  struct partial_alloc mcc_alloc = MCC_alloc_plus_coarse_cutoffs(&pr_scp,coarse);
  
  print_partial_alloc(&mcc_alloc); 

  free(coarse);
  destroy_input_sch_ch_prob(input_scp);
  destroy_process_scp(pr_scp);
  destroy_partial_alloc(mcc_alloc);

  return 0;
}