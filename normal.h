#ifndef NORMAL_H
#define NORMAL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double min(double a, double b);

double max(double a, double b);

int int_max(int a, int b);

int is_integer(double nmbr);

double uniform();

double normal();

int random_integer_in_one_to_max(int max);

int* random_ordering(int no_elmts);

#endif /* NORMAL_H */
