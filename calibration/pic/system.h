#ifndef INCLUDED_SYSTEM_H
#define INCLUDED_SYSTEM_H


// --- System State --- //

typedef enum
{
	IDLE = 0,
	TUNE_CURRENT_GAINS,
	TUNE_TORQUE_GAINS,
	DIRECT_PWM,
	TRACK_CURRENT,
	TRACK_FORCE_TRAJECTORY,
	HOLD_FORCE,
	BIOTAC_CAL_SINGLE
} state_t;


state_t system_get_state();

void system_set_state(state_t state);


#endif  // INCLUDED_SYSTEM_H
