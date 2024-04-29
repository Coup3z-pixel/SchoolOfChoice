#include "gcps_solver.h"
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
  
  destroy_input_sch_ch_prob(input_scp);
  
  /*  print_process_scp(&input_process_scp); */

  int* no_segments = malloc(sizeof(int));
  *no_segments = 0;
  int* no_splits = malloc(sizeof(int));
  *no_splits = 0;
  int* no_new_pivots = malloc(sizeof(int));
  *no_new_pivots = 0;
  int* no_old_pivots = malloc(sizeof(int));
  *no_old_pivots = 0;
  int* h_sum = malloc(sizeof(int));
  *h_sum = 0;
  
  struct partial_alloc gcps_alloc = GCPS_allocation(&input_process_scp, no_segments, no_splits,
						    no_new_pivots, no_old_pivots, h_sum);
  char filename[20] = "allocate.mat";
  struct partial_alloc input_alloc = allocation_from_file(filename);

  if (!partial_allocs_are_same(&gcps_alloc,&input_alloc)) {
    printf("We seem to have screwed things up.\n");
  }
  else {
    printf("It seems that everything is OK.\n");
  }
  
  /*  print_partial_alloc(&gcps_alloc); */

  printf("There were %d segments, %d splits, %d new pivots, %d old pivots, and h_sum was %d.\n",
   *no_segments, *no_splits, *no_new_pivots, *no_old_pivots, *h_sum); 

  free(no_segments);
  free(no_splits);
  free(no_new_pivots);
  free(no_old_pivots);
  free(h_sum);
  destroy_process_scp(input_process_scp);
  destroy_partial_alloc(gcps_alloc);
  destroy_partial_alloc(input_alloc);

  return 0;
}
