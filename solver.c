#include "solver.h"

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp) {
  if (my_scp->time_remaining == 0.0) {
    return zero_partial_alloc(my_scp);
  }
  else {    
    int nst = my_scp->cee.no_students;
    int nsc = my_scp->cee.no_schools;
    struct subset stu_subset = nullset(nst);
    struct subset sch_subset = nullset(nsc);

    double end_time = time_rem_of_first_gmc_eq(my_scp, &stu_subset, &sch_subset);

    struct partial_alloc first_allocation =  allocate_until_new_time(my_scp, end_time);

    if (end_time > 0.000001) {

    if (stu_subset.subset_size > 0) {      
      struct sch_ch_prob left_scp = sub_sch_ch_prob(my_scp, &stu_subset, &sch_subset);
    
      struct partial_alloc left_alloc = GCPS_schools_solver(&left_scp);
      struct index left_stu_index = index_of_subset(&stu_subset);
      struct index left_sch_index = index_of_subset(&sch_subset); 

      increment_partial_alloc(&first_allocation, &left_alloc, &left_stu_index, &left_sch_index);

      destroy_index(&left_stu_index);
      destroy_index(&left_sch_index);
      destroy_sch_ch_prob(&left_scp);
      destroy_partial_alloc(&left_alloc);
    }

    struct subset stu_compl = nullset(nst);
    struct subset sch_compl = nullset(nsc);
    complement(&stu_subset, &stu_compl);
    complement(&sch_subset, &sch_compl);
    
    struct sch_ch_prob right_scp = sub_sch_ch_prob(my_scp, &stu_compl, &sch_compl);

    struct partial_alloc right_alloc = GCPS_schools_solver(&right_scp); 
    
    struct index right_stu_index = index_of_subset(&stu_compl);
    struct index right_sch_index = index_of_subset(&sch_compl);
    increment_partial_alloc(&first_allocation, &right_alloc, &right_stu_index, &right_sch_index);

    destroy_index(&right_stu_index);
    destroy_index(&right_sch_index);
    destroy_subset(&stu_subset);
    destroy_subset(&sch_subset);
    destroy_subset(&stu_compl);
    destroy_subset(&sch_compl);
    destroy_sch_ch_prob(&right_scp);
    destroy_partial_alloc(&right_alloc);

    }

    return first_allocation;
  }
}
