#include "getbtheta.h"
  
int_sparse_matrix* NEW_initialize_btheta(process_scp* working_scp, partial_alloc* feasible_guide,
					int* favorites, int* allowed_to_eat) {
  int i, j, done, nst;

  int_sparse_matrix* btheta;
  
  nst = feasible_guide->no_students;

  btheta = malloc(sizeof(int_sparse_matrix));
  *btheta = zero_int_sp_mat_for_process(working_scp);
  
  for (i = 1; i <= nst; i++) {
    if (get_entry(feasible_guide, i, favorites[i-1]) < 0.000000001 && allowed_to_eat[i-1] == 1) {
      
      set_int_entry(btheta, i, favorites[i-1], 1);
    
      done = 0;
      j = 1;
      
      while (!done) {
	if (j != favorites[i-1] && get_entry(feasible_guide, i, j) > 0.000000001) {
	  done = 1;
	  set_int_entry(btheta, i, j, -1);
	}
	else {
	  j++;
	}
      }
      
    }
  }

  return btheta;
}

int* initialize_btheta_sums(int_sparse_matrix* btheta, int nst, int nsc) {
  int i, j;
  int* new_btheta_sums;
  
  new_btheta_sums = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_btheta_sums[j-1] = 0;
    for (i = 1; i <= nst; i++) {
      new_btheta_sums[j-1] += int_entry(btheta, i, j);
    }
  }

  return new_btheta_sums;
}

