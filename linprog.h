#ifndef LINPROG_h
#define LINPROG_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct mixed_tableau {
  int no_eq;
  int no_ineq;
  int no_var;
  double* obj_fnc;
  double** eq_coeff;
  double** ineq_coeff;
};

struct standard_tableau {
  int no_constraints;
  int no_variables;
  double* obj_fnc;
  double** coeff;
};

struct standard_tableau standard_tableau_from_mixed_tableau(struct mixed_tableau* given);

void pivot(struct standard_tableau* given, int row_no, int col_no);

void solve_standard_tableau_from_feasible_basis(struct standard_tableau* given);

struct standard_tableau basis_finding_tableau(struct standard_tableau* given);

void solve_basis_finding_tableau(struct standard_tableau* given);

void drive_artificial_variables_out_of_basis(struct standard_tableau* given, int* index);

void solve_standard_tableau(struct standard_tableau* given);

double solve_mixed_tableau(struct mixed_tableau* given);

int is_feasible(struct standard_tableau* given);

struct standard_tableau two_by_two();

struct standard_tableau griffin();

struct standard_tableau dan_collins_example();

struct standard_tableau robert_vanderbei_example();

struct mixed_tableau michel_goemans_example();

struct standard_tableau copy_tableau(struct standard_tableau* given);

void print_mixed_tableau(struct mixed_tableau* given);
  
void print_standard_tableau(struct standard_tableau* given);

void destroy_standard_tableau(struct standard_tableau given);

void destroy_mixed_tableau(struct mixed_tableau given);

#endif /* LINPROG_h */
