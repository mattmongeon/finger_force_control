#ifndef INCLUDED_SYSTEM_H
#define INCLUDED_SYSTEM_H

////////////////////////////////////////////////////////////////////////////////
//  Enum Declarations
////////////////////////////////////////////////////////////////////////////////

// Represents the various possible states of the overall state machine.
typedef enum
{
	IDLE = 0,
	TUNE_TORQUE_GAINS,
	DIRECT_PWM,
	TRACK_CURRENT,
	TRACK_FORCE_TRAJECTORY,
	HOLD_FORCE,
	BIOTAC_CAL_SINGLE,
	BIOTAC_CONTINUOUS_READ,
	LOAD_CELL_CONTINUOUS_READ
} state_t;

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// Returns the current state of the state machine.
//
// Return - The state machine's current state.
state_t system_get_state();


// Sets the state machine to the parameter state.
//
// Params:
// state - The new requested state.
void system_set_state(state_t state);


#endif  // INCLUDED_SYSTEM_H
