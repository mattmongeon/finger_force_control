#ifndef INCLUDED_MOTOR_H
#define INCLUDED_MOTOR_H


#define MAX_CURRENT_MA 449

void motor_init();  // Initializes the module and the peripherals it uses.

void motor_duty_cycle_pct_set(int duty_pct);

void motor_pwm_set(int pwm_new);  // Specifies the user's PWM value.

void motor_mA_set(int mA);  // Specifies the motor current reference in mA.

void motor_gains_read();  // Read the gains from the client.

void motor_gains_write();  // Sends the motor gains to the client.


#endif // INCLUDED_MOTOR_H

