#include "linprog.h"

struct standard_tableau standard_tableau_from_mixed_tableau(struct mixed_tableau* given) {
  int i, j, no_con, no_var, gno_eq, gno_ineq, gno_var;
  
  struct standard_tableau answer;

  gno_eq = given->no_eq;
  gno_ineq = given->no_ineq;
  gno_var = given->no_var;

  no_con = gno_eq + gno_ineq;
  no_var = gno_var + gno_ineq;

  answer.no_constraints = no_con;
  answer.no_variables = no_var;

  answer.obj_fnc = malloc((no_var + 1) * sizeof(double));
  for (j = 1; j <= gno_var; j++) {
    answer.obj_fnc[j-1] = given->obj_fnc[j-1];
  }
  for (j = gno_var + 1; j <= no_var; j++) {
    answer.obj_fnc[j-1] = 0.0;
  }
  answer.obj_fnc[no_var] = given->obj_fnc[gno_var];

  answer.coeff = malloc(no_con * sizeof(double*));
  for (i = 1; i <= no_con; i++) {
    answer.coeff[i-1] = malloc((no_var + 1) * sizeof(double));
    for (j = 1; j <= no_var + 1; j++) {
      answer.coeff[i-1][j-1] = 0.0;
    }
  }

  for (i = 1; i <= gno_eq; i++) {
    for (j = 1; j <= gno_var; j++) {
      answer.coeff[i-1][j-1] = given->eq_coeff[i-1][j-1];
    }
    answer.coeff[i-1][no_var] = given->eq_coeff[i-1][gno_var];
  }

  for (i = gno_eq + 1; i <= gno_eq + gno_ineq; i++) {
    for (j = 1; j <= gno_var; j++) {
      answer.coeff[i-1][j-1] = given->ineq_coeff[i - gno_eq - 1][j-1];
    }
    answer.coeff[i-1][gno_var + i - gno_eq - 1] = 1.0;
    answer.coeff[i-1][no_var] = given->ineq_coeff[i - gno_eq - 1][gno_var];
  }

  return answer;
}

void pivot(struct standard_tableau* given, int row_no, int col_no) {
  int i, j, no_b, no_n;

  no_b = given->no_constraints;
  no_n = given->no_variables;
    
  double pivot_no, obj_col_coeff;
  
  pivot_no = given->coeff[row_no-1][col_no-1];
  obj_col_coeff = given->obj_fnc[col_no-1];

  for (j = 1; j <= no_n; j++) {
    if (j != col_no) {
      given->obj_fnc[j-1] -= obj_col_coeff * given->coeff[row_no-1][j-1]/pivot_no;
    }
  }
  given->obj_fnc[col_no-1] = 0.0;
  given->obj_fnc[no_n] += obj_col_coeff * given->coeff[row_no-1][no_n]/pivot_no;

  for (i = 1; i <= no_b; i++) {
    if (i != row_no) {
      given->coeff[i-1][no_n]  -= given->coeff[row_no-1][no_n] * given->coeff[i-1][col_no-1] / pivot_no;
    }
  }
  given->coeff[row_no-1][no_n] /= pivot_no;

  for (i = 1; i <= no_b; i++) {
    if (i != row_no) {
      for (j = 1; j <= no_n; j++) {
	if (j != col_no) {
	  given->coeff[i-1][j-1] -= given->coeff[i-1][col_no-1]
	    * given->coeff[row_no-1][j-1]/pivot_no;
	}
      }
    }
  }

  for (i = 1; i <= no_b; i++) {
    if (i != row_no) {
      given->coeff[i-1][col_no-1] = 0.0;
    }
  }
  
  for (j = 1; j <= no_n; j++) {
    if (j != col_no) {
      given->coeff[row_no-1][j-1] = given->coeff[row_no-1][j-1]/pivot_no;
    }
  }

  given->coeff[row_no-1][col_no-1] = 1.0;

  /*
  printf("After pivoting on row %i and column %i the tableau is:\n", row_no, col_no);
  print_standard_tableau(given);
  */
}

void solve_standard_tableau_from_feasible_basis(struct standard_tableau* given) {
  int done, j, row_no, col_no, no_b, no_n, probe;

  double max, min;

  no_b = given->no_constraints;
  no_n = given->no_variables;

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

      /*
      printf("After pivoting on row %i and column %i the tableau is:\n", row_no, col_no);
      print_standard_tableau(given);
      */
    }
  }
}

