#include "parser.h"

int is_white_space(char c) {
  if (c == ' ' || c == '\t' || c == '\n' || c == ',' || c == '(' || c == ')' ) {
    return 1;
  }
  else {
    return 0;
  }
}

char get_next(FILE *fp) {
  char next = getc(fp);
  while (is_white_space(next)) {
    next = getc(fp);
  }
  return next;
}

void check_There(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: formatting for number of students and number of schools is incorrect.\n");
    exit(0);
  }
}

void check_are_first(FILE *fp, char next) {
  if (next != 'a' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: formatting for number of students and number of schools is incorrect.\n");
    exit(0);
  }
}

void check_students_first(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 't' || getc(fp) != 'u' || getc(fp) != 'd' || getc(fp) != 'e' ||
                     getc(fp) != 'n' || getc(fp) != 't' || getc(fp) != 's') {
    printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

void check_and(FILE *fp, char next) {
  if (next != 'a' || getc(fp) != 'n' || getc(fp) != 'd') {
    printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

void check_schools_first(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 'c' || getc(fp) != 'h' || getc(fp) != 'o' || getc(fp) != 'o' ||
                     getc(fp) != 'l' || getc(fp) != 's') {
    printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

void check_The_first(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_vector(FILE *fp, char next) {
  if (next != 'v' || getc(fp) != 'e' || getc(fp) != 'c' || getc(fp) != 't' || getc(fp) != 'o' ||
                     getc(fp) != 'r') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_of_first(FILE *fp, char next) {
  if (next != 'o' || getc(fp) != 'f') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_quotas(FILE *fp, char next) {
  if (next != 'q' || getc(fp) != 'u' || getc(fp) != 'o' || getc(fp) != 't' || getc(fp) != 'a' ||
                     getc(fp) != 's') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_is_first(FILE *fp, char next) {
  if (next != 'i' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_The_second(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
    exit(0);
  }
}

void check_priority_first(FILE *fp, char next) {
  if (next != 'p' || getc(fp) != 'r' || getc(fp) != 'i' || getc(fp) != 'o' || getc(fp) != 'r' ||
                     getc(fp) != 'i' || getc(fp) != 't' || getc(fp) != 'y') {
    printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
    exit(0);
  }
}

void check_matrix(FILE *fp, char next) {
  if (next != 'm' || getc(fp) != 'a' || getc(fp) != 't' || getc(fp) != 'r' ||
                     getc(fp) != 'i' || getc(fp) != 'x') {
    printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
    exit(0);
  }
}

void check_is_second(FILE *fp, char next) {
  if (next != 'i' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
    exit(0);
  }
}

void check_The_third(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_students_second(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 't' || getc(fp) != 'u' || getc(fp) != 'd' || getc(fp) != 'e' ||
                     getc(fp) != 'n' || getc(fp) != 't' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_numbers(FILE *fp, char next) {
  if (next != 'n' || getc(fp) != 'u' || getc(fp) != 'm' || getc(fp) != 'b' || getc(fp) != 'e' ||
                     getc(fp) != 'r' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_of_second(FILE *fp, char next) {
  if (next != 'o' || getc(fp) != 'f') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_ranked(FILE *fp, char next) {
  if (next != 'r' || getc(fp) != 'a' || getc(fp) != 'n' || getc(fp) != 'k' ||
                     getc(fp) != 'e' || getc(fp) != 'd') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_schools_second(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 'c' || getc(fp) != 'h' || getc(fp) != 'o' ||
                     getc(fp) != 'o' || getc(fp) != 'l' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_are_second(FILE *fp, char next) {
  if (next != 'a' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_The_fourth(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_preferences(FILE *fp, char next) {
  if (next != 'p' || getc(fp) != 'r' || getc(fp) != 'e' || getc(fp) != 'f' ||
  getc(fp) != 'e' || getc(fp) != 'r' || getc(fp) != 'e' || getc(fp) != 'n' ||
  getc(fp) != 'c' || getc(fp) != 'e' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_of_third(FILE *fp, char next) {
  if (next != 'o' || getc(fp) != 'f') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_the_first(FILE *fp, char next) {
  if (next != 't' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_students_third(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 't' || getc(fp) != 'u' || getc(fp) != 'd' || getc(fp) != 'e' ||
                     getc(fp) != 'n' || getc(fp) != 't' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_are_third(FILE *fp, char next) {
  if (next != 'a' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_The_fifth(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_priority_second(FILE *fp, char next) {
  if (next != 'p' || getc(fp) != 'r' || getc(fp) != 'i' || getc(fp) != 'o' || getc(fp) != 'r' ||
                     getc(fp) != 'i' || getc(fp) != 't' || getc(fp) != 'y') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_thresholds(FILE *fp, char next) {
  if (next != 't' || getc(fp) != 'h' || getc(fp) != 'r' || getc(fp) != 'e' || getc(fp) != 's' ||
                     getc(fp) != 'h' || getc(fp) != 'o' || getc(fp) != 'l' || getc(fp) != 'd' ||
                     getc(fp) != 's') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_of_fourth(FILE *fp, char next) {
  if (next != 'o' || getc(fp) != 'f') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_the_third(FILE *fp, char next) {
  if (next != 't' || getc(fp) != 'h' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_schools_third(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 'c' || getc(fp) != 'h' || getc(fp) != 'o' || getc(fp) != 'o' ||
                     getc(fp) != 'l' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_are_fourth(FILE *fp, char next) {
  if (next != 'a' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

int get_number(FILE *fp, char next) {
  if (!isdigit(next)) {
    printf("ERROR: what should be a number begins with a nondigit.\n");
    exit(0);
  }
  int answer = next - '0';
  next = getc(fp);
  while (isdigit(next)) {
    answer = 10 * answer + next - '0';
    next = getc(fp);
  }
  if (!is_white_space(next)) {
    printf("ERROR: numbers must be surrounded by white space (\' \',\'\\n\',\'\\t\',\'(\',\')\',\',\').\n");
    exit(0);
  }
  return answer;
}

double get_double(FILE *fp, char next) {
  if (!isdigit(next) && next != '.') {
    printf("ERROR: what should be a number begins with a nondigit other than \'.\'.\n");
    exit(0);
  }
  char str[20];
  str[0] = next;
  int count = 0;
  next = getc(fp);
  while (isdigit(next) || next == '.') {
    count++;
    str[count] = next;
    next = getc(fp);
  }
  if (!is_white_space(next)) {
    printf("ERROR: numbers must be surrounded by white space (\' \',\'\\n\',\'\\t\',\'(\',\')\',\',\').\n");
    exit(0);
  }
  count++; 
  str[count] = '\0';

  return atof(str);
}

void check_student_tag(FILE *fp, char next) {
  if (!isdigit(next)) {
    printf("ERROR: what should be a student number begins with a nondigit.\n");
    exit(0);
  }
  int answer = next - '0';
  next = getc(fp);
  while (isdigit(next)) {
    answer = 10 * answer + next - '0';
    next = getc(fp);
  }
  char colon = ':';
  if (next != colon) {
    printf("ERROR: incorrect formatting of student tag.\n");
    exit(0);
  }
  next = getc(fp);
  if (!is_white_space(next)) {
    printf("ERROR: incorrect formatting of student tag.\n");
    exit(0);
  }
}

struct input_sch_ch_prob sch_ch_prob_from_file(const char filename[]) {
  struct input_sch_ch_prob my_sch_ch_prob;

  FILE *fp;

  if (!(fp = fopen(filename, "r"))) {
    printf("ERROR: the file schools.scp does not exist.\n");
    exit(0);
  }

  if (getc(fp) != '/') {
    printf("ERROR: the input file must begin with /*\n");
    exit(0);
  }

  if (getc(fp) != '*') {
    printf("ERROR: the input file must begin with /*\n");
    exit(0);
  }

  while (getc(fp) != '*') {}

  if (getc(fp) != '/') {
    printf("ERROR: the initial comment must end with */\n");
    exit(0);
  }

  char next = get_next(fp);
  check_There(fp,next);
  next = get_next(fp);
  check_are_first(fp,next);
  next = get_next(fp);
  my_sch_ch_prob.cee.no_students = get_number(fp,next);
  next = get_next(fp);
  check_students_first(fp,next);
  next = get_next(fp);
  check_and(fp,next);
  next = get_next(fp);
  my_sch_ch_prob.cee.no_schools = get_number(fp,next);
  next = get_next(fp);
  check_schools_first(fp,next);
  
  next = get_next(fp);
  check_The_first(fp,next);
  next = get_next(fp);
  check_vector(fp,next);
  next = get_next(fp);
  check_of_first(fp,next);
  next = get_next(fp);
  check_quotas(fp,next);
  next = get_next(fp);
  check_is_first(fp,next);

  int i,j;
  my_sch_ch_prob.cee.quotas = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int));
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
      next = get_next(fp);
      my_sch_ch_prob.cee.quotas[j-1] = get_number(fp,next);
  }
  
  next = get_next(fp);
  check_The_second(fp,next);
  next = get_next(fp);
  check_priority_first(fp,next);
  next = get_next(fp);
  check_matrix(fp,next);
  next = get_next(fp);
  check_is_second(fp,next);
   
  my_sch_ch_prob.cee.priority = malloc(my_sch_ch_prob.cee.no_students * sizeof(int*));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    my_sch_ch_prob.cee.priority[i-1] = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int));
    for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
      next = get_next(fp);
      my_sch_ch_prob.cee.priority[i-1][j-1] = get_number(fp,next);
    }
  } 
  
  next = get_next(fp);
  check_The_third(fp,next);
  next = get_next(fp);
  check_students_second(fp,next);
  next = get_next(fp);
  check_numbers(fp,next);
  next = get_next(fp);
  check_of_second(fp,next);
  next = get_next(fp);
  check_ranked(fp,next);
  next = get_next(fp);
  check_schools_second(fp,next);
  next = get_next(fp);
  check_are_second(fp,next);
   
  my_sch_ch_prob.no_eligible_schools = malloc(my_sch_ch_prob.cee.no_students * sizeof(int));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    next = get_next(fp);
    my_sch_ch_prob.no_eligible_schools[i-1] = get_number(fp,next);
  } 

  next = get_next(fp);
  check_The_fourth(fp,next);
  next = get_next(fp);
  check_preferences(fp,next);
  next = get_next(fp);
  check_of_third(fp,next);
  next = get_next(fp);
  check_the_first(fp,next);
  next = get_next(fp);
  check_students_third(fp,next);
  next = get_next(fp);
  check_are_third(fp,next);
   
  my_sch_ch_prob.preferences = malloc(my_sch_ch_prob.cee.no_students * sizeof(int*));
  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    next = get_next(fp);
    check_student_tag(fp,next);
    my_sch_ch_prob.preferences[i-1] = malloc(my_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    for (j = 1; j <= my_sch_ch_prob.no_eligible_schools[i-1]; j++) {
      next = get_next(fp);
      my_sch_ch_prob.preferences[i-1][j-1] = get_number(fp,next);
    }
  }
  
  next = get_next(fp);
  check_The_fifth(fp,next);
  next = get_next(fp);
  check_priority_second(fp,next);
  next = get_next(fp);
  check_thresholds(fp,next);
  next = get_next(fp);
  check_of_fourth(fp,next);
  next = get_next(fp);
  check_the_third(fp,next);
  next = get_next(fp);
  check_schools_third(fp,next);
  next = get_next(fp);
  check_are_fourth(fp,next);
  
  my_sch_ch_prob.priority_threshold = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int));
  for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
      next = get_next(fp);
      my_sch_ch_prob.priority_threshold[j-1] = get_number(fp,next);
  }
    
  fclose(fp); 

  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    int count = 0;
    for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
      if (my_sch_ch_prob.cee.priority[i-1][j-1] > 0) {
	count++;
      }
    }
    if (count != my_sch_ch_prob.no_eligible_schools[i-1]) {
      printf("Student %i\'s numbers of positive priority schools and ranked schools differ.\n",i);
      exit(0);
    }
  }

  for (i = 1; i <= my_sch_ch_prob.cee.no_students; i++) {
    for (j = 1; j <= my_sch_ch_prob.cee.no_schools; j++) {
      if (my_sch_ch_prob.cee.priority[i-1][j-1] > 0) {
	int found = 0;
	for (int k = 1; k <= my_sch_ch_prob.no_eligible_schools[i-1]; k++) {
	  if (my_sch_ch_prob.preferences[i-1][k-1] == j) {
	    found = 1;
	  }
	}
	if (!found) {
	  printf("ERROR: we did not find school %i in student %i\'s preferences.\n",j,i);
	  exit(0);
	}
      }
    }
  }


  return my_sch_ch_prob;
}



void check_There_third(FILE *fp, char next) {
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: formatting for numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

void check_are_sixth(FILE *fp, char next) {
  if (next != 'a' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

void check_students_fourth(FILE *fp, char next) {
  if (next != 's' || getc(fp) != 't' || getc(fp) != 'u' || getc(fp) != 'd' || getc(fp) != 'e' ||
                     getc(fp) != 'n' || getc(fp) != 't' || getc(fp) != 's') {
    printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

struct partial_alloc allocation_from_file(const char filename[]) {
  struct partial_alloc allocation;

  FILE *fp;

  if (!(fp = fopen(filename, "r"))) {
    printf("ERROR: the file %s does not exist.\n", filename);
    exit(0);
  }

  if (getc(fp) != '/') {
    printf("ERROR: the input file must begin with /*\n");
    exit(0);
  }

  if (getc(fp) != '*') {
    printf("ERROR: the input file must begin with /*\n");
    exit(0);
  }

  while (getc(fp) != '*') {}

  if (getc(fp) != '/') {
    printf("ERROR: the initial comment must end with */\n");
    exit(0);
  }

  char next = get_next(fp);
  check_There_third(fp,next);
  next = get_next(fp);
  check_are_sixth(fp,next);
  next = get_next(fp);
  allocation.no_students = get_number(fp,next);
  next = get_next(fp);
  check_students_fourth(fp,next);
  next = get_next(fp);
  check_and(fp,next);
  next = get_next(fp);
  allocation.no_schools = get_number(fp,next);
  next = get_next(fp);
  check_schools_first(fp,next);
  
  int i, j;
  for (j = 1; j <= allocation.no_schools; j++) {
      next = get_next(fp);
      check_student_tag(fp,next);
  }
  allocation.allocations = malloc(allocation.no_students * sizeof(double*));
  for (i = 1; i <= allocation.no_students; i++) {
    next = get_next(fp);
    check_student_tag(fp,next);
    allocation.allocations[i-1] = malloc(allocation.no_schools * sizeof(double));
    for (j = 1; j <= allocation.no_schools; j++) {
      next = get_next(fp);
      allocation.allocations[i-1][j-1] = get_double(fp,next);
    }
  }
    
  fclose(fp);

  return allocation;
}
