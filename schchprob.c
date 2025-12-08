#include "schchprob.h"

int get_input_priority(struct input_sch_ch_prob* myiscp, int i, int j){
      return myiscp->priorities[i-1][j-1];
}

int get_priority(struct process_scp* myscp, int i, int j) {
  return int_entry(&(myscp->priorities), i, j);
}

int maximum_priority(struct process_scp* myscp) {
  int i, j, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;
  
  int answer;

  answer = 0;
  for (i = 1; i <= nst; i++) {
    for (j = 1; j <= nsc; j++) {
      answer = int_max(answer, get_priority(myscp, i, j));
    }
  }

  return answer;
}

int* get_favorites(struct process_scp* working_scp) {
  int i;
  int* favorites;

  favorites = malloc(working_scp->no_students * sizeof(int));

  for (i = 1; i <= working_scp->no_students; i++) {
    favorites[i-1] = working_scp->preferences[i-1][0];
  }
  
  return favorites;
}

int is_eligible(struct process_scp* myscp, int i, int j) {
  int k;

  for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
    if (myscp->preferences[i-1][k-1] == j) {
      return 1;
    }
  }

  return 0;
}

int inp_safe_school(struct input_sch_ch_prob* myiscp, int i) {
  return myiscp->preferences[i-1][myiscp->no_eligible_schools[i-1]-1];
}

int safe_school(struct process_scp* myscp, int i) {
  return myscp->preferences[i-1][myscp->no_eligible_schools[i-1]-1];
}

int student_ranking_of_school(struct process_scp* myscp, int i, int j) {
  int k;

  for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
    if (myscp->preferences[i-1][k-1] == j) {
      return k;
    }
  }

  return 0;
}

int safe_schools_are_safe(struct input_sch_ch_prob* myiscp) {
  int i, j, nst, nsc, top_pr, no_safe_st;

  nst = myiscp->no_students;
  nsc = myiscp->no_schools;

  for (j = 1; j <= nsc; j++) {
    top_pr = get_input_priority(myiscp, 1, j);
    for (i = 2; i <= nst; i++) {
      top_pr = max(top_pr, get_input_priority(myiscp, i, j));
    }

    no_safe_st = 0;
    for (i = 1; i <= nst; i++) {
      if (get_input_priority(myiscp, i, j) == top_pr) {
	no_safe_st++;
      }
    }

    if (no_safe_st > myiscp->quotas[j-1]) {
      fprintf(stderr, "At school %i the quota is %i and no_safe_st is %i.\n",
	      j, myiscp->quotas[j-1], no_safe_st);
      return 0;
    }
  }

  return 1;
}

struct int_sparse_matrix new_int_sp_mat(struct process_scp* myscp) {
  int i, k, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  struct int_sparse_matrix answer;

  answer.no_rows = nst;
  answer.no_cols = nsc;
  
  answer.nos_active_cols = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    answer.nos_active_cols[i-1] = myscp->no_eligible_schools[i-1];
  }
  
  answer.index_of_active_cols = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.index_of_active_cols[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(int*));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.index_of_active_cols[i-1][k-1] = myscp->preferences[i-1][k-1];
    }
  }
  
  answer.entries = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.entries[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.entries[i-1][k-1] = 0;
    }
  }

  return answer;
} 

struct dbl_sparse_matrix new_dbl_sp_mat_for_process(struct process_scp* myscp) {
  int i, k, swap, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  struct dbl_sparse_matrix answer;

  answer.no_rows = nst;
  answer.no_cols = nsc;
  
  answer.nos_active_cols = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    answer.nos_active_cols[i-1] = myscp->no_eligible_schools[i-1];
  }
  
  answer.index_of_active_cols = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.index_of_active_cols[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.index_of_active_cols[i-1][k-1] = myscp->preferences[i-1][k-1];
    }
    k = 1;
    while (k < answer.nos_active_cols[i-1]) {
      if (answer.index_of_active_cols[i-1][k-1] > answer.index_of_active_cols[i-1][k]) {
	swap = answer.index_of_active_cols[i-1][k-1];
	answer.index_of_active_cols[i-1][k-1] = answer.index_of_active_cols[i-1][k];
	answer.index_of_active_cols[i-1][k] = swap;
	if (k == 1) {
	  k++;
	}
	else {
	  k--;
	}
      }
      else {
	k++;
      }
    }
  }
  
  answer.entries = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    answer.entries[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(double));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.entries[i-1][k-1] = 0.0;
    }
  }

  return answer;
}

