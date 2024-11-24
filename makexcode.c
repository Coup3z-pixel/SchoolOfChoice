#include <stdio.h>
#include <stdlib.h>

#include "makexcode.h"

void make_example(int nsc, int no_students_per_school, int school_capacity,
		  double school_valence_std_dev, double idiosyncratic_std_dev,
		  double test_std_dev, int no_nontop_priority_grades) {
  
  int nst = nsc * no_students_per_school;

  double* location = compute_locations(nst, no_students_per_school);

  double** distance = compute_distances(nst, nsc, location);
  
  int* safe_school = compute_safe_schools(nst, nsc, no_students_per_school);

  double* valence = compute_valences(nsc, school_valence_std_dev);

  double** utility = compute_utilities(nst, nsc, valence, distance,
				       idiosyncratic_std_dev);
  
  int** eligible = compute_eligible_pairs(nst, nsc, utility, safe_school);

  int* no_ranked_schools = compute_numbers_of_ranked_schools(nst, nsc, eligible);

  int* no_eligible_students = compute_numbers_of_eligible_students(nst, nsc, eligible);

  int** preferences = compute_preferences(nst, nsc, safe_school, no_ranked_schools, utility);

  int** priorities = compute_schools_priorities(nst, nsc, test_std_dev, no_nontop_priority_grades,
						distance, safe_school, no_eligible_students,
						eligible);

  print_results_makex(nst, nsc, no_students_per_school, school_capacity, school_valence_std_dev,
		      idiosyncratic_std_dev, test_std_dev, no_nontop_priority_grades,
		      priorities, no_ranked_schools, preferences);

  clean_up_makex(nst, location, distance, valence, utility, safe_school, no_ranked_schools,
		 eligible, preferences, no_eligible_students, priorities);
}
  
double* compute_locations(int nst, int no_students_per_school) {
  int i;
  
  double* answer = malloc(nst * sizeof(double));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = (double)i/no_students_per_school;
  }
  return answer;
}

double** compute_distances(int nst, int nsc, double* location) {
  int i, j;
  
  double** answer = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      double a = location[i-1];
      double b = (double)j;
      double c = (double)nsc;
      answer[i-1][j-1] = min(min(fabs(a - b), fabs(a - b - c)),
			     fabs(a - b + c));
    }
  }
  return answer;
}

int* compute_safe_schools(int nst, int nsc, int no_students_per_school) {
  int i, j, k;
  
  int* answer = malloc(nst * sizeof(int));
  i = 1;
  answer[i-1] = nsc;
  while (2 * i < no_students_per_school) {
    i++;
    answer[i-1] = nsc;
  }
  for (j = 1; j < nsc; j++) {
    for (k = 1; k <= no_students_per_school; k++) {
      i++;
      answer[i-1] = j;
    }
  }
  while (i < nst) {
    i++;
    answer[i-1] = nsc;
  }
  
  return answer;
}

double* compute_valences(int nsc, double school_valence_std_dev) {
  int j;
  
  double* answer = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer[j-1] = school_valence_std_dev * normal();
  }
  return answer;
}

double** compute_utilities(int nst, int nsc, double* valence,
			   double** distance, double idiosyncratic_std_dev) {
  int i, j;
  
  double** answer = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      answer[i-1][j-1] = valence[j-1] + idiosyncratic_std_dev * normal() - distance[i-1][j-1];
    }
  }
  return answer;
}

int** compute_eligible_pairs(int nst, int nsc, double** utility, int* safe_school) {
  int i, j;
  
  int** answer = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      if (utility[i-1][safe_school[i-1] - 1] <= utility[i-1][j-1]) {
	answer[i-1][j-1] = 1;
      }
      else {
	answer[i-1][j-1] = 0;
      }
    }
  }
  return answer;
}

