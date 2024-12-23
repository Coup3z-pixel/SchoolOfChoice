#include "partalloc.h"

double get_entry(struct partial_alloc* alloc, int i, int j) {
  double val;
  val = dbl_entry(&(alloc->sparse), i, j);

  return val;
}

void set_entry(struct partial_alloc* alloc, int i, int j, double val) {
  set_dbl_entry(&(alloc->sparse), i, j, val);
}

void increment_entry(struct partial_alloc* alloc, int i, int j, double incr) {
  increment_dbl_entry(&(alloc->sparse), i, j, incr);
}

int partial_allocs_are_same(struct partial_alloc* first, struct partial_alloc* second) {
  int i, j;

  int nst = first->no_students;
  int nsc = first->no_schools;

  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (fabs(get_entry(first, i, j) - get_entry(second, i, j)) > 0.000001)  {
	return 0;
      }
    }
  }

  return 1;
}

int students_are_fully_allocated(struct partial_alloc* my_alloc) {
  int i, j;
  double sum;

  for (i = 1; i <= my_alloc->no_students; i++) {
    sum = 0.0;
    for (j = 1; j <= my_alloc->no_schools; j++) {
      sum += get_entry(my_alloc, i, j);
    }
    if (sum <= 0.999999 || sum >= 1.000001) {
      return 0;
    }
  }

  return 1;
}

int is_a_feasible_allocation(struct partial_alloc* my_alloc, struct process_scp* my_scp) {
  int i, j;
  double sum;
  
  if (!students_are_fully_allocated(my_alloc)) {
    return 0;
  }

  for (j = 1; j <= my_scp->no_schools; j++) {
    sum = 0.0;
    for (i = 1; i <= my_scp->no_students; i++) {
      sum += get_entry(my_alloc, i, j);
    }
    if (sum > my_scp->quotas[j-1] + 0.000001) {
      return 0;
    }
  }

  return 1;
}

struct partial_alloc zero_alloc_for_process_scp(struct process_scp* my_scp) {
  int nst, nsc;
  
  nst = my_scp->no_students;
  nsc = my_scp->no_schools;
  
  struct partial_alloc answer;
  answer.no_students = nst;
  answer.no_schools = nsc;

  answer.sparse = new_dbl_sp_mat(my_scp);
  
  return answer;
}


double* school_sums(struct partial_alloc* my_alloc) {
  int i, j;
  double* sums = malloc(my_alloc->no_schools * sizeof(double));
  for (j = 1; j <= my_alloc->no_schools; j++) {
    sums[j-1] = 0.0;
    for (i = 1; i <= my_alloc->no_students; i++) {
      sums[j-1] += get_entry(my_alloc, i, j);
    }
  }
  return sums;
}

struct partial_alloc left_sub_process_feasible_guide(struct partial_alloc* feasible_guide,
					    struct subset* J_subset, struct subset* P_subset) {
  int i, j, k, l, m, elt_no, new_nst, new_nsc;

  int* stu_no_key;
  int* sch_no_key;

  new_nst = J_subset->subset_size;
  new_nsc = P_subset->subset_size;

  struct partial_alloc new_guide;

  new_guide.no_students = new_nst;
  new_guide.no_schools = new_nsc;
  
  stu_no_key = malloc(J_subset->subset_size * sizeof(int));
  elt_no = 0;
  for (i = 1; i <= J_subset->large_set_size; i++) {
    if (J_subset->indicator[i-1] == 1) {
      elt_no++;
      stu_no_key[elt_no-1] = i;
    }
  }

  sch_no_key = malloc(P_subset->subset_size * sizeof(int));
  elt_no = 0;
  for (j = 1; j <= P_subset->large_set_size; j++) {
    if (P_subset->indicator[j-1] == 1) {
      elt_no++;
      sch_no_key[elt_no-1] = j;
    }
  }

  new_guide.sparse = zero_dbl_sp_mat_for_subsets(&(feasible_guide->sparse), J_subset, P_subset);

  for (k = 1; k <= new_nst; k++) {
    i = stu_no_key[k-1];

    for (m = 1; m <= new_guide.sparse.nos_active_cols[k-1]; m++) {
      l = new_guide.sparse.index_of_active_cols[k-1][m-1];
      j = sch_no_key[l-1];
      set_dbl_entry(&(new_guide.sparse), k, l, get_entry(feasible_guide, i, j));
    }
  }

  free(stu_no_key);
  free(sch_no_key);

  return new_guide;
}

struct partial_alloc right_sub_process_feasible_guide(struct partial_alloc* feasible_guide,
					  struct subset* J_subset, struct subset* P_subset) {
  struct subset J_compl = complement_of_subset(J_subset);
  struct subset P_compl = complement_of_subset(P_subset);

  struct partial_alloc new_guide = left_sub_process_feasible_guide(feasible_guide,
								   &J_compl, &P_compl);

  destroy_subset(J_compl);
  destroy_subset(P_compl);

  return new_guide;
}

void increment_partial_alloc(struct partial_alloc* base, struct partial_alloc* increment,
			     struct index* stu_index,struct index* sch_index) {
  int i,j;
  
  for (i = 1; i <= increment->no_students; i++) {
    for (j = 1; j <= increment->no_schools; j++) {

      
      increment_entry(base, stu_index->indices[i-1], sch_index->indices[j-1], get_entry(increment, i, j));
      
    }
  }
}


struct partial_alloc copy_of_partial_alloc(struct partial_alloc* given) {
  int nst, nsc;

