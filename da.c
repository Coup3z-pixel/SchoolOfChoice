#include "defaccep.h"
#include "purifycode.h"
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
    fprintf(stderr, "gcps invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }
  
  struct pure_alloc da_alloc;
  da_alloc = deferred_acceptance(&input_scp);

  print_pure_alloc(&da_alloc); 
  
  destroy_input_sch_ch_prob(input_scp); 
  destroy_pure_alloc(da_alloc); 

  return 0;
}
