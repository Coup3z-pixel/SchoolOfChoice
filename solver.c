#include "solver.h"

struct partial_alloc GCPS_schools_solver_top_level(struct sch_ch_prob* my_scp) {  

  struct sch_ch_prob copy;
  copy_sch_ch_prob(my_scp,&copy);

  struct square_matrix related = related_matrix(&(my_scp->cee));

  struct subset_list* known_facets = initialized_subset_list();
  struct subset_list* found_critical_sets = initialized_subset_list();
  struct subset_list* found_overalloc_sets = initialized_subset_list();

  int depth = 1;

  int* we_found_something = malloc(sizeof(int));
  *we_found_something = 0;

  struct partial_alloc allocation = GCPS_schools_solver(&copy, &related, depth, known_facets,
							found_critical_sets, found_overalloc_sets,
							we_found_something);

  while (!is_empty_list(found_overalloc_sets) || !first_list_contains_second(known_facets,found_critical_sets)) {

    if (*we_found_something == 0) {
      printf("Somehow we are back here.\n");
      exit(0);
    }
    
    destroy_partial_alloc(allocation);
    
    if (first_list_contains_second(known_facets,found_critical_sets)) {
      depth++;
      if (depth >= 2) {
	/* printf("We have reached depth %d.\n",depth); */
	if (depth >= 10) {
	  printf("We have reached depth 10 and are quitting.\n");
	  exit(0);
	}
      }
    }
    else {
      depth = 1;
    }

    *we_found_something = 0;
  
    add_second_list_to_first(known_facets,found_critical_sets);
    
    destroy_subset_list(found_critical_sets);
    found_critical_sets = initialized_subset_list();
    destroy_subset_list(found_overalloc_sets);
    found_overalloc_sets = initialized_subset_list();    
    destroy_sch_ch_prob(copy);
    copy_sch_ch_prob(my_scp,&copy);

    struct subset_list* watch_list = copy_of_subset_list(known_facets);
    
    allocation = GCPS_schools_solver(&copy, &related, depth, watch_list,
				     found_critical_sets, found_overalloc_sets,
				     we_found_something);

    destroy_subset_list(watch_list);
  }

  /* printf("The maximum support size of a facet is %d.\n", maximum_set_size(known_facets)); */

  destroy_sch_ch_prob(copy);
  destroy_square_matrix(related);
  destroy_subset_list(known_facets);
  destroy_subset_list(found_critical_sets);
  destroy_subset_list(found_overalloc_sets);
  free(we_found_something);

  return allocation;
}

