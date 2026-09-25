#ifndef GCPSB_h
#define GCPSB_h

#include "mcccode.h"
#include "gcpscode.h"

/*
partial_alloc gcpsb_allocation(input_sch_ch_prob* myiscp);
*/

partial_alloc NEW_gcpsb_allocation(input_sch_ch_prob* myiscp, int allowed_speed_type);

#endif /* GCPSB_H */
