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
	int32_t error;
	int32_t error_int;
	int32_t current_mA;
	uint32_t timestamp;
	float loop_exe_time_ms;
} torque_tune_data;

#pragma pack(pop)


#endif  // INCLUDED_TORQUE_CONTROL_COMM_H

