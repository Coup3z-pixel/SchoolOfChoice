#include <stdio.h>
#include <stdlib.h> 

struct square_matrix {
  int dimension;
  int** entries;
};

struct square_matrix matrix_of_zeros(int dim) {

  printf("We got into matrix_of_zeros.\n");
  
  int j, k;
  struct square_matrix zero_matrix;
  zero_matrix.dimension = dim;
  zero_matrix.entries = malloc(dim * sizeof(int*));
  for (j = 1; j <= dim; j++) {
    zero_matrix.entries[j-1] = malloc(dim * sizeof(int));
    for (k = 1; k <= dim; k++) {
      zero_matrix.entries[j-1][k-1] = 0;
    }
  }

  printf("We are about to leave matrix_of_zeros.\n");
  
  return zero_matrix;
}


int main(int argc, char const *argv[])
{
  struct square_matrix* my_matrix;

  struct square_matrix temp;

  temp = matrix_of_zeros(3);
  my_matrix = &temp;

  printf("We made my_matrix.\n");

  return 0;
}
