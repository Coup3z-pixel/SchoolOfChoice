#ifndef LINPROG_h
#define LINPROG_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct tableau {
  int no_basis;
  int no_nonbasis;
  int* index;
  double* obj_fnc;
  double** coeff;
};

struct tableau auxiliary_lp(struct tableau* given);

void pivot(struct tableau* given, int row_no, int col_no);

void extract_from_feasible_auxiliary(struct tableau* given, struct tableau* auxiliary);

void solve_from_feasible_basis(struct tableau* given);

int is_feasible(struct tableau* given);

double full_solve(struct tableau* given);

struct tableau dan_collins_example();

struct tableau robert_vanderbei_example();

struct tableau copy_tableau(struct tableau* given);
  
void print_tableau(struct tableau* given);

void destroy_tableau(struct tableau given);

#endif /* LINPROG_h */
