#include <stdio.h>
#include <stdlib.h>

#include "normal.h"

void make_example(int nsc, int no_students_per_school, int school_capacity,
		  double school_valence_std_dev, double idiosyncratic_std_dev,
		  double test_std_dev, int no_priority_grades);

  /* The schools are the integers 0..nsc, with 0 and nsc identified.  location[i]
     is the point i/no_students_per_school. */
  
double* compute_locations(int nst, int no_students_per_school);

  /* distance[i-1][j-1] is the distance (possibly around the end of the interval) from i to j. */

double** compute_distances(int nst, int nsc, double* location);

  /* valence[j-1] is the valence of school j. */

double* compute_valences(int nsc, double school_valence_std_dev);

  /* utility[i-1][j-1] = valence[j-1] - distance[i-1][j-1]. */

double** compute_utilities(int nst, int nsc, double* valence,
			   double** distance, double idiosyncratic_std_dev);

  /* safe_school[i-1] is the school nearest to location[i-1]. */

int* compute_safe_schools(int nst, int nsc, int no_students_per_school);

  /* eligible[i-1][j-1] is 1 if i ranks j, 0 otherwise. */

int** compute_eligible_pairs(int nst, int nsc, double** utility, int* safe_school);

  /* no_ranked_schools[i-1] is the number of schools at least as good as i's safe school. */

int* compute_numbers_of_ranked_schools(int nst, int nsc, int** eligible);

  /* no_eligible_students[j-1] is j's number of eligible students. */

int* compute_numbers_of_eligible_students(int nst, int nsc, int** eligible);

  /* preferences[i-1] is the list of i's ranked schools from best to worst. */

int** compute_preferences(int nst, int nsc, int* safe_school, int* no_ranked_schools,
			  double** utility);

  /* test_score[i-1] is i's score on standardized test. */

double* compute_test_scores(int nst, double test_std_dev);

  /* raw_priority[i-1][j-1] = test_score[i-1] - distance[i-1][j-1]. */

double** compute_raw_priorities(int nst, int nsc, double* test_score,
				double** distance);

  /* We increase each student's raw priority at her safe school. */

void increase_safe_school_raw_priorities(int nst,int nsc, double** raw_priorities,
					 int* safe_school);

  /* schools_rankings_of_students[j-1] is j's eligible students, listed from highest 
     to lowest raw_priority. */

int** compute_schools_rankings_of_students(int nst,int nsc, int** eligible, double** raw_priority);

  /* For each school the eligible students are divided into no_priority_grade groups
     of roughly equal size, with highest priority assignments to the highest raw priorities. */

int** finalize_priorities(int nst, int nsc, int no_priority_grades, int* no_eligible_students,
			 int** eligible, int** schools_rankings_of_students);

int** compute_schools_priorities(int nst, int nsc, double test_std_dev, int no_priority_grades,
				 double** distance, int* safe_school, int* no_eligible_students,
				 int** eligible);

void print_results_makex(int nst, int nsc, int no_students_per_school, int school_capacity,
			 double school_valence_std_dev, double idiosyncratic_std_dev,
			 double test_std_dev, int no_priority_grades,
			 int** priority, int* no_ranked_schools, int** preferences);

void clean_up_makex(int nst, double* location, double** distance, double* valence,
		    double** utility, int* safe_school, int* no_ranked_schools, int** priority,
		    int** preferences, int* no_eligible_students,
		    int** schools_rankings_of_students);
