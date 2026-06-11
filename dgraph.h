#ifndef DGRAPH_H
#define DGRAPH_H

#include "normal.h"
#include "pairlist.h"

typedef struct Graph_Node {
  int stu;
  int sch;
  stu_sch_pair_list_node* sources;
  stu_sch_pair_list_node* targets;
} graph_node;

typedef struct Graph_Node_List_Node {
  graph_node* node;
  struct Graph_Node_List_Node* next;
} graph_node_list_node;

typedef struct Dgraph {
  graph_node_list_node* listptr;
  struct Dgraph* next;
} dgraph;

typedef struct List_of_Dgraphs {
  dgraph* component;
  struct List_of_Dgraphs* next;
} list_of_dgraphs;


/********************/

graph_node* new_graph_node(int i, int j);

graph_node* copy_of_graph_node(graph_node* given);

graph_node_list_node* copy_of_graph_node_list_node(graph_node_list_node* given);

/********************/

int number_of_nodes(dgraph* graph);

stu_sch_pair_list_node* node_of_number(dgraph* graph, int node_no);

int graph_contains_stu_sch_pair(dgraph* graph, int stu, int sch);

graph_node* node_of_pair(dgraph* graph, int stu, int sch);

int graph_contains_node(dgraph* graph, graph_node* node);

int all_arcs_are_in_loops(dgraph* graph);

int has_only_loops(dgraph* graph);

int is_a_loop(dgraph* graph);

stu_sch_pair_list_node* random_node(dgraph* graph);

stu_sch_pair_list_node* random_loop(dgraph* graph);

void report_on_the_directed_graph(dgraph* graph);

/********************/

dgraph* copy_of_graph(dgraph* given);

dgraph* subgraph_of_stu_sch_list(dgraph* given, stu_sch_pair_list_node* list);

dgraph* subgraph_of_pairs_outside_stu_sch_list(dgraph* given, stu_sch_pair_list_node* list);

graph_node* node_of_pair(dgraph* graph, int i, int j);

int next_student(dgraph* loop, int stu, int sch);

int next_school(dgraph* loop, int stu, int sch);

stu_sch_pair_list_node* nodes_in_graph(dgraph* graph);

/*******************/

stu_sch_pair_list_node* pairs_connected_to_pair(dgraph* graph, int stu, int sch);

dgraph* component_of_node(dgraph* graph, graph_node* node);

list_of_dgraphs* components(dgraph* graph);

int number_of_components(list_of_dgraphs* graph);

void report_on_the_components(dgraph* graph);

/**********************/

stu_sch_pair_list_node* indirect_desire_set_of_pair(dgraph* graph, int stu, int sch);

stu_sch_pair_list_node* a_minimum_trading_set_list(dgraph* graph);

dgraph* a_minimum_trading_graph(dgraph* graph);

list_of_dgraphs* trading_set_decomposition(dgraph* graph);

void report_on_trade_set_decomp(dgraph* graph);

/********************/

int graph_contains_stu_sch_pair(dgraph* graph, int stu, int sch);

int graph_contains_node(dgraph* graph, graph_node* node);

dgraph* copy_of_graph(dgraph* given);

dgraph* subgraph_of_stu_sch_list(dgraph* given, stu_sch_pair_list_node* list);

dgraph* subgraph_of_pairs_outside_stu_sch_list(dgraph* given, stu_sch_pair_list_node* list);

graph_node* node_of_pair(dgraph* graph, int i, int j);

stu_sch_pair_list_node* nodes_in_graph(dgraph* graph);

/**************************/

void remove_pair_from_source_and_target_lists(dgraph* graph, int stu, int sch);

void remove_node_from_graph(dgraph** list, graph_node* target);

void remove_node_of_pair_from_graph(dgraph** graph, int stu, int sch);

void remove_nodes_of_list_from_graph(dgraph** given, stu_sch_pair_list_node* list);

void remove_node_from_list_of_graph_node_lists(dgraph** graph, dgraph* target);

void remove_nodes_and_arcs_outside_loops(dgraph** graph);

void remove_nodes_with_no_arcs(dgraph** graph);

/************************/

void destroy_graph_node(graph_node* node);

void destroy_graph_node_list(graph_node_list_node* list);

void destroy_dgraph(dgraph* graph);

void destroy_list_of_dgraphs(list_of_dgraphs* list);


#endif /* DGRAPH_H */
