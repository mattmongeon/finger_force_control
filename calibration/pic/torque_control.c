#include "torque_control.h"
#include "load_cell.h"
#include "system.h"
#include "motor.h"
#include "uart.h"
#include "biotac.h"
#include "NU32.h"


////////////////////////////////////////////////////////////////////////////////
//  File-Local Declarations
////////////////////////////////////////////////////////////////////////////////

// --- Constants --- //

#define LOOP_RATE_HZ 5000
#define LOOP_TIMER_PRESCALAR 16

#define ZERO_DEADBAND_G 10

#define MAX_PWM_CHANGE 500

#define MAX_PWM_OFF_LOADCELL 400

#define SAMPLE_RATE_HZ 1000


// --- Variables --- //

static volatile int desired_force_g = 0;

static volatile float kp = 0.05; // 0.05;
static volatile float ki = 0.05; // 0.5;

static int error_int = 0;
static volatile int saved_load_cell_g = 0;

// Default to 2 seconds.
static int max_tuning_samples = 3 * SAMPLE_RATE_HZ;


static torque_tune_data buffer[SAMPLE_RATE_HZ * 3];
static uint8_t save_counter = 0;


static int get_pwm_from_desired_force(int force_g)
{
	return 0;
	/* return (int)(4.9333333 * force_g + 26.66667); */
}


// --- Local Functions --- //

// Implements the PI controller with integrator saturation.
//
// Params:
// load_cell_g - The most recent reading of the load cell in grams.
//
// Return - The new control signal representing a current value in mA.
static int torque_control_loop(int current_force_g, torque_tune_data* pTuneData, int valToSave)
{
	// --- Feed Forward --- //
	
	int u_new = get_pwm_from_desired_force(desired_force_g);


	// --- Feedback --- //
	
	int error = desired_force_g - current_force_g;
	saved_load_cell_g = valToSave;

	// This is a safety measure.  When it reaches 0 grams the integral term
	// can still cause there to be some PWM send to the motor, which can cause
	// the motor to run backwards.  This ensures the motor doesn't do that.  It
	// will also keep the integral from blowing up.
	if( !((desired_force_g == 0) && (current_force_g < ZERO_DEADBAND_G)) )
	{
		int maxError = 2000/ki;
		error_int += error;
		if( error_int > maxError )
			error_int = maxError;
		else if( error_int < -maxError )
			error_int = -maxError;

		u_new += (kp*error) + (ki*error_int);
	}
	else
	{
		u_new = 0;
	}


	// --- Protection --- //

	// We don't want to be slamming the BioTac against the surface.  Set a maximum
	// limit to how much the PWM can change at a time.
	int current_pwm = motor_pwm_get();
	if( abs(u_new - current_pwm) > MAX_PWM_CHANGE )
	{
		if( current_pwm < u_new )
			u_new = current_pwm + MAX_PWM_CHANGE;
		else
			u_new = current_pwm - MAX_PWM_CHANGE;
	}

	// The BioTac is currently off the load cell.  Limit the maximum PWM so that
	// we don't slam it back onto the surface.  Once it is on there, it can go back
	// to normal operation.
	if( valToSave < ZERO_DEADBAND_G )
	{
		if( u_new > MAX_PWM_OFF_LOADCELL )
		{
			// While the BioTac is making its way back to the load cell, don't
			// slam it down - be a little more gentle.
			u_new = MAX_PWM_OFF_LOADCELL;
		}
		else if( u_new < 0 )
		{
			// If we are off the load cell, don't back up any more.  We don't want
			// to give the BioTac any room to wind up and slam back down.
			u_new = 0;
		}
	}


	// --- Send New Command --- //

	motor_pwm_set(u_new);
	
	pTuneData->load_cell_g = current_force_g;
	/* pTuneData->error = error; */
	pTuneData->error_int = error_int;
	/* pTuneData->pwm = u_new; */
	
	return u_new;
}

////////////////////////////////////////////////////////////////////////////////
//  Interrupt Functions
////////////////////////////////////////////////////////////////////////////////

