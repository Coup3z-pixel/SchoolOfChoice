#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {  
  int i,j;
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  struct sch_ch_prob copy;
  copy_sch_ch_prob(my_scp,&copy);

  int* popular = popular_schools(&(my_scp->cee));  
  struct square_matrix related = related_matrix(&(my_scp->cee),popular);

  struct subset overallocated_schools  = nullset(nsc);
  struct subset new_critical_set  = nullset(nsc);

  int depth = 0;

  struct subset_list* watch_list = initialized_subset_list();

  struct partial_alloc allocation = GCPS_schools_solver(&copy,&related,popular,
							&overallocated_schools, &new_critical_set,
							watch_list, watch_list);

  while (overallocated_schools.subset_size > 0 || new_critical_set.subset_size > 0) {
    
    destroy_partial_alloc(allocation);
    struct subset_list* submission_list = copy_of_subset_list(watch_list);

    if (new_critical_set.subset_size > 0) {

      /*
      printf("We are adding the critical set ");
      struct index new_crit_index = index_of_subset(&new_critical_set);
      print_index(&new_crit_index);
      destroy_index(new_crit_index);
      printf(" to the watch_list.\n");
      */
      
      depth = 0;
      struct index crit_set_index = index_of_subset(&new_critical_set);
      add_subset(watch_list,&crit_set_index);
      destroy_index(crit_set_index);
    }
    
    if (new_critical_set.subset_size == 0 && overallocated_schools.subset_size > 0) {

      /*
      printf("We have the overallocated_schools = ");
      print_index_of_subset(&overallocated_schools);
      printf(" with watch_list ");
      print_subset_list(watch_list);
      printf(".\n");
      */
      
      struct index new_list_element = index_of_subset(&overallocated_schools);
      
      if (list_contains_index(watch_list,&new_list_element)) {
	depth++;

	/* printf("We are now at depth %d.\n",depth); */

	if (depth >= 8) {
	  printf("We have reached depth 8 and are quitting.\n");
	  exit(0);
	}
	
	add_second_list_to_first(submission_list,supersets_of_subsets(&new_list_element, &related, popular, depth));
	
      }
      else {
	depth = 0;
	add_subset(watch_list,&new_list_element);
	add_subset(submission_list,&new_list_element);
      }
    }
    
    destroy_subset(overallocated_schools);
    overallocated_schools = nullset(nsc);
    destroy_subset(new_critical_set);
    new_critical_set = nullset(nsc);

    destroy_sch_ch_prob(copy);
    
    copy_sch_ch_prob(my_scp,&copy);
    allocation = GCPS_schools_solver(&copy,&related,popular,
				     &overallocated_schools,&new_critical_set,
				     submission_list, watch_list);
    
    destroy_subset_list(submission_list);
  }

  destroy_sch_ch_prob(copy);
  free(popular);
  destroy_square_matrix(related);
  destroy_subset_list(watch_list);

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp, struct square_matrix* related, int* popular, 
					 struct subset* overallocated_schools, struct subset* new_critical_set,
					 struct subset_list* submission_list, struct subset_list* watch_list) {
  
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
  struct subset left_new_critical_set;
  struct subset right_new_critical_set;

  int* left_popular;
  int* right_popular;

  struct subset_list* expanded_submission_list;
  struct subset_list* left_submission_list;
  struct subset_list* right_submission_list;
  struct subset_list* left_watch_list;
  struct subset_list* right_watch_list;
  struct subset_list* expanded_watch_list;

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

  expanded_submission_list = expanded_list(submission_list, related, popular);
      
  double end_time = time_rem_after_first_gmc_eq(my_scp, related, 
						&stu_subset, &sch_subset,
						overallocated_schools,
						expanded_submission_list);

  expanded_watch_list = expanded_list(watch_list, related, popular);

  if (sch_subset.subset_size > 0) {
    
    struct index index_of_sch_subset = index_of_subset(&sch_subset);
    if (!list_contains_index(expanded_watch_list,&index_of_sch_subset)) {
      destroy_index(index_of_sch_subset);
      
      copy_subset(&sch_subset,new_critical_set);
    
      first_alloc = zero_partial_alloc(&(my_scp->cee));

      destroy_subset_list(expanded_submission_list);
      destroy_subset_list(expanded_watch_list);
      destroy_subset(stu_subset);
      destroy_subset(sch_subset);
    
      return first_alloc;
    }

    destroy_index(index_of_sch_subset);
  }

  if (overallocated_schools->subset_size > 0) {
    
    first_alloc = zero_partial_alloc(&(my_scp->cee));

    destroy_subset_list(expanded_submission_list);
    destroy_subset_list(expanded_watch_list);
    destroy_subset(stu_subset);
    destroy_subset(sch_subset);

    return first_alloc;
  }

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
      left_scp = sub_sch_ch_prob(my_scp, &stu_subset, &sch_subset);
    }  
      
    if (stu_compl.subset_size > 0) {
      right_scp = sub_sch_ch_prob(my_scp, &stu_compl, &sch_compl);
    }

    if (stu_subset.subset_size > 0) {      
      left_related = submatrix(related,&sch_subset);

      left_overallocated = nullset(sch_subset.subset_size);
      left_new_critical_set = nullset(sch_subset.subset_size);

      left_popular = malloc(sch_subset.subset_size * sizeof(int));
      for (j = 1; j <= sch_subset.subset_size; j++) {
	left_popular[j-1] = popular[left_sch_index.indices[j-1]-1];
      }

      left_submission_list = reduced_subset_list(submission_list,&sch_subset);
      left_watch_list = reduced_subset_list(watch_list,&sch_subset);
      
      left_alloc = GCPS_schools_solver(&left_scp,&left_related,left_popular,
				       &left_overallocated, &left_new_critical_set,
				       left_submission_list, left_watch_list);

      
      if (left_overallocated.subset_size > 0) {	
	for (j = 1; j <= sch_subset.subset_size; j++) {
	  if (left_overallocated.indicator[j-1] == 1) {
	    overallocated_schools->subset_size++;
	    overallocated_schools->indicator[left_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      if (left_new_critical_set.subset_size > 0) {	
	for (j = 1; j <= sch_subset.subset_size; j++) {
	  if (left_new_critical_set.indicator[j-1] == 1) {
	    new_critical_set->subset_size++;
	    new_critical_set->indicator[left_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      increment_partial_alloc(&first_alloc, &left_alloc, &left_stu_index, &left_sch_index);

      destroy_subset(left_overallocated);
      destroy_sch_ch_prob(left_scp);
      destroy_square_matrix(left_related);
      destroy_partial_alloc(left_alloc);
      free(left_popular);
      destroy_subset_list(left_submission_list);
    }
    
    if (stu_compl.subset_size > 0) {      
      right_related = submatrix(related,&sch_compl);

      right_overallocated = nullset(sch_compl.subset_size);
      right_new_critical_set = nullset(sch_compl.subset_size);

      right_popular = malloc(sch_compl.subset_size * sizeof(int));
      for (j = 1; j <= sch_compl.subset_size; j++) {
	right_popular[j-1] = popular[right_sch_index.indices[j-1]-1];
      }

      right_submission_list = reduced_subset_list(submission_list,&sch_compl);
      right_watch_list = reduced_subset_list(watch_list,&sch_compl);
      
      right_alloc = GCPS_schools_solver(&right_scp,&right_related,right_popular,
					&right_overallocated,&right_new_critical_set,
					right_submission_list, right_watch_list);

      
      if (right_overallocated.subset_size > 0) {
	
	for (j = 1; j <= sch_compl.subset_size; j++) {
	  if (right_overallocated.indicator[j-1] == 1) {
	    overallocated_schools->subset_size++;
	    overallocated_schools->indicator[right_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      if (right_new_critical_set.subset_size > 0) {	
	for (j = 1; j <= sch_compl.subset_size; j++) {
	  if (right_new_critical_set.indicator[j-1] == 1) {
	    new_critical_set->subset_size++;
	    new_critical_set->indicator[right_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      increment_partial_alloc(&first_alloc, &right_alloc, &right_stu_index, &right_sch_index);

      destroy_subset(right_overallocated);
      destroy_square_matrix(right_related);
      destroy_sch_ch_prob(right_scp);
      destroy_partial_alloc(right_alloc);
      free(right_popular);
      destroy_subset_list(right_submission_list);
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
     
  return first_alloc;
}
