#include "pivot.h"

struct pivot initialized_pivot(int h) {
  struct pivot my_pivot;
  
  my_pivot.h = h;
  my_pivot.schools = malloc((h+1) * sizeof(int));
  my_pivot.students = malloc(h * sizeof(int));

  return my_pivot;
}

struct pivot_list void_pivot_list() {
  struct pivot_list my_list;
  
  my_list.first_node = malloc(sizeof(struct pivot_list_node));
  my_list.first_node->the_pivot = NULL;
  my_list.first_node->next = NULL;
  my_list.last_node = my_list.first_node;

  return my_list;
}

struct pivot* copy_of_pivot(struct pivot* my_pivot) {
  int g, h;
  
  struct pivot* new_pivot = malloc(sizeof(struct pivot));
  h = my_pivot->h;
  new_pivot->h = h;
  new_pivot->schools = malloc((h+1) * sizeof(int));

  new_pivot->students = malloc(h * sizeof(int));
  for (g = 1; g <= h; g++) {
    new_pivot->schools[g-1] = my_pivot->schools[g-1];
    new_pivot->students[g-1] = my_pivot->students[g-1];
  }
  new_pivot->schools[h] = my_pivot->schools[h];

  return new_pivot;
}

int pivot_list_is_void(struct pivot_list* my_list) {
  if (my_list->first_node == my_list->last_node && (my_list->first_node)->next == NULL &&
      (my_list->first_node)->the_pivot == NULL) {
    return 1;
  }
  else {
    return 0;
  }
}

void add_pivot_to_list(struct pivot_list* my_list, struct pivot* new_pivot) {
  struct pivot* new_pvt = copy_of_pivot(new_pivot);
  if (pivot_list_is_void(my_list)) {
    (my_list->first_node)->the_pivot = new_pvt;
  }
  else {
    struct pivot_list_node* new_node = malloc(sizeof(struct pivot_list_node));
    new_node->next = NULL;
    new_node->the_pivot = new_pvt;
    (my_list->last_node)->next = new_node;
    my_list->last_node = new_node;
  }
}

void concatenate_pivot_lists(struct pivot_list* target, struct pivot_list* addition) {
  struct pivot_list_node* probe = addition->first_node;
  if (probe->the_pivot != NULL) {
    add_pivot_to_list(target, probe->the_pivot);
  }
  while (probe-> next != NULL) {
    probe = probe->next;
    add_pivot_to_list(target, probe->the_pivot);
  }
}

