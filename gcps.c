#include "gcpscode.h"
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
  
  struct process_scp input_process_scp;
  input_process_scp = process_scp_from_input(&input_scp);

  if (!satisfies_the_GMC(&input_process_scp)) {
    fprintf(stderr, "The input_scp does not satisfy the GMC.\n");
    exit(0);
  }
  
  struct partial_alloc gcps_alloc;
  gcps_alloc = simple_GCPS_alloc(&input_process_scp);

  if (!is_a_feasible_allocation(&gcps_alloc, &input_process_scp)) {
    fprintf(stderr, "gcps has produced a nonallocation!!\n");
    exit(0);
  }
  else {
    if (!allocation_is_efficient(&gcps_alloc, &input_process_scp)) {
      fprintf(stderr, "gcps has produced an inefficient allocation!!\n");
      exit(0);
    }
    else {
      struct partial_alloc gcps_copy = copy_of_partial_alloc(&gcps_alloc);
      transform_to_random_floating_point_pure_allocation(&gcps_copy);
      if (!allocation_is_efficient(&gcps_copy, &input_process_scp)) {
	fprintf(stderr, "gcps has produced an inefficient allocation!!\n");
	exit(0);
      }
      destroy_partial_alloc(gcps_copy);
    }
  }

  print_partial_alloc(&gcps_alloc); 
  
  destroy_input_sch_ch_prob(input_scp);
  destroy_process_scp(input_process_scp);
  destroy_partial_alloc(gcps_alloc);

  return 0;
}
