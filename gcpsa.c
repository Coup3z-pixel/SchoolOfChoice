#include <stdio.h>

#include "parser.h"
#include "schchprob.h"
#include "gcpsacode.h"

int main(int argc, char *argv[]) {
  input_sch_ch_prob input_scp;
  partial_alloc gcpsa_alloc;
  process_scp input_process_scp;

  dgraph* graph; 

  if (argc == 1) {
    const char input_file[20] = "schools.scp";
    input_scp = sch_ch_prob_from_file(input_file);
  }
  if (argc == 2) {
    input_scp = sch_ch_prob_from_file(argv[1]);
  }
  if (argc > 2)  {
    fprintf(stderr, "emcc invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }
  
  input_process_scp = process_scp_from_input(&input_scp);

  gcpsa_alloc = gcpsa_allocation(&input_scp);

  if (!allocation_is_efficient(&gcpsa_alloc, &input_process_scp, 0)) {
    fprintf(stderr, "The gcpsa allocation is inefficient.\n");

    graph = reduced_graph(&gcpsa_alloc, &input_process_scp, 0);
    report_on_the_directed_graph(graph); 
    destroy_dgraph(graph);

    graph = reduced_graph(&gcpsa_alloc, &input_process_scp, 0);
    report_on_the_components(graph);
    destroy_dgraph(graph);

    graph = reduced_graph(&gcpsa_alloc, &input_process_scp, 0);
    report_on_trade_set_decomp(graph);
    destroy_dgraph(graph);
  }

  
  print_sparse_partial_alloc(&gcpsa_alloc); 

  destroy_partial_alloc(gcpsa_alloc);
  destroy_process_scp(input_process_scp);
  destroy_input_sch_ch_prob(input_scp);

  return 0;
}
