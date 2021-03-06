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


// Returns the force in grams value to be held by the controller.
//
// Return - The force value in grams.
int torque_control_get_desired_force_g();


// Returns the current force value as read from the load cell by the
// torque controller.
//
// Note:
// This is a horrendous hack.  To get the load cell reading everything
// should just read from the load cell.  However, when running the BioTac
// loop, the load cell consistently hands back a value that does not
// match the value given to the torque controller even though both would
// call the same function.  Until this gets resolved, everything needs to
// use the torque controller to get the "real" load cell reading.
//
// Return - The force value in grams as read from the load cell.
int torque_control_get_load_cell_g();


// Sets the length of time in seconds for which the tuning session should last.
//
// Params:
// seconds - The length of time in seconds.
void torque_control_set_time_length(int seconds);


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


#endif  // INCLUDED_TORQUE_CONTROL_H

