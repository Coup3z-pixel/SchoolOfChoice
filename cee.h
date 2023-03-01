#include <stdio.h>

struct cee {
  int no_students;
  int no_schools;
  double* requirements;
  double* quotas;
  int* eligibility; /* In the computer the no_students x no_schools matrix is a one dimensional
                       array with no_students * no_schools entries. */
};

void print_cee_no_students(struct cee my_cee);
