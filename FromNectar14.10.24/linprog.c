#include "linprog.h"

struct tableau auxiliary_lp(struct tableau* given) {
  int i, j, no_b, no_n, n;

  no_b = given->no_basis;
  no_n = given->no_nonbasis;
  
  struct tableau answer;

  answer.no_basis = no_b;
  answer.no_nonbasis = no_n + 1;
  
  n = no_b + no_n + 1;
  
  answer.index = malloc(n * sizeof(int));
  for (i = 1; i <= n-1; i++) {
    answer.index[i-1] = given->index[i-1];
  }
  answer.index[n-1] = n;

  answer.obj_fnc = malloc((no_n + 1 + 1) * sizeof(double));
  for (i = 1; i <= no_n; i++) {
    answer.obj_fnc[i-1] = 0.0;
  }
  answer.obj_fnc[no_n] = -1.0;
  answer.obj_fnc[no_n+1] = 0.0;

  answer.coeff = malloc(no_b * sizeof(double*));
  for (i = 1; i <= no_b; i++) {
    answer.coeff[i-1] = malloc((no_n + 2) * sizeof(double));
    for (j = 1; j <= no_n; j++) {
      answer.coeff[i-1][j-1] = given->coeff[i-1][j-1];
    }
    answer.coeff[i-1][no_n] = -1.0;
    answer.coeff[i-1][no_n + 1] = given->coeff[i-1][no_n];
  }

  return answer;
}

void pivot(struct tableau* given, int row_no, int col_no) {
  int i, j, no_b, no_n;

  printf("\nWe are pivoting with row_no = %i and col_no = %i.\n", row_no, col_no);

  no_b = given->no_basis;
  no_n = given->no_nonbasis;
    
  double pivot_no, obj_col_coeff;
    
  int tmp = given->index[row_no-1];
  given->index[row_no-1] = given->index[no_b + col_no - 1];
  given->index[no_b + col_no - 1] = tmp;
  
  pivot_no = given->coeff[row_no-1][col_no-1];
  obj_col_coeff = given->obj_fnc[col_no-1];

  for (j = 1; j <= no_n; j++) {
    if (j != col_no) {
      given->obj_fnc[j-1] -= obj_col_coeff * given->coeff[row_no-1][j-1]/pivot_no;
    }
  }
  given->obj_fnc[col_no-1] = -obj_col_coeff/pivot_no;
  given->obj_fnc[no_n] += obj_col_coeff * given->coeff[row_no-1][no_n]/pivot_no;

  for (i = 1; i <= no_b; i++) {
    if (i != row_no) {
      for (j = 1; j <= no_n + 1; j++) {
	if (j != col_no) {
	  given->coeff[i-1][j-1] -= given->coeff[i-1][col_no-1]
	    * given->coeff[row_no-1][j-1]/pivot_no;
	}
      }
      given->coeff[i-1][col_no-1] = - given->coeff[i-1][col_no-1]/pivot_no;
    }
  }
  
  for (j = 0; j <= no_n; j++) {
    if (j != col_no-1) {
      given->coeff[row_no-1][j] = given->coeff[row_no-1][j]/pivot_no;
    }
  }

  given->coeff[row_no-1][col_no-1] = 1/pivot_no;

  print_tableau(given);
}

void extract_from_feasible_auxiliary(struct tableau* given, struct tableau* auxiliary) {
  int i, j, k, no_b, no_n, row_no, sum, sub_one;

  no_b = given->no_basis;
  no_n = given->no_nonbasis;

  int* old_index = malloc(no_n * sizeof(int));
  for (j = 1; j <= no_n; j++) {
    old_index[j-1] = given->index[no_b + j-1];
  }

  sum = auxiliary->no_basis + auxiliary->no_nonbasis;
  sub_one = 0;

  for (k = 1; k <= sum; k++) {
    if (auxiliary->index[k-1] == sum) {
      sub_one = 1;
    }
    else {
      given->index[k-sub_one-1] = auxiliary->index[k-1];
    }
  }

  for (i = 1; i <= no_b; i++) {
    sub_one = 0;
    for (j = 1; j <= no_n + 2; j++) {
      if (j <= no_n + 1 && auxiliary->index[no_b + j-1] == sum) {
	sub_one = 1;
      }
      else {
	given->coeff[i-1][j-sub_one-1] = auxiliary->coeff[i-1][j-1];
      }
    }
  }

  double* new_obj_fnc = malloc(auxiliary->no_nonbasis * sizeof(double));
  for (j = 1; j <= no_n; j++) {
    new_obj_fnc[j-1] = 0.0;
  }
  new_obj_fnc[no_n] = given->obj_fnc[no_n];

  for (j = 1; j <= no_n; j++) {
    
    row_no = 0;
    for (i = 1; i <= no_b; i++) {
      if (given->index[i-1] == old_index[j-1]) {
	row_no = i;
      }
    }

    if (row_no > 0) {
      for (k = 1; k <= no_n + 1; k++) {
	new_obj_fnc[k-1] -= given->coeff[row_no-1][k-1] * given->obj_fnc[j-1];
      }
    }
    else {
      for (k = 1; k <= no_n; k++) {
	if (given->index[no_b + k - 1] == old_index[k-1]) {
	  new_obj_fnc[j-1] += given->obj_fnc[k-1];
	}
      }
    }
  }

  for (j = 1; j <= no_n + 1; j++) {
    given->obj_fnc[j-1] = new_obj_fnc[j-1];
  }

  free(old_index);
  free(new_obj_fnc);

  printf("\nAt the end of extract_from_a_feasible_basis we have:\n");
  print_tableau(given);
}