struct dbl_sparse_matrix new_dbl_sp_mat_for_input(struct input_sch_ch_prob* myscp) {
  int i, k, swap, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  struct dbl_sparse_matrix answer;

  answer.no_rows = nst;
  answer.no_cols = nsc;
  
  answer.nos_active_cols = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    answer.nos_active_cols[i-1] = myscp->no_eligible_schools[i-1];
  }
  
  answer.index_of_active_cols = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.index_of_active_cols[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.index_of_active_cols[i-1][k-1] = myscp->preferences[i-1][k-1];
    }
    k = 1;
    while (k < answer.nos_active_cols[i-1]) {
      if (answer.index_of_active_cols[i-1][k-1] > answer.index_of_active_cols[i-1][k]) {
	swap = answer.index_of_active_cols[i-1][k-1];
	answer.index_of_active_cols[i-1][k-1] = answer.index_of_active_cols[i-1][k];
	answer.index_of_active_cols[i-1][k] = swap;
	if (k == 1) {
	  k++;
	}
	else {
	  k--;
	}
      }
      else {
	k++;
      }
    }
  }
  
  answer.entries = malloc(nst * sizeof(double*));
  for (i = 1; i <= nst; i++) {
    answer.entries[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(double));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.entries[i-1][k-1] = 0.0;
    }
  }

  return answer;
}

struct int_sparse_matrix new_int_sp_mat_for_input(struct input_sch_ch_prob* myscp) {
  int i, k, swap, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  struct int_sparse_matrix answer;

  answer.no_rows = nst;
  answer.no_cols = nsc;
  
  answer.nos_active_cols = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    answer.nos_active_cols[i-1] = myscp->no_eligible_schools[i-1];
  }
  
  answer.index_of_active_cols = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.index_of_active_cols[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.index_of_active_cols[i-1][k-1] = myscp->preferences[i-1][k-1];
    }
    k = 1;
    while (k < answer.nos_active_cols[i-1]) {
      if (answer.index_of_active_cols[i-1][k-1] > answer.index_of_active_cols[i-1][k]) {
	swap = answer.index_of_active_cols[i-1][k-1];
	answer.index_of_active_cols[i-1][k-1] = answer.index_of_active_cols[i-1][k];
	answer.index_of_active_cols[i-1][k] = swap;
	if (k == 1) {
	  k++;
	}
	else {
	  k--;
	}
      }
      else {
	k++;
      }
    }
  }
  
  answer.entries = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.entries[i-1] = malloc(myscp->no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      answer.entries[i-1][k-1] = 0;
    }
  }

  return answer;
}

struct process_scp process_scp_from_input(struct input_sch_ch_prob* myscp) {
  
  struct process_scp new_scp;

  int i, j, k;

  int nst = myscp->no_students;
  int nsc = myscp->no_schools;

  new_scp.no_students = nst;
  new_scp.no_schools = nsc;

  new_scp.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.quotas[j-1] = (double)myscp->quotas[j-1];
  }

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = myscp->no_eligible_schools[i-1];
  }
  
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    for (k = 1; k <= myscp->no_eligible_schools[i-1]; k++) {
      new_scp.preferences[i-1][k-1] = myscp->preferences[i-1][k-1];
    }
  }

  new_scp.priorities = sparse_priorities(myscp);

  new_scp.time_remaining = 1.0;

  return new_scp;
} 

struct int_sparse_matrix sparse_priorities(struct input_sch_ch_prob* myscp) {
  int i, k, l, nst, nsc;

  struct int_sparse_matrix answer;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  answer.no_rows = nst;
  answer.no_cols = nsc;

  answer.nos_active_cols = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    answer.nos_active_cols[i-1] = myscp->no_eligible_schools[i-1];
  }

  answer.index_of_active_cols = malloc(nst * sizeof(int*));
  answer.entries = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    answer.index_of_active_cols[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(int));
    answer.entries[i-1] = malloc(answer.nos_active_cols[i-1] * sizeof(int));
    for (k = 1; k <= answer.nos_active_cols[i-1]; k++) {
      l = myscp->preferences[i-1][k-1];
      answer.index_of_active_cols[i-1][k-1] = l;
      answer.entries[i-1][k-1] = myscp->priorities[i-1][l-1];
    }
  }

  return answer;
}

