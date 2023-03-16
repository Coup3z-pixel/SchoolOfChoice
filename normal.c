#include "normal.h"

double min(double a, double b) {
  if (a < b) {
    return a;
  }
  else {
    return b;
  }
}

int rand_int() {
  return rand() % 10000;
}

double uniform() {
  return ((double)rand_int())/10000;
}

double normal() {
  double pi = 3.1415926;
  double u_one = uniform();
  double u_two = uniform();
  return sqrt(-2 * log(u_one)) * cos(2 * pi * u_two); /* Box-Muller */
}
