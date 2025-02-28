#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
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
    fprintf(stderr, "mcc invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }

  if (!safe_schools_are_safe(&input_scp)) {
    fprintf(stderr, "We got an unsafe input_scp.\n");
    exit(0);
  }
  
  struct process_scp pr_scp = process_scp_from_input(&input_scp);  
  destroy_input_sch_ch_prob(input_scp);

  struct partial_alloc mcc_alloc = MCC_alloc(&pr_scp);

  if (!students_are_fully_allocated(&mcc_alloc)) {
    printf("mcc was about to return a nonallocation.\n");
    exit(0);
  }
  
  print_sparse_partial_alloc(&mcc_alloc); 

  destroy_process_scp(pr_scp);
  destroy_partial_alloc(mcc_alloc);

  return 0;
}