struct process_scp left_sub_process_scp(struct process_scp* myscp, struct subset* J_subset,
					      struct subset* P_subset) {
  struct process_scp new_scp;
  
  int i, j, k, l, p, count;

  struct index J_index, P_index;
  J_index = index_of_subset(J_subset);
  P_index = index_of_subset(P_subset);

  int* reverse_P_index = malloc(myscp->no_schools * sizeof(int));
  count = 0;

  for (j = 1; j <= myscp->no_schools; j++) {
    if (P_subset->indicator[j-1] == 1) {
      count++;
      reverse_P_index[j-1] = count;
    }
    else {
      reverse_P_index[j-1] = 0;
    }
  }
  
  int nst = J_subset->subset_size;
  int nsc = P_subset->subset_size;

  new_scp.no_students = nst;
  new_scp.no_schools = nsc;

  new_scp.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.quotas[j-1] = myscp->quotas[P_index.indices[j-1]-1];
  }

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = 0;
    for (l = 1; l <= myscp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_subset->indicator[myscp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.no_eligible_schools[i-1]++;
      }
    }
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    count = 0;
    for (l = 1; l <= myscp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_subset->indicator[myscp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.preferences[i-1][count] =
	  reverse_P_index[myscp->preferences[J_index.indices[i-1]-1][l-1]-1];
	count++;
      }
    }
  }

  new_scp.priorities = new_int_sp_mat(&new_scp);
  for (i = 1; i <= nst; i++ ) {
    for (k = 1; k <= new_scp.no_eligible_schools[i-1]; k++) {
      l = new_scp.preferences[i-1][k-1];
      p = get_priority(myscp, J_index.indices[i-1], P_index.indices[l-1]);
      new_scp.priorities.entries[i-1][k-1] = p;
    }
  }

  new_scp.time_remaining = myscp->time_remaining;

  destroy_index(J_index);
  destroy_index(P_index);
  free(reverse_P_index); 

  return new_scp;    
}

struct process_scp right_sub_process_scp(struct process_scp* myscp, struct subset* J_subset,
					      struct subset* P_subset) {
  struct process_scp new_scp;
  
  int i, j, k, l, p, count;

  struct subset J_compl, P_compl;
  J_compl = complement_of_subset(J_subset);
  P_compl = complement_of_subset(P_subset);
  struct index J_index, P_index;
  J_index = index_of_subset(&J_compl);
  P_index = index_of_subset(&P_compl);

  int* reverse_P_index = malloc(myscp->no_schools * sizeof(int));
  count = 0;
  for (j = 1; j <= myscp->no_schools; j++) {
    if (P_compl.indicator[j-1] == 1) {
      count++;
      reverse_P_index[j-1] = count;
    }
    else {
      reverse_P_index[j-1] = 0;
    }
  }
  
  int nst = J_compl.subset_size;
  int nsc = P_compl.subset_size;

  new_scp.no_students = nst;
  new_scp.no_schools = nsc;

  new_scp.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    new_scp.quotas[j-1] = myscp->quotas[P_index.indices[j-1]-1];
  }

  new_scp.no_eligible_schools = malloc(nst * sizeof(int));
  new_scp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    new_scp.no_eligible_schools[i-1] = 0;
    for (l = 1; l <= myscp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[myscp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.no_eligible_schools[i-1]++;
      }
    }
    new_scp.preferences[i-1] = malloc(new_scp.no_eligible_schools[i-1] * sizeof(int));
    count = 0;
    for (l = 1; l <= myscp->no_eligible_schools[J_index.indices[i-1]-1]; l++) {
      if (P_compl.indicator[myscp->preferences[J_index.indices[i-1]-1][l-1]-1] == 1) {
	new_scp.preferences[i-1][count] =
	  reverse_P_index[myscp->preferences[J_index.indices[i-1]-1][l-1]-1];
	count++;
      }
    }
  }

  new_scp.priorities = new_int_sp_mat(&new_scp);
  for (i = 1; i <= nst; i++ ) {
    for (k = 1; k <= new_scp.no_eligible_schools[i-1]; k++) {
      l = new_scp.preferences[i-1][k-1];
      p = get_priority(myscp, J_index.indices[i-1], P_index.indices[l-1]);
      new_scp.priorities.entries[i-1][k-1] = p;
    }
  }

  new_scp.time_remaining = myscp->time_remaining;

  destroy_subset(J_compl);
  destroy_subset(P_compl);
  destroy_index(J_index);
  destroy_index(P_index);
  free(reverse_P_index); 

  return new_scp;    
}

