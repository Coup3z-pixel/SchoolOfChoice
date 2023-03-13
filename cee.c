#include "cee.h"


struct int_cee make_toy_cee() {
  int i,j;
  struct int_cee my_cee;
  
  my_cee.no_students = 4;
  my_cee.no_schools = 3;
  
  my_cee.quotas = malloc(3 * sizeof(int));
  my_cee.quotas[0] = 1;
  my_cee.quotas[1] = 2;
  my_cee.quotas[2] = 1;
  
  my_cee.priority = malloc(4 * sizeof(int*));
  for (i = 1; i <= my_cee.no_students; i++) {
  my_cee.priority[i-1] = malloc(3 * sizeof(int));
    for (j = 1; j <= my_cee.no_schools; j++) {
      my_cee.priority[i-1][j-1] = 1; 
    }
  }
  
  return my_cee;
}

void destroy_int_cee(struct int_cee* my_cee) {
  int i;
  free(my_cee->quotas);
  for (i = 1; i <= my_cee->no_students; i++) {
    free(my_cee->priority[i-1]);
  }
  free(my_cee->priority);
}

void destroy_double_cee(struct double_cee* my_cee) {
  int i;
  free(my_cee->quotas);
  
  for (i = 1; i <= my_cee->no_students; i++) {
    free(my_cee->priority[i-1]);
  }
  free(my_cee->priority);
}

struct double_cee double_cee_from_int_cee(struct int_cee* my_cee){
  int i,j;
  struct double_cee my_new_cee;
  
  my_new_cee.no_students = my_cee->no_students;
  my_new_cee.no_schools = my_cee->no_schools;
  
  my_new_cee.quotas = malloc(my_cee->no_schools * sizeof(double));
  for (j = 1; j <= my_new_cee.no_schools; j++) {
    my_new_cee.quotas[j-1] = (double) my_cee->quotas[j-1];
  }
  
  my_new_cee.priority = malloc(my_cee->no_students * sizeof(int*));
  for (i = 1; i <= my_cee->no_students; i++) {
  my_new_cee.priority[i-1] = malloc(my_cee->no_schools * sizeof(int));
    for (j = 1; j <= my_new_cee.no_schools; j++) {
      my_new_cee.priority[i-1][j-1] =
	my_cee->priority[i-1][j-1]; 
    }
  }
  
  return my_new_cee;
}

struct double_cee make_toy_double_cee() {
  struct int_cee my_new_int_cee = make_toy_cee();
  struct double_cee my_new_double_cee = double_cee_from_int_cee(&my_new_int_cee);
  destroy_int_cee(&my_new_int_cee);
  return my_new_double_cee;
}

void print_int_cee(struct int_cee* my_cee) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");
  
  printf("There are %d students and %d schools\n", my_cee->no_students, my_cee->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < my_cee->no_schools; i++) {
    printf("%d,",my_cee->quotas[i-1]);
  }
  printf("%d)\n",my_cee->quotas[my_cee->no_schools-1]);
  
  
  printf("The priority matrix is");
    for (i = 1; i <= my_cee->no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_cee->no_schools; j++) {
      printf("     %d", my_cee->priority[i-1][j-1]);
    }
  }
    printf("\n");
}

void print_double_cee(struct double_cee* my_cee) {
  int i,j;
  printf("/* This is a sample introductory comment. */\n");
  printf("There are %d students and %d schools.\n", my_cee->no_students, my_cee->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < my_cee->no_schools; i++) {
    printf("%1.2f,",my_cee->quotas[i-1]);
  }
  printf("%1.2f)\n",my_cee->quotas[my_cee->no_schools-1]);
  
  printf("The priority matrix is");
    for (i = 1; i <= my_cee->no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_cee->no_schools; j++) {
      printf("     %d", my_cee->priority[i-1][j-1]);
    }
  }
    printf("\n");
}

void sub_double_cee(struct double_cee* given_cee, struct double_cee* sub_cee,
		    int no_stu, int no_sch, int* stu_index, int* sch_index) {
  int i,j;
  sub_cee->no_students = no_stu;
  sub_cee->no_schools = no_sch;
  
  sub_cee->quotas = malloc(no_sch * sizeof(double));
  for (j = 1; j <= no_sch; j++) {
    sub_cee->quotas[j-1] = given_cee->quotas[sch_index[j-1]-1];
  }
  
  sub_cee->priority = malloc(no_stu * sizeof(int*));
  for (i = 1; i <= no_stu; i++) {
    sub_cee->priority[i-1] = malloc(no_sch * sizeof(int));
      for (j = 1; j <= no_sch; j++) {
	sub_cee->priority[i-1][j-1] = given_cee->priority[stu_index[i-1]-1][sch_index[j-1]-1];
      }
  }
}


int minimum_gmc_inequality(struct double_cee* my_cee, struct subset* school_subset) {
  int i,j;
  double school_sum = 0;
  double student_sum = 0;
  struct subset student_subset = nullset(my_cee->no_students);  

  for (i = 1; i <= my_cee->no_students; i++) {
    student_subset.indicator[i-1] = 1;
    j = 1; 
    while (student_subset.indicator[i-1] == 1 && j <= my_cee->no_schools) {
      if (school_subset->indicator[j] == 0 && my_cee->priority[i-1][j-1] == 1) {
      student_subset.indicator[i-1] = 0;
      }
      j++;      
    }
  }
    
  for (j = 1; j <= my_cee->no_students; j++) {
    if (school_subset->indicator[j-1] == 1) {
      school_sum += my_cee->quotas[j-1];
    }
  }

  destroy_subset(&student_subset);

  if (student_sum <= school_sum) {
    return 1;
  }
  else {
    return 0;
  }
}


int gmc_holds(struct double_cee* my_cee) {
  int answer = 1;
  struct subset school_subset = nullset(my_cee->no_schools);

  while (answer == 1 && school_subset.subset_size < my_cee->no_schools) {
    iterate(&school_subset);
    if (!minimum_gmc_inequality(my_cee,&school_subset)) {
      answer = 0;
    }
  }

  destroy_subset(&school_subset);

  return answer;
}
