#include "solver.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
  struct input_sch_ch_prob my_sch_ch_prob = sch_ch_prob_from_file();
  
  struct sch_ch_prob new_sch_ch_prob = sch_ch_prob_from_input(&my_sch_ch_prob);
  destroy_input_sch_ch_prob(&my_sch_ch_prob);
  
  struct sch_ch_prob red_sch_ch_prob = reduced_sch_ch_prob(&new_sch_ch_prob);
  destroy_sch_ch_prob(&new_sch_ch_prob); 

  struct partial_alloc solution = GCPS_schools_solver_top_level(&red_sch_ch_prob);

  print_partial_alloc(solution);

  destroy_sch_ch_prob(&red_sch_ch_prob); 
  destroy_partial_alloc(&solution); 

  return 0;
}
