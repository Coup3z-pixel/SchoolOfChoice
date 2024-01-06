#include "gcps_solver.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
  struct input_sch_ch_prob input_scp = sch_ch_prob_from_file(); 
  
  struct process_scp input_process_scp = process_scp_from_input(&input_scp);
  
  destroy_input_sch_ch_prob(input_scp);
  
  /*  print_process_scp(&input_process_scp); */

  int* segments = malloc(sizeof(int));
  *segments = 0;
  int* splits = malloc(sizeof(int));
  *splits = 0;
  int* pivots = malloc(sizeof(int));
  *pivots = 0;
  int* h_sum = malloc(sizeof(int));
  *h_sum = 0;
  
  struct partial_alloc gcps_alloc = GCPS_allocation(&input_process_scp, segments, splits,
						    pivots, h_sum);
  print_partial_alloc(&gcps_alloc); 

  /*
  printf("There were %d segments, %d splits, and %d pivots, and h_sum was %d.\n",
	 *segments, *splits, *pivots, *h_sum);
	 */

  free(segments);
  free(splits);
  free(pivots);
  free(h_sum);
  destroy_process_scp(input_process_scp);
  destroy_partial_alloc(gcps_alloc);

  return 0;
}