int student_qualified_for_school(int i, int j,
				 struct partial_alloc* feasible_guide,
				 int** theta, int* favorites) {
  if (feasible_guide->allocations[i-1][j-1] > 0.000001) {
    return 1;
  }
  else {
    if (theta[i-1][j-1] > 0 && (j != favorites[i-1] || theta[i-1][j-1] > 1)) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

int school_qualified_for_student(int i, int j,
				 struct partial_alloc* feasible_guide,
				 int** theta,
				 struct process_scp* working_cee) {
  if (theta[i-1][j-1] < 0 || feasible_guide->allocations[i-1][j-1] + 0.000001 <
      working_cee->eligible[i-1][j-1] * working_cee->time_remaining) {
    return 1;
  }
  else {
    return 0;
  }
}

int pivot_is_valid(struct pivot* my_pivot, int** theta, int* theta_sums, struct index* alpha,
		   struct partial_alloc* feasible_guide, struct process_scp* working_cee,
		   int* favorites, int* fully_allocated) {
  int g, h, i, j, k;
  h = my_pivot->h;

  if (!fully_allocated[my_pivot->schools[0]-1] || theta_sums[my_pivot->schools[0]-1] <= 0) {
    return 0;
  }
  
  for (g = 1; g <= h; g++) {
    i = my_pivot->students[g-1];
    j = my_pivot->schools[g-1];
    k = my_pivot->schools[g];
    if (!index_has_element(&alpha[i-1],j) || !index_has_element(&alpha[i-1],k) ||
	!student_qualified_for_school(i, j, feasible_guide, theta, favorites) ||
	!school_qualified_for_student(i, k, feasible_guide, theta, working_cee)) {
      return 0;
    }
  }

  if (fully_allocated[my_pivot->schools[h]-1] && theta_sums[my_pivot->schools[h]-1] >= 0) {
    return 0;
  }

  return 1;
}

void execute_pivot(struct pivot* my_pivot, int** theta, int* theta_sums) {
  int g;

  theta_sums[my_pivot->schools[0]-1]--;
  for (g = 1; g <= my_pivot->h; g++) {
    theta[my_pivot->students[g-1]-1][my_pivot->schools[g-1]-1]--;
    theta[my_pivot->students[g-1]-1][my_pivot->schools[g]-1]++;
  }
  theta_sums[my_pivot->schools[my_pivot->h]-1]++;
}

void reuse_prior_pivots(struct pivot_list* old_list, struct pivot_list* new_list,
		       int** theta, int* theta_sums, struct index* alpha,
		       struct partial_alloc* feasible_guide, struct process_scp* working_cee,
			int* favorites, int* fully_allocated, int* no_old_pivots, int* h_sum) {
  struct pivot_list_node* probe = old_list->first_node;
  if (probe->the_pivot != NULL) {
    if (pivot_is_valid(probe->the_pivot, theta, theta_sums, alpha, feasible_guide, working_cee,
		       favorites, fully_allocated)) {
      (*no_old_pivots)++;
      *h_sum += probe->the_pivot->h;
      execute_pivot(probe->the_pivot, theta, theta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
  while (probe->next != NULL) {
    probe = probe->next;
    if (pivot_is_valid(probe->the_pivot, theta, theta_sums, alpha, feasible_guide, working_cee,
		       favorites, fully_allocated)) {
      (*no_old_pivots)++;
      *h_sum += probe->the_pivot->h;
      execute_pivot(probe->the_pivot, theta, theta_sums);
      add_pivot_to_list(new_list, probe->the_pivot);
    }
  }
}

struct pivot_list reduced_pivot_list(struct pivot_list* given_list,
				     struct subset* J_subset, struct subset* P_subset) {
  int g, h, valid;

  int* J_numbers = subset_element_numbers(J_subset);
  int* P_numbers = subset_element_numbers(P_subset);
  
  struct pivot_list new_list = void_pivot_list();

  if (!pivot_list_is_void(given_list)) {
    struct pivot_list_node* probe = given_list->first_node;
    while (probe != NULL) {
      struct pivot* current = probe->the_pivot;
      valid = 1;
      h = current->h;
      for (g = 1; g <= h; g++) {
	if (J_subset->indicator[g-1] == 0 || P_subset->indicator[g-1] == 0) {
	  valid = 0;
	}
      }
      if (P_subset->indicator[h] == 0) {
	valid = 0;
      }
      if (valid) {
	struct pivot new_pivot = initialized_pivot(h);
	for (g = 1; g <= h; g++) {
	  new_pivot.schools[g-1] = P_numbers[current->schools[g-1]-1];
	  new_pivot.students[g-1] = J_numbers[current->students[g-1]-1];
	}
	new_pivot.schools[h] = P_numbers[current->schools[h]-1];
	add_pivot_to_list(&new_list,&new_pivot);
	destroy_pivot(new_pivot);
      }
        
      probe = probe->next;
    }
  }

  free(J_numbers);
  free(P_numbers);

  /*printf("There were %d reducible pivots and %d irreducible pivots.\n", no_valid, no_invalid);*/

  return new_list;
}

struct pivot_list left_reduced_pivot_list(struct pivot_list* given_list,
					  struct subset* J_subset, struct subset* P_subset) {
    struct subset all_schools = fullset(P_subset->large_set_size);
    struct pivot_list left_list = reduced_pivot_list(given_list,J_subset,&all_schools);
    destroy_subset(all_schools);
    return left_list;
}

struct pivot_list right_reduced_pivot_list(struct pivot_list* given_list,
					   struct subset* J_subset, struct subset* P_subset) {

    struct subset J_compl = complement_of_subset(J_subset);
    struct subset P_compl = complement_of_subset(P_subset);

    struct pivot_list right_list = reduced_pivot_list(given_list,&J_compl,&P_compl);

    destroy_subset(J_compl);
    destroy_subset(P_compl);

    return right_list;
}


void destroy_pivot(struct pivot my_pivot) {
  free(my_pivot.schools);
  free(my_pivot.students);
}

void destroy_pivot_ptr(struct pivot* my_pivot) {
  destroy_pivot(*my_pivot);
  free(my_pivot);
}

void destroy_pivot_list_node(struct pivot_list_node my_pivot_list_node) {
  if (my_pivot_list_node.the_pivot != NULL) {
    destroy_pivot(*(my_pivot_list_node.the_pivot));
  }
  free(my_pivot_list_node.the_pivot);
}

void destroy_pivot_list(struct pivot_list my_pivot_list) {  
  while (my_pivot_list.first_node != my_pivot_list.last_node) {
    struct pivot_list_node* new_list_node = my_pivot_list.first_node->next;
    destroy_pivot_list_node(*(my_pivot_list.first_node));
    free(my_pivot_list.first_node);
    my_pivot_list.first_node = new_list_node;
  }

  destroy_pivot_list_node(*(my_pivot_list.first_node));
  free(my_pivot_list.first_node);
}
