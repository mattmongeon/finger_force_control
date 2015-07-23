#ifndef INCLUDED_MOTOR_H
#define INCLUDED_MOTOR_H


// --- Function Declarations --- //

void motor_init();  // Initializes the module and the peripherals it uses.

void motor_duty_cycle_pct_set(int duty_pct);

void motor_pwm_set(int pwm_new);  // Specifies the user's PWM value.

// Returns the current PWM value used by the motor.  Will be a signed value
// based on the direction of the motor.
//
// Return - A signed integer specifying the current PWM value.
int motor_pwm_get();


#endif // INCLUDED_MOTOR_H

