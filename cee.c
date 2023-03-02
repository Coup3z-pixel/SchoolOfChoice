#include "cee.h"


struct int_cee make_toy_cee() {
  int i,j;
  struct int_cee my_cee;
  char str[20] = "My Little CEE";
  strcpy(my_cee.name,str);
  my_cee.no_students = 4;
  my_cee.no_schools = 3;
  
  my_cee.requirements = malloc(4 * sizeof(int));
  for (i = 1; i <= my_cee.no_students; i++) {
    my_cee.requirements[i-1] = 1;
  }
  
  my_cee.quotas = malloc(3 * sizeof(int));
  my_cee.quotas[0] = 1;
  my_cee.quotas[1] = 2;
  my_cee.quotas[2] = 1;
  
  my_cee.eligibility = malloc(12 * sizeof(int));
  for (i = 1; i <= my_cee.no_students; i++) {
    for (j = 1; j <= my_cee.no_schools; j++) {
      my_cee.eligibility[(i - 1) * my_cee.no_schools + j - 1] = 1; 
    }
  }
  
  return my_cee;
}

void destroy_int_cee(struct int_cee my_cee) {
  free(my_cee.requirements);
  free(my_cee.quotas);
  free(my_cee.eligibility);
}

void destroy_double_cee(struct double_cee my_cee) {
  free(my_cee.requirements);
  free(my_cee.quotas);
  free(my_cee.eligibility);
}

struct double_cee double_cee_from_int_cee(struct int_cee my_cee){
  int i,j;
  struct double_cee my_new_cee;
  char str[20] = "My Little CEE";
  strcpy(my_new_cee.name,str);
  my_new_cee.no_students = my_cee.no_students;
  my_new_cee.no_schools = my_cee.no_schools;
  
  my_new_cee.requirements = malloc(my_cee.no_students * sizeof(double));
  for (i = 1; i <= my_new_cee.no_students; i++) {
    my_new_cee.requirements[i-1] = (double) my_cee.requirements[i-1];
  }
  
  my_new_cee.quotas = malloc(my_cee.no_schools * sizeof(double));
  for (i = 1; i <= my_new_cee.no_students; i++) {
    my_new_cee.quotas[i-1] = (double) my_cee.quotas[i-1];
  }
  
  my_new_cee.eligibility = malloc(my_cee.no_students * my_cee.no_schools * sizeof(int));
  for (i = 1; i <= my_cee.no_students; i++) {
    for (j = 1; j <= my_new_cee.no_schools; j++) {
      my_new_cee.eligibility[(i - 1) * my_cee.no_schools + j - 1] =
	my_cee.eligibility[(i - 1) * my_cee.no_schools + j - 1]; 
    }
  }
  
  return my_new_cee;
}

void print_int_cee(struct int_cee my_cee) {
  int i,j;
  printf("%s\n",my_cee.name);
  printf("There are %d students and %d schools.\n", my_cee.no_students, my_cee.no_schools);
  
  printf("The vector of requirements is: (");
  for (i = 1; i < my_cee.no_students; i++) {
    printf("%d,",my_cee.requirements[i-1]);
  }
  printf("%d).\n",my_cee.requirements[my_cee.no_students-1]);
    
  printf("The vector of quotas is: (");
  for (i = 1; i < my_cee.no_schools; i++) {
    printf("%d,",my_cee.quotas[i-1]);
  }
  printf("%d).\n",my_cee.quotas[my_cee.no_schools-1]);
  
  
  printf("The eligibility matrix is:");
    for (i = 1; i <= my_cee.no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_cee.no_schools; j++) {
      printf("     %d", my_cee.eligibility[(i - 1) * my_cee.no_schools + j - 1]);
    }
  }
    printf("\n");
}

void print_double_cee(struct double_cee my_cee) {
  int i,j;
  printf("%s\n",my_cee.name);
  printf("There are %d students and %d schools.\n", my_cee.no_students, my_cee.no_schools);
  
  printf("The vector of requirements is: (");
  for (i = 1; i < my_cee.no_students; i++) {
    printf("%1.2f,",my_cee.requirements[i-1]);
  }
  printf("%1.2f).\n",my_cee.requirements[my_cee.no_students-1]);
    
  printf("The vector of quotas is: (");
  for (i = 1; i < my_cee.no_schools; i++) {
    printf("%1.2f,",my_cee.quotas[i-1]);
  }
  printf("%1.2f).\n",my_cee.quotas[my_cee.no_schools-1]);
  
  printf("The eligibility matrix is:");
    for (i = 1; i <= my_cee.no_students; i++) {
    printf("\n");
    for (j = 1; j <= my_cee.no_schools; j++) {
      printf("     %d", my_cee.eligibility[(i - 1) * my_cee.no_schools + j - 1]);
    }
  }
    printf("\n");
}
