#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "schchprob.h"
#include "partalloc.h"

struct input_sch_ch_prob sch_ch_prob_from_file(const char filename[]);

struct partial_alloc allocation_from_file(const char filename[]); 

#endif /* PARSER_H */
