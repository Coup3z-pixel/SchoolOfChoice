#include "gcpscode.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
  struct input_sch_ch_prob input_scp;

  if (argc == 1) {
    const char input_file[20] = "schools.scp";
    input_scp = sch_ch_prob_from_file(input_file);
  }
  if (argc == 2) {
    input_scp = sch_ch_prob_from_file(argv[1]);
  }
  if (argc > 2)  {
    printf("gcps invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }
  
  struct process_scp input_process_scp = process_scp_from_input(&input_scp);
  
  struct partial_alloc gcps_alloc = simple_GCPS_alloc(&input_process_scp);

  print_partial_alloc(&gcps_alloc); 
  
  destroy_input_sch_ch_prob(input_scp);
  destroy_process_scp(input_process_scp);
  destroy_partial_alloc(gcps_alloc);

  return 0;
}
