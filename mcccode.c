#include "mcccode.h"

int linear_adjustment_a(struct process_scp* myscp, struct partial_alloc* demands,
			double* cutoffs, int debug) {
  int j, nsc, no_act_sch, done;

  int* active_schools;
  int* lexicon;
  int* rev_lex;
  int* next_sch;
  
  struct index** constrained_stu;
  
  struct square_int_matrix displacements;
  struct subset T_sub;

  nsc = myscp->no_schools;

  active_schools = get_active_schools(nsc, cutoffs);
  no_act_sch = get_no_act_sch(nsc, active_schools);
  lexicon = get_lexicon(active_schools, nsc);
  rev_lex = get_rev_lex(active_schools, nsc, no_act_sch);
  
  adjust_cutoffs_to_create_constrained_students_a(myscp, demands, cutoffs);

  constrained_stu = constrained_students(myscp, demands, cutoffs);
  
  next_sch = next_schools(myscp, demands, cutoffs);

  displacements = displacement_matrix(myscp, next_sch, constrained_stu, no_act_sch, rev_lex);
  T_sub = minimal_T_subset(&displacements);

  if (is_nullset(&T_sub)) {

    if (debug) {
      fprintf(stderr, "Adjusting with empty T_sub.\n");
    }
    
    done = adjust_cutoffs_empty_T_sub_a(myscp, cutoffs, demands, next_sch,
					active_schools, constrained_stu, &displacements,
					lexicon, rev_lex);
  }
  else {

    if (debug) {
      fprintf(stderr, "Adjusting with nonempty T_sub.\n");
    }
    
    adjust_cutoffs_nonempty_T_sub_a(myscp, cutoffs, demands, 
				    next_sch, &displacements, &T_sub, lexicon);
    done = 0;
  }

  free(active_schools);
  destroy_square_int_matrix(&displacements);
  free(next_sch);
  for (j = 1; j <= nsc; j++) {
    if (constrained_stu[j-1]  != NULL) {
      destroy_index(*constrained_stu[j-1]);
      free(constrained_stu[j-1]);
    }
  }
  free(constrained_stu);
  destroy_subset(T_sub);
  free(lexicon);
  free(rev_lex);

  return done;
}

void adjust_cutoffs_to_create_constrained_students_a(struct process_scp* myscp,
						   struct partial_alloc* demands,
						   double* cutoffs) {
  int i, j, k, nst, nsc;
  double dem;

  double* min_cutoffs;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  min_cutoffs = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    min_cutoffs[j-1] = -1.0;
  }
  
  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      j = myscp->preferences[i-1][k-1];
      dem = get_entry(demands, i, j);
      if (dem > 0.000001) {
	if (min_cutoffs[j-1] < 0.0) {
	  min_cutoffs[j-1] = (double)get_priority(myscp, i, j) + 1.0 - dem;
	}
	else {
	  min_cutoffs[j-1] = min(min_cutoffs[j-1], (double)get_priority(myscp, i, j) + 1.0 - dem);
	}
      }
    }
  }
  
  for (j = 1; j <= nsc; j++) {
    if (cutoffs[j-1] > 0.000001) {
      cutoffs[j-1] = min_cutoffs[j-1];
    }
  }

  free(min_cutoffs);
}

struct index** constrained_students(struct process_scp* myscp, struct partial_alloc* demands,
					double* cutoffs) {
  int i, j, k, p, f, nst, nsc;
  
  struct index** answer;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  answer = malloc(nsc * sizeof(struct index*));
  for (j = 1; j <= nsc; j++) {
    answer[j-1] = NULL;
  }

  for (i = 1; i <= nst; i++) {
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      j = myscp->preferences[i-1][k-1];
      p = get_priority(myscp, i, j);
      f = (int)floor(cutoffs[j-1]);
      if (p == f && dbl_entry(&(demands->sparse), i, j) > (double)f + 0.999999 - cutoffs[j-1]) {
	add_element_to_index_ptr(&(answer[j-1]),i);
      }
    }
  }

  return answer;
}

int* next_schools(struct process_scp* myscp, struct partial_alloc* demands, double* cutoffs) {
  int i, j, k, p, nst, done, found, no_sch, candidate_rank;
  double c, f;
  
  int* answer;

  nst = myscp->no_students;

  answer = malloc(nst * sizeof(int));

  for (i = 1; i <= nst; i++) {
    no_sch = myscp->no_eligible_schools[i-1];
    if (no_sch == 1) {
      answer[i-1] = 0;      
    }
    else {      
      done = 0;
      for (k = 1; k <= no_sch && !done; k++) {
	j = myscp->preferences[i-1][k-1];
	p = get_priority(myscp, i, j);
	c = cutoffs[j-1];
	f = floor(cutoffs[j-1]);
	
	if (p > (int)f || (p == (int)f && get_entry(demands, i, j) < f + 0.999999 - c)){
	  done = 1;
	  candidate_rank = k;
	  answer[i-1] = j;
	}
      }
      
      found = 0;
      for (k = 1; k < candidate_rank; k++) {
	j = myscp->preferences[i-1][k-1];
	if (get_priority(myscp, i, j) == (int)floor(cutoffs[j-1])) {
	  found = 1;
	}
      }
      if (!found) {
	answer[i-1] = 0;      
      }
    }
  }

  return answer;
}

