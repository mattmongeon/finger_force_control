#ifndef INCLUDED_TORQUE_CONTROL_COMM_H
#define INCLUDED_TORQUE_CONTROL_COMM_H


#include <stdint.h>


////////////////////////////////////////////////////////////////////////////////
//  Struct Declarations
////////////////////////////////////////////////////////////////////////////////

#pragma pack(push,1)

// Represents data that is useful when tuning the PI parameters for holding a force.
typedef struct
{
	int32_t load_cell_g;
	int32_t error_int;
	uint32_t timestamp;
} torque_tune_data;

#pragma pack(pop)


#endif  // INCLUDED_TORQUE_CONTROL_COMM_H

