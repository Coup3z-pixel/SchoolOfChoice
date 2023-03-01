#include <stdio.h>

struct student {
	int requirement;
	int* eligibility;
};

void print_requirement(struct student my_student);

void print_name(struct student my_student);
