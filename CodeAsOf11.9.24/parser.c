#include "parser.h"

struct input_sch_ch_prob sch_ch_prob_from_file(const char filename[]) {
  struct input_sch_ch_prob my_sch_ch_prob;

  int i, j;

  FILE* fp;

  if (!(fp = fopen(filename, "r"))) {
    printf("ERROR: the file (either schools.scp or command line argument)  does not exist.\n");
    exit(0);
  }

  file_begins_with_comment(fp);

  check_There(fp);
  check_are(fp,1);
  my_sch_ch_prob.no_students = get_number(fp);
  /*  my_sch_ch_prob.cee.no_students = my_sch_ch_prob.no_students; */
  check_students(fp,1);
  check_and(fp);
  my_sch_ch_prob.no_schools = get_number(fp);
  /*  my_sch_ch_prob.cee.no_schools = my_sch_ch_prob.no_schools; */
  check_schools(fp,1);
  
  check_The(fp,1);
  check_vector(fp);
  check_of(fp,1);
  check_quotas(fp);
  check_is(fp,1);

  /*  my_sch_ch_prob.cee.quotas = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int)); */
  my_sch_ch_prob.quotas = malloc(my_sch_ch_prob.no_schools * sizeof(int));
  for (j = 1; j <= my_sch_ch_prob.no_schools; j++) {
      my_sch_ch_prob.quotas[j-1] = get_number(fp);
      /* my_sch_ch_prob.cee.quotas[j-1] = my_sch_ch_prob.quotas[j-1]; */
  }
  
  check_The(fp,2);
  check_priority(fp,1);
  check_matrix(fp);
  check_is(fp,2);
   
  /*  my_sch_ch_prob.cee.priority = malloc(my_sch_ch_prob.cee.no_students * sizeof(int*)); */
  my_sch_ch_prob.priorities = malloc(my_sch_ch_prob.no_students * sizeof(int*));
  for (i = 1; i <= my_sch_ch_prob.no_students; i++) {
    /* my_sch_ch_prob.cee.priority[i-1] = malloc(my_sch_ch_prob.cee.no_schools * sizeof(int)); */
    my_sch_ch_prob.priorities[i-1] = malloc(my_sch_ch_prob.no_schools * sizeof(int));
    for (j = 1; j <= my_sch_ch_prob.no_schools; j++) {
      my_sch_ch_prob.priorities[i-1][j-1] = get_number(fp);
      /*   my_sch_ch_prob.cee.priority[i-1][j-1] = my_sch_ch_prob.priorities[i-1][j-1]; */
    }
  } 
  
  check_The(fp,3);
  check_students(fp,2);
  check_numbers(fp);
  check_of(fp,2);
  check_ranked(fp);
  check_schools(fp,2);
  check_are(fp,2);
   
  my_sch_ch_prob.no_eligible_schools = malloc(my_sch_ch_prob.no_students * sizeof(int));
  for (i = 1; i <= my_sch_ch_prob.no_students; i++) {
    my_sch_ch_prob.no_eligible_schools[i-1] = get_number(fp);
  } 

  check_The(fp,4);
  check_preferences(fp);
  check_of(fp,3);
  check_the(fp,1);
  check_students(fp,3);
  check_are(fp,3);
   
  my_sch_ch_prob.preferences = malloc(my_sch_ch_prob.no_students * sizeof(int*));
  for (i = 1; i <= my_sch_ch_prob.no_students; i++) {
    check_student_tag(fp);
    my_sch_ch_prob.preferences[i-1] = malloc(my_sch_ch_prob.no_eligible_schools[i-1]*sizeof(int));
    for (j = 1; j <= my_sch_ch_prob.no_eligible_schools[i-1]; j++) {
      my_sch_ch_prob.preferences[i-1][j-1] = get_number(fp);
    }
  }
    
  fclose(fp); 

