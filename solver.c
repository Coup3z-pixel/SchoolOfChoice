#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {  
  int nsc = my_scp->cee.no_schools;

  struct sch_ch_prob copy;
  copy_sch_ch_prob(my_scp,&copy);

  int level = 1;
  int depth = 1;
  int* target_level = malloc(sizeof(int));
  *target_level = 1;

  struct subset overallocated_schools  = nullset(nsc);

  struct subset new_critical_set  = nullset(nsc);

  struct subset_list* observed_overallocated_sets = initialized_subset_list();

  struct subset_list* known_facets = initialized_subset_list();

  int* exit_status = malloc(sizeof(int));
  *exit_status = 0;

  struct partial_alloc allocation = GCPS_schools_solver(&copy, level, target_level, depth,
							&overallocated_schools, &new_critical_set,
							observed_overallocated_sets, known_facets,
							exit_status);

  while (*exit_status != 0) {
    
    destroy_partial_alloc(allocation);

    if (*exit_status == 1) {

      /*
      if (new_critical_set.subset_size > 7) {
	printf("We got a big one.\n");
	exit(0);
      }
      */
      
      depth = 1;
      *target_level = 1;
      struct index crit_set_index = index_of_subset(&new_critical_set);
      
      if (list_contains_index(known_facets,&crit_set_index)) {
	printf("We somehow got a critical set twice.\n");
	exit(0);
      }
      
      add_subset(known_facets,&crit_set_index);
      if (list_contains_index(observed_overallocated_sets,&crit_set_index)) {
	remove_subset(observed_overallocated_sets,&crit_set_index);
      }
      destroy_index(crit_set_index);
    }
    
    if (*exit_status == 2) {

      struct index new_list_element = index_of_subset(&overallocated_schools);
      
      if (list_contains_index(observed_overallocated_sets,&new_list_element)) {
	
	depth++;

	if (depth >= 8) {
	  printf("We have reached depth 8 and are quitting.\n");
	  exit(0);
	}
	
      }
      else {
	depth = 1;
	add_subset(observed_overallocated_sets,&new_list_element);
      }

      destroy_index(new_list_element);
    }
    
    destroy_subset(overallocated_schools);
    overallocated_schools = nullset(nsc);
    destroy_subset(new_critical_set);
    new_critical_set = nullset(nsc);

    destroy_sch_ch_prob(copy);
    copy_sch_ch_prob(my_scp,&copy);

    *exit_status = 0;

    allocation = GCPS_schools_solver(&copy, level, target_level, depth,
				     &overallocated_schools, &new_critical_set,
				     observed_overallocated_sets, known_facets,
				     exit_status);
  }

  destroy_subset(overallocated_schools);
  destroy_subset(new_critical_set);
  destroy_sch_ch_prob(copy);
  destroy_subset_list(known_facets);
  destroy_subset_list(observed_overallocated_sets);
  free(target_level);
  free(exit_status);

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp, 
					 int level, int* target_level, int depth,
					 struct subset* overallocated_schools,
					 struct subset* new_critical_set,
					 struct subset_list* observed_overallocated_sets,
					 struct subset_list* known_facets,
					 int* exit_status) {
  
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

  struct subset_list* left_observed_overallocated_sets;
  struct subset_list* right_observed_overallocated_sets;
  struct subset_list* left_known_facets;
  struct subset_list* right_known_facets;

  struct partial_alloc first_alloc;
  struct partial_alloc left_alloc;
  struct partial_alloc right_alloc;
  
  struct sch_ch_prob left_scp;
  struct sch_ch_prob right_scp;

  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;
    
  if (my_scp->time_remaining == 0.0) {
    return zero_partial_alloc(&my_scp->cee);
  }

  stu_subset = nullset(nst);
  sch_subset = nullset(nsc);

  /* printf("We are about to enter time_rem.\n"); */
      
  double end_time = time_rem_after_first_gmc_eq(my_scp, level, target_level, depth,
						&stu_subset, &sch_subset,
						overallocated_schools,
						observed_overallocated_sets,
						known_facets,
						exit_status);

  /* printf("We just left time_rem.\n"); */

  if (*exit_status == 1) {
    
    struct index index_of_sch_subset = index_of_subset(&sch_subset);
    
    if (list_contains_index(known_facets,&index_of_sch_subset)) {
      printf("Somehow we dragged up something already seen, which is not OK.\n");
      exit(0);
    }
    destroy_index(index_of_sch_subset);
      
    copy_subset(&sch_subset,new_critical_set);
    
    first_alloc = zero_partial_alloc(&(my_scp->cee));

    destroy_subset(stu_subset);
    destroy_subset(sch_subset);
      
    return first_alloc;
  }

  if (*exit_status == 2) {
    
    first_alloc = zero_partial_alloc(&(my_scp->cee));

    destroy_subset(stu_subset);
    destroy_subset(sch_subset);

    return first_alloc;
  }
  
  if (new_critical_set->subset_size != 0) {
    printf("We got past the exit points with a nonempty new_critical_set.\n");
    exit(0);
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
      left_overallocated = nullset(sch_subset.subset_size);
      left_new_critical_set = nullset(sch_subset.subset_size);

      left_observed_overallocated_sets = reduced_subset_list(observed_overallocated_sets,
							     &sch_subset);
      
      left_known_facets = reduced_subset_list(known_facets,&sch_subset);
      
      left_alloc = GCPS_schools_solver(&left_scp, level + 1, target_level, depth,
				       &left_overallocated, &left_new_critical_set,
				       left_observed_overallocated_sets, left_known_facets,
				       exit_status);

      if (*exit_status == 1) {	
	for (j = 1; j <= sch_subset.subset_size; j++) {
	  if (left_new_critical_set.indicator[j-1] == 1) {
	    new_critical_set->subset_size++;
	    new_critical_set->indicator[left_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      if (*exit_status == 2) {	
	for (j = 1; j <= sch_subset.subset_size; j++) {
	  if (left_overallocated.indicator[j-1] == 1) {
	    overallocated_schools->subset_size++;
	    overallocated_schools->indicator[left_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      increment_partial_alloc(&first_alloc, &left_alloc, &left_stu_index, &left_sch_index);

      destroy_subset_list(left_known_facets);
      destroy_subset(left_overallocated);
      destroy_subset(left_new_critical_set);
      destroy_partial_alloc(left_alloc);
      destroy_subset_list(left_observed_overallocated_sets);
    }
    
    if (*exit_status == 0 && stu_compl.subset_size > 0) {      
      right_overallocated = nullset(sch_compl.subset_size);
      right_new_critical_set = nullset(sch_compl.subset_size);

      right_observed_overallocated_sets = reduced_subset_list(observed_overallocated_sets,
							      &sch_compl);
      
      right_known_facets = reduced_subset_list(known_facets,&sch_compl);
      
      right_alloc = GCPS_schools_solver(&right_scp, level + 1, target_level, depth,
					&right_overallocated, &right_new_critical_set,
					right_observed_overallocated_sets, right_known_facets,
					exit_status);

      if (*exit_status == 1) {	
	for (j = 1; j <= sch_compl.subset_size; j++) {
	  if (right_new_critical_set.indicator[j-1] == 1) {
	    new_critical_set->subset_size++;
	    new_critical_set->indicator[right_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
            
      if (*exit_status == 2) {	
	for (j = 1; j <= sch_compl.subset_size; j++) {
	  if (right_overallocated.indicator[j-1] == 1) {
	    overallocated_schools->subset_size++;
	    overallocated_schools->indicator[right_sch_index.indices[j-1]-1] = 1;
	  }
	}
      }
      
      increment_partial_alloc(&first_alloc, &right_alloc, &right_stu_index, &right_sch_index);

      destroy_subset_list(right_known_facets);
      destroy_subset(right_overallocated);
      destroy_subset(right_new_critical_set);
      destroy_partial_alloc(right_alloc);
      destroy_subset_list(right_observed_overallocated_sets);
    }

    if (stu_subset.subset_size > 0) {      
      destroy_sch_ch_prob(left_scp);
    }  
      
    if (stu_compl.subset_size > 0) {
      destroy_sch_ch_prob(right_scp);
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
  if (sch_compl.subset_size > 0) {
    destroy_index(right_sch_index);
  }
  
  destroy_subset(stu_subset);
  destroy_subset(sch_subset);
  destroy_subset(stu_compl);
  destroy_subset(sch_compl);
     
  return first_alloc;
}
