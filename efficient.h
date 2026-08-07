#ifndef EFFICIENT_H
#define EFFICIENT_H

#include "dgraph.h"
#include "partalloc.h"

/*
  This implementation is based on the idea of a directed graph with
arcs going from student-school pairs, where the student is willing to
give up some of the school, to pairs where the student is getting
something better.  We will repeatedly reduce this graph, by
eliminating nodes with either no arcs in or no arcs out, until no
further reductions are possible.  If the reduction is complete, the
allocation is efficient, and otherwise we get information concerning
what trades are possible.
*/

/**********************/

dgraph* initialize_nodes(partial_alloc* myalloc, process_scp* myscp);

int i_is_justifiably_envied_for_l(partial_alloc* myalloc, process_scp* myscp, int i, int l);

int is_ex_ante_stable(partial_alloc* myalloc, process_scp* myscp);

void add_arcs(dgraph* graph, partial_alloc* myalloc, process_scp* myscp, int ex_ante_stable); 

dgraph* reduced_graph(partial_alloc* myalloc, process_scp* myscp, int ex_ante_stable);

int someone_would_like_some_j(int j, partial_alloc* myalloc, process_scp* myscp);

int allocation_is_wasteful(partial_alloc* myalloc, process_scp* myscp);

int allocation_is_efficient(partial_alloc* myalloc, process_scp* myscp, int ex_ante_stable);

double cyclic_trade(partial_alloc* myalloc, stu_sch_pair_list_node* loop);

int rank_gain_of_loop(process_scp* myscp, stu_sch_pair_list_node* loop);

void make_alloc_eff_and_par_dominant(partial_alloc* my_alloc, process_scp* myscp,
				     int ex_ante_stable);

/*****************************/

void fprint_arc(int i, int j, int k, int l);

#endif /* EFFICIENT_H */
