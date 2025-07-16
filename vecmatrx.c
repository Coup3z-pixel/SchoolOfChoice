#include "vecmatrx.h"


struct vector zero_vector(int dimension) {
  int i;

  struct vector answer;
  answer.dimension = dimension;
  answer.entries = malloc(dimension * sizeof(double));
  for (i = 1; i <= dimension; i++) {
    answer.entries[i-1] = 0.0;
  }

  return answer;
}

void set_vector_entry(struct vector* vec, int index, double entry) {
  vec->entries[index - 1] = entry;
}

struct square_int_matrix zero_int_matrix(int dimension) {
  int i, j;

  struct square_int_matrix answer;
  answer.dimension = dimension;
  answer.entries = malloc(dimension * sizeof(int*));
  for (i = 1; i <= dimension; i++) {
    answer.entries[i-1] = malloc(dimension * sizeof(int));
    for (j = 1; j <= dimension; j++) {
      answer.entries[i-1][j-1] = 0;
    }
  }

  return answer;
}

struct square_matrix zero_matrix(int dimension) {
  int i, j;

  struct square_matrix answer;
  answer.dimension = dimension;
  answer.entries = malloc(dimension * sizeof(double*));
  for (i = 1; i <= dimension; i++) {
    answer.entries[i-1] = malloc(dimension * sizeof(double));
    for (j = 1; j <= dimension; j++) {
      answer.entries[i-1][j-1] = 0.0;
    }
  }

  return answer;
}

void set_matrix_entry(struct square_matrix* mat, int row, int col, double entry) {
  mat->entries[row-1][col-1] = entry;
}

struct square_matrix copy_of_square_matrix(struct square_matrix* mat) {
  int i, j, dim;

  struct square_matrix copy;

  dim = mat->dimension;

  copy = zero_matrix(dim);
  for (i = 1; i <= dim; i++) {
    for (j = 1; j <= dim; j++) {
      copy.entries[i-1][j-1] = mat->entries[i-1][j-1];
    }
  }

  return copy;
}

struct square_matrix transpose(struct square_matrix* mat) {
  int i, j, dim;

  struct square_matrix transpose;

  dim = mat->dimension;

  transpose = zero_matrix(dim);
  for (i = 1; i <= dim; i++) {
    for (j = 1; j <= dim; j++) {
      transpose.entries[i-1][j-1] = mat->entries[j-1][i-1];
    }
  }

  return transpose;
}

struct vector matrix_times_vector(struct square_matrix* mat, struct vector* vec) {
  int i, j;
  struct vector answer;

  answer = zero_vector(vec->dimension);

  for (i = 1; i <= vec->dimension; i++) {
    for (j = 1; j <= vec->dimension; j++) {
      answer.entries[i-1] += mat->entries[i-1][j-1] * vec->entries[j-1];
    }
  }

  return answer;
}

struct vector int_matrix_times_vector(struct square_int_matrix* mat, struct vector* vec) {
  int i, j;
  struct vector answer;

  answer = zero_vector(vec->dimension);

  for (i = 1; i <= vec->dimension; i++) {
    for (j = 1; j <= vec->dimension; j++) {
      answer.entries[i-1] += (double)mat->entries[i-1][j-1] * vec->entries[j-1];
    }
  }

  return answer;
}

void multiply_row_by_factor(struct square_matrix* mat, int row, double factor) {
  int j;

  for (j = 1; j <= mat->dimension; j++) {
    mat->entries[row-1][j-1] *= factor;
  }
}

void swap_rows(struct square_matrix* mat, int row_one, int row_two) {
  int j;
  double swap;

  for (j = 1; j <= mat->dimension; j++) {
    swap = mat->entries[row_one-1][j-1];
    mat->entries[row_one-1][j-1] = mat->entries[row_two-1][j-1];
    mat->entries[row_two-1][j-1] = swap;
  }
}

void add_row_times_factor_to_row(struct square_matrix* mat, double factor, int r_one, int r_two) {
  int j;

  for (j = 1; j <= mat->dimension; j++) {
    mat->entries[r_two-1][j-1] += factor * mat->entries[r_one-1][j-1];
  }
}

struct square_matrix dbl_matrix_from_int_matrix(struct square_int_matrix* mat) {
  int k, l, dim;

  struct square_matrix answer;

  dim = mat->dimension;

  answer.dimension = dim;
  answer.entries = malloc(dim * sizeof(double*));
  for (k = 1; k <= dim; k++) {
    answer.entries[k-1] = malloc(dim * sizeof(double));
    for (l = 1; l <= dim; l++) {
      answer.entries[k-1][l-1] = (double)mat->entries[k-1][l-1];
    }
  }

  return answer;
}

struct square_matrix inverse(struct square_matrix* mat) {
  int i, k, dim;

