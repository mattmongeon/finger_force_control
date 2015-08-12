#ifndef INCLUDED_BIOTAC_H
#define INCLUDED_BIOTAC_H


#include "../common/biotac_comm.h"


////////////////////////////////////////////////////////////////////////////////
//  Structs
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	float a;
	float b;
	float c;
	float d;
} electrode_compensator;

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


// Reads from the UART line to take in a force trajectory.  The length is sent
// as part of the stream.
unsigned int biotac_receive_force_trajectory();


// Returns the force in grams measured by the BioTac sensors as derived based on the
// incoming BioTac data.
//
// Return - The force value in grams.
int biotac_get_force_g();


#endif  // INCLUDED_BIOTAC_H

