#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {  
  int i,j;
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  struct sch_ch_prob copy;
  copy_sch_ch_prob(my_scp,&copy);
    
  int* subset_sizes;
  subset_sizes = popular_schools(&(my_scp->cee));
      
  int* popular = popular_schools(&(my_scp->cee));
  
  struct square_matrix related = related_matrix(&(my_scp->cee),subset_sizes);
      
  int* underallocated_student;
  underallocated_student = malloc(sizeof(int));
  *underallocated_student = 0;

  struct subset overallocated_schools  = nullset(nsc);

  struct subset last_overallocated_schools  = nullset(nsc);

  int depth = 0;

  struct subset_list* watch_list = initialized_subset_list();

  struct partial_alloc allocation = GCPS_schools_solver(&copy,&related,subset_sizes,
							underallocated_student,&overallocated_schools,
							popular,watch_list);

  while (*underallocated_student != 0 || overallocated_schools.subset_size > 0) {
    
    destroy_partial_alloc(allocation);
    if (*underallocated_student != 0) {

      printf("We are increasing.\n");
      
      increase_subset_sizes(subset_sizes,&(my_scp->cee),underallocated_student);
    }
    else {
      
      printf("We have the overallocated_schools = ");
      print_index_of_subset(&overallocated_schools);
      printf(".\n");
      
      
      augment_subset_sizes(subset_sizes,&overallocated_schools);

      struct index new_list_element = index_of_subset(&overallocated_schools);
      if (list_contains_index(watch_list,&new_list_element)) {
	depth++;

	if (depth >= 2) {
	  printf("We have reached depth 2 and are quitting.\n");
	  exit(0);
	}
	
	add_supersets_of_subsets_to_list(watch_list,&new_list_element,&related,popular,depth);
      }
      else {
	depth = 0;
	add_subset(watch_list,&new_list_element);
      }

      copy_subset(&overallocated_schools,&last_overallocated_schools);

      printf("After adding the subset, watch list is ");
      print_subset_list(watch_list);
      printf(".\n");
    }

    int max = 0;
    for (j = 1; j <= nsc; j++) {
      if (subset_sizes[j-1] > max) {
	max = subset_sizes[j-1];
      }
    }
    if (max > 6) {
      printf("We seem to have a critical set with more than 6 elements.\n");
      exit(0);
    }
    
    *underallocated_student = 0;
    destroy_subset(overallocated_schools);
    overallocated_schools = nullset(nsc);

    destroy_sch_ch_prob(copy);
    
    copy_sch_ch_prob(my_scp,&copy);
    allocation = GCPS_schools_solver(&copy,&related,subset_sizes,underallocated_student,
				     &overallocated_schools,
				     popular,watch_list);
  }

  destroy_sch_ch_prob(copy);
  free(underallocated_student);
  free(subset_sizes);
  free(popular);
  destroy_square_matrix(related);
  destroy_subset_list(watch_list);

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp,struct square_matrix* related,int* subset_sizes,
					 int* underallocated_student,struct subset* overallocated_schools,
					 int* popular, struct subset_list* watch_list) {
  
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

  struct subset left_overallocated;
  struct subset right_overallocated;

  int* left_subset_sizes;
  int* right_subset_sizes;

  int* left_popular;
  int* right_popular;

  struct subset_list* expanded_watch_list;
  struct subset_list* left_watch_list;
  struct subset_list* right_watch_list;

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

  expanded_watch_list = reversed_subset_list(expanded_list(watch_list,related,popular));
      
  double end_time = time_rem_after_first_gmc_eq(my_scp, related, subset_sizes,
						&stu_subset, &sch_subset,
						overallocated_schools,
						expanded_watch_list);

  if (overallocated_schools->subset_size > 0) {
    first_alloc = zero_partial_alloc(&(my_scp->cee));
    return first_alloc;
  }

  if (stu_subset.subset_size > 0) {
    left_stu_index = index_of_subset(&stu_subset);
  }
  if (sch_subset.subset_size > 0) {

    /*
    if (sch_subset.subset_size > 0) {
      printf("The critical school subset is ");
      print_index_of_subset(&sch_subset);
      printf(".\n");
    }
    */
    
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

      left_overallocated = nullset(sch_subset.subset_size);

      left_subset_sizes = malloc(sch_subset.subset_size * sizeof(int));
      for (j = 1; j <= sch_subset.subset_size; j++) {
	left_subset_sizes[j-1] = subset_sizes[left_sch_index.indices[j-1]-1];
      }

      left_popular = malloc(sch_subset.subset_size * sizeof(int));
      for (j = 1; j <= sch_subset.subset_size; j++) {
	left_popular[j-1] = popular[left_sch_index.indices[j-1]-1];
      }

      left_watch_list = reduced_subset_list(watch_list,&sch_subset);
      
      left_alloc = GCPS_schools_solver(&left_scp,&left_related,left_subset_sizes,
				       underallocated_student,&left_overallocated,
				       left_popular,left_watch_list);

      free(left_subset_sizes);
      
      if (*underallocated_student != 0) {
	*underallocated_student = left_stu_index.indices[*underallocated_student - 1];
      }
      
      if (left_overallocated.subset_size > 0) {
	
	for (j = 1; j <= sch_subset.subset_size; j++) {
	  if (left_overallocated.indicator[j-1] == 1) {
	    overallocated_schools->subset_size++;
	    overallocated_schools->indicator[left_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      increment_partial_alloc(&first_alloc, &left_alloc, &left_stu_index, &left_sch_index);

      destroy_subset(left_overallocated);
      destroy_sch_ch_prob(left_scp);
      destroy_square_matrix(left_related);
      destroy_partial_alloc(left_alloc);
      free(left_popular);
      destroy_subset_list(left_watch_list);
    }
    
    if (stu_compl.subset_size > 0) {      
      right_related = submatrix(related,&sch_compl);

      right_overallocated = nullset(sch_compl.subset_size);

      right_subset_sizes = malloc(sch_compl.subset_size * sizeof(int));
      for (j = 1; j <= sch_compl.subset_size; j++) {
	right_subset_sizes[j-1] = subset_sizes[right_sch_index.indices[j-1]-1];
      }

      right_popular = malloc(sch_compl.subset_size * sizeof(int));
      for (j = 1; j <= sch_compl.subset_size; j++) {
	right_popular[j-1] = popular[right_sch_index.indices[j-1]-1];
      }

      right_watch_list = reduced_subset_list(watch_list,&sch_compl);
      
      right_alloc = GCPS_schools_solver(&right_scp,&right_related,right_subset_sizes,
					underallocated_student,&right_overallocated,
					right_popular,right_watch_list);

      free(right_subset_sizes);
      
      if (*underallocated_student != 0) {
	*underallocated_student = right_stu_index.indices[*underallocated_student - 1];
      }
      if (right_overallocated.subset_size > 0) {
	
	for (j = 1; j <= sch_compl.subset_size; j++) {
	  if (right_overallocated.indicator[j-1] == 1) {
	    overallocated_schools->subset_size++;
	    overallocated_schools->indicator[right_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      increment_partial_alloc(&first_alloc, &right_alloc, &right_stu_index, &right_sch_index);

      destroy_subset(right_overallocated);
      destroy_square_matrix(right_related);
      destroy_sch_ch_prob(right_scp);
      destroy_partial_alloc(right_alloc);
      free(right_popular);
      destroy_subset_list(right_watch_list);
    }
  }

  if (stu_subset.subset_size > 0) {    
    destroy_index(left_stu_index);
  }
  if (sch_subset.subset_size > 0) {
    destroy_index(left_sch_index);
  }
  if (stu_compl.subset_size > 0) {
    destroy_index(right_stu_index);
  }
  if (stu_subset.subset_size > 0) {
    destroy_index(right_sch_index);
  }
  
  destroy_subset(stu_subset);
  destroy_subset(sch_subset);
  destroy_subset(stu_compl);
  destroy_subset(sch_compl);

  /*   print_partial_alloc(&first_alloc); */
 
     
  return first_alloc;
}