int* compute_numbers_of_ranked_schools(int nst, int nsc, int** eligible) {
  int i, j;
  
  int* answer = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = 0;
    for (j = 1; j <= nsc; j++) {
      if (eligible[i-1][j-1] == 1) {
	answer[i-1]++;
      }
    }
  }
  
  return answer;
}

int* compute_numbers_of_eligible_students(int nst, int nsc, int** eligible) {
  int i, j;
  
  int* answer = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    answer[j-1] = 0;
    for (i = 1; i <= nst; i++) {
      if (eligible[i-1][j-1]) {
	answer[j-1]++;
      }
    }
  }
  
  return answer;
}

int** compute_preferences(int nst, int nsc, int* safe_school, int* no_ranked_schools,
			    double** utility) {
  int i, j, k;
  
  int** answer = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(no_ranked_schools[i-1] * sizeof(int));
    int no_already_ranked = 0;
    for (j = 1; j <= nsc; j++) {
      if (utility[i-1][safe_school[i-1] - 1] <= utility[i-1][j-1]) {
	if (no_already_ranked == 0) {
	  answer[i-1][0] = j;
	  no_already_ranked = 1;
	}
	else {
	  k = no_already_ranked;
	  while (k > 0 && utility[i-1][j-1] > utility[i-1][answer[i-1][k-1]-1]) {
	    answer[i-1][k] = answer[i-1][k-1]; /*Oh no! Bubble sort*/
	    k--;
	  }
	  answer[i-1][k] = j;
	  no_already_ranked++;
	}
      }
    }
  }
  return answer;
}

double* compute_test_scores(int nst, double test_std_dev) {
  int i;
  
  double* answer = malloc(nst * sizeof(double));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = test_std_dev * normal();
  }
  return answer;
}

double** compute_raw_priorities(int nst, int nsc, double* test_score, double** distance) {
  int i, j;
  
  double** answer = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(double));
    for (j = 1; j <= nsc; j++) {
      answer[i-1][j-1] = test_score[i-1] - distance[i-1][j-1];
    }
  }
  return answer;
}

int** compute_schools_rankings_of_students(int nst, int nsc, int** eligible,
					   int* no_eligible_students, double** raw_priority) {
  int i, j, k, index;

  int** answer = malloc(nsc * sizeof(int*));
  for (j = 1; j <= nsc; j++) {
    answer[j-1] = malloc(no_eligible_students[j-1] * sizeof(int));
    for (i = 1; i <= nst; i++) {
      if (eligible[i-1][j-1]) {
	index = 1;
	for (k = 1; k <= nst; k++) {
	  if (eligible[k-1][j-1] && raw_priority[i-1][j-1] < raw_priority[k-1][j-1]) {
	    index++;
	  }
	}
	answer[j-1][index-1] = i;
      }
    }
  }
  
  return answer;
}

int** finalize_priorities(int nst, int nsc, int no_nontop_priority_grades,
			  int* no_eligible_students, int** schools_rankings_of_students,
			  int* safe_school) {  
  int i, j, k, quotient, remainder, cursor, scan_no;

  int** answer = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      answer[i-1][j-1] = 0;
    }
  }

  int* no_nonsafe_eligible = malloc(nsc * sizeof(int));
  for (j = 1; j <= nsc; j++) {
    no_nonsafe_eligible[j-1] = no_eligible_students[j-1];
  }
    
  for (i = 1; i <= nst; i++) {
    answer[i-1][safe_school[i-1]-1] = no_nontop_priority_grades;
    no_nonsafe_eligible[safe_school[i-1]-1]--;
  }
  
  for (j = 1; j <= nsc; j++) {
    quotient = no_nonsafe_eligible[j-1] / no_nontop_priority_grades;
    remainder = no_nonsafe_eligible[j-1] % no_nontop_priority_grades;
    cursor = 0;
    for (k = no_nontop_priority_grades; k >= 1; k--) {
      
      scan_no = quotient;
      if (k <= remainder) {
	scan_no++;
      }
      
      while (scan_no > 0) {
	cursor++;
	i = schools_rankings_of_students[j-1][cursor-1];	
	if (safe_school[i-1] != j) {
	  answer[i-1][j-1] = k - 1;
	  scan_no--;
	}
      }
    }
  }
  
  free(no_nonsafe_eligible);

  return answer;
}