struct square_int_matrix displacement_matrix(struct process_scp* myscp, int* next_sch,
					     struct index** constrained_stu,
					     int no_act_sch, int* rev_lex) {
  int i, j;

  struct square_int_matrix big_displ;

  struct square_int_matrix answer;
  
  big_displ = big_displ_mat(myscp, next_sch, constrained_stu);
  
  answer = zero_int_matrix(no_act_sch);
  for (i = 1; i <= no_act_sch; i++) {
    for (j = 1; j <= no_act_sch; j++) {
      answer.entries[i-1][j-1] = big_displ.entries[rev_lex[i-1]-1][rev_lex[j-1]-1];
    }
  }

  destroy_square_int_matrix(&big_displ);

  return answer;
}

int adjust_cutoffs_empty_T_sub_a(struct process_scp* myscp, double* cutoffs,
				 struct partial_alloc* demands, int* next_sch,
				 int* active_schools, struct index** constrained_stu,
				 struct square_int_matrix* displacements, 
				 int* lexicon, int* rev_lex) {
  int nsc, no_act_sch;
  double tuncc, tuncs, tunas, t_star;

  struct vector delta;
  
  nsc = myscp->no_schools;
  no_act_sch = displacements->dimension;

  delta = get_delta_empty_T_sub(myscp, demands,	no_act_sch, lexicon, rev_lex, displacements);

  int delta_is_zero = 1;
  for (int j = 1; j <= nsc; j++) {
    if (delta.entries[j-1] > 0.000001) {
      delta_is_zero = 0;
    }
  }
  if (delta_is_zero) {
    fprintf(stderr, "Somehow delta is zero.\n");
    exit(0);
  }

  tuncc = time_until_new_coarse_cutoff(myscp, cutoffs, &delta, 1);  
  tunas = time_until_new_active_school(myscp, demands, active_schools, constrained_stu,
				       next_sch, &delta);
  tuncs = time_until_new_constrained_student(myscp, demands, cutoffs, &delta, next_sch);

  t_star = tuncc;
  if (tunas > 0.0) {
    t_star = min(tunas, t_star);
  }
  if (tuncs > 0.0) {
    t_star = min(tuncs, t_star);
  }
  t_star = min(t_star, 1.0);

  adjust_cutoffs(cutoffs, &delta, nsc, t_star, 1);

  destroy_vector(&delta);

  if (t_star > 0.99999) {    
    return 1;
  }
  else {
    return 0;
  }
}

double time_until_new_active_school(struct process_scp* myscp, struct partial_alloc* demands,
				    int* active_schools, struct index** constrained_stu,
				    int* next_sch, struct vector* delta) {
  int i, j, k, l, nsc, hit;
  double answer, total_demand, time_required;

  double* pressures;

  answer = -1.0;

  nsc = myscp->no_schools;

  pressures = malloc(nsc * sizeof(double));

  for (j = 1; j <= nsc; j++) {
    pressures[j-1] = 0.0;
  }

  for (j = 1; j <= nsc; j++) {
    if (constrained_stu[j-1] != NULL) {
      for (k = 1; k <= constrained_stu[j-1]->no_elements; k++) {
	i = constrained_stu[j-1]->indices[k-1];
	l = next_sch[i-1];
	pressures[l-1] += delta->entries[j-1];
      }
    }
  }
  
  hit = 0;
  for (j = 1; j <= nsc; j++) {
    if (!active_schools[j-1] && pressures[j-1] > 0.000001) {
      total_demand = get_total_demand_for_school(demands, j);
      time_required = (myscp->quotas[j-1] - total_demand)/pressures[j-1];
      if (!hit) {
	hit = 1;
	answer = time_required;
      }
      else {
	answer = min(answer, time_required);
      }
    }
  }

  free(pressures);
  
  return answer;
}

double time_until_new_coarse_cutoff(struct process_scp* myscp, double* cutoffs,
				    struct vector* delta, int sign) {
  int j, nsc, hit;
  double distance, candidate, answer;
  
  nsc = myscp->no_schools;

  hit = 0;
  for (j = 1; j <= nsc; j++) {
    if (delta->entries[j-1] > 0.000001) {
      if (is_integer(cutoffs[j-1])) {
	distance = 1.0;
      }
      else {
	if (sign == 1) {
	  distance = ceil(cutoffs[j-1]) - cutoffs[j-1];
	}
	else {
	  distance = cutoffs[j-1] - floor(cutoffs[j-1]);
	}
      }
      
      candidate = distance/delta->entries[j-1];      
      if (!hit) {
	answer = candidate;
	hit = 1;    
      }
      else {
	answer = min(candidate, answer);
      }
    }
  }

  return answer;
}