struct standard_tableau basis_finding_tableau(struct standard_tableau* given) {
  int i, j, no_b, no_n;

  no_b = given->no_constraints;
  no_n = given->no_variables;
  
  struct standard_tableau answer;

  answer.no_constraints = no_b;
  answer.no_variables = no_n + no_b; 

  answer.obj_fnc = malloc((no_n + no_b + 1) * sizeof(double));
  for (j = 1; j <= no_n; j++) {
    answer.obj_fnc[j-1] = 0.0;
    for (i = 1; i <= no_b; i++) {
      answer.obj_fnc[j-1] += given->coeff[i-1][j-1];
    }
  }
  for (j = no_n + 1; j <= no_n + no_b; j++) {
    answer.obj_fnc[j-1] = 0.0;
  }
  answer.obj_fnc[no_n + no_b] = 0.0;
  for (i = 1; i <= no_b; i++) {
    answer.obj_fnc[no_n + no_b] -= given->coeff[i-1][no_n];
  }

  answer.coeff = malloc(no_b * sizeof(double*));
  for (i = 1; i <= no_b; i++) {
    answer.coeff[i-1] = malloc((no_n + no_b + 1) * sizeof(double));
    for (j = 1; j <= no_n; j++) {
      answer.coeff[i-1][j-1] = given->coeff[i-1][j-1];
    }
    for (j = no_n + 1; j <= no_n + no_b; j++) {
      if (j == no_n + i) {
	answer.coeff[i-1][j-1] = 1.0;
      }
      else {
	answer.coeff[i-1][j-1] = 0.0;
      }
    }
    answer.coeff[i-1][no_n + no_b] = given->coeff[i-1][no_n];
  }

  return answer;
}

void solve_basis_finding_tableau(struct standard_tableau* given) {
  int done, i, j, row_no, col_no, no_b, no_n, probe;

  double max, min;

  int* index;

  no_b = given->no_constraints;
  no_n = given->no_variables;

  index = malloc(no_b * sizeof(int));
  for (i = 1; i <= no_b; i++) {
    index[i-1] = no_n + i;
  }

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
      index[row_no-1] = col_no;
    }
  }

  drive_artificial_variables_out_of_basis(given, index);
  free(index);
}

void drive_artificial_variables_out_of_basis(struct standard_tableau* given, int* index) {
  int i, j, no_b, no_n, done;

  no_b = given->no_constraints;
  no_n = given->no_variables - given->no_constraints;

  done = 0;

  while (!done) {
    done = 1;
    i = 1;
    while (i <= no_b && done) {
      if (index[i-1] > no_n) {
	done = 0;
	j = 1;
	while (fabs(given->coeff[i-1][j-1]) < 0.000001) {
	  j++;
	}

	/*
	printf("We are pivoting in drive_articial_variables_our_of_basis.\n");
	*/
	pivot(given, i, j);
	index[i-1] = j;
      }
      else {
	i++;
      }
    }
  }
}

void solve_standard_tableau(struct standard_tableau* given) {
  int i, j, no_b, no_n;

  no_b = given->no_constraints;
  no_n = given->no_variables;
  
  struct standard_tableau finding_basis_tableau;

  finding_basis_tableau = basis_finding_tableau(given);

  /*
  printf("The initial finding_basis_tableau is:\n");
  print_standard_tableau(&finding_basis_tableau);
  */

  solve_basis_finding_tableau(&finding_basis_tableau);

  /*
  printf("After finding the basis, finding_basis_tableau is:\n");
  print_standard_tableau(&finding_basis_tableau);
  */

  for (i = 1; i <= no_b; i++) {
    for (j = 1; j <= no_n; j++) {
      given->coeff[i-1][j-1] = finding_basis_tableau.coeff[i-1][j-1];
    }
    given->coeff[i-1][no_n] = finding_basis_tableau.coeff[i-1][no_b + no_n];
  }

  /*
  printf("After changing the constraints the given tableau is:\n");
  print_standard_tableau(given);
  */

  solve_standard_tableau_from_feasible_basis(given);

  destroy_standard_tableau(finding_basis_tableau);
}

double solve_mixed_tableau(struct mixed_tableau* given) {
  double answer;

  struct standard_tableau std_tableau = standard_tableau_from_mixed_tableau(given);

  /*
  printf("The derived standard_tableau is:\n");
  print_standard_tableau(&std_tableau);
  */

  solve_standard_tableau(&std_tableau);

  /*
  printf("After solving the derived standard_tableau is:\n");
  print_standard_tableau(&std_tableau);
  */

  answer = std_tableau.obj_fnc[std_tableau.no_variables];

  destroy_standard_tableau(std_tableau);

  return answer;
}

