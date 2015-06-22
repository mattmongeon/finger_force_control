#include "torque_control.h"
#include "load_cell.h"
#include "system.h"
#include "motor.h"
#include "NU32.h"
#include "utils.h"


// --- Private Variables --- //

static volatile int desired_force_g = 0;

static volatile float kp = 2.0;
static volatile float ki = 1.5;
static volatile float kd = -1.0;

static int max_error_int = 1000;
static int error_int = 0;
static int prev_error = 0;

static int holdTorqueTuneIndex = 0;
static torque_tune_data holdTorqueTuneBuffer[100];


// --- Control Loop Function --- //

//static int torque_control_loop(int force_sensor_g)
static int torque_control_loop(torque_tune_data* pData)
{
	int error = desired_force_g - pData->load_cell_g;

	error_int += error;
	if( error_int > max_error_int )
		error_int = max_error_int;
	else if( error_int < -max_error_int )
		error_int = -max_error_int;

	int u_new = (kp*error) + (ki*error_int) + (kd*(error-prev_error));

	// We are outputting a current value.  Limit it at the maximum current.
	/*
	if( u_new > MAX_CURRENT_MA )
		u_new = MAX_CURRENT_MA;

	if( u_new < -MAX_CURRENT_MA )
		u_new = -MAX_CURRENT_MA;
	*/
	
	motor_mA_set(u_new);
	
	pData->error = error;
	pData->error_int = error_int;

	prev_error = error;
	
	return u_new;
}


// --- The Timer Interrupt --- //

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) torque_controller()
{
	switch(system_get_state())
	{
	case IDLE:
	case DIRECT_PWM:
	case TUNE_CURRENT_GAINS:
	case TRACK_CURRENT:
		holdTorqueTuneIndex = 0;
		error_int = 0;
		prev_error = 0;
		break;

	case TUNE_TORQUE_GAINS:
	{
		holdTorqueTuneBuffer[holdTorqueTuneIndex].load_cell_g = load_cell_read_grams();

		torque_control_loop(&(holdTorqueTuneBuffer[holdTorqueTuneIndex]));
		holdTorqueTuneBuffer[holdTorqueTuneIndex].timestamp = _CP0_GET_COUNT();
		
		++holdTorqueTuneIndex;
		if( holdTorqueTuneIndex >= 100 )
		{
			system_set_state(IDLE);
		}
		
		break;
	}

	case TRACK_FORCE_TRAJECTORY:
		break;

	case HOLD_FORCE:
		break;

	default:
		break;
	}


	IFS0CLR = 0x10000;  // clear the interrupt flag
}


// --- Interface Functions --- //

void torque_control_init()
{
	// --- Set Up Timer 4 For Current Controller ISR --- //

	int prescalar = 1;
	switch(load_cell_sample_rate_hz())
	{
	case 320:
	case 80:
		prescalar = 16;
		T4CON = 0x0040;
		break;

	case 40:
	case 20:
		prescalar = 64;
		T4CON = 0x0060;
		break;

	case 10:
		prescalar = 256;
		T4CON = 0x0070;
		break;

	default:
		break;
	}

	// Timer rate needs to be set based on the ADC sampling rate.  The default is
	// 10 Hz, although it can be configured differently.
	PR4 = ((NU32_SYS_FREQ / load_cell_sample_rate_hz()) / prescalar) - 1;
	TMR4 = 0;


	// --- Set Up Timer 4 Interrupt --- //

	IPC4SET = 0x15;     // priority 5, subpriority 1
	IFS0CLR = 0x10000;  // clear the interrupt flag
	IEC0SET = 0x10000;  // Enable the interrupt
	

	// --- Start Controller Timer --- //

	T4CONSET = 0x8000;
}

void torque_control_set_desired_force(int force_g)
{
	desired_force_g = force_g;
}

void torque_control_set_gains(float kp_new, float ki_new, float kd_new)
{
	__builtin_disable_interrupts();

	kp = kp_new;
	ki = ki_new;
	kd = kd_new;
	
	__builtin_enable_interrupts();
}

void torque_control_get_gains(float* p, float* i, float* d)
{
	*p = kp;
	*i = ki;
	*d = kd;
}

unsigned char* torque_control_get_raw_tune_buffer()
{
	return (unsigned char*)(&holdTorqueTuneBuffer[0]);
}