double time_until_student_constrained_at_school(struct process_scp* myscp,
						struct partial_alloc* demands, double* cutoffs,
						struct vector* delta, int stu, int sch) {
  int j, k, rank, done;
  double delta_sum, distance;

  done = 0;
  for (k = myscp->no_eligible_schools[stu-1]; !done; k--) {
    if (myscp->preferences[stu-1][k-1] == sch) {
      done = 1;
      rank = k;
    }
  }

  delta_sum = 0.0;
  for (k = rank; k >= 1 ; k--) {
    j = myscp->preferences[stu-1][k-1];
    if (floor(cutoffs[j-1]) <= int_entry(&(myscp->priorities), stu, j)) {
      delta_sum += delta->entries[j-1];
    }
  }

  distance = floor(cutoffs[sch-1]) + 1.0 - cutoffs[sch-1] - get_entry(demands, stu, sch);
  
  return distance/delta_sum;;
}

double time_until_new_constrained_student(struct process_scp* myscp, struct partial_alloc* demands,
					  double* cutoffs, struct vector* delta, int* next_sch) {
  int i, nst, hit;
  double candidate, answer;

  int* stu_could_become_constrained;

  nst = myscp->no_students;

  stu_could_become_constrained = malloc(nst * sizeof(int));
  hit = 0;
  for (i = 1; i <= nst; i++) {
    if (student_could_become_constrained_at_next_school(myscp, demands, delta, i, next_sch[i-1]) &&
	get_priority(myscp, i, next_sch[i-1]) == (int)floor(cutoffs[next_sch[i-1]-1])) {
      hit = 1;
      stu_could_become_constrained[i-1] = 1;
    }
    else {
      stu_could_become_constrained[i-1] = 0;
    }
  }

  if (hit == 0) {
    free(stu_could_become_constrained);
    return -1.0;
  }

  hit = 0;
  for (i = 1; i <= nst; i++) {
    if (stu_could_become_constrained[i-1]) {
      if (hit == 0) {
	
	answer = time_until_student_constrained_at_school(myscp, demands, cutoffs,
							  delta, i, next_sch[i-1]);
	hit = 1;
      }
      else {
	candidate = time_until_student_constrained_at_school(myscp, demands, cutoffs,
							     delta, i, next_sch[i-1]);
	answer = min(candidate, answer);
      }
    }
  }

  free(stu_could_become_constrained);
      
  return answer;
}

void adjust_cutoffs_nonempty_T_sub_a(struct process_scp* myscp,  double* cutoffs,
				     struct partial_alloc* demands, int* next_sch,
				     struct square_int_matrix* displacements, struct subset* T_sub,
				     int* lexicon) {
  int nsc;

  struct vector delta;
  double tuncc, tuncs, t_star;
  
  nsc = myscp->no_schools;

  delta = get_delta_nonempty_T_sub(displacements, T_sub, nsc, lexicon);

  tuncc = time_until_new_coarse_cutoff(myscp, cutoffs, &delta, 1);
  tuncs = time_until_new_constrained_student(myscp, demands, cutoffs, &delta, next_sch);
  if (tuncs < 0.0) {
    t_star = tuncc;
  }
  else  {
    t_star = min(tuncc, tuncs);
  }
  adjust_cutoffs(cutoffs, &delta, nsc, t_star, 1);

  destroy_vector(&delta);
}

struct partial_alloc mcca_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse) {
  int j, nsc, done, debug;
  double* cutoffs;

  debug = 0;

  nsc = myscp->no_schools;

  cutoffs = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    cutoffs[j-1] = 0.0;
  }

  int count = 0;

  if (debug) {
    fprintf(stderr, "The count is %i.\n", count);
  }

  done = 0;
  while (!done) {
    done = compute_new_cutoffs_a(myscp, cutoffs, debug);

    count++;
    
    if (debug) {
      fprintf(stderr, "The count is %i.\n", count);
    }
  }

  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = floor(cutoffs[j-1]);
  }

  if (!cutoffs_are_an_mcc(myscp,cutoffs)) {
    fprintf(stderr, "We failed to produce an mcc.\n");
    exit(0);
  }

  struct partial_alloc answer = compute_demands(myscp, cutoffs);

  free(cutoffs);

  return answer;
}

