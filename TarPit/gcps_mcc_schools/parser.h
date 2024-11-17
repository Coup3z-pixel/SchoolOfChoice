#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "schchprob.h"
#include "partalloc.h"

struct input_sch_ch_prob sch_ch_prob_from_file(const char filename[]);

struct partial_alloc allocation_from_file(const char filename[]);

void file_begins_with_comment(FILE* fp);

int is_white_space(char c);

char get_next_nonwhite(FILE *fp);
  
void check_There(FILE *fp);
void check_are(FILE *fp, int index);
void check_students(FILE *fp, int index);
void check_and(FILE *fp);
void check_schools(FILE *fp, int index);
void check_The(FILE *fp, int index);
void check_vector(FILE *fp);
void check_of(FILE *fp, int index);
void check_quotas(FILE *fp);
void check_is(FILE *fp, int index);
void check_priority(FILE *fp, int index);
void check_matrix(FILE *fp);
void check_numbers(FILE *fp);
void check_ranked(FILE *fp);
void check_preferences(FILE *fp);
void check_the(FILE *fp, int index);
void check_thresholds(FILE *fp);
int get_number(FILE *fp);
double get_double(FILE *fp);
void check_student_tag(FILE *fp);

#endif /* PARSER_H */