int i_is_in_J_of_j(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* btheta,
		   int* favorites) {
  
  if (get_entry(feasible_guide, i, j) > 0.000000001) {
    return 1;
  }
  else {
    if (int_entry(btheta, i, j) > 0 && (j != favorites[i-1] || int_entry(btheta, i, j) > 1)) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

int NEW_i_is_in_J_of_j(int i, int j, partial_alloc* feasible_guide, int_sparse_matrix* btheta,
		       int* favorites, int* allowed_to_eat) {
  
  if (get_entry(feasible_guide, i, j) > 0.000000001) {
    return 1;
  }
  else {
    if (int_entry(btheta, i, j) > 0 && (j != favorites[i-1] ||
					int_entry(btheta, i, j) > allowed_to_eat[i-1])) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

element_list* NEW_J_of_j(int j,
			 partial_alloc* feasible_guide,
			 int_sparse_matrix* btheta,
			 int* favorites,
			 int* allowed_to_eat) {
  int i, nst;
  
  element_list* answer;
  answer = NULL;

  nst = feasible_guide->no_students;

  for (i = 1; i <= nst; i++) {
    if (NEW_i_is_in_J_of_j(i, j, feasible_guide, btheta, favorites, allowed_to_eat)) {
      if (answer == NULL) {
	answer = malloc(sizeof(element_list));
	*answer = singleton_element_list(i);
      }
      else {
	add_element_to_element_list(answer, i);
      }
    }
  }

  return answer;
}

element_list* NEW_J_of_P(subset* P, partial_alloc* feasible_guide, int_sparse_matrix* btheta,
			 int* favorites, int* allowed_to_eat) {
  int j, nsc;
  
  element_list* answer;
  element_list* new_set;

  /*  nst = feasible_guide->no_students; */
  nsc = feasible_guide->no_schools;

  answer = NULL;

  for (j = 1; j <= nsc; j++) {
    if (is_element(P, j)) {
      new_set = NEW_J_of_j(j, feasible_guide, btheta, favorites, allowed_to_eat);
      if (new_set != NULL) {
	if (answer == NULL) {
	  answer = new_set;
	}
	else {
	  
	  add_element_list_to_element_list(answer, new_set);
	  destroy_element_list(*new_set);
	  free(new_set);

	}

      }
    }
  }

  return answer;
}

int j_is_in_P_of_i(int i, int j, process_scp* working_scp) {  
  if (is_eligible(working_scp, i, j) == 1) {
    return 1;
  }
  else {
    return 0;
  }
}

subset P_of_i(int i, process_scp* working_scp) {
  int j, nsc;
  
  subset answer;

  nsc = working_scp->no_schools;

  answer = nullset(nsc);

  for (j = 1; j <= nsc; j++) {
    if (j_is_in_P_of_i(i, j, working_scp)) {
      add_element(&answer, j);
    }
  }

  return answer;
}

element_list* NEW_P_of_i(int i, process_scp* working_scp) {
  int j, nsc;
  
  element_list* answer;

  nsc = working_scp->no_schools;

  answer = NULL;

  for (j = 1; j <= nsc; j++) {
    if (j_is_in_P_of_i(i, j, working_scp)) {
      if (answer == NULL) {
	answer = malloc(sizeof(element_list));
	*answer = singleton_element_list(j);
      }
      else {
	add_element_to_element_list(answer, j);
      }
    }
  }

  return answer;
}

subset P_of_J(subset* J, process_scp* working_scp) {
  int i, nst;
  
  subset answer;
  subset new_set;

  nst = working_scp->no_students;

  answer = nullset(nst);

  for (i = 1; i <= nst; i++) {
    if (is_element(J, i)) {      
      new_set = P_of_i(i, working_scp);
      add_subset(&answer, &new_set);
      destroy_subset(new_set);
    }
  }

  return answer;
}

element_list* NEW_next_J_increment(subset* J_subset, subset* P_increment,
				   partial_alloc* feasible_guide,
				   int_sparse_matrix* btheta,
				   int* favorites, int* allowed_to_eat) {
  element_list* answer;
  
  answer = NEW_J_of_P(P_increment, feasible_guide, btheta, favorites, allowed_to_eat);

  remove_subset_from_element_list(answer, J_subset);

  return answer;
}

subset next_P_increment(subset* J_increment, subset* P_subset, process_scp* working_scp) {
  subset answer;
  
  answer = P_of_J(J_increment, working_scp);
  remove_subset(&answer, P_subset);

  return answer;
}

int school_could_be_terminal(int* btheta_sums, int* sch_fully_alloc, int j) {

  /*
  fprintf(stderr, "We have j = %i, sch_fully_alloc[j-1] = %i, and btheta_sums[j-1] = %i.\n",
	  j, sch_fully_alloc[j-1], btheta_sums[j-1]);
  */
  
  if (!sch_fully_alloc[j-1] || btheta_sums[j-1] < 0) {	    
    return 1;
  }

  /*
  fprintf(stderr, "School %i could not be terminal.\n", j);
  */

  return 0;
}

int set_of_schools_has_terminal_element(int* btheta_sums, int* sch_fully_alloc,
					element_list* list) {
  int j, n;

  n = list->no_elements;
  for (j = 1; j <= n; j++) {
    if (school_could_be_terminal(btheta_sums, sch_fully_alloc, list->indices[j-1])) {
      return list->indices[j-1];
    }
  }

  return 0;
}

correspondence* NEW_set_of_schools_for_each_student(element_list* domain, 
						    process_scp* working_scp) {
  int i, n;
  correspondence* answer;
  correspondence_node* probe;

  /* subset image_set; */

  n = domain->no_elements;
  if (n == 0) {
    answer = NULL;
  }
  else {
    answer = malloc(sizeof(correspondence));
    probe = malloc(sizeof(correspondence_node));
    answer->first_node = probe;
    probe->domain_pt = domain->indices[0];
    probe->image = NEW_P_of_i(domain->indices[0], working_scp);
    probe->next = NULL;
    
    for (i = 2; i <= n; i++) {
      probe->next = malloc(sizeof(correspondence_node));
      probe = probe->next;
      probe->domain_pt = domain->indices[i-1];
      probe->image = NEW_P_of_i(domain->indices[i-1], working_scp);
      probe->next = NULL;
    }
  }

  return answer;
}

correspondence* NEW_set_of_studants_for_each_school(element_list* domain,
						    partial_alloc* feasible_guide,
						    int_sparse_matrix* btheta,
						    int* favorites,
						    int* allowed_to_eat) {
  int i, n;
  correspondence* answer;
  correspondence_node* probe;

  n = domain->no_elements;
  
  if (n == 0) {
    answer = NULL;
  }
  else {
    answer = malloc(sizeof(correspondence));
    probe = malloc(sizeof(correspondence_node));
    answer->first_node = probe;
    probe->domain_pt = domain->indices[0];
    probe->image = NEW_J_of_j(domain->indices[0], feasible_guide, btheta,
			      favorites, allowed_to_eat);
    probe->next = NULL;
    for (i = 2; i <= n; i++) {
      probe->next = malloc(sizeof(correspondence_node));
      probe = probe->next;
      probe->domain_pt = domain->indices[i-1];
      probe->image = NEW_J_of_j(domain->indices[i-1], feasible_guide, btheta,
			      favorites, allowed_to_eat);
      probe->next = NULL;
    }
  }

  return answer;
}

correspondence_list* alternating_correspondences(int j_0,
						 partial_alloc* feasible_guide,
						 int_sparse_matrix* btheta,
						 int* btheta_sums,
						 int* favorites,
						 int* allowed_to_eat, 
						 process_scp* working_scp,
						 int* j_h) {
  int done;
  
  correspondence_list* answer;  
  correspondence_list* probe;
  
  element_list* P_domain;
  element_list* J_domain;
  element_list* P_total;
  element_list* J_total;
  
  int* sch_fully_alloc;

  *j_h = 0;
  
  sch_fully_alloc = compute_fully_allocated_schools(working_scp, feasible_guide);

  P_domain = malloc(sizeof(element_list));
  *P_domain = singleton_element_list(j_0);
	
  P_total = copy_of_element_list(P_domain);
  
  answer = malloc(sizeof(correspondence_list));
  answer->next = NULL;
  answer->node_corr = NEW_set_of_studants_for_each_school(P_domain,
							  feasible_guide,
							  btheta,
							  favorites,
					 		  allowed_to_eat);
  
  J_domain = image_of_correspondence(answer->node_corr);
    
  J_total = copy_of_element_list(J_domain);
  
  probe = answer;  
  probe->next = malloc(sizeof(correspondence_list));
  probe = probe->next;
  probe->next = NULL;
  probe->node_corr = NEW_set_of_schools_for_each_student(J_domain, working_scp);
  
  done = 0;

  destroy_element_list_ptr(P_domain);
  P_domain = image_of_correspondence(probe->node_corr);
  if (first_element_list_contained_in_second(P_domain, P_total)) {
    done = 1;
  }
  else {
    remove_element_list_from_element_list(P_domain, P_total);
    add_element_list_to_element_list(P_total, P_domain);
    *j_h = set_of_schools_has_terminal_element(btheta_sums, sch_fully_alloc, P_domain);
    
    if (*j_h > 0) {
      done = 1;
    }
  }
  
  while (!done) {
    probe->next = malloc(sizeof(correspondence_list));
    probe = probe->next;
    probe->next = NULL;
    probe->node_corr = NEW_set_of_studants_for_each_school(P_domain,
							   feasible_guide,
							   btheta,
							   favorites,
							   allowed_to_eat);
    destroy_element_list_ptr(J_domain);
    J_domain = image_of_correspondence(probe->node_corr);

    /*
    fprintf(stderr, "We have J_domain = ");
    fprint_element_list(J_domain);
    fprintf(stderr, ".\n");
    */
	
    if (first_element_list_contained_in_second(J_domain, J_total)) {
      done = 1;

      /*
      fprintf(stderr, "We should be exiting with a critical pair.\n");
      */
      
    }
    else {
      remove_element_list_from_element_list(J_domain, J_total);
      add_element_list_to_element_list(J_total, J_domain);
    
      probe->next = malloc(sizeof(correspondence_list));
      probe = probe->next;
      probe->next = NULL;
      probe->node_corr = NEW_set_of_schools_for_each_student(J_domain, working_scp);

      destroy_element_list_ptr(P_domain);
      P_domain = image_of_correspondence(probe->node_corr);
      if (first_element_list_contained_in_second(P_domain, P_total)) {
	done = 1;

	/*
	fprintf(stderr, "We should be exiting with a critical pair.\n");
	fprint_correspondence_list(answer);
	*/
      
      }
      else {
	remove_element_list_from_element_list(P_domain, P_total);
	add_element_list_to_element_list(P_total, P_domain);
	*j_h = set_of_schools_has_terminal_element(btheta_sums, sch_fully_alloc, P_domain);
	if (*j_h > 0) {
	  done = 1;
	}

	/*
	fprintf(stderr, "We have P_domain = ");
	fprint_element_list(P_domain);
	fprintf(stderr, " and *j_h = %i.\n", *j_h);
	*/

      }	
    }
  }

  destroy_element_list_ptr(J_domain);
  destroy_element_list_ptr(P_domain);
  destroy_element_list_ptr(J_total);
  destroy_element_list_ptr(P_total);

  free(sch_fully_alloc);

  return answer;
}

pivot* NEW_extract_pivot(correspondence_list* list, int j_h) {
  int h, i;
  pivot* my_pivot;

  h = length_of_cl(list)/2;

  my_pivot = malloc(sizeof(pivot));
  *my_pivot = initialized_pivot(h);

  my_pivot->schools[h] = j_h;
  for (i = h; i >= 1; i--) {
    my_pivot->students[i-1] = preimage_of_correspondence(nth_corr_of_cl(list, 2 * i),
							my_pivot->schools[i]);
    my_pivot->schools[i-1] = preimage_of_correspondence(nth_corr_of_cl(list, 2 * i - 1),
							my_pivot->students[i-1]);
  }

  return my_pivot;
}


int NEW_pivot_is_valid(pivot* piv, correspondence_list* list) {
  int h, i;

  h = piv->h;;


  for (i = 0; i <= h-1; i++) {
    if (!y_is_in_image_of_x(nth_corr_of_cl(list,2i+1), piv->schools[i], piv->students[i]) ||
	!y_is_in_image_of_x(nth_corr_of_cl(list,2i+2), piv->students[i], piv->schools[i+1])) {
      return 0;
    }
  }

  return 1;
}

int pivot_is_valid(pivot* my_pivot,
		   int_sparse_matrix* btheta, int* btheta_sums,
		   partial_alloc* feasible_guide,
		   process_scp* working_scp,
		   int* favorites,
		   int* sch_fully_alloc) {
  int g, h, k, l, m;
  h = my_pivot->h;
  
  if (!sch_fully_alloc[my_pivot->schools[0]-1] || btheta_sums[my_pivot->schools[0]-1] <= 0) {
    return 0;
  }
  
  for (g = 1; g <= h; g++) {
    k = my_pivot->schools[g-1];
    l = my_pivot->students[g-1];
    if (!is_eligible(working_scp, l, k) ||
	!i_is_in_J_of_j(l, k, feasible_guide, btheta, favorites)) {
      return 0;
    }
    m = my_pivot->schools[g];
    if (!is_eligible(working_scp, l, m) ||
	!j_is_in_P_of_i(l, m, working_scp)) {
      return 0;
    }
  }

  if (sch_fully_alloc[my_pivot->schools[h]-1] && btheta_sums[my_pivot->schools[h]-1] >= 0) {
    return 0;
  }

  return 1;
}

void reuse_prior_pivots(pivot_list* old_list, pivot_list* new_list,
			int_sparse_matrix* btheta,
			int* btheta_sums, 
			partial_alloc* feasible_guide,
			process_scp* working_scp,
			int* favorites,
			int* sch_fully_alloc,
			run_data* data) {
  pivot_list_node* probe;

  probe = old_list->first_node;
  if (probe->the_pivot != NULL) {
    if (pivot_is_valid(probe->the_pivot, btheta, btheta_sums,
		       feasible_guide, working_scp,
		       favorites, sch_fully_alloc)) {
      data->no_old_pivots++;
      data->h_sum++;
      execute_pivot(probe->the_pivot, btheta, btheta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
  while (probe->next != NULL) {
    probe = probe->next;
    if (pivot_is_valid(probe->the_pivot, btheta, btheta_sums,
		       feasible_guide, working_scp,
		       favorites, sch_fully_alloc)) {
      data->no_old_pivots++;
      data->h_sum++;
      execute_pivot(probe->the_pivot, btheta, btheta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
}

int find_a_pivot_or_crit_pair_for_sch(int j,
				      int_sparse_matrix* btheta,  int* btheta_sums,
				      subset* P_subset, subset* J_subset,
				      process_scp* working_scp,
				      partial_alloc* feasible_guide,
				      int* favorites,
				      int* allowed_to_eat,
				      pivot_list* new_list,
				      run_data* data)  {

  int critical_pair_found, nst, nsc;
  int* j_h;
  pivot* my_NEW_pivot; 
  element_list* J_cl_list;
  element_list* P_cl_list;

  correspondence_list* corr_list;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;
  
  j_h = malloc(sizeof(int));

  corr_list = alternating_correspondences(j,
					  feasible_guide,
					  btheta,
					  btheta_sums,
					  favorites,
					  allowed_to_eat, 
					  working_scp,
					  j_h);

  if (*j_h == 0) {
    critical_pair_found = 1;
  }
  else {
    critical_pair_found = 0;
  }

  J_cl_list = J_list(corr_list);
  P_cl_list = P_list(corr_list);

  destroy_subset(*J_subset);
  destroy_subset(*P_subset);
  *J_subset = subset_of_element_list(J_cl_list, nst);
  *P_subset = subset_of_element_list(P_cl_list, nsc);

  destroy_element_list_ptr(J_cl_list);
  destroy_element_list_ptr(P_cl_list);
  
  if (!critical_pair_found) {
  
    data->no_new_pivots++;

    my_NEW_pivot = NEW_extract_pivot(corr_list, *j_h);
			
    execute_pivot(my_NEW_pivot, btheta, btheta_sums);    
    add_pivot_to_list(new_list, my_NEW_pivot);
    destroy_pivot_ptr(my_NEW_pivot);

    remove_all_elements(J_subset);
    remove_all_elements(P_subset); 
  }

  free(j_h);
  
  destroy_correspondence_list(corr_list); 

  return critical_pair_found;
}


int find_a_btheta_or_crit_pair(int_sparse_matrix* btheta, int* btheta_sums,
			       subset* P_subset, subset* J_subset,
			       process_scp* working_scp,
			       partial_alloc* feasible_guide,
			       int* favorites,
			       int* allowed_to_eat,
			       int* sch_fully_alloc,
			       pivot_list* new_list,
			       run_data* data) {
  int j, nsc, critical_pair_found; 

  critical_pair_found = 0;
  nsc = working_scp->no_schools;
  for (j = 1; j <= nsc && !critical_pair_found; j++) {
    if (sch_fully_alloc[j-1]) {      
      while (!critical_pair_found && btheta_sums[j-1] > 0) {
	
 	critical_pair_found = find_a_pivot_or_crit_pair_for_sch(j, btheta, btheta_sums,
								       P_subset, J_subset,
								       working_scp,
								       feasible_guide, 
								       favorites,
								       allowed_to_eat,
								       new_list,
								       data);
	
      }
    }
  }

  return critical_pair_found;
}