int compute_new_cutoffs_a(struct process_scp* myscp, double* cutoffs, int debug) {
  int done;

  double excess_sum;
  
  struct partial_alloc demands;

  if (demand_of_zero_cutoff_school_above_quota(myscp, cutoffs)) {

    if (debug) {
      fprintf(stderr, "Demand of some zero cutoff school goes above quota.\n");
    }
    
    naive_increase_of_cutoffs(myscp, cutoffs);    
    return 0;
  }

  excess_sum = sum_of_excesses(myscp,cutoffs);
  if (excess_sum > 1.0) { 

    if (debug) {
      fprintf(stderr, "The excess_sum is %1.6f.\n", excess_sum);
    }
    
    naive_increase_of_cutoffs(myscp, cutoffs);    
    return 0;
  }
  
  demands = compute_demands(myscp, cutoffs);

  if (!there_is_excess_demand(myscp, &demands)) {
    destroy_partial_alloc(demands);
    return 1;
  }

  done = linear_adjustment_a(myscp, &demands, cutoffs, debug);
  
  destroy_partial_alloc(demands);

  return done;
}

int demand_of_zero_cutoff_school_above_quota(struct process_scp* myscp, double* cutoffs) {
  int i, j, nst, nsc;
  double total_demand;

  struct partial_alloc demands;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;
  
  demands = compute_demands(myscp, cutoffs);

  for (j = 1; j <= nsc; j++) {
    total_demand = 0.0;
    for (i = 1; i <= nst; i++) {
      total_demand += get_entry(&demands, i, j);
    }
    if (total_demand > myscp->quotas[j-1] && cutoffs[j-1] < 0.00001) {
      destroy_partial_alloc(demands);
      return 1;
    }
  }

  destroy_partial_alloc(demands);
  return 0;
}

int student_could_become_constrained_at_next_school(struct process_scp* myscp,
						    struct partial_alloc* demands,
						    struct vector* delta, int stu, int next_sch) {
  int j, k, next_sch_rank, found, answer;

  if (next_sch == 0) {
    return 0;
  }

  found = 0;
  for (k = myscp->no_eligible_schools[stu-1]; k >= 1 && !found; k--) {
    if (myscp->preferences[stu-1][k-1] == next_sch) {
      next_sch_rank = k;
      found = 1;
    }
  }

  if (!found) {
    return 0;
  }
  
  answer = 0;
  for (k = next_sch_rank; k >= 1 && answer == 0; k--) {
    j = myscp->preferences[stu-1][k-1];
    if (get_entry(demands, stu, j) > 0 && delta->entries[j-1] > 0.000001) {
      answer = 1;
    }
  }

  return answer;
}

struct square_int_matrix  big_displ_mat(struct process_scp* myscp, int* next_sch,
						  struct index** constrained_stu) {
  int i, j, k, l, m, nst, nsc, no_sch, seen_next;

  struct square_int_matrix answer;
  
  nst = myscp->no_students;  
  nsc = myscp->no_schools;  

  answer = zero_int_matrix(nsc);
  
  for (i = 1; i <= nst; i++) {
    no_sch = myscp->no_eligible_schools[i-1];
    seen_next = 0;
    for (k = 1; k <= no_sch && !seen_next; k++) {
      j = myscp->preferences[i-1][k-1];
      
      if (next_sch[i-1] == j) {
	seen_next = 1;
	for (l = 1; l < k; l++) {
	  m = myscp->preferences[i-1][l-1];
	  if (constrained_stu[m-1] != NULL) {
	    if (index_has_element(constrained_stu[m-1],i)) {
	      answer.entries[m-1][j-1] += -1;
	    }
	  }
	}
      }
      
      else {
	if (constrained_stu[j-1] != NULL) {
	  if (index_has_element(constrained_stu[j-1],i)) {
	    answer.entries[j-1][j-1] += 1;
	  }
	}
      }
      
    }
  }

  return answer;
}

int there_is_excess_demand(struct process_scp* myscp, struct partial_alloc* demands) {
  int j, nsc;

  nsc = myscp->no_schools;

  for (j = 1; j <= nsc; j++) {
    if (get_total_demand_for_school(demands, j) > (double)myscp->quotas[j-1] + 0.000001) {
      return 1;
    }
  }

  return 0;
}	

int cutoffs_are_below_mcca(struct process_scp* myscp, double* cutoffs) {
  int i, j, k, nst, nsc;

  struct partial_alloc demands;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  demands = compute_demands(myscp, cutoffs);

  for (i = 1; i <= nst; i++) {
    if (fabs(get_total_demand_for_student(myscp, &demands, i) - 1.0) > 0.000001) {

      printf("Student %i is not exactly allocated.\n", i);
      
      destroy_partial_alloc(demands);
      return 0;
    }
  }

  for (j = 1; j <= nsc; j++) {
    if (get_total_demand_for_school(&demands, j) - (double)myscp->quotas[j-1] > 0.000001) {

      printf("School %i is overallocated.\n", j);
      
      destroy_partial_alloc(demands);
      return 0;
    }
    if (cutoffs[j-1] > 0.000001 &&
	get_total_demand_for_school(&demands, j) < (double)myscp->quotas[j-1] - 0.000001) {

      for (k = 1; k <= nsc; k++) {
	printf("For school %i the cutoff is %1.2f, the sum is %1.2f, and the quota is %1.2f,\n",
	       k, cutoffs[k-1], get_total_demand_for_school(&demands,k), myscp->quotas[k-1]);
      }
      
      destroy_partial_alloc(demands);
      return 0;
    }
  }

  destroy_partial_alloc(demands);
  return 1;
}