int is_feasible(struct standard_tableau* given) {
  int i;
  
  for (i = 1; i <= given->no_constraints; i++) {
    if (given->coeff[i-1][given->no_variables] < -0.000001) {
      return 0;
    }
  }

  return 1;
}

struct standard_tableau two_by_two() {
  int i;
  struct standard_tableau answer;
  answer.no_constraints = 2;
  answer.no_variables = 2;

  answer.obj_fnc = malloc(3 * sizeof(double));
  answer.obj_fnc[0] = 1.0;
  answer.obj_fnc[1] = 1.0;
  answer.obj_fnc[2] = 0.0;

  answer.coeff = malloc(3 * sizeof(double*));
  for (i = 1; i <= 2; i++) {
    answer.coeff[i-1]  = malloc(3 * sizeof(double*));
  }
  answer.coeff[0][0] = 2.0;
  answer.coeff[0][1] = 1.0;
  answer.coeff[0][2] = 1.0;
  answer.coeff[1][0] = 1.0;
  answer.coeff[1][1] = 2.0;
  answer.coeff[1][2] = 1.0;

  return answer;
}

struct standard_tableau griffin() {
  int i;
  struct standard_tableau answer;
  answer.no_constraints = 2;
  answer.no_variables = 2;

  answer.obj_fnc = malloc(3 * sizeof(double));
  answer.obj_fnc[0] = -1.0;
  answer.obj_fnc[1] = -2.0;
  answer.obj_fnc[2] = 0.0;

  answer.coeff = malloc(3 * sizeof(double*));
  for (i = 1; i <= 2; i++) {
    answer.coeff[i-1]  = malloc(3 * sizeof(double*));
  }
  answer.coeff[0][0] = -1.0;
  answer.coeff[0][1] = -2.0;
  answer.coeff[0][2] = -12.0;
  answer.coeff[1][0] = -2.0;
  answer.coeff[1][1] = -3.0;
  answer.coeff[1][2] = -20.0;

  return answer;
}

struct standard_tableau dan_collins_example() {
  int i;
  struct standard_tableau answer;
  answer.no_constraints = 3;
  answer.no_variables = 2;

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

struct standard_tableau robert_vanderbei_example() {
  int i;
  struct standard_tableau answer;
  answer.no_constraints = 5;
  answer.no_variables = 3;

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

struct mixed_tableau michel_goemans_example() {
  int i;
  
  struct mixed_tableau answer;

  answer.no_eq = 0;
  answer.no_ineq = 3;
  answer.no_var = 3;

  answer.obj_fnc = malloc(4 * sizeof(double));
  answer.obj_fnc[0] = 20.0;
  answer.obj_fnc[1] = 16.0;
  answer.obj_fnc[2] = 12.0;
  answer.obj_fnc[3] = 10.0;

  answer.eq_coeff = malloc(0 * sizeof(double*));
  
  answer.ineq_coeff = malloc(3 * sizeof(double*));
  for (i = 1; i <= 3; i++) {
    answer.ineq_coeff[i-1] = malloc(4 * sizeof(double));
  }
  answer.ineq_coeff[0][0] = 1.0;
  answer.ineq_coeff[0][1] = 0.0;
  answer.ineq_coeff[0][2] = 0.0;
  answer.ineq_coeff[0][3] = 4.0;
  
  answer.ineq_coeff[1][0] = 2.0;
  answer.ineq_coeff[1][1] = 1.0;
  answer.ineq_coeff[1][2] = 1.0;
  answer.ineq_coeff[1][3] = 10.0;
  
  answer.ineq_coeff[2][0] = 2.0;
  answer.ineq_coeff[2][1] = 2.0;
  answer.ineq_coeff[2][2] = 1.0;
  answer.ineq_coeff[2][3] = 16.0;

  return answer;
}

struct standard_tableau copy_tableau(struct standard_tableau* given) {
  int i, j, no_b, no_n;

  no_b = given->no_constraints;
  no_n = given->no_variables;

