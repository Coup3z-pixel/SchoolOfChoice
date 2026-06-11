#include "pairlist.h"

void add_pair_to_stu_sch_pair_list(stu_sch_pair_list_node** list, int stu, int sch) {
  stu_sch_pair_list_node* head;
    
  if (*list == NULL) {
    *list = malloc(sizeof(stu_sch_pair_list_node));
    (*list)->stu = stu;
    (*list)->sch = sch;
    (*list)->next = NULL;
  }
  else {
    head = malloc(sizeof(stu_sch_pair_list_node));
    head->stu = stu;
    head->sch = sch;
    head->next = *list;
    *list = head;   
  }
}

void remove_pair_from_stu_sch_pair_list(stu_sch_pair_list_node** list, int i, int j) {
  int found;
  stu_sch_pair_list_node* probe;
  stu_sch_pair_list_node* trailer;

  probe = *list;
  trailer = probe;
  found = 0;

  while (probe != NULL && !found) {
    if (probe->stu == i && probe->sch == j) {
      found = 1;
      if (trailer != probe) {
	trailer->next = probe->next;
      }
      else {
	*list = probe->next;
      }
      free(probe);
    }
    else {
      if (trailer != probe) {
	trailer = probe;
      }
      probe = probe->next;
    }
  }
}

int list_contains_pair(stu_sch_pair_list_node* list, int stu, int sch) {
  
  while (list != NULL) {
    
    if (list->stu == stu && list->sch == sch) {
      return 1;
    }
    
    list = list->next;
  }

  return 0;
}

int is_a_subset(stu_sch_pair_list_node* poss_subset, stu_sch_pair_list_node* set) {
  while (poss_subset != NULL) {
    if (!list_contains_pair(set, poss_subset->stu, poss_subset->sch)) {
      return 0;
    }
    poss_subset = poss_subset->next;
  }

  return 1;
}

int is_a_proper_subset(stu_sch_pair_list_node* poss_subset, stu_sch_pair_list_node* set) {
  if (!is_a_subset(poss_subset, set)) {
    return 0;
  }
  
  while (set != NULL) {
    if (!list_contains_pair(poss_subset, set->stu, set->sch)) {
      return 1;
    }
    set = set->next;
  }

  return 0;
}

int length_of_list(stu_sch_pair_list_node* list) {
  int answer;

  if (list == NULL) {
    answer = 0;
  }
  else {
    answer = 1;
    while (list != NULL) {
      answer++;
      list = list->next;
    }
  }

  return answer;
}

stu_sch_pair_list_node* copy_of_list_element_n(stu_sch_pair_list_node* list, int n) {
  int count;

  stu_sch_pair_list_node* answer;

  count = 1;
  while (count < n) {
    list = list->next;
    count++;
  }

  answer = malloc(sizeof(stu_sch_pair_list_node));
  answer->stu = list->stu;
  answer->sch = list->sch;
  answer->next = NULL;

  return answer;
}

stu_sch_pair_list_node* copy_of_stu_sch_pair_list(stu_sch_pair_list_node* list) {
  stu_sch_pair_list_node* answer;
  stu_sch_pair_list_node* cursor;

  if (list == NULL) {
    answer = NULL;
  }
  else {
    answer = malloc(sizeof(stu_sch_pair_list_node));
    answer->stu = list->stu;
    answer->sch = list->sch;
    answer->next = NULL;
    cursor = answer;
    while (list->next != NULL) {
      list = list->next;
      cursor->next = malloc(sizeof(stu_sch_pair_list_node));
      cursor = cursor->next;
      cursor->stu = list->stu;
      cursor->sch = list->sch;
      cursor->next = NULL;
    }
  }

  return answer;
}

stu_sch_pair_list_node* concatenate(stu_sch_pair_list_node* first, stu_sch_pair_list_node* second)
{
  stu_sch_pair_list_node* cursor;
  
  if (first == NULL) {
    return second;
  }
  else {
    cursor = first;
    while (cursor->next != NULL) {
      cursor = cursor->next;
    }
    cursor->next = second;
    return first;
  }
}

void fprint_pair_list(stu_sch_pair_list_node* list) {
  while (list != NULL) {
    fprintf(stderr, "(%i,%i) ", list->stu, list->sch);
    list = list->next;
  }
}

void destroy_stu_sch_pair_list(stu_sch_pair_list_node** list) {
  stu_sch_pair_list_node* trailing_node;

  if (*list != NULL) {
    if ((*list)->next == NULL) {
      free(*list);
    }
    else {
      trailing_node = *list;
      while ((*list)->next != NULL) {
	trailing_node = *list;
	*list = (*list)->next;
	free(trailing_node);
      }
      free(*list);
    }
  }

  *list = NULL;
}