struct partial_alloc mcca_alloc(struct process_scp* myscp) {
  int nsc;
  int* coarse;
  struct partial_alloc answer;

  nsc = myscp->no_schools;
  coarse = malloc(nsc * sizeof(int));
  answer = mcca_alloc_plus_coarse_cutoffs(myscp, coarse);

  struct process_scp reduced;

  reduced = reduced_scp(myscp, coarse);

  destroy_process_scp(reduced);

  free(coarse);

  return answer;
}

/*************** Below here is for mccb ***************/

int linear_adjustment_b(struct process_scp* myscp, double* cutoffs) {
  int j, nsc, no_act_sch, done;
  
  int* active_schools;
  int* lexicon;
  int* rev_lex;

  struct partial_alloc demands;
  int* last_sch;
  struct index** constrained_stu;
  
  struct square_int_matrix replacements;
  struct subset T_sub;

  nsc = myscp->no_schools;
  
  demands = compute_demands(myscp, cutoffs);

  last_sch = last_schools(myscp, &demands);
  adjust_cutoffs_to_create_constrained_students_b(myscp, &demands, cutoffs, last_sch);

  active_schools = get_active_schools(nsc, cutoffs);
  no_act_sch = get_no_act_sch(nsc, active_schools);

  lexicon = get_lexicon(active_schools, nsc);
  rev_lex = get_rev_lex(active_schools, nsc, no_act_sch);

  replacements = replacement_matrix(myscp, last_sch, &demands, cutoffs, no_act_sch, rev_lex);
  
  constrained_stu = constrained_students(myscp, &demands, cutoffs);
  
  T_sub = minimal_T_subset(&replacements);

  if (is_nullset(&T_sub)) {
    done = adjust_cutoffs_empty_T_sub_b(myscp, &demands, &replacements, lexicon, rev_lex, cutoffs,
					last_sch);
  }
  else {
    adjust_cutoffs_nonempty_T_sub_b(myscp,  &demands, &replacements, &T_sub, lexicon, cutoffs,
				    last_sch);
    done = 0;
  }

  free(active_schools);
  destroy_square_int_matrix(&replacements);
  free(last_sch);
  for (j = 1; j <= nsc; j++) {
    if (constrained_stu[j-1]  != NULL) {
      destroy_index(*constrained_stu[j-1]);
      free(constrained_stu[j-1]);
    }
  }
  free(constrained_stu);
  destroy_subset(T_sub);
  destroy_partial_alloc(demands);
  free(lexicon);
  free(rev_lex);

  return done;
}

void adjust_cutoffs_to_create_constrained_students_b(struct process_scp* myscp,
						     struct partial_alloc* demands,
						     double* cutoffs, int* last_sch) {
  int i, j, k, l, p, nst, nsc, done, hit, maxcandidate;
  double max_demand;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  for (j = 1; j <= nsc; j++) {
    done = 0;
    if (cutoffs[j-1] < 0.000001) {
      done = 1;      
    }
    if (!done) {
      max_demand = 0.0;
      for (i = 1; i <= nst; i++) {
	if (get_priority(myscp, i, j) == (int)floor(cutoffs[j-1]) &&
	    j != last_sch[i-1] &&
	    get_entry(demands, i, j) > 0.000001 &&
	    get_entry(demands, i, j) < 0.999999) {
	  max_demand = max(max_demand, get_entry(demands, i, j));
	  done = 1;		      
	}
      }
      if (done) {
	cutoffs[j-1] = floor(cutoffs[j-1]) + 1.0 - max_demand;
      }
    }
    if (!done) {
      maxcandidate = 0;
      for (i = 1; i <= nst; i++) {
	if (is_eligible(myscp, i, j)) {
	  hit = 0;
	  for (k = myscp->no_eligible_schools[i-1]; k >= 1 && !hit; k--) {
	    l = myscp->preferences[i-1][k-1];
	    if (l != j && get_entry(demands, i, l) > 0.000001) {
	      hit = 1;
	      p = get_priority(myscp, i, j);
	      maxcandidate = max(maxcandidate, p + 1);
	    }
	    if (l == j) {
	      hit = 1;
	    }
	  }
	}
      }
      cutoffs[j-1] = (double)maxcandidate;
    }
  }
}

