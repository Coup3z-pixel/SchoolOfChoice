#include "parser.h"
#include "implement.h"

int main(int argc, char const *argv[])
{
  struct partial_alloc input_alloc = allocation_from_file();
  
  struct pure_alloc purified = random_pure_allocation(&input_alloc);

  print_pure_alloc(purified);

  destroy_partial_alloc(input_alloc);

  destroy_pure_alloc(purified);

  return 0;
}
