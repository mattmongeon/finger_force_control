#ifndef INCLUDED_BIOTAC_H
#define INCLUDED_BIOTAC_H


#include "../common/biotac_comm.h"


////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// Sets up communication and initializes the BioTac.
void biotac_init();


// Reads a full set of data from the BioTac's sensors and sets the values in
// the parameter struct pointer.
//
// Params:
// pData - [OUT] - a pointer representing the struct to be filled with data.
void read_biotac(biotac_data* pData);


// Sets the length of time in seconds for which the tuning session should last.
//
// Params:
// seconds - The length of time in seconds.
void biotac_set_time_length(int seconds);


#endif  // INCLUDED_BIOTAC_H