  nst = given->no_students;
  nsc = given->no_schools;
  
  struct partial_alloc copy;

  copy.no_students = nst;
  copy.no_schools = nsc;

  copy.sparse = copy_of_dbl_sp_mat(&(given->sparse));

  return copy;
}

struct pure_alloc pure_allocation_from_partial(struct partial_alloc* my_alloc) {
  int i, j;
  int nst = my_alloc->no_students;
  int nsc = my_alloc->no_schools;
  
  struct pure_alloc my_pure;
  my_pure.no_students = nst;
  my_pure.no_schools = nsc;

  my_pure.sparse = zero_int_sp_mat_from_dbl_sp_mat(&(my_alloc->sparse));
  
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      if (get_entry(my_alloc, i, j) > 0.99999) {
	set_int_entry(&(my_pure.sparse), i, j, 1);
      }
      else {
	set_int_entry(&(my_pure.sparse), i, j, 0);
      }
    }
  }
  
  return my_pure;
}

int get_pure_entry(struct pure_alloc* alloc, int i, int j)  {
  return int_entry(&(alloc->sparse), i, j);
}

void set_pure_entry(struct pure_alloc* alloc, int i, int j, int val) {
  set_int_entry(&(alloc->sparse), i, j, val);
}

void increment_pure_entry(struct pure_alloc* alloc, int i, int j, int incr) {
  increment_int_entry(&(alloc->sparse), i, j, incr);
}


void print_sparse_partial_alloc(struct partial_alloc* my_alloc) {
  int i, k, l, nst, nsc, sch_no;
  
  nst = my_alloc->no_students;
  nsc = my_alloc->no_schools;
  
  printf("/* This is a sample introductory comment. */\n");

  printf("There are %d students and %d schools\n",nst,nsc);

  printf("The numbers of eligible schools are\n");
  printf("(");
  for (i = 1; i <= nst - 1; i++) {
    printf("%i,", my_alloc->sparse.nos_active_cols[i-1]);
  }
  printf("%i)\n", my_alloc->sparse.nos_active_cols[nst-1]);
  printf("The lists of eligible schools are");
  for (i = 1; i <= nst; i++) {
    printf("\n");
    l = my_alloc->sparse.nos_active_cols[i-1];
    printf("%i: ", i);
    for (k = 1; k <= l - 1; k++) {
      printf("%i, ", my_alloc->sparse.index_of_active_cols[i-1][k-1]);
    }
    printf("%i", my_alloc->sparse.index_of_active_cols[i-1][l-1]);
  }

  printf("\nThe allocations are");
  for (i = 1; i <= my_alloc->no_students; i++) {
    printf("\n%i:",i);
    if (i < 10) {
      printf(" ");
    }
    for (k = 1; k <= my_alloc->sparse.nos_active_cols[i-1]; k++) {
      sch_no = my_alloc->sparse.index_of_active_cols[i-1][k-1];
      if (sch_no < 10) {
	printf(" ");
      }
      printf(" %i: %2.8f", sch_no, fabs(get_entry(my_alloc, i, sch_no)));
    }
  }
  printf("\n");
}

void print_partial_alloc(struct partial_alloc* my_alloc) {
  int i, j, nst, nsc;
  
  nst = my_alloc->no_students;
  nsc = my_alloc->no_schools;
  
  printf("/* This is a sample introductory comment. */\n");

  printf("There are %d students and %d schools\n",nst,nsc);
  
  for (j = 1; j <= my_alloc->no_schools; j++) {
    if (j < 10) {
      printf(" ");
    }
    printf("         %i:", j);
  }
  
  for (i = 1; i <= my_alloc->no_students; i++) {
    printf("\n%i:",i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= my_alloc->no_schools; j++) {
      if (get_entry(my_alloc, i, j) < -0.000001) {
	fprintf(stderr, "We have a negative allocation probability.\n");
	exit(0);
      }
      printf("  %2.8f", fabs(get_entry(my_alloc, i, j)));
    }
  }
  printf("\n");
}

void print_pure_alloc(struct pure_alloc my_pure_alloc) {
  int i, k, done, sch_no, tencount, hundredcount;
  printf("/* This is a sample introductory comment. */\n");

  tencount = 0;
  hundredcount = 0;
    
  for (i = 1; i <= my_pure_alloc.no_students; i++) {
    done = 0;    
    k = 1;
    while (!done) {
      if (my_pure_alloc.sparse.entries[i-1][k-1] == 1) {
	sch_no = my_pure_alloc.sparse.index_of_active_cols[i-1][k-1];
	done = 1;
      }
      else {
	k++;
      }
    }
    if (i < 1000) {
      printf(" ");
    }
    if (i < 100) {
      printf(" ");
    }
    if (i < 10) {
      printf(" ");
    }
    printf("%i",i);
    printf(" -> ");
    if (sch_no < 100) {
      printf(" ");
    }
    if (sch_no < 10) {
      printf(" ");
    }
    printf("%i", sch_no);
    printf(";");
    tencount++;
    if (tencount == 10) {
      printf("\n");
      tencount = 0;
    }
    hundredcount++;
    if (hundredcount == 10) {
      printf("\n");
      hundredcount = 0;
    }
  }
  printf("\n");
}

void destroy_partial_alloc(struct partial_alloc my_alloc) {
  destroy_dbl_sp_mat(&(my_alloc.sparse));
}

void destroy_pure_alloc(struct pure_alloc my_pure_alloc) {
  destroy_int_sp_mat(&(my_pure_alloc.sparse));
}
