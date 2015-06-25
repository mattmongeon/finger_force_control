#ifndef INCLUDED_TORQUE_CONTROL_H
#define INCLUDED_TORQUE_CONTROL_H


// This file contains function declarations for the module that is responsible for
// trying to generate a requested motor torque value based on the desired force
// at the contact point.  It contains the PID controller for turning the error in
// desired end-point force into a desired torque value from the motor.  The motor
// controller will be responsible for using the torque error to generate a PWM
// value for changing the current through the motor.


// --- Structs --- //

typedef struct
{
	int load_cell_g;
	int error;
	int error_int;
	int current_mA;
	unsigned int timestamp;
	float loop_exe_time_ms;
} torque_tune_data;


// --- Function Declarations --- //

void torque_control_init();

void torque_control_set_desired_force(int force_g);

void torque_control_set_gains(float kp_new, float ki_new);

void torque_control_get_gains(float* p, float* i);

unsigned char* torque_control_get_raw_tune_buffer();

#endif  // INCLUDED_TORQUE_CONTROL_H

