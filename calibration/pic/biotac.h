#ifndef INCLUDED_BIOTAC_H
#define INCLUDED_BIOTAC_H

////////////////////////////////////////////////////////////////////////////////
//  Struct Declarations
////////////////////////////////////////////////////////////////////////////////

#pragma pack(push,1)

// Represents the data from a single BioTac reading.
typedef struct
{
	unsigned short pac;
	unsigned short pdc;

	unsigned short tac;
	unsigned short tdc;

	unsigned short e1;
	unsigned short e2;
	unsigned short e3;
	unsigned short e4;
	unsigned short e5;
	unsigned short e6;
	unsigned short e7;
	unsigned short e8;
	unsigned short e9;
	unsigned short e10;
	unsigned short e11;
	unsigned short e12;
	unsigned short e13;
	unsigned short e14;
	unsigned short e15;
	unsigned short e16;
	unsigned short e17;
	unsigned short e18;
	unsigned short e19;
} biotac_data;

// Contains the data used when tuning the BioTac.
typedef struct
{
	unsigned long timestamp;
	biotac_data data;
	int load_cell;
} biotac_tune_data;
#pragma pack(pop)


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


#endif  // INCLUDED_BIOTAC_H