struct partial_alloc GCPS_schools_solver(struct sch_ch_prob* my_scp, struct square_matrix* related,
					 int depth, struct subset_list* watch_list,
					 struct subset_list* found_critical_sets,
					 struct subset_list* found_overalloc_sets,
					 int* we_found_something) {
  struct subset overallocated_schools;
  
  struct subset stu_subset;
  struct subset sch_subset;
  struct subset stu_compl;
  struct subset sch_compl;
  struct index left_stu_index;
  struct index left_sch_index;
  struct index right_stu_index;
  struct index right_sch_index;

  struct subset_list* left_found_critical_sets;
  struct subset_list* right_found_critical_sets;

  struct subset_list* left_found_overalloc_sets;
  struct subset_list* right_found_overalloc_sets;

  struct subset_list* left_watch_list;
  struct subset_list* right_watch_list;

  struct partial_alloc first_alloc;
  struct partial_alloc left_alloc;
  struct partial_alloc right_alloc;
  
  struct sch_ch_prob left_scp;
  struct sch_ch_prob right_scp;

  struct square_matrix left_related;
  struct square_matrix right_related;
  
  int nst = my_scp->cee.no_students;
  int nsc = my_scp->cee.no_schools;
    
  if (*we_found_something == 1) {
    
    first_alloc = zero_partial_alloc(&(my_scp->cee));

    return first_alloc;
  }
    
  if (my_scp->time_remaining == 0.0) {
    return zero_partial_alloc(&my_scp->cee);
  }

  stu_subset = nullset(nst);
  sch_subset = nullset(nsc);
  overallocated_schools = nullset(nsc);

  double end_time = time_rem_after_first_gmc_eq(my_scp, related, depth,
						    &stu_subset, &sch_subset,
						    &overallocated_schools,
						    watch_list);
    
  if (overallocated_schools.subset_size > 0) {
    *we_found_something = 1;
    
    struct index index_of_overalloc_subset = index_of_subset(&overallocated_schools);
    add_subset(found_overalloc_sets,&index_of_overalloc_subset); 
    destroy_index(index_of_overalloc_subset);
    
    first_alloc = zero_partial_alloc(&(my_scp->cee));

    destroy_subset(overallocated_schools);
    destroy_subset(stu_subset);
    destroy_subset(sch_subset);

    /* printf("We found an overallocated_schools with %d schools remaining.\n",nsc); */

    return first_alloc;
  }
  
  struct index index_of_sch_subset;
  if (sch_subset.subset_size > 0) {
    index_of_sch_subset = index_of_subset(&sch_subset);
  
    if (!list_contains_index(watch_list,&index_of_sch_subset)) {
      *we_found_something = 1;
      
      add_subset(found_critical_sets,&index_of_sch_subset);

      /* printf("We found a new critical set.\n"); */

      /*
      first_alloc = zero_partial_alloc(&(my_scp->cee));
    
      destroy_index(index_of_sch_subset);
      destroy_subset(overallocated_schools);
      destroy_subset(stu_subset);
      destroy_subset(sch_subset);

      return first_alloc;
      */
    }
    
    destroy_index(index_of_sch_subset);
  }

  destroy_subset(overallocated_schools);

  
  stu_compl = nullset(nst);
  sch_compl = nullset(nsc);
  complement(&stu_subset, &stu_compl);
  complement(&sch_subset, &sch_compl);

  first_alloc =  allocate_until_new_time(my_scp, end_time);

  if (end_time > 0.000001) {
    
    if (stu_compl.subset_size > 0) {

      right_scp = sub_sch_ch_prob(my_scp, &stu_compl, &sch_compl);
      right_related = submatrix(related,&sch_compl);    
      /*      right_overallocated = nullset(sch_compl.subset_size); */
      right_watch_list = reduced_subset_list(watch_list,&sch_compl);
      right_found_critical_sets = initialized_subset_list();
      right_found_overalloc_sets = initialized_subset_list();
      
      right_alloc = GCPS_schools_solver(&right_scp, &right_related, depth,
					right_watch_list, right_found_critical_sets,
					right_found_overalloc_sets,
					we_found_something);

      destroy_subset_list(right_watch_list);
      destroy_square_matrix(right_related);
      destroy_sch_ch_prob(right_scp);
      
      add_subsets_of_subset_to_list(found_critical_sets,&sch_compl,right_found_critical_sets);
      destroy_subset_list(right_found_critical_sets);

      
      add_subsets_of_subset_to_list(found_overalloc_sets,&sch_compl,right_found_overalloc_sets);
      destroy_subset_list(right_found_overalloc_sets);

      right_sch_index = index_of_subset(&sch_compl);      
      right_stu_index = index_of_subset(&stu_compl);      
      
      increment_partial_alloc(&first_alloc, &right_alloc, &right_stu_index, &right_sch_index);
      
      destroy_index(right_stu_index);
      destroy_index(right_sch_index);
      destroy_partial_alloc(right_alloc);      
    }

    if (stu_subset.subset_size > 0) {

      left_scp = sub_sch_ch_prob(my_scp, &stu_subset, &sch_subset);
      left_related = submatrix(related,&sch_subset);
      left_watch_list = reduced_subset_list(watch_list,&sch_subset);
      left_found_critical_sets = initialized_subset_list();
      left_found_overalloc_sets = initialized_subset_list();

      left_alloc = GCPS_schools_solver(&left_scp, &left_related, depth,
				       left_watch_list, left_found_critical_sets,
				       left_found_overalloc_sets,
				       we_found_something);

      destroy_sch_ch_prob(left_scp);
      destroy_square_matrix(left_related);
      destroy_subset_list(left_watch_list);

      add_subsets_of_subset_to_list(found_critical_sets,&sch_subset,left_found_critical_sets);
      destroy_subset_list(left_found_critical_sets);

      /* add_subsets_of_subset_to_list(found_overalloc_sets,&sch_subset,left_found_overalloc_sets);
       */
      destroy_subset_list(left_found_overalloc_sets);

      left_sch_index = index_of_subset(&sch_subset);      
      left_stu_index = index_of_subset(&stu_subset);
      
      increment_partial_alloc(&first_alloc, &left_alloc, &left_stu_index, &left_sch_index);
      
      destroy_index(left_stu_index);      
      destroy_index(left_sch_index);
      destroy_partial_alloc(left_alloc);
    }
  }
  
  destroy_subset(stu_subset);
  destroy_subset(sch_subset);
  destroy_subset(stu_compl);
  destroy_subset(sch_compl);

  return first_alloc;
}
