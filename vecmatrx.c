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

void swap_rows(int dimension, double** mat, int row_one, int row_two) {
  int j;
  double swap;

  for (j = 1; j <= dimension; j++) {
    swap = mat[row_one-1][j-1];
    mat[row_one-1][j-1] = mat[row_two-1][j-1];
    mat[row_two-1][j-1] = swap;
  }
}

struct square_matrix inverse(struct square_matrix* mat) {
  int i, j, k, dim;
  
  double** inp;
  double** ans;
  
  struct square_matrix answer;

  dim = mat->dimension;
  
  inp = malloc(dim * sizeof(double*));
  ans = malloc(dim * sizeof(double*));
  for (i = 1; i <= dim; i++) {
    inp[i-1] = malloc(dim * sizeof(double));
    ans[i-1] = malloc(dim * sizeof(double));
    for (j = 1; j <= dim; j++) {
      inp[i-1][j-1] = mat->entries[i-1][j-1];
      ans[i-1][j-1] = 0.0;
    }
    ans[i-1][i-1] = 1.0;
  }

  for (i = 1; i <= dim; i++) {
    k = i;
    while (fabs(inp[i-1][k-1]) < 0.000001) {
      k++;
    }
    if (k > i) {
      swap_rows(dim, inp, i, k);
      swap_rows(dim, ans, i, k);
    }
    for (j = 1; j <= dim; j++) {
      inp[i-1][j-1] *= 1.0/inp[i-1][i-1];
      ans[i-1][j-1] *= 1.0/inp[i-1][i-1];
    }
    for (k = 1; k < dim; k++) {
      if (k != i) {
	for (j = 1; j <= dim; j++) {
	  inp[k-1][j-1] -= inp[k-1][i-1] * inp[i-1][j-1];
	  ans[k-1][j-1] -= inp[k-1][i-1] * ans[i-1][j-1];
	}
      }
    }
  }

  answer.dimension = dim;
  answer.entries = malloc(dim * sizeof(double*));
  for (i = 1; i <= dim; i++) {
    answer.entries[i-1] = malloc(dim * sizeof(double));
    for (j = 1; j <= dim; j++) {
      answer.entries[i-1][j-1] = ans[i-1][j-1];
    }
  }

  for (i = 1; i <= dim; i++) {
    free(inp[i-1]);
    free(ans[i-1]);
  }
  free(inp);
  free(ans);

  return answer;
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