int* last_schools(struct process_scp* myscp, struct partial_alloc* demands) {
  int i, j, k, nst, done, no_sch;
  
  int* answer;

  nst = myscp->no_students;

  answer = malloc(nst * sizeof(int));

  for (i = 1; i <= nst; i++) {
    no_sch = myscp->no_eligible_schools[i-1];
    if (no_sch == 1) {
      answer[i-1] = 0;
    }
    else {      
      done = 0;
      for (k = no_sch; k >= 1 && !done; k--) {
	j = myscp->preferences[i-1][k-1];
	if (get_entry(demands, i, j) > 0.000001) {
	  done = 1;
	  if (k == 1) {
	    answer[i-1] = 0;
	  }
	  else {
	    answer[i-1] = j;
	  }
	}
      }
    }
  }

  return answer;
}

struct square_int_matrix replacement_matrix(struct process_scp* myscp, int* last_sch,
					     struct partial_alloc* demands, double* cutoffs,
					     int no_act_sch, int* rev_lex) {
  int i, j;

  struct square_int_matrix big_repl;

  struct square_int_matrix answer;
  
  big_repl = big_repl_mat(myscp, last_sch, demands, cutoffs);
  
  answer = zero_int_matrix(no_act_sch);
  for (i = 1; i <= no_act_sch; i++) {
    for (j = 1; j <= no_act_sch; j++) {
      answer.entries[i-1][j-1] = big_repl.entries[rev_lex[i-1]-1][rev_lex[j-1]-1];
    }
  }

  destroy_square_int_matrix(&big_repl);

  return answer;
}

int adjust_cutoffs_empty_T_sub_b(struct process_scp* myscp, struct partial_alloc* demands,
				 struct square_int_matrix* replacements, 
				 int* lexicon, int* rev_lex, double* cutoffs, int* last_sch) {
  int nsc, no_act_sch;
  double tuncc, t_star, tuncoals;

  struct vector delta;
  
  nsc = myscp->no_schools;
  no_act_sch = replacements->dimension;
  
  delta = get_delta_empty_T_sub(myscp, demands,	no_act_sch, lexicon, rev_lex, replacements);
  negate_vector(&delta);

  tuncoals = time_until_no_consumption_of_a_last_school(myscp, demands, cutoffs, last_sch, &delta);
  tuncc = time_until_new_coarse_cutoff(myscp, cutoffs, &delta, -1);  
  t_star = min(min(tuncc, tuncoals), 1.0);
  
  adjust_cutoffs(cutoffs, &delta, nsc, t_star, -1);

  destroy_vector(&delta);

  if (t_star > 0.99999) {
    return 1;
  }
  else {
    return 0;
  }
}

double time_until_no_consumption_of_a_last_school(struct process_scp* myscp,
						  struct partial_alloc* demands, double* cutoffs,
						  int* last_sch, struct vector* delta) {
  int i, j, k, l, nst, hit, positive, done;

  double answer, delta_sum;

  nst = myscp->no_students;

  hit = 0;
  for (i = 1; i <= nst; i++) {
    l = last_sch[i-1];
    if (l != 0) {
      positive = 0;
      done = 0;
      delta_sum = 0.0;
      for (k = 1; k <= myscp->no_eligible_schools[i-1] && !done; k++) {
	j = myscp->preferences[i-1][k-1];
	if (j == l) {
	  done = 1;
	}
	if (!done) {
	  if ((double)get_priority(myscp, i, j) >= cutoffs[j-1] - 1.000001 &&
	      delta->entries[j-1] > 0.000001)  {
	    positive = 1;
	    delta_sum += delta->entries[j-1];
	  }
	}		  
      }
      if (positive) {
	if (!hit) {
	  answer = get_entry(demands, i, l)/delta_sum;
	  hit = 1;
	}
	else {
	  answer = min(answer, get_entry(demands, i, l)/delta_sum);
	}
      }
    }
  }

  return answer;
}

void adjust_cutoffs_nonempty_T_sub_b(struct process_scp* myscp,  struct partial_alloc* demands,
				     struct square_int_matrix* replacements, struct subset* T_sub,
				     int* lexicon, double* cutoffs, int* last_sch) {
  int nsc;

  struct vector delta;
  double tuncc, t_star, tuncoals;

  nsc = myscp->no_schools;

  delta = get_delta_nonempty_T_sub(replacements, T_sub, nsc, lexicon);

  tuncc = time_until_new_coarse_cutoff(myscp, cutoffs, &delta, -1);
  tuncoals = time_until_no_consumption_of_a_last_school(myscp, demands, cutoffs, last_sch, &delta);
  t_star = min(tuncc, tuncoals);
  adjust_cutoffs(cutoffs, &delta, nsc, t_star, -1);
  
  destroy_vector(&delta);
}