struct process_scp reduced_scp(struct process_scp* myscp, int* coarse_cutoffs) {
  int i, j, k, l, p, safe_school_index, hit, cursor, nst, nsc;
  struct process_scp answer;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  answer.no_students = nst;
  answer.no_schools = nsc;

  answer.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer.quotas[j-1] = myscp->quotas[j-1];
  }

  answer.no_eligible_schools = malloc(nst * sizeof(int));
  answer.preferences = malloc(nst * sizeof(int*));
  
  for (i = 1; i <= nst; i++) {
    safe_school_index = myscp->no_eligible_schools[i-1];
    hit = 0;
    for (k = 1; k < myscp->no_eligible_schools[i-1] && !hit; k++) {
      l = myscp->preferences[i-1][k-1];
      if (get_priority(myscp, i, l) > coarse_cutoffs[l-1]) {
	safe_school_index = k;
	hit = 1;
      }
    }
    
    answer.no_eligible_schools[i-1] = 0;
    for (k = 1; k <= safe_school_index; k++) {
      l = myscp->preferences[i-1][k-1];
      if (get_priority(myscp, i, l) >= coarse_cutoffs[l-1]) {
	answer.no_eligible_schools[i-1]++;
      }
    }
    answer.preferences[i-1] = malloc(answer.no_eligible_schools[i-1] * sizeof(int));
    cursor = 0;
    for (k = 1; k <= safe_school_index; k++) {
      l = myscp->preferences[i-1][k-1];
      if (get_priority(myscp, i, l) >= coarse_cutoffs[l-1]) {
	cursor++;
	answer.preferences[i-1][cursor-1] = l;
      }
    }
  }

  answer.priorities = new_int_sp_mat(&answer);
  for (i = 1; i <= nst; i++ ) {
    for (k = 1; k <= answer.no_eligible_schools[i-1]; k++) {
      l = answer.preferences[i-1][k-1];
      p = get_priority(myscp, i, l);
      answer.priorities.entries[i-1][k-1] = p;
    }
  }

  answer.time_remaining = myscp-> time_remaining;

  return answer;
}

struct input_sch_ch_prob reduced_input_scp(struct input_sch_ch_prob* myiscp, int* coarse_cutoffs) {
  int i, j, k, l, p, safe_school_index, hit, cursor, nst, nsc;
  struct input_sch_ch_prob answer;

  nst = myiscp->no_students;
  nsc = myiscp->no_schools;

  answer.no_students = nst;
  answer.no_schools = nsc;

  answer.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer.quotas[j-1] = myiscp->quotas[j-1];
  }

  answer.no_eligible_schools = malloc(nst * sizeof(int));
  answer.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    safe_school_index = myiscp->no_eligible_schools[i-1];
    hit = 0;
    for (k = 1; k < myiscp->no_eligible_schools[i-1] && !hit; k++) {
      l = myiscp->preferences[i-1][k-1];
      if (get_input_priority(myiscp, i, l) > coarse_cutoffs[l-1]) {
	safe_school_index = k;
	hit = 1;
      }
    }
    
    answer.no_eligible_schools[i-1] = 0;
    for (k = 1; k <= safe_school_index; k++) {
      l = myiscp->preferences[i-1][k-1];
      if (get_input_priority(myiscp, i, l) >= coarse_cutoffs[l-1]) {
	answer.no_eligible_schools[i-1]++;
      }
    }
    answer.preferences[i-1] = malloc(answer.no_eligible_schools[i-1] * sizeof(int));
    cursor = 0;
    for (k = 1; k <= safe_school_index; k++) {
      l = myiscp->preferences[i-1][k-1];
      if (get_input_priority(myiscp, i, l) >= coarse_cutoffs[l-1]) {
	cursor++;
	answer.preferences[i-1][cursor-1] = l;
      }
    }
  }

  answer.priorities = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++ ) {
    answer.priorities[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      answer.priorities[i-1][j-1] = 0;
    }
    for (k = 1; k <= answer.no_eligible_schools[i-1]; k++) {
      j = answer.preferences[i-1][k-1];
      p = get_input_priority(myiscp, i, j);
      answer.priorities[i-1][j-1] = p;
    }
  }

  return answer;
}

