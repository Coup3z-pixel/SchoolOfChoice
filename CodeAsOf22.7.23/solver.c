#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {  
  int i,j;
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;

  int mode = 0;

  struct sch_ch_prob copy;
  copy_sch_ch_prob(my_scp,&copy);

  struct square_matrix related = related_matrix(&(my_scp->cee));

  struct subset overallocated_schools  = nullset(nsc);
  struct subset target  = nullset(nsc);
  struct subset new_critical_set  = nullset(nsc);

  struct subset_list* known_facets = initialized_subset_list();

  int depth = 0;

  struct partial_alloc allocation = GCPS_schools_solver(&copy, &related, &target, depth,
							&overallocated_schools, &new_critical_set,
							known_facets, known_facets, mode);

  while (overallocated_schools.subset_size > 0 || new_critical_set.subset_size > 0) {
    
    destroy_partial_alloc(allocation);
    struct subset_list* submission_list = copy_of_subset_list(known_facets);

    if (new_critical_set.subset_size > 0) {

      /*
      printf("\n    We are adding the critical set ");
      struct index new_crit_index = index_of_subset(&new_critical_set);
      print_index(&new_crit_index);
      destroy_index(new_crit_index);
      printf(" to the known_facets.\n");
      */
      
      depth = 0;
      struct index crit_set_index = index_of_subset(&new_critical_set);
      add_subset(known_facets,&crit_set_index);
      destroy_index(crit_set_index);
    }
    
    if (new_critical_set.subset_size == 0 && overallocated_schools.subset_size > 0) {

      /*
      printf("\nWe have the overallocated_schools = ");
      print_index_of_subset(&overallocated_schools);
      printf(" with known_facets ");
      print_subset_list(known_facets);
      printf(".\n");
      */

      copy_subset(&overallocated_schools,&target);
      
      struct index new_list_element = index_of_subset(&overallocated_schools);
      
      if (list_contains_index(known_facets,&new_list_element)) {
	depth++;

	/* printf("  We are now at depth %d.\n",depth); */

	if (depth >= 10) {
	  printf("We have reached depth 10 and are quitting.\n");
	  exit(0);
	}
	
	add_second_list_to_first(submission_list,supersets_of_subsets(&new_list_element, &related, depth));
	
      }
      else {
	depth = 0;
	add_subset(known_facets,&new_list_element);
	add_subset(submission_list,&new_list_element);
      }
    }
    
    destroy_subset(overallocated_schools);
    overallocated_schools = nullset(nsc);
    destroy_subset(new_critical_set);
    new_critical_set = nullset(nsc);

    destroy_sch_ch_prob(copy);
    
    copy_sch_ch_prob(my_scp,&copy);

    /* printf("\n\n"); */
    
    allocation = GCPS_schools_solver(&copy, &related, &target, depth,
				     &overallocated_schools, &new_critical_set,
				     submission_list, known_facets, mode);
    
    destroy_subset_list(submission_list);
  }

  destroy_subset(target);
  destroy_sch_ch_prob(copy);
  destroy_square_matrix(related);
  destroy_subset_list(known_facets);

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp, struct square_matrix* related,
					 struct subset* target, int depth,
					 struct subset* overallocated_schools, struct subset* new_critical_set,
					 struct subset_list* submission_list, struct subset_list* known_facets,
					 int mode) {
  
  int j;

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

  struct subset_list* expanded_submission_list;
  struct subset_list* left_submission_list;
  struct subset_list* right_submission_list;
  struct subset_list* left_known_facets;
  struct subset_list* right_known_facets;
  struct subset_list* expanded_known_facets;

  struct partial_alloc first_alloc;
  struct partial_alloc left_alloc;
  struct partial_alloc right_alloc;
  
  struct sch_ch_prob left_scp;
  struct sch_ch_prob right_scp;

  struct square_matrix left_related;
  struct square_matrix right_related;
  
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;
    
  if (my_scp->time_remaining == 0.0) {
    return zero_partial_alloc(&my_scp->cee);
  }

  stu_subset = nullset(nst);
  sch_subset = nullset(nsc);

  expanded_known_facets = expanded_list(known_facets, related);

  expanded_submission_list = expanded_list(submission_list, related);
      
  double end_time = new_time_rem_after_first_gmc_eq(my_scp, related, target, depth,
						    &stu_subset, &sch_subset,
						    overallocated_schools,
						    expanded_submission_list);

  /*
  if (overallocated_schools->subset_size == 0 && sch_subset.subset_size > 1) {
    struct index big_index = index_of_subset(&sch_subset);
    print_index(&big_index);
    destroy_index(big_index);
  }
  */

  if (sch_subset.subset_size > 0) {
    
    struct index index_of_sch_subset = index_of_subset(&sch_subset);
    if (!list_contains_index(expanded_known_facets,&index_of_sch_subset)) {
      destroy_index(index_of_sch_subset);
      
      copy_subset(&sch_subset,new_critical_set);
    
      first_alloc = zero_partial_alloc(&(my_scp->cee));

      destroy_subset_list(expanded_submission_list);
      destroy_subset_list(expanded_known_facets);
      destroy_subset(stu_subset);
      destroy_subset(sch_subset);
    
      return first_alloc;
    }

    destroy_index(index_of_sch_subset);
  }

  if (overallocated_schools->subset_size > 0) {
    
    first_alloc = zero_partial_alloc(&(my_scp->cee));

    destroy_subset_list(expanded_submission_list);
    destroy_subset_list(expanded_known_facets);
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


      left_submission_list = reduced_subset_list(submission_list,&sch_subset);
      
      left_known_facets = reduced_subset_list(known_facets,&sch_subset);
      
      left_alloc = GCPS_schools_solver(&left_scp, &left_related, target, depth,
				       &left_overallocated, &left_new_critical_set,
				       left_submission_list, left_known_facets, mode);

      
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
      destroy_subset_list(left_submission_list);
    }
    
    if (stu_compl.subset_size > 0) {      
      right_related = submatrix(related,&sch_compl);

      right_overallocated = nullset(sch_compl.subset_size);
      right_new_critical_set = nullset(sch_compl.subset_size);

      right_submission_list = reduced_subset_list(submission_list,&sch_compl);
      
      right_known_facets = reduced_subset_list(known_facets,&sch_compl);
      
      right_alloc = GCPS_schools_solver(&right_scp, &right_related, target, depth,
					&right_overallocated, &right_new_critical_set,
					right_submission_list, right_known_facets, mode);

      
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
