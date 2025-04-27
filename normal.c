#include "normal.h"

double min(double a, double b) {
  if (a < b) {
    return a;
  }
  else {
    return b;
  }
}

double max(double a, double b) {
  if (a > b) {
    return a;
  }
  else {
    return b;
  }
}

int int_max(int a, int b) {
  if (a > b) {
    return a;
  }
  else {
    return b;
  }
}

int is_integer(double nmbr) {
  if (nmbr - floor(nmbr) < 0.00001 || ceil(nmbr) - nmbr < 0.00001) {
    return 1;
  }
  else {
    return 0;
  }
}

int rand_int() {
  return rand() % 10000;
}

double uniform() {
  return ((double)rand_int())/10000;
}

double normal() {
  double pi, u_one, u_two;
  
  pi = 3.1415926;
  u_one = 0.0;
  u_two = 0.0;
  
  while (u_one < 0.000001) {
    u_one = uniform();
    u_two = uniform();
  }

  return sqrt(-2 * log(u_one)) * cos(2 * pi * u_two); /* Box-Muller */
}