  double factor;
  
  struct square_matrix answer;
  struct square_matrix copy;

  dim = mat->dimension;

  copy = copy_of_square_matrix(mat);

  answer = zero_matrix(dim);
  for (i = 1; i <= dim; i++) {
    answer.entries[i-1][i-1] = 1.0;
  }

  for (i = 1; i <= dim; i++) {

    /*
    printf("The copy matrix is\n");
    print_square_matrix(&copy);

    printf("The answer matrix is\n");
    print_square_matrix(&answer);
    */
    
    
    k = i;
    while (fabs(copy.entries[i-1][k-1]) < 0.000001) {
      printf("We got in with i = %i, k = %i, and copy.entries[i-1][k-1] = %1.2f.\n", i, k,
	  copy.entries[i-1][k-1]);
      k++;
    }
    if (k > i) {
      swap_rows(&copy, i, k);
      swap_rows(&answer, i, k);
    }
    factor = 1.0/copy.entries[i-1][i-1];
    multiply_row_by_factor(&copy, i, factor);
    multiply_row_by_factor(&answer, i, factor);
    for (k = 1; k <= dim; k++) {
      if (k != i) {
	/* printf("Adding -copy.entries[%i][%i] = %1.2f times row %i from row %i,\n", k, i,
	   -copy.entries[k-1][i-1], i, k); */
	factor = -copy.entries[k-1][i-1];
	add_row_times_factor_to_row(&copy, factor, i, k);
	add_row_times_factor_to_row(&answer, factor, i, k);
      }
    }
  }

  destroy_square_matrix(&copy);

  return answer;
}

int is_WDD(struct square_int_matrix* mat) {
  int i, j, row_sum;

  for (i = 1; i <= mat->dimension; i++) {
    if (abs(mat->entries[i-1][i-1]) == 0) {
      return 0;
    }
    else {
      row_sum = 0;
      for (j = 1; j <= mat->dimension; j++) {
	if (j != i) {
	  row_sum += abs(mat->entries[i-1][j-1]);
	}
      }
      if (row_sum > abs(mat->entries[i-1][i-1])) {
	return 0;
      }
    }
  }

  return 1;
}

int** directed_graph(struct square_int_matrix* mat) {
  int i, j, dim;

  dim = mat->dimension;
  
  int** answer;
  answer = malloc(dim * sizeof(int *));
  for (i = 1; i <= dim; i++) {
    answer[i-1] = malloc(dim * sizeof(int));
    for (j = 1; j <= dim; j++) {
      if (mat->entries[i-1][j-1] != 0) {
	answer[i-1][j-1] = 1;
      }
      else {
	answer[i-1][j-1] = 0;
      }
    }
  }

  return answer;
}

struct subset T_subset(struct square_int_matrix* mat) {
  int i, j, dim, done, little_done, row_sum;
  int** dir_gr;
  struct subset answer;
  
  dim = mat->dimension;

  answer = nullset(dim);

  for (i = 1; i <= dim; i++) {
    row_sum = 0;
    for (j = 1; j <= dim; j++) {
      if (j != i) {
	row_sum += abs(mat->entries[i-1][j-1]);
      }
    }
    if (row_sum == abs(mat->entries[i-1][i-1])) {
      add_element(&answer, i);
    }
  }

  dir_gr = directed_graph(mat);
  
  done = 0;
  while (!done) {
    done = 1;
    for (i = 1; i <= dim; i++) {
      little_done = 0;
      if (is_element(&answer, i)) {
	for (j = 1; j <= dim && !little_done; j++) {
	  if (j != i && dir_gr[i-1][j-1] == 1 && !is_element(&answer,j)) {
	    remove_element(&answer, i);
	    little_done = 1;
	    done = 0;
	  }
	}
      }
    }
  }
  
  for (i = 1; i <= dim; i++) {
    free(dir_gr[i-1]);
  }
  free(dir_gr);

  return answer;
}

struct subset get_component(struct subset* T_subset, int** dir_gr, int seed) {
  int i, j, dim, done;

  dim = T_subset->large_set_size;

  struct subset answer;

  answer = singleton_subset(seed, dim);

  done = 0;
  while (!done) {
    done = 1;
    for (i = 1; i <= dim; i++) {
      if (answer.indicator[i-1] == 1) {
	for (j = 1; j <= dim; j++) {
	  if (T_subset->indicator[j-1] == 1 && answer.indicator[j-1] == 0 &&
	      dir_gr[i-1][j-1] == 1) {
	    add_element(&answer, j);
	    done = 0;
	  }
	}
      }
    }
  }

  return answer;
}

struct subset minimum_T_subset(struct subset* T_subset, int** dir_gr) {
  int i, dim, done;

  struct subset answer;
  struct subset component;

