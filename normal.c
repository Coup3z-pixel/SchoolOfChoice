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

int random_integer_in_one_to_max(int max) {
  int answer;
  double unif_rv;

  unif_rv = uniform();

  answer = 1;
  
  while ((double)(answer - 1) > max * unif_rv || (double)answer < max * unif_rv) {
    answer++;
  }

  return answer;
}

int* random_ordering(int no_elmts) {
  int i, no_remaining, random_int, cand_slot, count;

  int* answer;

  answer = malloc(no_elmts * sizeof(int));
  for (i = 1; i <= no_elmts; i++) {
    answer[i-1] = 0;
  }

  no_remaining = no_elmts;
  while (no_remaining > 0) {
    random_int = random_integer_in_one_to_max(no_remaining);
    cand_slot = 1;
    count = 1;
    
    while (answer[cand_slot-1] != 0 || count < random_int) {
      if (answer[cand_slot-1] == 0) {
	count++;
      }
      cand_slot++;
    }
    
    answer[cand_slot-1] = no_remaining;
    no_remaining--;
  }

  return answer;
}
