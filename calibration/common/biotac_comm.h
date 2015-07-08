#ifndef INCLUDED_BIOTAC_COMM_H
#define INCLUDED_BIOTAC_COMM_H

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
//  Struct Declarations
////////////////////////////////////////////////////////////////////////////////

#pragma pack(push,1)
// The struct containing all of the BioTac data from a single read.
typedef struct
{
	uint16_t pac;
	uint16_t pdc;

	uint16_t tac;
	uint16_t tdc;
 
	uint16_t e1;
	uint16_t e2;
	uint16_t e3;
	uint16_t e4;
	uint16_t e5;
	uint16_t e6;
	uint16_t e7;
	uint16_t e8;
	uint16_t e9;
	uint16_t e10;
	uint16_t e11;
	uint16_t e12;
	uint16_t e13;
	uint16_t e14;
	uint16_t e15;
	uint16_t e16;
	uint16_t e17;
	uint16_t e18;
	uint16_t e19;
} biotac_data;


// Contains the data recorded while performing tuning with the BioTac.
typedef struct
{
	uint32_t mTimestamp;
	biotac_data mData;
	int32_t mLoadCell_g;
} biotac_tune_data;
#pragma pack(pop)


#endif // INCLUDED_BIOTAC_COMM_H


