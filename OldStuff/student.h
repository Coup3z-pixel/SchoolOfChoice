#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>

struct school
{
	int quota;
	int position;
	int reserve;
};

struct student {

  int requirement;
  int* eligibility;

};

void print_requirement(struct student my_student);

void print_name(struct student my_student);

#endif /* STUDENT_H */