  dim = T_subset->large_set_size;

  answer = copy_of_subset(T_subset);

  done = 0;
  while (!done) {
    done = 1;
    for (i = 1; i <= dim && done == 1; i++) {
      if (answer.indicator[i-1] == 1) {
	component = get_component(&answer, dir_gr, i);
	if (!subsets_are_same(&component, &answer)) {
	  done = 0;
	  copy_subset(&component, &answer);
	}
	destroy_subset(component);
      }
    }
  }

  return answer;  
}

struct subset minimal_T_subset(struct square_int_matrix* mat) {
  int k, dim;
  
  int** dir_gr;
  struct subset big_T_subset;
  struct subset answer;

  dir_gr = directed_graph(mat);
  big_T_subset = T_subset(mat);
  answer = minimum_T_subset(&big_T_subset, dir_gr);

  dim = mat->dimension;
  for (k = 1; k <= dim; k++) {
    free(dir_gr[k-1]);
  }
  free(dir_gr);
  destroy_subset(big_T_subset);

  return answer;
}

struct square_int_matrix sub_mat(struct square_int_matrix* given, struct subset* rows_and_cols) {
  int k, l, dim, little_dim, row_count, col_count;

  dim = given->dimension;
  little_dim = rows_and_cols->subset_size;
  
  struct square_int_matrix answer;

  answer.dimension = rows_and_cols->subset_size;
  answer.entries = malloc(little_dim * sizeof(int*));
  row_count = 0;
  for (k = 1; k <= dim; k++) {
    if (rows_and_cols->indicator[k-1] == 1) {
      row_count++;
      answer.entries[row_count-1] = malloc(little_dim * sizeof(int));
      col_count = 0;
      for (l = 1; l <= dim; l++) {
	if (rows_and_cols->indicator[l-1] == 1) {
	  col_count++;
	  answer.entries[row_count-1][col_count-1] = given->entries[k-1][l-1];
	}
      }
    }
  }

  return answer;
}

struct vector convex_weights(struct square_int_matrix* mat) {
  int j, dim;

  struct square_matrix dbl_mat;
  struct square_matrix dbl_trans;
  struct square_matrix dbl_trans_inv;
  struct vector target;
  struct vector answer;

  dbl_mat = dbl_matrix_from_int_matrix(mat);
  dbl_trans = transpose(&dbl_mat);
  
  dim = mat->dimension;

  for (j = 1; j <= dim; j++) {
    dbl_trans.entries[dim-1][j-1] = 1.0;
  }

  dbl_trans_inv = inverse(&dbl_trans);

  target = zero_vector(dim);
  target.entries[dim-1] = 1.0;

  answer = matrix_times_vector(&dbl_trans_inv, &target);

  destroy_square_matrix(&dbl_mat);
  destroy_square_matrix(&dbl_trans);
  destroy_square_matrix(&dbl_trans_inv);
  destroy_vector(&target);

  return answer;
}

void print_square_matrix(struct square_matrix* mat) {
  int k, l, dim;

  dim = mat->dimension;

  for (k = 1; k <= dim; k++) {
    for (l = 1; l <= dim; l++) {
      if (mat->entries[k-1][l-1] >= 0.0 && mat->entries[k-1][l-1] <= 9.99) {
	fprintf(stderr, " ");
      }
      fprintf(stderr, "%1.2f  ", mat->entries[k-1][l-1]);;
    }
    fprintf(stderr, "\n");
  }
}

void print_vector(struct vector* vec) {
  int i;
  
  fprintf(stderr, "(");
  for (i = 1; i < vec->dimension; i++) {
    fprintf(stderr, "%1.3f, ", vec->entries[i-1]);
  }
  fprintf(stderr, "%1.3f)", vec->entries[vec->dimension-1]);
}

void print_square_int_matrix(struct square_int_matrix* mat) {
  int k, l, dim;

  dim = mat->dimension;

  for (k = 1; k <= dim; k++) {
    for (l = 1; l <= dim; l++) {
      if (mat->entries[k-1][l-1] >= 0 && mat->entries[k-1][l-1] <= 9) {
	fprintf(stderr, " ");
      }
      fprintf(stderr, "%i  ", mat->entries[k-1][l-1]);;
    }
    fprintf(stderr, "\n");
  }
}

void destroy_vector(struct vector* vec) {
  free(vec->entries);
}

void destroy_square_matrix(struct square_matrix* mat) {
  int i;

  for (i = 1; i <= mat->dimension; i++) {
    free(mat->entries[i-1]);
  }
  free(mat->entries);
}

void destroy_square_int_matrix(struct square_int_matrix* mat) {
  int i;

  for (i = 1; i <= mat->dimension; i++) {
    free(mat->entries[i-1]);
  }
  free(mat->entries);
}