struct partial_alloc mccb_alloc_plus_coarse_cutoffs(struct process_scp* myscp, int* coarse) {  
  int j, nsc, done;
  double max;
  double* cutoffs;

  nsc = myscp->no_schools;

  max = 1.0 + (double)maximum_priority(myscp);

  cutoffs = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    cutoffs[j-1] = max;
  }

  done = 0;
  while (!done) {
    done = compute_new_cutoffs_b(myscp, cutoffs);
  }

  if (!cutoffs_are_an_mcc(myscp, cutoffs)) {
    fprintf(stderr, "We are exiting mbpcc with a nonmcc!\n");
    exit(0);
  }  

  for (j = 1; j <= nsc; j++) {
    coarse[j-1] = floor(cutoffs[j-1]);
  }

  struct partial_alloc answer = compute_demands(myscp, cutoffs);

  free(cutoffs);

  return answer;
}

int compute_new_cutoffs_b(struct process_scp* myscp, double* cutoffs) {
  int done;

  double deficit_sum;
  
  struct partial_alloc demands;
  
  demands = compute_demands(myscp, cutoffs);

  if (!there_is_deficient_demand(myscp, &demands, cutoffs)) {
    destroy_partial_alloc(demands);
    return 1;
  }

  if (a_student_does_not_consume_a_probability_measure(myscp, &demands)) {
    naive_reduction_of_cutoffs(myscp, cutoffs);
    destroy_partial_alloc(demands);
    return 0;
  }

  deficit_sum = sum_of_deficits(myscp,cutoffs);
  if (deficit_sum > 1.0) { 
    naive_reduction_of_cutoffs(myscp, cutoffs);    
    destroy_partial_alloc(demands);
    return 0;
  }
  
  destroy_partial_alloc(demands);

  done = linear_adjustment_b(myscp, cutoffs);

  return done;
}


struct square_int_matrix  big_repl_mat(struct process_scp* myscp, int* last_sch,
					   struct partial_alloc* demands, double* cutoffs) {
  int i, j, k, nst, nsc, pri, hit, last_rank, j_better_than_last;
  double dem, cut;

  struct square_int_matrix big_repl;
  
  nst = myscp->no_students;
  nsc = myscp->no_schools;

  big_repl = zero_int_matrix(nsc);
  
  for (j = 1; j <= nsc; j++) {
    if (cutoffs[j-1] > 0.000001) {
      for (i = 1; i <= nst; i++) {
	if (is_eligible(myscp, i, j) && last_sch[i-1] != 0) {
	  hit = 0;
	  for (int k = myscp->no_eligible_schools[i-1]; k >= 1 && !hit; k--) {
	    if (myscp->preferences[i-1][k-1] == last_sch[i-1]) {
	      hit = 1;
	      last_rank = k;
	    }
	  }
	  j_better_than_last = 0;
	  for (k = last_rank - 1; k >= 1; k--) {
	    if (myscp->preferences[i-1][k-1] == j) {
	      j_better_than_last = 1;
	    }
	  }
	  if (j_better_than_last) {
	    dem = get_entry(demands, i, j);
	    pri = get_priority(myscp, i, j);
	    cut = cutoffs[j-1];
	
	    if ((dem > 0.000001 && fabs(dem -(floor(cut) + 1.0 - cut)) < 0.000001) ||
		(dem < 0.000001 && fabs((double)pri + 1.0 - cut) < 0.000001)) {  
	      big_repl.entries[j-1][j-1]++;
	      big_repl.entries[j-1][last_sch[i-1]-1]--;
	    }
	  }
	}
      }
    }
  }

  return big_repl;
}

int there_is_deficient_demand(struct process_scp* myscp, struct partial_alloc* demands,
			      double* cutoffs) {
  int j, nsc;

  nsc = myscp->no_schools;

  for (j = 1; j <= nsc; j++) {
    if (get_total_demand_for_school(demands, j) < (double)myscp->quotas[j-1] - 0.000001 &&
	cutoffs[j-1] > 0.000001) {
      return 1;
    }
  }

  return 0;
}

int a_student_does_not_consume_a_probability_measure(struct process_scp* myscp,
						     struct partial_alloc* demands) {
  int i, nst, deficient_stu;

  nst = myscp->no_students;

  deficient_stu = 0;
  for (i = 1; i <= nst && !deficient_stu; i++) {
    if (get_total_demand_for_student(myscp, demands, i) < 0.999999) {
      deficient_stu = 1;
    }
  }

  return deficient_stu;
}

struct partial_alloc mccb_alloc(struct process_scp* myscp) {
  int nsc;
  int* coarse;
  struct partial_alloc answer;

  nsc = myscp->no_schools;
  coarse = malloc(nsc * sizeof(int));
  answer = mccb_alloc_plus_coarse_cutoffs(myscp, coarse);

