#ifndef INCLUDED_TORQUE_CONTROL_H
#define INCLUDED_TORQUE_CONTROL_H


#include "../common/torque_control_comm.h"


// This file contains function declarations for the module that is responsible for
// trying to generate a requested motor torque value based on the desired force
// at the contact point.  It contains the PI controller for turning the error in
// desired end-point force into a desired torque value from the motor.  The motor
// controller will be responsible for using the torque error to generate a PWM
// value for changing the current through the motor.


////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// Initializes everything necessary for being able to control the applied torque.
void torque_control_init();


// Sets a force value in grams to be held by the controller.
//
// Params:
// force_g - The new desired force value in grams.
void torque_control_set_desired_force(int force_g);


// Adjusts the PI gain values to the parameter values.
//
// Params:
// kp_new - The new Kp term.
// ki_new - The new Ki term.
void torque_control_set_gains(float kp_new, float ki_new);


// Gathers the Kp and Ki terms and copies them into the parameter pointers.
//
// Params:
// p - [OUT] - Pointer to a float that will hold the current Kp constant.
// i - [OUT] - Pointer to a float that will hold the current Ki constant.
void torque_control_get_gains(float* p, float* i);


// Returns a pointer to the beginning of the buffer containing all of the
// data recorded while tuning the torque control PI constants.
//
// Return - pointer to the recorded tuning buffer.
unsigned char* torque_control_get_raw_tune_buffer();


#endif  // INCLUDED_TORQUE_CONTROL_H