void solve_from_feasible_basis(struct tableau* given) {
  int done, j, row_no, col_no, no_b, no_n, probe;

  double max, min;

  no_b = given->no_basis;
  no_n = given->no_nonbasis;

  done = 0;

  while (!done) {
    done = 1;

    j = 1;
    while (given->obj_fnc[j-1] < 0.000001 && j <= no_n) {
      j++;
    }

    if (j <= no_n) {
      done = 0;
      col_no = j;
      max = given->obj_fnc[j-1];
      while (j < no_n) {
	j++;
	if (given->obj_fnc[j-1] > max) {
	  col_no = j;
	  max = given->obj_fnc[j-1];
	}
      }
    }

    if (!done) {
      row_no = 1;
      while (given->coeff[row_no-1][col_no-1] < 0.000001) {
	row_no++;
      }
      min = given->coeff[row_no-1][no_n]/given->coeff[row_no-1][col_no-1];
      probe = row_no + 1;
      while (probe <= no_b) {
	if (given->coeff[probe-1][col_no-1] > 0.000001) {
	  if (given->coeff[probe-1][no_n]/given->coeff[probe-1][col_no-1] < min) {
	    row_no = probe;
	    min = given->coeff[probe-1][no_n]/given->coeff[probe-1][col_no-1];
	  }
	}
	probe++;
      }
      pivot(given, row_no, col_no);
    }
  }
}

int is_feasible(struct tableau* given) {
  int i;
  
  for (i = 1; i <= given->no_basis; i++) {
    if (given->coeff[i-1][given->no_nonbasis] < -0.000001) {
      return 0;
    }
  }

  return 1;
}

double full_solve(struct tableau* given) {
  int i, row_no;
  double min;
  
  if (!is_feasible(given)) {
    struct tableau auxiliary = auxiliary_lp(given);

    min = 0.0;
    row_no = 1;
    for (i = 1; i <= given->no_basis; i++) {
      if (given->coeff[i-1][given->no_nonbasis] < min - 0.000001) {
	min = given->coeff[i-1][given->no_nonbasis];
	row_no = i;
      }
    }
    pivot(&auxiliary,row_no,given->no_nonbasis+1);
    
    solve_from_feasible_basis(&auxiliary);
    
    extract_from_feasible_auxiliary(given, &auxiliary);
    destroy_tableau(auxiliary);
  }

  solve_from_feasible_basis(given);

  return given->obj_fnc[given->no_nonbasis];
}

struct tableau dan_collins_example() {
  int i;
  struct tableau answer;
  answer.no_basis = 3;
  answer.no_nonbasis = 2;
  answer.index = malloc(5 * sizeof(int));
  answer.index[0] = 3;
  answer.index[1] = 4;
  answer.index[2] = 5;
  answer.index[3] = 1;
  answer.index[4] = 2;

  answer.obj_fnc = malloc(3 * sizeof(double));
  answer.obj_fnc[0] = -2.0;
  answer.obj_fnc[1] = -1.0;
  answer.obj_fnc[2] = 0.0;

  answer.coeff = malloc(3 * sizeof(double*));
  for (i = 1; i <= 3; i++) {
    answer.coeff[i-1]  = malloc(3 * sizeof(double*));
  }
  answer.coeff[0][0] = -1.0;
  answer.coeff[0][1] = 1.0;
  answer.coeff[0][2] = -1.0;
  answer.coeff[1][0] = -1.0;
  answer.coeff[1][1] = -2.0;
  answer.coeff[1][2] = -2.0;
  answer.coeff[2][0] = 0.0;
  answer.coeff[2][1] = 1.0;
  answer.coeff[2][2] = 1.0;

  return answer;
}