struct input_sch_ch_prob copy_of_input_scp(struct input_sch_ch_prob* myiscp) {
  int i, j, k, nst, nsc;

  struct input_sch_ch_prob answer;

  nst = myiscp->no_students;
  nsc = myiscp->no_schools;

  answer.no_students = nst;
  answer.no_schools = nsc;

  answer.quotas = malloc(nsc * sizeof(double));
  for (j = 1; j <= nsc; j++) {
    answer.quotas[j-1] = myiscp->quotas[j-1];
  }
  
  answer.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    (answer.no_eligible_schools)[i-1] = myiscp->no_eligible_schools[i-1];
  }

  answer.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    (answer.preferences)[i-1] = malloc((answer.no_eligible_schools)[i-1] * sizeof(int));
    for (k = 1; k <= (myiscp->no_eligible_schools)[i-1]; k++) {
      (answer.preferences)[i-1][k-1] = myiscp->preferences[i-1][k-1];
    }
  }

  answer.priorities = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++ ) {
    answer.priorities[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      answer.priorities[i-1][j-1] = myiscp->priorities[i-1][j-1];
    }
  }

  return answer;
}

struct input_sch_ch_prob stu_no_priority_scp(struct input_sch_ch_prob* myiscp) {
  int i, j, k, nst, nsc;
  
  struct input_sch_ch_prob answer;

  answer = copy_of_input_scp(myiscp);

  nst = answer.no_students;
  nsc = answer.no_schools;

  for (i = 1; i <= nst; i++ ) {
    for (j = 1; j <= nsc; j++) {
      answer.priorities[i-1][j-1] = i;
    }
    k = answer.no_eligible_schools[i-1];
    j = answer.preferences[i-1][k-1];
    answer.priorities[i-1][j-1] = nst + 1;
  }

  return answer;
}

struct input_sch_ch_prob make_toy_sch_ch_prob() {
  int i,j;
  struct input_sch_ch_prob myscp;
  
  
  myscp.no_students = 4;
  myscp.no_schools = 3;
  
  myscp.quotas = malloc(3 * sizeof(int));
  myscp.quotas[0] = 1;
  myscp.quotas[1] = 2;
  myscp.quotas[2] = 1;
   
  int nst = myscp.no_students;
  int nsc = myscp.no_schools;

  myscp.no_eligible_schools = malloc(nst * sizeof(int));
  for (i = 1; i <= nst; i++) {
    myscp.no_eligible_schools[i-1] = nsc;
  }

  myscp.preferences = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    myscp.preferences[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      myscp.preferences[i-1][j-1] = j;
    }
  }

  myscp.priorities = malloc(nst * sizeof(int*));
  for (i = 1; i <= nst; i++) {
    myscp.priorities[i-1] = malloc(nsc * sizeof(int));
    for (j = 1; j <= nsc; j++) {
      myscp.priorities[i-1][j-1] = 0;
    }
  }

  return myscp;
}

struct process_scp make_toy_process_scp() {
  struct process_scp myscp;

  myscp.no_eligible_schools = malloc(4 * sizeof(int));
  myscp.no_eligible_schools[0] = 3;
  myscp.no_eligible_schools[1] = 1;
  myscp.no_eligible_schools[2] = 3;
  myscp.no_eligible_schools[3] = 3;

  myscp.preferences = malloc(4 * sizeof(int*));
  myscp.preferences[0] = malloc(3 * sizeof(int));
  myscp.preferences[1] = malloc(1 * sizeof(int));
  myscp.preferences[2] = malloc(3 * sizeof(int));
  myscp.preferences[3] = malloc(3 * sizeof(int));

  myscp.preferences[0][0] = 1;
  myscp.preferences[1][0] = 1;
  myscp.preferences[2][0] = 1;
  myscp.preferences[3][0] = 1;

