#include <stdio.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "solver.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
  int j, k;
  struct input_sch_ch_prob my_sch_ch_prob;
  struct sch_ch_prob new_sch_ch_prob;
  struct sch_ch_prob sub_prob;
  struct sch_ch_prob red_sch_ch_prob;
    
  my_sch_ch_prob = sch_ch_prob_from_file(); 
  new_sch_ch_prob = sch_ch_prob_from_input(&my_sch_ch_prob);
  destroy_input_sch_ch_prob(&my_sch_ch_prob);
  print_sch_ch_prob(&new_sch_ch_prob);
  printf("\n");

  struct partial_alloc solution = GCPS_schools_solver_top_level(&new_sch_ch_prob);

  printf("The allocation is:\n");
  print_partial_alloc(solution);

  destroy_sch_ch_prob(&new_sch_ch_prob); 
  destroy_partial_alloc(&solution); 

  return 0;
}