struct tableau robert_vanderbei_example() {
  int i;
  struct tableau answer;
  answer.no_basis = 5;
  answer.no_nonbasis = 3;
  answer.index = malloc(8 * sizeof(int));
  answer.index[0] = 4;
  answer.index[1] = 5;
  answer.index[2] = 6;
  answer.index[3] = 7;
  answer.index[4] = 8;
  answer.index[5] = 1;
  answer.index[6] = 2;
  answer.index[7] = 3;

  answer.obj_fnc = malloc(4 * sizeof(double));
  answer.obj_fnc[0] = -1.0;
  answer.obj_fnc[1] = 3.0;
  answer.obj_fnc[2] = -3.0;
  answer.obj_fnc[3] = 0.0;

  answer.coeff = malloc(5 * sizeof(double*));
  for (i = 1; i <= 5; i++) {
    answer.coeff[i-1]  = malloc(4 * sizeof(double*));
  }
  answer.coeff[0][0] = 3.0;
  answer.coeff[0][1] = -1.0;
  answer.coeff[0][2] = -2.0;
  answer.coeff[0][3] = 7.0;
  answer.coeff[1][0] = -2.0;
  answer.coeff[1][1] = -4.0;
  answer.coeff[1][2] = 4.0;
  answer.coeff[1][3] = 3.0;
  answer.coeff[2][0] = 1.0;
  answer.coeff[2][1] = 0.0;
  answer.coeff[2][2] = -2.0;
  answer.coeff[2][3] = 4.0;
  answer.coeff[3][0] = -2.0;
  answer.coeff[3][1] = 2.0;
  answer.coeff[3][2] = 1.0;
  answer.coeff[3][3] = 8.0;
  answer.coeff[4][0] = 3.0;
  answer.coeff[4][1] = 0.0;
  answer.coeff[4][2] = 0.0;
  answer.coeff[4][3] = 5.0;

  return answer;
}

struct tableau copy_tableau(struct tableau* given) {
  int i, j, k, no_b, no_n;

  no_b = given->no_basis;
  no_n = given->no_nonbasis;

  struct tableau imit;
  imit.no_basis = no_b;
  imit.no_nonbasis = no_n;

  imit.index = malloc((no_b + no_n) * sizeof(int));
  for (k = 1; k <= no_b + no_n; k++) {
    imit.index[k-1] = given->index[k-1];
  }

  imit.obj_fnc = malloc((no_n + 1) * sizeof(double));;
  for (j = 1; j <= no_n + 1; j++) {
    imit.obj_fnc[j-1] = given->obj_fnc[j-1];
  }

  imit.coeff = malloc(no_b * sizeof(double*));
  for (i = 1; i <= no_b; i++) {
    imit.coeff[i-1] = malloc((no_n + 1) * sizeof(double));
    for (j = 1; j <= no_n + 1; j++) {
      imit.coeff[i-1][j-1] = given->coeff[i-1][j-1]; 
    }
  }

  return imit;
}

void print_tableau(struct tableau* given) {
  int i, j, no_b, no_n;

  no_b = given->no_basis;
  no_n = given->no_nonbasis;

  printf("obj_fnc =");
  for (j = 1; j <= no_n+1; j++) {
    printf(" ");
    if (given->obj_fnc[j-1] >= -0.000001) {
      if (j > 1) {
	printf("+ ");
      }
      else {
	printf("  ");
      }
    }
    if (given->obj_fnc[j-1] < -0.000001) {
      printf("- ");
    }
    if (j == no_n+1) {
      printf(" ");
    }
    printf("%1.3f", fabs(given->obj_fnc[j-1]));
    if (j <= no_n) {
      printf(" x_%i", given->index[no_b + j - 1]);
    }
  }
  printf("\n");

  printf("------");
  for (j = 0; j <= no_n; j++) {
    printf("------------");
  }
  printf("\n");

  for (i = 1; i <= no_b; i++) {
    printf("      x_%i", given->index[i-1]);
    
    for (j = 1; j <= no_n; j++) {  
      printf(" ");
      if (given->coeff[i-1][j-1] >= -0.000001) {
	printf("+ ");
      }
      if (given->coeff[i-1][j-1] < -0.000001) {
	printf("- ");
      }
      printf("%1.3f x_%i", fabs(given->coeff[i-1][j-1]), given->index[no_b + j - 1]);
    }
    
    if (given->coeff[i-1][no_n] >= -0.000001) {
      printf(" =  ");
    }
    if (given->coeff[i-1][no_n] < -0.000001) {
      printf(" = -");
    }
    printf("%1.3f", fabs(given->coeff[i-1][no_n]));

    printf("\n");
  }
}

void destroy_tableau(struct tableau given) {
  int i;
  
  free(given.index);
  free(given.obj_fnc);
  for (i = 1; i <= given.no_basis; i++) {
    free(given.coeff[i-1]);
  }
  free(given.coeff);
}