  myscp.preferences[0][1] = 2;
  myscp.preferences[2][1] = 2;
  myscp.preferences[3][1] = 2;

  myscp.preferences[0][2] = 3;
  myscp.preferences[2][2] = 3;
  myscp.preferences[3][2] = 3;

  myscp.priorities = new_int_sp_mat(&myscp);

  return myscp;
}

void print_generic_header() {
  printf("/* This is a sample introductory comment. */\n");
}

void print_input_scp_body(struct input_sch_ch_prob* myscp) {
  int i, j, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;
  
  printf("There are %d students and %d schools\n", myscp->no_students, myscp->no_schools);
    
  printf("The vector of quotas is (");
  for (i = 1; i < myscp->no_schools; i++) {
    printf("%d,",myscp->quotas[i-1]);
  }
  printf("%d)\n",myscp->quotas[myscp->no_schools-1]);
  
  /*  printf("\n"); */
  
  printf("The priority matrix is\n");
  for (i = 1; i <= nst; i++) {
    printf("    ");
    for (j = 1; j <= nsc; j++) {
      printf("%i    ",  get_input_priority(myscp, i, j));
    }
    printf("\n");
  }

  printf("The students numbers of ranked schools are\n");
  printf("(");
  for (i = 1; i <= nst - 1; i++) {
    printf("%i,", myscp->no_eligible_schools[i-1]);
  }
    printf("%i)\n", myscp->no_eligible_schools[nst-1]);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= nst; i++) {
    printf("%i:  ", i);
    for (j = 1; j <= myscp->no_eligible_schools[i-1]; j++) {
      printf("%i  ",  myscp->preferences[i-1][j-1]);
    }
    printf("\n");
  }
}

void print_input_sch_ch_prob(struct input_sch_ch_prob* myscp) {
  print_generic_header();
  print_input_scp_body(myscp);
}

void print_process_scp(struct process_scp* myscp) {
  int i, j, nst, nsc;

  nst = myscp->no_students;
  nsc = myscp->no_schools;

  printf("/* This is a sample introductory comment. */\n");
  
  printf("There are %d students and %d schools\n", nst, nsc);
    
  printf("The vector of quotas is (");
  for (i = 1; i < nsc; i++) {
    printf("%1.3f,",myscp->quotas[i-1]);
  }
  printf("%1.3f)\n",myscp->quotas[nsc-1]);

  printf("The students numbers of ranked schools are ");
  printf("(");
  for (i = 1; i <= nst - 1; i++) {
    printf("%i,", myscp->no_eligible_schools[i-1]);
  }
    printf("%i)\n", myscp->no_eligible_schools[nst-1]);
  
  printf("The preferences of the students are\n");
  for (i = 1; i <= nst; i++) {
    printf("%i:  ", i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= myscp->no_eligible_schools[i-1]; j++) {
      printf("%i  ",  myscp->preferences[i-1][j-1]);
    }
    printf("\n");
  }
  
  printf("The priorities of the schools are\n");
  for (i = 1; i <= nst; i++) {
    printf("%i:  ", i);
    if (i < 10) {
      printf(" ");
    }
    for (j = 1; j <= nsc; j++) {
      printf("%i  ",  get_priority(myscp, i, j));
    }
    printf("\n");
  }
  printf("The time remaining is %1.3f.\n", myscp->time_remaining);
}


void destroy_input_sch_ch_prob(struct input_sch_ch_prob myscp) {
  free(myscp.quotas);
  for (int i = 1; i <= myscp.no_students; i++) {
    free(myscp.priorities[i-1]);
  }
  free(myscp.priorities);
  for (int i = 1; i <= myscp.no_students; i++) {
    free(myscp.preferences[i-1]);
  }
  free(myscp.preferences);
  free(myscp.no_eligible_schools);
}

void destroy_process_scp(struct process_scp myscp) {
  int i;
  
  int nst = myscp.no_students;

  free(myscp.quotas);
  
  free(myscp.no_eligible_schools);
  
  for (i = 1; i <= nst; i++) {
    free(myscp.preferences[i-1]);
  }
  free(myscp.preferences);

  destroy_int_sp_mat(&myscp.priorities);
}