  for (i = 1; i <= my_sch_ch_prob.no_students; i++) {
    for (j = 1; j <= my_sch_ch_prob.no_schools; j++) {
      if (my_sch_ch_prob.priorities[i-1][j-1] > 0) {
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

struct partial_alloc allocation_from_file(const char filename[]) {
  struct partial_alloc allocation;

  FILE *fp;

  if (!(fp = fopen(filename, "r"))) {
    printf("ERROR: the file %s does not exist.\n", filename);
    exit(0);
  }

  file_begins_with_comment(fp);

  check_There(fp);
  check_are(fp,5);
  allocation.no_students = get_number(fp);
  check_students(fp,4);
  check_and(fp);
  allocation.no_schools = get_number(fp);
  check_schools(fp,1);
  
  int i, j;
  for (j = 1; j <= allocation.no_schools; j++) {
      check_student_tag(fp);
  }
  allocation.allocations = malloc(allocation.no_students * sizeof(double*));
  for (i = 1; i <= allocation.no_students; i++) {
    check_student_tag(fp);
    allocation.allocations[i-1] = malloc(allocation.no_schools * sizeof(double));
    for (j = 1; j <= allocation.no_schools; j++) {
      allocation.allocations[i-1][j-1] = get_double(fp);
    }
  }
    
  fclose(fp);

  return allocation;
}

void file_begins_with_comment(FILE* fp) {
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
    printf("ERROR: the initial comment must end with *");
    printf("/\n");
    exit(0);
  }
}

int is_white_space(char c) {
  if (c == ' ' || c == '\t' || c == '\n' || c == ',' || c == '(' || c == ')' ) {
    return 1;
  }
  else {
    return 0;
  }
}

char get_next_nonwhite(FILE *fp) {
  char next = getc(fp);
  while (is_white_space(next)) {
    next = getc(fp);
  }
  return next;
}
 
void check_There(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e' || getc(fp) != 'r' || getc(fp) != 'e') {
    printf("ERROR: formatting for number of students and number of schools is incorrect.\n");
    exit(0);
  }
}

void check_are(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'a' || getc(fp) != 'r' || getc(fp) != 'e') {
    switch (index) {
    case 1:
      printf("ERROR: formatting for number of students and number of schools is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
      break;
    case 3:
      printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
      break;
    case 4:
      printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
      break;
    case 5:
      printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_students(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 's' || getc(fp) != 't' || getc(fp) != 'u' || getc(fp) != 'd' || getc(fp) != 'e' ||
                     getc(fp) != 'n' || getc(fp) != 't' || getc(fp) != 's') {
    switch (index) {
    case 1:
      printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
      break;
    case 3:
      printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
      break;
    case 4:
      printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_and(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'a' || getc(fp) != 'n' || getc(fp) != 'd') {
    printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
    exit(0);
  }
}

void check_schools(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 's' || getc(fp) != 'c' || getc(fp) != 'h' || getc(fp) != 'o' || getc(fp) != 'o' ||
                     getc(fp) != 'l' || getc(fp) != 's') {
    switch (index) {
      case 1:
	printf("ERROR: formatting for the numbers of students and schools is incorrect.\n");
	break;
      case 2:
	printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
	break;
      case 3:
	printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
	break;
    }
    exit(0);
  }
}

void check_The(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'T' || getc(fp) != 'h' || getc(fp) != 'e') {
    switch (index) {
    case 1:
      printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
      break;
    case 3:
      printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
      break;
    case 4:
      printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
      break;
    case 5:
      printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_vector(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'v' || getc(fp) != 'e' || getc(fp) != 'c' || getc(fp) != 't' || getc(fp) != 'o' ||
                     getc(fp) != 'r') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_of(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'o' || getc(fp) != 'f') {
    switch (index) {
    case 1:
      printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
      break;
    case 3:
      printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
      break;
    case 4:
      printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_quotas(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'q' || getc(fp) != 'u' || getc(fp) != 'o' || getc(fp) != 't' || getc(fp) != 'a' ||
                     getc(fp) != 's') {
    printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
    exit(0);
  }
}

void check_is(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'i' || getc(fp) != 's') {
    switch (index) {
    case 1:
      printf("ERROR: the phrase \'The vector of quotas is\' is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_priority(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'p' || getc(fp) != 'r' || getc(fp) != 'i' || getc(fp) != 'o' || getc(fp) != 'r' ||
                     getc(fp) != 'i' || getc(fp) != 't' || getc(fp) != 'y') {
    switch (index) {
    case 1:
      printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_matrix(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'm' || getc(fp) != 'a' || getc(fp) != 't' || getc(fp) != 'r' ||
                     getc(fp) != 'i' || getc(fp) != 'x') {
    printf("ERROR: the phrase \'The priority matrix is\' is incorrect.\n");
    exit(0);
  }
}

void check_numbers(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'n' || getc(fp) != 'u' || getc(fp) != 'm' || getc(fp) != 'b' || getc(fp) != 'e' ||
                     getc(fp) != 'r' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_ranked(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'r' || getc(fp) != 'a' || getc(fp) != 'n' || getc(fp) != 'k' ||
                     getc(fp) != 'e' || getc(fp) != 'd') {
    printf("ERROR: the phrase \'The students\' numbers of ranked schools are\' is incorrect.\n");
    exit(0);
  }
}

void check_preferences(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 'p' || getc(fp) != 'r' || getc(fp) != 'e' || getc(fp) != 'f' ||
  getc(fp) != 'e' || getc(fp) != 'r' || getc(fp) != 'e' || getc(fp) != 'n' ||
  getc(fp) != 'c' || getc(fp) != 'e' || getc(fp) != 's') {
    printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
    exit(0);
  }
}

void check_the(FILE *fp, int index) {
  char next = get_next_nonwhite(fp);
  
  if (next != 't' || getc(fp) != 'h' || getc(fp) != 'e') {
    switch (index) {
    case 1:
      printf("ERROR: the phrase \'The preferences of the students are\' is incorrect.\n");
      break;
    case 2:
      printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
      break;
    }
    exit(0);
  }
}

void check_thresholds(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
  if (next != 't' || getc(fp) != 'h' || getc(fp) != 'r' || getc(fp) != 'e' || getc(fp) != 's' ||
                     getc(fp) != 'h' || getc(fp) != 'o' || getc(fp) != 'l' || getc(fp) != 'd' ||
                     getc(fp) != 's') {
    printf("ERROR: the phrase \'The priority thresholds of the schools are\' is incorrect.\n");
    exit(0);
  }
}

int get_number(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
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

double get_double(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
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

void check_student_tag(FILE *fp) {
  char next = get_next_nonwhite(fp);
  
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

