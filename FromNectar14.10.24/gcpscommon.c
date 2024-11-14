#include "gcpscommon.h"

struct pivot* extract_pivot(int** theta, struct index* alpha, int o_h, 
			   struct partial_alloc* feasible_guide,
			   struct process_scp* working_scp,
 			   struct index_list* J_increments,
			   struct index_list* P_increments, int* favorites) {
  int g, h, i, j, l, done, current_o, current_i;
  struct pivot* my_pivot;

  h = length_of_index_list(J_increments);

  my_pivot = malloc(sizeof(struct pivot));
  *my_pivot = initialized_pivot(h);

  my_pivot->schools[h] = o_h;
  
  current_o = o_h;
  for (g = h; g >= 1; g--) {
    
    done = 0;
    l = 1;
    while (!done) {
      i = element_of_list_set(J_increments,g,l);      
      if (index_has_element(&(alpha[i-1]),current_o) && 
	  school_qualified_for_student(i, current_o, feasible_guide, theta,
					 working_scp)) {
	done = 1;
	current_i = i;

	my_pivot->students[g-1] = i;
	
      }
      else {	
	l++;
      }
    }
      
    done = 0;
    l = 1;
    while (!done) {
      j = element_of_list_set(P_increments,g,l);
      if (index_has_element(&(alpha[current_i-1]),j) &&
	  student_qualified_for_school(i,j,feasible_guide,theta,favorites)) {
	done = 1;
	current_o = j;

	my_pivot->schools[g-1] = j;
	
      }
      else {
	l++;
      }
    }
  }

  return my_pivot;
}


void next_J_h(struct subset* next_J_subset, struct subset* J_subset, struct subset* P_subset,
	      struct partial_alloc* feasible_guide,
	      struct index* active_schools_index,
	      struct index* omega, int** theta, int* favorites) {
  int i, j, k, l;

  for (k = 1; k <= active_schools_index->no_elements; k++) {
    j = active_schools_index->indices[k-1];
    if (P_subset->indicator[j-1]) {
      for (l = 1; l <= omega[k-1].no_elements; l++) {
	i = omega[k-1].indices[l-1];
	if (!J_subset->indicator[i-1] &&
	    student_qualified_for_school(i,j,feasible_guide,theta,favorites)) {
	  add_element(next_J_subset,i);
	  add_element(J_subset,i);
	}
      }
    }
  }
}

void next_P_h(struct subset* next_P_subset, struct subset* J_subset,
	      struct subset* P_subset, struct partial_alloc* feasible_guide,
	      struct process_scp* working_scp,
	      struct index* alpha, int** theta) {
  int i, j, k;
  
  int nst = feasible_guide->no_students;
  
  for (i = 1; i <= nst; i++) {
    if (J_subset->indicator[i-1]) {
      for (k = 1; k <= alpha[i-1].no_elements; k++) {
	j = alpha[i-1].indices[k-1];
	if (!P_subset->indicator[j-1] &&
	    school_qualified_for_student(i, j, feasible_guide, theta,
					 working_scp)) {
	  add_element(next_P_subset,j);
	  add_element(P_subset,j);
	}
      }
    }
  }
}




/* COMBINATORIC CALCULATIONS */


void get_alpha(struct process_scp* working_scp, struct index* alpha) {
  int i, j, nst, nsc;
    struct subset alpha_subset;
  
  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  for (i = 1; i <= nst; i++) {
    alpha_subset = nullset(nsc);
    for (j = 1; j <= nsc; j++) {
      if (working_scp->eligible[i-1][j-1] == 1 && working_scp->time_remaining > 0.000001 &&
	  working_scp->quotas[j-1] > 0.000001) {
	alpha_subset.indicator[j-1] = 1;
	alpha_subset.subset_size++;
      }
    }

    if (is_nullset(&alpha_subset)) {
      printf("The alpha for agent %d is empty.\n",i);
      exit(0);
    }
    
    alpha[i-1] = index_of_subset(&alpha_subset);
    destroy_subset(alpha_subset);
  }
}

void destroy_alpha_or_omega(struct index* alpha, int nst) {    
    for (int i = 1; i <= nst; i++) {
      destroy_index(alpha[i-1]);
    }
    free(alpha);
}

void destroy_theta(int** theta, int nst) {
  for (int i = 1; i <= nst; i++) {
    free(theta[i-1]);
  }
  free(theta);
}

int* compute_fully_allocated(struct process_scp* working_scp,
			     struct partial_alloc* feasible_guide) {
  int i, j, nst, nsc;
  
  double school_sum;
  int* fully_allocated;

  nst = working_scp->no_students;
  nsc = working_scp->no_schools;

  fully_allocated = malloc(nsc * sizeof(int));
  
  for (j = 1; j <= nsc; j++) {
    school_sum = 0.0;
    for (i = 1; i <= nst; i++) {
      school_sum += feasible_guide->allocations[i-1][j-1];
    }
    if (school_sum > working_scp->quotas[j-1] - 0.000001) {
      fully_allocated[j-1] = 1;
    }
    else {
      fully_allocated[j-1] = 0;
    }
  }

  return fully_allocated;
}

int** initialize_theta(struct partial_alloc* feasible_guide, int* favorites) {
  int i, j, done;
  
  int nst = feasible_guide->no_students;
  int nsc = feasible_guide->no_schools;
  
  int** theta = malloc(nst * sizeof (int*));
  for (i = 1; i <= nst; i++) {
    theta[i-1] = malloc(nsc * sizeof(int));
  }  

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      theta[i-1][j-1] = 0;
    }
  }
  
  for (i = 1; i <= nst; i++) {
    if (feasible_guide->allocations[i-1][favorites[i-1]-1] < 0.000001) {
      theta[i-1][favorites[i-1]-1] = 1;
    }
  }
  
  for (i = 1; i <= nst; i++) {
    if (feasible_guide->allocations[i-1][favorites[i-1]-1] < 0.000001) {
      
      theta[i-1][favorites[i-1]-1] = 1;
    
      done = 0;
      j = 1;
      while (!done) {
		if (j != favorites[i-1] && feasible_guide->allocations[i-1][j-1] > 0.000001) {
	  done = 1;
	  theta[i-1][j-1] = -1;
	}
	else {
	  j++;
	}
      }
    }
  }

  return theta;
}

void print_theta(int** theta, int nst, int nsc) {
  int i, j;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      printf("%i    ", theta[i-1][j-1]);
    }
    printf("\n");
  }
}

int* initialize_theta_sums(int **theta, int nst, int nsc) {
  int i, j;
  int* new_theta_sums;
  
  new_theta_sums = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    new_theta_sums[j-1] = 0;
    for (i = 1; i <= nst; i++) {
      new_theta_sums[j-1] += theta[i-1][j-1];
    }
  }

  return new_theta_sums;
}
