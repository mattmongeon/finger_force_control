#include "motor.h"
#include "utils.h"
#include "isense.h"
#include "NU32.h"


// --- Timer Constants --- //

#define PWM_PERIOD 4000
#define CONTROLLER_PERIOD 4000


// --- Protected Variables --- //

static volatile state_t state = IDLE;

static volatile int current_ref_mA = 0;

static volatile float kp = 8.25;
static volatile float ki = 1.25;
static volatile int max_error_int = 0;
static volatile int error_int = 0;

typedef enum
{
	CLOCKWISE = 0,
	COUNTERCLOCKWISE
} enumDirection;

static volatile enumDirection motorDirection = CLOCKWISE;


static int control_loop(int current_sensor_mA)
{
	int error = current_ref_mA - current_sensor_mA;

	error_int += error;
	if( error_int > max_error_int )
		error_int = max_error_int;
	else if( error_int < -max_error_int )
		error_int = -max_error_int;
	
	float u_new = (kp*error) + (ki*error_int);

	// 1. Limit between -100, 100
	// 2. Limit between -PWM_PERIOD and PWM_PERIOD
	if( u_new > (float)(PWM_PERIOD - 1) )
		u_new = (float)(PWM_PERIOD - 1);

	if( u_new < (float)(-(PWM_PERIOD - 1)) )
		u_new = (float)(-(PWM_PERIOD - 1));

	motor_pwm_set(u_new);

	return u_new;
}


void __ISR(_TIMER_2_VECTOR, IPL5SOFT) current_controller()
{
	static int square_wave_timer = 0;
	
	// --- State Machine --- //

	switch(motor_state_get())
	{
	case IDLE:
		error_int = 0;
		square_wave_timer = 0;
		OC1RS = 0;
		break;

		
	case PWM:
		error_int = 0;
		square_wave_timer = 0;
		break;

		
	case TUNE:
	{
		// --- Update the Tuning Reference --- //

		int reference = 0;
		if( square_wave_timer < 25 )
		{
			reference = -100;
		}
		else if( square_wave_timer < 50 )
		{
			reference = 100;
		}

		motor_mA_set(reference);


		// --- Run the Control Loop --- //
		
	    int sensor = isense_mA();
		int u = control_loop(sensor);


		// --- Write Data To Matlab --- //
		
		++square_wave_timer;
		square_wave_timer %= 50;
		
		break;
	}
		
	case TRACK:
		square_wave_timer = 0;
		
		// follow the current reference set by motor_amps_set
		control_loop(isense_mA());
		
		break;

		
	default:
		// error, unknown state
		NU32_LED2 = 0;
	}
	
	
	// --- Finish Well --- //
	
	IFS0CLR = 0x100;
}


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

	// Set up for 75% duty cycle initially.
	OC1RS = 3000;
	OC1R = 3000;


	// --- Set Up Timer 2 For Current Controller ISR --- //

	// Setting up timer 2 to trigger at 5 kHz.
	PR2 = CONTROLLER_PERIOD - 1;
	TMR2 = 0;
	T2CON = 0x0020;


	// --- Set Up Timer 2 Interrupt --- //

    IPC2SET = 0x14;   // priority 5, subpriority 0
	IFS0CLR = 0x100;  // Clear the interrupt flag
	IEC0SET = 0x100;  // Enable the interrupt
	

	// --- Start PWMing --- //

	T3CONSET = 0x8000;
	OC1CONSET = 0x8000;


	// --- Start Controller Timer --- //
	
	T2CONSET = 0x8000;
}

void motor_duty_cycle_pct_set(int duty_pct)
{
	motor_pwm_set( duty_pct * ((int)(PR3)) / 100 );
}

void motor_pwm_set(int pwm_new)
{
	if( motor_state_get() != IDLE )
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

void motor_mA_set(int mA)
{
	current_ref_mA = mA;
	if( current_ref_mA > MAX_CURRENT_MA )
		current_ref_mA = MAX_CURRENT_MA;
	else if( current_ref_mA < -MAX_CURRENT_MA )
		current_ref_mA = -MAX_CURRENT_MA;
}

void motor_gains_read()
{
	char buffer[30];
	NU32_ReadUART1(buffer, 30);
	float temp_kp = 0;
	float temp_ki = 0;
	sscanf(buffer, "%f %f", &temp_kp, &temp_ki);
	
	__builtin_disable_interrupts();

	// Using temporary variables at this point instead of
	// reading directly from sscanf() because we want this
	// part to be as fast as possible.  We don't want the
	// interrupts to be off too long.
	kp = temp_kp;
	ki = temp_ki;
	max_error_int = (int)(((float)PR3) / ki);

	__builtin_enable_interrupts();
}

void motor_gains_write()
{
	char buffer[50];
	sprintf(buffer, "%f %f\r\n", kp, ki);
	NU32_WriteUART1(buffer);
}

state_t motor_state_get()
{
	return state;
}

void motor_state_set(state_t s)
{
	__builtin_disable_interrupts();

	state = s;

	__builtin_enable_interrupts();
}


