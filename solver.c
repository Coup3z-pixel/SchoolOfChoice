#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {
  int i,j;
  
  struct square_matrix related;

  double* copy_quotas;
  copy_quotas = malloc(my_scp->cee.no_schools * sizeof(double));
  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    copy_quotas[j-1] = my_scp->cee.quotas[j-1];
  }

  struct stat buffer;
  if (stat("related.mat",&buffer) == 0) {
    related = related_matrix_from_file();
    if (related.dimension != my_scp->cee.no_schools) {
      printf("ERROR: related.mat has the wrong number of schools.\n");
      exit(0);
    }
  }
  else {
    related = matrix_of_ones(my_scp->cee.no_schools);
  }

  edit_out_unpopular_schools(my_scp,&related); 

  struct partial_alloc allocation = GCPS_schools_solver(my_scp,&related);

  destroy_square_matrix(&related);

  for (j = 1; j <= my_scp->cee.no_schools; j++) {
    double total = 0.0;
    for (i = 1; i <= my_scp->cee.no_students; i++) {
      total += allocation.allocations[i-1][j-1];
    }
    if (total > copy_quotas[j-1] + 0.0001) {
      printf("The quota for school %i is %1.2f but %1.2f was allocated.\n", j,
	     copy_quotas[j-1], total);
      exit(0);
    }
  }
  free(copy_quotas);

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp,
					 struct square_matrix* related) {
  int j;
  if (my_scp->time_remaining == 0.0) {
    return zero_partial_alloc(my_scp);
  }
  else {    
    int nst = my_scp->cee.no_students;
    int nsc = my_scp->cee.no_schools;
    struct subset stu_subset = nullset(nst);
    struct subset sch_subset = nullset(nsc);

    double end_time = time_rem_of_first_gmc_eq(my_scp, related, &stu_subset, &sch_subset);

    struct partial_alloc first_allocation =  allocate_until_new_time(my_scp, end_time);

    if (end_time > 0.000001) {

    if (stu_subset.subset_size > 0) {      
      struct sch_ch_prob left_scp = sub_sch_ch_prob(my_scp, &stu_subset, &sch_subset);
      struct square_matrix subrelated = submatrix(related,&sch_subset);

      /*
      if (my_scp->cee.no_schools >= 1) {
	printf("The critical set of schools is ");
	print_subset(&sch_subset);
	printf("\n");
      }
      */
    
      struct partial_alloc left_alloc = GCPS_schools_solver(&left_scp,&subrelated);
      struct index left_stu_index = index_of_subset(&stu_subset);
      struct index left_sch_index = index_of_subset(&sch_subset); 

      increment_partial_alloc(&first_allocation, &left_alloc, &left_stu_index, &left_sch_index);

      destroy_square_matrix(&subrelated);
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
    struct square_matrix subrelated = submatrix(related,&sch_compl);

    /*
    if (my_scp->cee.no_schools >= 1) {
      printf("The residual set of schools is ");
      print_subset(&sch_compl);
      printf("\n");
    }
    */

    struct partial_alloc right_alloc = GCPS_schools_solver(&right_scp,&subrelated); 
    
    struct index right_stu_index = index_of_subset(&stu_compl);
    struct index right_sch_index = index_of_subset(&sch_compl);
    increment_partial_alloc(&first_allocation, &right_alloc, &right_stu_index, &right_sch_index);

    destroy_square_matrix(&subrelated);
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
