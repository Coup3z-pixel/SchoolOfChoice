#include "parser.h"
#include "purifycode.h"

int main(int argc, char const *argv[])
{
  struct pure_alloc purified;

  struct partial_alloc input_alloc;
  
  if (argc == 1) {
    const char filename[20] = "allocate.mat";
    input_alloc = allocation_from_file(filename);
  }
  if (argc == 2) {
    input_alloc = allocation_from_file(argv[1]);
  }
  if (argc > 2) {
    fprintf(stderr, "purify invoked with too many (> 2) command line arguments.\n");
    exit(0);
  }
  purified = random_pure_allocation(&input_alloc);
  destroy_partial_alloc(input_alloc);
  
  print_pure_alloc(&purified); 

  destroy_pure_alloc(purified);

  return 0;
}
