#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {  
  int i,j;
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  struct sch_ch_prob* copy;
  copy = malloc(sizeof(struct sch_ch_prob));
  copy_sch_ch_prob(my_scp,copy);
    
  int* subset_sizes;
  subset_sizes = popular_schools(&(my_scp->cee));
  
  struct square_matrix* related;
  struct square_matrix dummy;
  dummy = matrix_of_zeros(nsc);
  related = &dummy;
  relatedness_matrix(&(my_scp->cee),subset_sizes,related);
      
  int* underallocated_student;
  underallocated_student = malloc(sizeof(int));
  *underallocated_student = 0;

  struct partial_alloc allocation = GCPS_schools_solver(copy,related,subset_sizes,
							underallocated_student);
  destroy_sch_ch_prob(copy);

  while (*underallocated_student != 0) {
    destroy_partial_alloc(&allocation);
    increase_subset_sizes(subset_sizes,&(my_scp->cee),underallocated_student);
    *underallocated_student = 0;
    copy_sch_ch_prob(my_scp,copy);
    allocation = GCPS_schools_solver(copy,related,subset_sizes,underallocated_student);
    destroy_sch_ch_prob(copy);
  }

  free(underallocated_student);
  free(subset_sizes);
  destroy_square_matrix(related); 

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp,
					 struct square_matrix* related,
					 int* subset_sizes, int* underallocated_student) {  
  int j;
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  struct subset stu_subset;
  struct subset sch_subset;
  struct subset stu_compl;
  struct subset sch_compl;
  struct index left_stu_index;
  struct index left_sch_index;
  struct index right_stu_index;
  struct index right_sch_index;

  struct partial_alloc first_alloc;
  struct partial_alloc left_alloc;
  struct partial_alloc right_alloc;
  
  struct sch_ch_prob left_scp;
  struct sch_ch_prob right_scp;

  struct square_matrix left_related;
  struct square_matrix right_related;
    
  if (my_scp->time_remaining == 0.0) {
    return zero_partial_alloc(&my_scp->cee);
  }

  stu_subset = nullset(nst);
  sch_subset = nullset(nsc);
      
  double end_time = time_rem_after_first_gmc_eq(my_scp, related, subset_sizes,
					       &stu_subset, &sch_subset);

  if (stu_subset.subset_size > 0) {
    left_stu_index = index_of_subset(&stu_subset);
  }
  if (sch_subset.subset_size > 0) {
    left_sch_index = index_of_subset(&sch_subset);
  }
  
  stu_compl = nullset(nst);
  sch_compl = nullset(nsc);
  complement(&stu_subset, &stu_compl);
  complement(&sch_subset, &sch_compl);

  if (stu_compl.subset_size > 0) {
    right_stu_index = index_of_subset(&stu_compl);
  }
  if (sch_compl.subset_size > 0) {
    right_sch_index = index_of_subset(&sch_compl);
  }
    
  first_alloc =  allocate_until_new_time(my_scp, end_time);
  
  if (end_time > 0.000001) {

    if (stu_subset.subset_size > 0) {      
      left_scp = sub_sch_ch_prob(my_scp, &stu_subset, &sch_subset,underallocated_student);
      if (*underallocated_student != 0) {
	*underallocated_student = left_stu_index.indices[*underallocated_student - 1];
	return first_alloc;
      }
    }  
      
    if (stu_compl.subset_size > 0) {
      right_scp = sub_sch_ch_prob(my_scp, &stu_compl, &sch_compl,underallocated_student);
      if (*underallocated_student != 0) {        
	*underallocated_student = right_stu_index.indices[*underallocated_student - 1];
	return first_alloc;
      }
    }

    if (stu_subset.subset_size > 0) {      
      left_related = submatrix(related,&sch_subset);
      
      left_alloc = GCPS_schools_solver(&left_scp,&left_related,subset_sizes,
							    underallocated_student);
      if (*underallocated_student != 0) {
	*underallocated_student = left_stu_index.indices[*underallocated_student - 1];
      }
      increment_partial_alloc(&first_alloc, &left_alloc, &left_stu_index, &left_sch_index);

      destroy_sch_ch_prob(&left_scp);
      destroy_square_matrix(&left_related);
      destroy_partial_alloc(&left_alloc);
    }
    
    if (stu_compl.subset_size > 0) {      
      struct square_matrix right_related = submatrix(related,&sch_compl);
      
      struct partial_alloc right_alloc=GCPS_schools_solver(&right_scp,&right_related,subset_sizes,
							   underallocated_student); 
      if (*underallocated_student != 0) {
	*underallocated_student = right_stu_index.indices[*underallocated_student - 1];
      }
      increment_partial_alloc(&first_alloc, &right_alloc, &right_stu_index, &right_sch_index);

      destroy_square_matrix(&right_related);
      destroy_sch_ch_prob(&right_scp);
      destroy_partial_alloc(&right_alloc);
    }
  }

  if (stu_subset.subset_size > 0) {    
    destroy_index(&left_stu_index);
  }
  if (sch_subset.subset_size > 0) {
    destroy_index(&left_sch_index);
  }
  if (stu_compl.subset_size > 0) {
    destroy_index(&right_stu_index);
  }
  if (stu_subset.subset_size > 0) {
    destroy_index(&right_sch_index);
  }
  
  destroy_subset(&stu_subset);
  destroy_subset(&sch_subset);
  destroy_subset(&stu_compl);
  destroy_subset(&sch_compl);
     
  return first_alloc;
}