int** compute_schools_priorities(int nst, int nsc, double test_std_dev,
				 int no_nontop_priority_grades, double** distance,
				 int* safe_school, int* no_eligible_students, int** eligible) {
  int i, j;

  double* test_score = compute_test_scores(nst, test_std_dev);

  double** raw_priority = compute_raw_priorities(nst, nsc, test_score, distance);

  int** schools_rankings_of_students = compute_schools_rankings_of_students(nst, nsc,
						  eligible, no_eligible_students, raw_priority);
  
  int** final_priorities = finalize_priorities(nst, nsc, no_nontop_priority_grades,
					       no_eligible_students, schools_rankings_of_students,
					       safe_school);

  free(test_score);
  for (i = 1; i <= nst; i++) {
    free(raw_priority[i-1]);
  }
  free(raw_priority);
  for (j = 1; j <= nsc; j++) {
    free(schools_rankings_of_students[j-1]);
  }
  free(schools_rankings_of_students);

  return final_priorities;
}

void print_results_makex(int nst, int nsc, int no_students_per_school, int school_capacity,
			 double school_valence_std_dev, double idiosyncratic_std_dev,
			 double test_std_dev, int no_nontop_priority_grades,
			 int** priority, int* no_ranked_schools, int** preferences) {
  int i, j, k;
  
  printf("/* This file was generated by makex with %i schools,\n", nsc);
  
  printf("%i students per school, capacity %i for all schools,\n",
	 no_students_per_school, school_capacity);
  printf("school valence std dev %1.2f, idiosyncratic std dev %1.2f,\n",
	 school_valence_std_dev, idiosyncratic_std_dev);
  printf("student test std dev %1.2f, and %i nontop priority grades. */\n",
	 test_std_dev, no_nontop_priority_grades);
  printf("There are %i students and %i schools\n",nst,nsc);
  printf("The vector of quotas is (");
  for (j = 1; j < nsc; j++) {
    printf("%i,",school_capacity);
  }
  printf("%i)\n",school_capacity);
  printf("The priority matrix is\n");
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      printf("    %i",priority[i-1][j-1]);
    }
    printf("\n");
  }
  printf("The students numbers of ranked schools are\n(");
  for (i = 1; i < nst; i++) {
    printf("%i,",no_ranked_schools[i-1]);
  }
  printf("%i)\n",no_ranked_schools[nst - 1]);
  printf("The preferences of the students are");
  for (i = 1; i <= nst; i++) {
    printf("\n%i:",i);
    if (i < 10) {
      printf(" ");
    }
    for (k = 1; k <= no_ranked_schools[i-1]; k++) {
      printf("   %i",preferences[i-1][k-1]);
    }
  }
  printf("\n");
}


void clean_up_makex(int nst, double* location, double** distance, double* valence,
		    double** utility, int* safe_school, int* no_ranked_schools, int** priority,
		    int** preferences, int* no_eligible_students,
		    int** schools_rankings_of_students) {
  int i;
  
  free(location);
  for (i = 1; i <= nst; i++) {
    free(distance[i-1]);
  }
  free(distance);
  free(valence);
  for (i = 1; i <= nst; i++) {
    free(utility[i-1]);
  }
  free(utility);
  free(safe_school);
  free(no_ranked_schools);
  for (i = 1; i <= nst; i++) {
    free(priority[i-1]);
  }
  free(priority);
  for (i = 1; i <= nst; i++) {
    free(preferences[i-1]);
  }
  free(preferences);
  free(no_eligible_students);
  for (i = 1; i <= nst; i++) {
    free(schools_rankings_of_students[i-1]);
  }
  free(schools_rankings_of_students);
}
