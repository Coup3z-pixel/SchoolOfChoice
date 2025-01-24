#include "sprsmtrx.h"

int int_entry(struct int_sparse_matrix* mymat, int row_no, int col_no) {
  int probe;

  probe = 1;
  while (probe <= mymat->nos_active_cols[row_no-1] &&
	 mymat->index_of_active_cols[row_no-1][probe-1] != col_no) {
    probe++;
  }

  if (probe > mymat->nos_active_cols[row_no-1]) {
    return 0;
  }
  else {
    return mymat->entries[row_no-1][probe-1];
  }
}

void set_int_entry(struct int_sparse_matrix* mymat, int row_no, int col_no, int val) {
  int probe;

  probe = 1;
  while (probe <= mymat->nos_active_cols[row_no-1] &&
	 mymat->index_of_active_cols[row_no-1][probe-1] != col_no) {
    probe++;
  }

  if (probe <= mymat->nos_active_cols[row_no-1]) {
    mymat->entries[row_no-1][probe-1] = val;
  }
}

void increment_int_entry(struct int_sparse_matrix* mymat, int row_no, int col_no, int incr) {
  int probe;

  probe = 1;
  while (probe <= mymat->nos_active_cols[row_no-1] &&
	 mymat->index_of_active_cols[row_no-1][probe-1] != col_no) {
    probe++;
  }

  if (probe > mymat->nos_active_cols[row_no-1] && incr != 0) {
    fprintf(stderr, "We are incrementing a zero in a int sparse matrix.\n");
    exit(0);
  }

  mymat->entries[row_no-1][probe-1] += incr;
}


void set_dbl_entry(struct dbl_sparse_matrix* mymat, int row_no, int col_no, double val) {
  int probe;
  
  probe = 1;
  while (probe <= mymat->nos_active_cols[row_no-1] &&
	 mymat->index_of_active_cols[row_no-1][probe-1] != col_no) {
    probe++;
  }

  if (probe <= mymat->nos_active_cols[row_no-1]) {

    /*
    if (row_no == 1 && col_no == 2) {
      printf("In set_dbl_entry we are setting the (1,2) entry to %1.3f.\n", val);
    }
    */
  
    mymat->entries[row_no-1][probe-1] = val;
  }
}

double dbl_entry(struct dbl_sparse_matrix* mymat, int row_no, int col_no) {
  int probe;

  probe = 1;
  while (probe <= mymat->nos_active_cols[row_no-1] &&
	 mymat->index_of_active_cols[row_no-1][probe-1] != col_no) {
    probe++;
  }

  if (probe > mymat->nos_active_cols[row_no-1]) {
    return 0.0;
  }
  else {
    /*
  if (row_no == 1 && col_no == 2) {
    printf("In dbl_entry we are returning the (1,2) entry %1.3f.\n",
	   mymat->entries[row_no-1][probe-1]);
  }
    */

    return mymat->entries[row_no-1][probe-1];
  }
}

void increment_dbl_entry(struct dbl_sparse_matrix* mymat, int row_no, int col_no, double incr) {
  int probe;

  probe = 1;
  while (probe <= mymat->nos_active_cols[row_no-1] &&
	 mymat->index_of_active_cols[row_no-1][probe-1] != col_no) {
    probe++;
  }

  if (probe <= mymat->nos_active_cols[row_no-1]) {
    mymat->entries[row_no-1][probe-1] += incr;
  }
}

struct dbl_sparse_matrix copy_of_dbl_sp_mat(struct dbl_sparse_matrix* given) {
  int i, j, no_rows;

  struct dbl_sparse_matrix copy;

  copy.no_rows = given->no_rows;
  copy.no_cols = given->no_cols;

  no_rows = copy.no_rows;

  copy.nos_active_cols = malloc(no_rows * sizeof(int));
  for (i = 1; i <= no_rows; i++) {
    copy.nos_active_cols[i-1] = given->nos_active_cols[i-1];
  }

  copy.index_of_active_cols = malloc(no_rows * sizeof(int*));
  for (i = 1; i <= no_rows; i++) {
    copy.index_of_active_cols[i-1] = malloc(copy.nos_active_cols[i-1] * sizeof(int));
    for (j = 1; j <= copy.nos_active_cols[i-1]; j++) {
      copy.index_of_active_cols[i-1][j-1] = given->index_of_active_cols[i-1][j-1];
    }
  }

  copy.entries = malloc(no_rows * sizeof(double*));
  for (i = 1; i <= no_rows; i++) {
    copy.entries[i-1] = malloc(copy.nos_active_cols[i-1] * sizeof(double));
    for (j = 1; j <= copy.nos_active_cols[i-1]; j++) {
      copy.entries[i-1][j-1] = given->entries[i-1][j-1];
    }
  }

  return copy;
}

struct int_sparse_matrix copy_int_sparse_matrix(struct int_sparse_matrix* given) {
  int i, k, no_rows;

  struct int_sparse_matrix answer;

  answer.no_rows = given->no_rows;
  answer.no_cols = given->no_cols;

  no_rows = answer.no_rows;

  answer.nos_active_cols = malloc(no_rows * sizeof(int));
  for (i = 1; i <= no_rows; i++) {
    answer.nos_active_cols[i-1] = given->nos_active_cols[i-1];
  }

  answer.index_of_active_cols = malloc(no_rows * sizeof(int*));
  for (i = 1; i <= no_rows; i++) {
    answer.index_of_active_cols[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(int));
    for (k = 1; k <= answer.nos_active_cols[i-1]; k++) {
      answer.index_of_active_cols[i-1][k-1] = given->index_of_active_cols[i-1][k-1];
    }
  }

