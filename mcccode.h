#ifndef MCCCODE_h
#define MCCCODE_h

#include "fdamcc.h"

int linear_adjustment_a(process_scp* myscp, partial_alloc* demands,
			double* cutoffs, int debug);

void adjust_cutoffs_to_create_constrained_students_a(process_scp* myscp,
						     partial_alloc* demands,
						     double* cutoffs);

element_list** constrained_students(process_scp* myscp, partial_alloc* demands,
				    double* cutoffs);

int* next_schools(process_scp* myscp, partial_alloc* demands, double* cutoffs);

square_int_matrix displacement_matrix(process_scp* myscp, int* next_sch,
					     element_list** constrained_stu,
					     int no_act_sch, int* rev_lex);

int adjust_cutoffs_empty_T_sub_a(process_scp* myscp, double* cutoffs,
				 partial_alloc* demands, int* next_sch,
				 int* active_schools, element_list** constrained_stu,
				 square_int_matrix* displacements, 
				 int* lexicon, int* rev_lex);

double time_until_new_active_school(process_scp* myscp, partial_alloc* demands,
				    int* active_schools, element_list** constrained_stu,
				    int* next_sch, vector* delta);

double time_until_new_coarse_cutoff(process_scp* myscp, double* cutoffs,
				    vector* delta, int sign);

double time_until_student_constrained_at_school(process_scp* myscp,
						partial_alloc* demands, double* cutoffs,
						vector* delta, int stu, int sch);

double time_until_new_constrained_student(process_scp* myscp, partial_alloc* demands,
					  double* cutoffs, vector* delta, int* next_sch);

void adjust_cutoffs_nonempty_T_sub_a(process_scp* myscp, double* cutoffs,
				     partial_alloc* demands, int* next_sch,
				     square_int_matrix* displacements,
				     subset* T_sub,
				     int* lexicon);

partial_alloc mcca_alloc_plus_coarse_cutoffs(process_scp* myscp, int* coarse);

int compute_new_cutoffs_a(process_scp* myscp, double* cutoffs, int debug);

int demand_of_zero_cutoff_school_above_quota(process_scp* myscp, double* cutoffs);

int student_could_become_constrained_at_next_school(process_scp* myscp,
						    partial_alloc* demands,
						    vector* delta, int stu, int next_sch);

square_int_matrix  big_displ_mat(process_scp* myscp, int* next_sch,
					element_list** constrained_stu);

int there_is_excess_demand(process_scp* myscp, partial_alloc* demands);

int cutoffs_are_below_mcca(process_scp* myscp, double* cutoffs);

partial_alloc mcca_alloc(process_scp* myscp);

/****************** below here is for mccb  *********************/

int linear_adjustment_b(process_scp* myscp, double* cutoffs);

void adjust_cutoffs_to_create_constrained_students_b(process_scp* myscp,
						     partial_alloc* demands,
						     double* cutoffs, int* next_sch);

int* last_schools(process_scp* myscp, partial_alloc* demands);

square_int_matrix replacement_matrix(process_scp* myscp, int* next_sch,
					    partial_alloc* demands, double* cutoffs,
					    int no_act_sch, int* rev_lex);

int adjust_cutoffs_empty_T_sub_b(process_scp* myscp, partial_alloc* demands,
				 square_int_matrix* replacements, 
				 int* lexicon, int* rev_lex, double* cutoffs, int* last_sch);

double time_until_no_consumption_of_a_last_school(process_scp* myscp,
						  partial_alloc* demands, double* cutoffs,
						  int* last_sch, vector* delta);

void adjust_cutoffs_nonempty_T_sub_b(process_scp* myscp,  partial_alloc* demands,
				     square_int_matrix* replacements, subset* T_sub,
				     int* lexicon, double* cutoffs, int* last_sch);

partial_alloc mccb_alloc_plus_coarse_cutoffs(process_scp* myscp, int* coarse);

int compute_new_cutoffs_b(process_scp* myscp, double* cutoffs);

int there_is_deficient_demand(process_scp* myscp, partial_alloc* demands,
			      double* cutoffs);

int a_student_does_not_consume_a_probability_measure(process_scp* myscp,
						     partial_alloc* demands);

square_int_matrix  big_repl_mat(process_scp* myscp, int* last_sch,
				       partial_alloc* demands, double* cutoffs);

partial_alloc mccb_alloc(process_scp* myscp);

/*************************** general functions and utilities *********************/

int* get_active_schools(int nsc, double* cutoffs);

int get_no_act_sch(int nsc, int* active_schools);

int* get_lexicon(int* active_schools, int nsc);

int* get_rev_lex(int* active_schools, int nsc, int no_act_sch);

vector get_delta_empty_T_sub(process_scp* myscp, partial_alloc* demands,
				    int no_act_sch, int* lexicon, int* rev_lex,
				    square_int_matrix* placements);

vector get_delta_nonempty_T_sub(square_int_matrix* placements, subset* T_sub,
				       int nsc, int* lexicon);

void adjust_cutoffs(double* cutoffs, vector* delta, int nsc, double t_star, int sign);

#endif /* MCCCODE_h */