  struct standard_tableau imit;
  imit.no_constraints = no_b;
  imit.no_variables = no_n;

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

void print_tableau_term(int j, double value) {
    if (fabs(value) > 0.000001) {
       printf(" "); 
      if (value >= -0.000001) {
	printf("+");
      }
      if (value < -0.000001) {
	printf("-");
      }
      /* printf(" "); */
      printf("%1.1f", fabs(value));
      printf(" x_%i", j);
    }
    else {
      if (j >= 10) {
	printf(" ");
      }
      printf("         ");
    }
}

void print_mixed_tableau(struct mixed_tableau* given) {
  int i, j, no_var, gno_eq, gno_ineq;

  gno_eq = given->no_eq;
  gno_ineq = given->no_ineq;

  no_var = given->no_var;

  printf("obj_fnc =");
  for (j = 1; j <= no_var; j++) {
    print_tableau_term(j, given->obj_fnc[j-1]);
  }
  printf("   ");
  if (given->obj_fnc[no_var] >= -0.000001) {
    printf("+ ");
  }
  else {
    printf("- ");
  }
  printf("%1.1f", fabs(given->obj_fnc[no_var]));
  printf("\n");

  /*  printf("------"); */
  for (j = 0; j <= no_var; j++) {
    printf("----------");
  }
  printf("\n");

  for (i = 1; i <= gno_eq; i++) {
    printf("         ");
    
    for (j = 1; j <= no_var; j++) {
      print_tableau_term(j, given->eq_coeff[i-1][j-1]);
    }
    
    if (given->eq_coeff[i-1][no_var] >= -0.000001) {
      printf("  =  ");
    }
    if (given->eq_coeff[i-1][no_var] < -0.000001) {
      printf("  = -");
    }
    printf("%1.1f", fabs(given->eq_coeff[i-1][no_var]));

    printf("\n");
  }

  for (i = 1; i <= gno_ineq; i++) {
    printf("         ");
    
    for (j = 1; j <= no_var; j++) {
      print_tableau_term(j,given->ineq_coeff[i-1][j-1]);
    }
    
    if (given->ineq_coeff[i-1][no_var] >= -0.000001) {
      printf(" <=  ");
    }
    if (given->ineq_coeff[i-1][no_var] < -0.000001) {
      printf(" <= -");
    }
    printf("%1.1f", fabs(given->ineq_coeff[i-1][no_var]));

    printf("\n");
  }

  printf("\n");
}

void print_standard_tableau(struct standard_tableau* given) {
  int i, j, no_b, no_n;

  no_b = given->no_constraints;
  no_n = given->no_variables;

  printf("obj_fnc =");
  for (j = 1; j <= no_n; j++) {
    print_tableau_term(j, given->obj_fnc[j-1]);
  }
  
  printf(" ");
  if (given->obj_fnc[no_n] >= -0.000001) {
    printf("+ ");
  }
  if (given->obj_fnc[no_n] < -0.000001) {
    printf("- ");
  }
  printf(" ");
  printf("%1.1f", fabs(given->obj_fnc[no_n]));
    
  printf("\n");

  /*  printf("------"); */
  for (j = 0; j <= no_n; j++) {
    printf("----------");
  }
  printf("\n");

  for (i = 1; i <= no_b; i++) {
    printf("         ");
    
    for (j = 1; j <= no_n; j++) {
      print_tableau_term(j, given->coeff[i-1][j-1]);
      /*
      printf(" ");
      if (given->coeff[i-1][j-1] >= -0.000001) {
	printf("+ ");
      }
      if (given->coeff[i-1][j-1] < -0.000001) {
	printf("- ");
      }
      printf("%1.1f x_%i", fabs(given->coeff[i-1][j-1]), j);
      */
    }
    
    if (given->coeff[i-1][no_n] >= -0.000001) {
      printf(" =  ");
    }
    if (given->coeff[i-1][no_n] < -0.000001) {
      printf(" = -");
    }
    printf("%1.1f", fabs(given->coeff[i-1][no_n]));

    printf("\n");
  }

  printf("\n");
}

void destroy_mixed_tableau(struct mixed_tableau given) {
  int i;

  free(given.obj_fnc);
  for (i = 1; i <= given.no_eq; i++) {
    free(given.eq_coeff[i-1]);
  }
  free(given.eq_coeff);
  for (i = 1; i <= given.no_ineq; i++) {
    free(given.ineq_coeff[i-1]);
  }
  free(given.ineq_coeff);
}

void destroy_standard_tableau(struct standard_tableau given) {
  int i;

  free(given.obj_fnc);
  for (i = 1; i <= given.no_constraints; i++) {
    free(given.coeff[i-1]);
  }
  free(given.coeff);
}

