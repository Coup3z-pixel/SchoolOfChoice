#include "solver.h"
#include "gcps_solver.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
  struct input_sch_ch_prob input_scp = sch_ch_prob_from_file(); 
  
  struct frac_scp input_frac_scp = frac_scp_from_input_sch_ch_prob(&input_scp);
  
  destroy_input_sch_ch_prob(input_scp);
  
  /*  print_frac_scp(&input_frac_scp); */
  
  struct partial_alloc gcps_alloc = GCPS_allocation(&input_frac_scp);
  print_partial_alloc(&gcps_alloc);
  
  destroy_frac_scp(input_frac_scp);

  /*  struct sch_ch_prob new_sch_ch_prob = sch_ch_prob_from_input(&input_scp); */

  /*
  
  struct sch_ch_prob red_sch_ch_prob = reduced_sch_ch_prob(&new_sch_ch_prob);
  destroy_sch_ch_prob(new_sch_ch_prob); 
  struct partial_alloc solution = GCPS_schools_solver_top_level(&red_sch_ch_prob);
  destroy_sch_ch_prob(red_sch_ch_prob);

  if (!partial_allocs_are_same(&gcps_alloc,&solution)) {
    printf("YIKES!! We have different allocations from the two algorithms.\n");
  }
  */
  
  /*  destroy_partial_alloc(gcps_alloc); */
  /*  destroy_partial_alloc(solution); */

  /*
  struct input_sch_ch_prob my_sch_ch_prob = sch_ch_prob_from_file();
  
  struct sch_ch_prob new_sch_ch_prob = sch_ch_prob_from_input(&my_sch_ch_prob);
  destroy_input_sch_ch_prob(my_sch_ch_prob);
  
  struct sch_ch_prob red_sch_ch_prob = reduced_sch_ch_prob(&new_sch_ch_prob);
  destroy_sch_ch_prob(new_sch_ch_prob); 

  struct partial_alloc solution = GCPS_schools_solver_top_level(&red_sch_ch_prob);

  print_partial_alloc(&solution);  

  destroy_sch_ch_prob(red_sch_ch_prob); 
  destroy_partial_alloc(solution);
  */

  return 0;
}