  answer.entries = malloc(no_rows * sizeof(int*));
  for (i = 1; i <= no_rows; i++) {
    answer.entries[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(int));
    for (k = 1; k <= answer.nos_active_cols[i-1]; k++) {
      answer.entries[i-1][k-1] = 0;
    }
  }

  return answer;
}


int* new_nos_active_cols(struct dbl_sparse_matrix* mydsp,
			 struct subset* J_subset, struct subset* P_subset) {
  int i, k, new_row_no;
  int* new_nos;

  new_nos = malloc(J_subset->subset_size * sizeof(int));
  new_row_no = 0;
  for (i = 1; i <= mydsp->no_rows; i++) {
    if (J_subset->indicator[i-1] == 1) {
      new_row_no++;
      new_nos[new_row_no-1] = 0;
      for (k = 1; k <= mydsp->nos_active_cols[i-1]; k++) {
	if (P_subset->indicator[mydsp->index_of_active_cols[i-1][k-1]-1] == 1) {
	  new_nos[new_row_no-1]++;
	}
      }
    }
  }

  return new_nos;
}

int** new_index_of_active_cols(struct dbl_sparse_matrix* mydsp,
			       struct subset* J_subset, struct subset* P_subset) {
  int i, j, k, new_elt_no, new_no_cols, new_row_no, new_col_no;
  
  int* elt_no_key;
  int** new_index;

  elt_no_key = malloc(P_subset->large_set_size * sizeof(int));
  new_elt_no = 0;
  for (j = 1; j <= P_subset->large_set_size; j++) {
    if (P_subset->indicator[j-1] == 1) {
      new_elt_no++;
      elt_no_key[j-1] = new_elt_no;
    }
  }
  

  new_index = malloc(J_subset->subset_size * sizeof(int*));
  new_row_no = 0;
  for (i = 1; i <= mydsp->no_rows; i++) {
    if (J_subset->indicator[i-1] == 1) {
      new_row_no++;
      new_no_cols = 0;
      new_index[new_row_no-1] = 0;
      for (k = 1; k <= mydsp->nos_active_cols[i-1]; k++) {
	if (P_subset->indicator[mydsp->index_of_active_cols[i-1][k-1]-1] == 1) {
	  new_no_cols++;
	}
      }
      
      new_index[new_row_no-1] = malloc(new_no_cols * sizeof(int));
      new_col_no = 0;
      for (k = 1; k <= mydsp->nos_active_cols[i-1]; k++) {
	if (P_subset->indicator[mydsp->index_of_active_cols[i-1][k-1]-1] == 1) {
	  new_col_no++;
	  new_index[new_row_no-1][new_col_no-1] = elt_no_key[mydsp->index_of_active_cols[i-1][k-1]-1];
	}
      }
    }
  }

  free(elt_no_key);

  return new_index;
}

struct dbl_sparse_matrix zero_dbl_sp_mat_for_subsets(struct dbl_sparse_matrix* mydsp,
						     struct subset* J_subset, struct subset* P_subset) {
  int i, j;
  
  struct dbl_sparse_matrix answer;

  answer.no_rows = J_subset->subset_size;
  answer.no_cols = P_subset->subset_size;
  answer.nos_active_cols = new_nos_active_cols(mydsp, J_subset, P_subset);
  answer.index_of_active_cols = new_index_of_active_cols(mydsp, J_subset, P_subset);

  answer.entries = malloc(answer.no_rows * sizeof(double*));
  for (i = 1; i <= answer.no_rows; i++) {
    answer.entries[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(double));
    for (j = 1; j <= answer.nos_active_cols[i-1]; j++) {
      answer.entries[i-1][j-1] = 0.0;
    }
  }

  return answer;
}

struct int_sparse_matrix zero_int_sp_mat_from_dbl_sp_mat(struct dbl_sparse_matrix* given) {
  int i, j, no_rows;

  struct int_sparse_matrix answer;

  answer.no_rows = given->no_rows;
  answer.no_cols = given->no_cols;

  no_rows = answer.no_rows;

  answer.nos_active_cols = malloc(no_rows * sizeof(int));
  for (i = 1; i <= no_rows; i++) {
    answer.nos_active_cols[i-1] = given->nos_active_cols[i-1];
  }

  answer.index_of_active_cols = malloc(no_rows * sizeof(int*));
  for (i = 1; i <= no_rows; i++) {
    answer.index_of_active_cols[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(int));
    for (j = 1; j <= answer.nos_active_cols[i-1]; j++) {
      answer.index_of_active_cols[i-1][j-1] = given->index_of_active_cols[i-1][j-1];
    }
  }

  answer.entries = malloc(no_rows * sizeof(int*));
  for (i = 1; i <= no_rows; i++) {
    answer.entries[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(int));
    for (j = 1; j <= answer.nos_active_cols[i-1]; j++) {
      answer.entries[i-1][j-1] = 0;
    }
  }

  return answer;
}


void destroy_int_sp_mat(struct int_sparse_matrix* mymat) {
  int i, nst;

  nst = mymat->no_rows;
  free(mymat->nos_active_cols);
  
  for (i = 1; i <= nst; i++) {
    free(mymat->index_of_active_cols[i-1]);
  }
  free(mymat->index_of_active_cols);
  
  for (i = 1; i <= nst; i++) {
    free(mymat->entries[i-1]);
  }
  free(mymat->entries);
}

void destroy_dbl_sp_mat(struct dbl_sparse_matrix* mymat) {
  int i, nst;

  nst = mymat->no_rows;
  free(mymat->nos_active_cols);
  
  for (i = 1; i <= nst; i++) {
    free(mymat->index_of_active_cols[i-1]);
  }
  free(mymat->index_of_active_cols);
  
  for (i = 1; i <= nst; i++) {
    free(mymat->entries[i-1]);
  }
  free(mymat->entries);
}
