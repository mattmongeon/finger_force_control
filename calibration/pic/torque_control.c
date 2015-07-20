#include "torque_control.h"
#include "load_cell.h"
#include "system.h"
#include "motor.h"
#include "uart.h"
#include "NU32.h"


////////////////////////////////////////////////////////////////////////////////
//  File-Local Declarations
////////////////////////////////////////////////////////////////////////////////

// --- Constants --- //

#define MAX_ERROR_INT 10000
#define LOOP_RATE_HZ 200
#define LOOP_TIMER_PRESCALAR 16

#define K_DENOM 1000


// --- Variables --- //

static volatile int desired_force_g = 0;

static volatile int kp_num = 300;
static volatile int ki_num = 250;

static int error_int = 0;

// Default to 2 seconds.
static int max_tuning_samples = 2 * LOOP_RATE_HZ;


// --- Local Functions --- //

// Implements the PI controller with integrator saturation.
//
// Params:
// load_cell_g - The most recent reading of the load cell in grams.
//
// Return - The new control signal representing a current value in mA.
static int torque_control_loop(int load_cell_g, torque_tune_data* pTuneData)
{
	int error = desired_force_g - load_cell_g;

	error_int += error;
	if( error_int > MAX_ERROR_INT )
		error_int = MAX_ERROR_INT;
	else if( error_int < -MAX_ERROR_INT )
		error_int = -MAX_ERROR_INT;

	int u_new = ((kp_num*error)/K_DENOM) + ((ki_num*error_int)/K_DENOM);

	motor_pwm_set(u_new);
	
	pTuneData->load_cell_g = load_cell_g;
	pTuneData->error = error;
	pTuneData->error_int = error_int;
	pTuneData->pwm = u_new;

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
	{
		unsigned int start = _CP0_GET_COUNT();
		
		torque_control_loop(load_cell_read_grams(), &tune_data);

		unsigned int end = _CP0_GET_COUNT();

		if( system_get_state() == TUNE_TORQUE_GAINS )
		{
			// Only do this part if we are specifically tuning torque gains.  Otherwise
			// the BioTac portion is actually in charge.
			tune_data.loop_exe_time_ms = end - start;
			tune_data.loop_exe_time_ms *= 0.000025;	// Put it in ms
			tune_data.timestamp = start;

			uart1_send_packet( (unsigned char*)(&tune_data), sizeof(torque_tune_data) );
			
			++torqueTuneSamples;
			if( torqueTuneSamples >= max_tuning_samples )
			{
				memset(&tune_data, 0, sizeof(torque_tune_data));
				uart1_send_packet( (unsigned char*)(&tune_data), sizeof(torque_tune_data) );
				
				system_set_state(IDLE);
			}
		}
		
		break;
	}

	case IDLE:
		motor_pwm_set(0);
		torqueTuneSamples = 0;
		error_int = 0;
	
	default:
		torqueTuneSamples = 0;
		error_int = 0;
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

void torque_control_set_time_length(int seconds)
{
	__builtin_disable_interrupts();

	max_tuning_samples = seconds * LOOP_RATE_HZ;

	__builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////

void torque_control_set_gains(float kp_new, float ki_new)
{
	int new_kp_num = (int)(kp_new * (float)(K_DENOM));
	int new_ki_num = (int)(ki_new * (float)(K_DENOM));
	
	__builtin_disable_interrupts();

	kp_num = new_kp_num;
	ki_num = new_ki_num;
	
	__builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////

void torque_control_get_gains(float* p, float* i)
{
	*p = ((float)(kp_num) / (float)(K_DENOM));
	*i = ((float)(ki_num) / (float)(K_DENOM));
}

