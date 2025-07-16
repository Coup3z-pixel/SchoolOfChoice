#ifndef MCCCODE_h
#define MCCCODE_h

#include "oldmcc.h"

struct partial_alloc mcca_alloc(struct process_scp* myscp);

struct partial_alloc mcca_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse);

int compute_new_cutoffs_a(struct process_scp* myscp, double* cutoffs);

int demand_of_zero_cutoff_school_above_quota(struct process_scp* myscp, double* cutoffs,
					     struct partial_alloc* demands);

int adjust_cutoffs_empty_T_sub(struct process_scp* myscp, struct partial_alloc* demands,
			       struct square_int_matrix* displacements, 
			       int* lexicon, int* rev_lex, double* cutoffs, int* next_sch,
			       int* active_schools, struct index** constrained_stu);

void adjust_cutoffs_nonempty_T_sub_a(struct process_scp* myscp, struct partial_alloc* demands,
				     struct square_int_matrix* displacements,
				     struct subset* T_sub,
				     int* lexicon, double* cutoffs, int* next_sch);

int* next_schools(struct process_scp* myscp, struct partial_alloc* demands, double* cutoffs);

void adjust_cutoffs_to_create_constrained_students_a(struct process_scp* myscp,
						     struct partial_alloc* demands,
						     double* cutoffs);

struct index** constrained_students(struct process_scp* myscp, struct partial_alloc* demands,
				    double* cutoffs);

double time_until_new_coarse_cutoff_a(struct process_scp* myscp, double* cutoffs,
				      struct vector* delta);

int student_could_become_constrained_at_next_school(struct process_scp* myscp,
						    struct partial_alloc* demands,
						    struct vector* delta, int stu, int next_sch);

double time_until_student_constrained_at_school(struct process_scp* myscp,
						struct partial_alloc* demands, double* cutoffs,
						struct vector* delta, int stu, int sch);

double time_until_new_constrained_student(struct process_scp* myscp, struct partial_alloc* demands,
					  double* cutoffs, struct vector* delta, int* next_sch);

double time_until_new_active_school(struct process_scp* myscp, struct partial_alloc* demands,
				    int* active_schools, struct index** constrained_stu,
				    int* next_sch, struct vector* delta);

int* get_active_schools(int nsc, double* cutoffs);

int* get_lexicon(int* active_schools, int nsc);

int* get_rev_lex(int* active_schools, int nsc, int no_act_sch);

double* aggregate_demands(struct process_scp* myscp, struct partial_alloc* demands);

struct square_int_matrix displacement_matrix(struct process_scp* myscp, int* next_sch,
					     struct index** constrained_stu,
					     int no_act_sch, int* rev_lex);

struct square_int_matrix  big_displ_mat(struct process_scp* myscp, int* next_sch,
					struct index** constrained_stu);

int there_is_excess_demand(struct process_scp* myscp, struct partial_alloc* demands);

void sanity_check_cutoffs(struct process_scp* myscp, double* cutoffs);

int cutoffs_are_below_mcca(struct process_scp* myscp, double* cutoffs);

/****************** below here is for mccb  *********************/

struct partial_alloc mccb_alloc(struct process_scp* myscp);

struct partial_alloc mccb_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse);

int compute_new_cutoffs_b(struct process_scp* myscp, double* cutoffs);

int adjust_cutoffs_empty_T_sub_b(struct process_scp* myscp, struct partial_alloc* demands,
				 struct square_int_matrix* replacements, 
				 int* lexicon, int* rev_lex, double* cutoffs, int* last_sch);

void adjust_cutoffs_nonempty_T_sub_b(struct process_scp* myscp,  struct partial_alloc* demands,
				     struct square_int_matrix* replacements, struct subset* T_sub,
				     int* lexicon, double* cutoffs, int* last_sch);

int* last_schools(struct process_scp* myscp, struct partial_alloc* demands);

struct square_int_matrix  big_repl_mat_NEW(struct process_scp* myscp, int* last_sch,
					   struct partial_alloc* demands, double* cutoffs);

struct square_int_matrix replacement_matrix(struct process_scp* myscp, int* next_sch,
					    struct partial_alloc* demands, double* cutoffs,
					    int no_act_sch, int* rev_lex);

void adjust_cutoffs_to_create_constrained_students_b(struct process_scp* myscp,
						     struct partial_alloc* demands,
						     double* cutoffs, int* next_sch);

int there_is_deficient_demand(struct process_scp* myscp, struct partial_alloc* demands,
			      double* cutoffs);

double time_until_new_coarse_cutoff_b(struct process_scp* myscp, double* cutoffs,
				      struct vector* delta);

double time_until_no_consumption_of_a_last_school(struct process_scp* myscp,
						  struct partial_alloc* demands, double* cutoffs,
						  int* last_sch, struct vector* delta);

int a_student_does_not_consume_a_probability_measure(struct process_scp* myscp,
						     struct partial_alloc* demands);

int new_cutoffs_have_new_zero(int nsc, double* cutoffs, double* new_cutoffs);

void adjust_integer_cutoffs_to_slightly_above(int nsc, double* cutoffs);

#endif /* MCCCODE_h */
