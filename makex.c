#include <stdio.h>
#include <stdlib.h>

#include "linprog.h"
#include "makexcode.h"

int main(int argc, char *argv[]) {
  int nsc, no_students_per_school, school_capacity, no_priority_grades;
  double school_valence_std_dev, idiosyncratic_std_dev, test_std_dev, answer;
  
  if (argc != 1 && argc != 5 && argc != 8) {
    fprintf(stderr, "The number of arguments is wrong.\n");
    exit(0);
  }
  
  if (argc == 1) {
    nsc = 2;
    no_students_per_school = 3;
    school_capacity = 4;
    school_valence_std_dev = 1.0;
    idiosyncratic_std_dev = 1.0;
    test_std_dev = 1.0;
    no_priority_grades = 2;
  }

  if (argc == 8) {
    nsc = atoi(argv[1]);
    no_students_per_school = atoi(argv[2]);
    school_capacity = atoi(argv[3]);
    school_valence_std_dev = atof(argv[4]);
    idiosyncratic_std_dev = atof(argv[5]);
    test_std_dev = atof(argv[6]);
    no_priority_grades = atoi(argv[7]);
    if (nsc <= 0 || no_students_per_school <= 0 || school_capacity <= 0) {
      fprintf(stderr, "The first three command line arguments (after make_ex) must be integers > 0.\n");
      exit(0);
    }
    if (school_valence_std_dev < 0.000001 || idiosyncratic_std_dev < 0.000001
	|| test_std_dev < 0.000001) {
      fprintf(stderr, "The fourth, fifth, and sixth command line arguments must be  floats > 0.0.\n");
      exit(0);
    }
    if (no_priority_grades <= 1) {
      fprintf(stderr, "The final command line argument must be an integer > 1.\n");
      exit(0);
    }
  }

  if (argc == 5) {
    nsc = atoi(argv[1]);
    no_students_per_school = atoi(argv[2]);
    school_capacity = atoi(argv[3]);
    no_priority_grades = atoi(argv[4]);
    if (nsc <= 0 || no_students_per_school <= 0 || school_capacity <= 0 ||
	no_priority_grades <= 0) {
      fprintf(stderr, "All command line arguments (after make_ex) must be integers > 0.\n");
      exit(0);
    }
    school_valence_std_dev = 1.0;
    idiosyncratic_std_dev = 1.0;
    test_std_dev = 1.0;
  }

  make_example(nsc, no_students_per_school, school_capacity, school_valence_std_dev,
	       idiosyncratic_std_dev, test_std_dev, no_priority_grades);
  
  return 0;
}
