#ifndef GCPSA_h
#define GCPSA_h

#include "mcccode.h"
#include "gcpscode.h"

/*
partial_alloc gcpsa_allocation(input_sch_ch_prob* myiscp);
*/

partial_alloc NEW_gcpsa_allocation(input_sch_ch_prob* myiscp, int allowed_speed_type);

#endif /* GCPSA_H */