  struct process_scp reduced;

  reduced = reduced_scp(myscp, coarse);

  destroy_process_scp(reduced);

  free(coarse);

  return answer;
}

/*************************** general functions and utilities *********************/

int* get_active_schools(int nsc, double* cutoffs) {
  int j;
  
  int* answer;

  answer = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    if (cutoffs[j-1] > 0.00001) {
      answer[j-1] = 1;
    }
    else {
      answer[j-1] = 0;      
    }
  }

  return answer;
}

int get_no_act_sch(int nsc, int* active_schools) {
  int j, answer;
  
  answer = 0;
  for (j = 1; j <= nsc; j++) {
    if (active_schools[j-1] == 1) {
      answer++;
    }
  }

  return answer;
}

int* get_lexicon(int* active_schools, int nsc) {
  int j, count;

  int* answer;

  answer = malloc(nsc * sizeof(int));
  count = 0;
  for (j = 1; j <= nsc; j++) {
    if (active_schools[j-1] == 1) {
      count++;
      answer[j-1] = count;
    }
    else {
      answer[j-1] = 0;
    }
  }

  return answer;
}

int* get_rev_lex(int* active_schools, int nsc, int no_act_sch) {
  int j, count;

  int* answer;

  answer = malloc(no_act_sch * sizeof(int));
  count = 0;
  for (j = 1; j <= nsc; j++) {
    if (active_schools[j-1] == 1) {
      count++;
      answer[count-1] = j;
    }
  }

  return answer;
}

struct vector get_delta_empty_T_sub(struct process_scp* myscp, struct partial_alloc* demands,
				    int no_act_sch, int* lexicon, int* rev_lex,
				    struct square_int_matrix* placements) {
  int j, k, m, nsc;

  double* large_excess_demands;

  struct vector epsilon;
  struct square_matrix dbl_place;
  struct square_matrix dbl_pl_trans;
  struct square_matrix pl_trans_inv;
  struct vector little_delta;
  struct vector answer;

  nsc = myscp->no_schools;
  
  large_excess_demands = excess_demands(myscp, demands);
  
  epsilon = zero_vector(no_act_sch);
  for (k = 1; k <= no_act_sch; k++) {
    epsilon.entries[k-1] = large_excess_demands[rev_lex[k-1]-1];
  }
  
  dbl_place = dbl_matrix_from_int_matrix(placements);
  dbl_pl_trans = transpose(&dbl_place);
  pl_trans_inv = inverse(&dbl_pl_trans);
  little_delta = matrix_times_vector(&pl_trans_inv, &epsilon);

  answer = zero_vector(nsc);
  for (j = 1; j <= nsc; j++) {
    m = lexicon[j-1];
    if (m > 0) {
      answer.entries[j-1] = little_delta.entries[m-1];
    }
  }

  free(large_excess_demands);
  destroy_vector(&epsilon);
  destroy_square_matrix(&dbl_place);
  destroy_square_matrix(&dbl_pl_trans);
  destroy_square_matrix(&pl_trans_inv);
  destroy_vector(&little_delta);

  return answer;
}

struct vector get_delta_nonempty_T_sub(struct square_int_matrix* placements, struct subset* T_sub,
				       int nsc, int* lexicon) {
  int j, count;

  struct vector answer;
  struct square_int_matrix sub_matrix;
  struct vector conv_weights;
  
  sub_matrix = sub_mat(placements, T_sub);

  conv_weights = convex_weights(&sub_matrix);

  answer = zero_vector(nsc);
  count = 0;
  for (j = 1; j <= nsc; j++) {
    if (lexicon[j-1] > 0 && T_sub->indicator[lexicon[j-1]-1] == 1) {
      count++;
      answer.entries[j-1] = conv_weights.entries[count-1];
    }	
    else {
      answer.entries[j-1] = 0.0;
    }
  }

  destroy_square_int_matrix(&sub_matrix);
  destroy_vector(&conv_weights);

  return answer;
}

void adjust_cutoffs(double* cutoffs, struct vector* delta, int nsc, double t_star, int sign) {
  int j;
  
  for (j = 1; j <= nsc; j++) {
    cutoffs[j-1] += sign * t_star * delta->entries[j-1];
    if (1.0 + floor(cutoffs[j-1]) - cutoffs[j-1] < 0.000001) {
      cutoffs[j-1] = 1.0 + floor(cutoffs[j-1]);
    }
  }
}

/*
void adjust_integer_cutoffs_to_slightly_above(int nsc, double* cutoffs){
  int j;
  
  for (j = 1; j <= nsc; j++) {
    if (1.0 + floor(cutoffs[j-1]) - cutoffs[j-1] < 0.000001) {
      cutoffs[j-1] = floor(cutoffs[j-1]) + 1.000001;
    }
  }
}
*/