// Timer 4 interrupt which is responsible for implementing the torque controller
// in real-time.
void __ISR(_TIMER_4_VECTOR, IPL5SOFT) torque_controller()
{
	static torque_tune_data tune_data;
	static int torqueTuneSamples = 0;
	
	switch(system_get_state())
	{
	case TUNE_TORQUE_GAINS:
	case BIOTAC_CAL_SINGLE:
	case BIOTAC_CAL_TRAJECTORY:
	case BIOTAC_TRACK:
	{
		unsigned int start = _CP0_GET_COUNT();

		int current_force_g = 0;
		int load_cell_g = load_cell_read_grams();

		// We might be tracking the BioTac's force sensing rather than the load cell.
		if( system_get_state() == BIOTAC_TRACK )
			current_force_g = biotac_get_force_g();
		else
			current_force_g = load_cell_g;

		torque_control_loop(current_force_g, &tune_data, load_cell_g);

		unsigned int end = _CP0_GET_COUNT();

		if( system_get_state() == TUNE_TORQUE_GAINS )
		{
			// Only do this part if we are specifically tuning torque gains.  Otherwise
			// the BioTac portion is actually in charge.
			/* tune_data.loop_exe_time_ms = end - start; */
			/* tune_data.loop_exe_time_ms *= 0.000025;	// Put it in ms */
			tune_data.timestamp = start;

			if( save_counter % (LOOP_RATE_HZ / SAMPLE_RATE_HZ) == 0 )
			{
				/* uart1_send_packet( (unsigned char*)(&tune_data), sizeof(torque_tune_data) ); */
				memcpy(&(buffer[torqueTuneSamples]), &tune_data, sizeof(torque_tune_data));
				++torqueTuneSamples;
			}

			++save_counter;
			
			if( torqueTuneSamples >= max_tuning_samples )
			{
				int i = 0;
				for( ; i < torqueTuneSamples; ++i )
				{
					uart1_send_packet( (unsigned char*)(&(buffer[i])), sizeof(torque_tune_data) );
				}
				
				memset(&tune_data, 0, sizeof(torque_tune_data));
				uart1_send_packet( (unsigned char*)(&tune_data), sizeof(torque_tune_data) );
				
				system_set_state(IDLE);
			}
		}
		
		break;
	}

	case HOLD_FORCE:
	{
		int force = load_cell_read_grams();
		torque_control_loop(force, &tune_data, force);
		
		break;
	}

	case BIOTAC_CONTINUOUS_READ:
	{
		// HACK - SEE COMMENTS IN BIOTAC TIMER LOOP
		saved_load_cell_g = load_cell_read_grams();
		
		break;
	}

	case IDLE:
		motor_pwm_set(0);
		torqueTuneSamples = 0;
		error_int = 0;
		save_counter = 0;
	
	default:
		torqueTuneSamples = 0;
		error_int = 0;
		save_counter = 0;
		break;
	}


	IFS0CLR = 0x10000;  // clear the interrupt flag
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void torque_control_init()
{
	// --- Set Up Timer 4 For Current Controller ISR --- //

	PR4 = ((NU32_SYS_FREQ / LOOP_RATE_HZ) / LOOP_TIMER_PRESCALAR) - 1;
	T4CON = 0x0040;
	TMR4 = 0;


	// --- Set Up Timer 4 Interrupt --- //

	IPC4SET = 0x15;     // priority 5, subpriority 1
	IFS0CLR = 0x10000;  // clear the interrupt flag
	IEC0SET = 0x10000;  // Enable the interrupt
	

	// --- Start Controller Timer --- //

	T4CONSET = 0x8000;
}

////////////////////////////////////////////////////////////////////////////////

void torque_control_set_desired_force(int force_g)
{
	desired_force_g = force_g;
}

////////////////////////////////////////////////////////////////////////////////

int torque_control_get_desired_force_g()
{
	return desired_force_g;
}

////////////////////////////////////////////////////////////////////////////////

int torque_control_get_load_cell_g()
{
	// THIS FUNCTION IS A HACK.  SEE FUNCTION COMMENTS IN HEADER FILE.
	return saved_load_cell_g;
}

////////////////////////////////////////////////////////////////////////////////

void torque_control_set_time_length(int seconds)
{
	__builtin_disable_interrupts();

	max_tuning_samples = seconds * SAMPLE_RATE_HZ;

	__builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////

void torque_control_set_gains(float kp_new, float ki_new)
{
	__builtin_disable_interrupts();

	kp = kp_new;
	ki = ki_new;
	
	__builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////

void torque_control_get_gains(float* p, float* i)
{
	*p = kp;
	*i = ki;
}

