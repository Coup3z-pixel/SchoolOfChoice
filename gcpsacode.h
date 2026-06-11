#ifndef GCPSA_h
#define GCPSA_h

#include "mcccode.h"
#include "gcpscode.h"

partial_alloc gcpsa_allocation(input_sch_ch_prob* myiscp);

partial_alloc gcpsaeff_allocation(input_sch_ch_prob* myiscp, int ex_ante_stable);

#endif /* GCPSA_H */
