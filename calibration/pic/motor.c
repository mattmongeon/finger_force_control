#include "motor.h"
#include "utils.h"
#include "isense.h"
#include "NU32.h"
#include "system.h"


// --- Timer Constants --- //

#define PWM_PERIOD 4000


// --- Enums --- //

typedef enum
{
	CLOCKWISE = 0,
	COUNTERCLOCKWISE
} enumDirection;

static volatile enumDirection motorDirection = CLOCKWISE;


void motor_init()
{
	// --- Set Up Motor Direction Control Bit  --- //

	// Using RB1
	TRISBCLR = 0x02;
	
	
	// --- Set Up PWM Timer --- //

	// Setting up for 20 KHz timer.
	PR3 = PWM_PERIOD - 1;
	TMR3 = 0;

	// Prescalar set to 1, 16-bit timer, using PBCLK
	T3CON = 0x0000;


	// --- Set Up OC1 For PWM --- //

	// Set up for PWM using timer 3, no fault pin.
	OC1CON = 0x0E;

	// Set up for 0% duty cycle initially.
	OC1RS = 0;
	OC1R = 0;


	// --- Start PWMing --- //

	T3CONSET = 0x8000;
	OC1CONSET = 0x8000;
}

void motor_duty_cycle_pct_set(int duty_pct)
{
	motor_pwm_set( duty_pct * ((int)(PR3)) / 100 );
}

void motor_pwm_set(int pwm_new)
{
	if( system_get_state() != IDLE )
	{
		int motorPWM = pwm_new;
		if( motorPWM >= 0 )
		{
			motorDirection = CLOCKWISE;
		}
		else
		{
			motorDirection = COUNTERCLOCKWISE;
			motorPWM *= -1;
		}

		OC1RS = motorPWM;

		
		// --- Set Motor Direction --- //

		if( motorDirection == CLOCKWISE )
		{
			LATBSET = 0x02;
		}
		else
		{
			LATBCLR = 0x02;
		}
	}
}

