#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "schchprob.h"

struct input_sch_ch_prob sch_ch_prob_from_file();

struct square_matrix related_matrix_from_file(int* max_clique_size);

#endif /* PARSER_H */
