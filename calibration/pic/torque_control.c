#include "torque_control.h"
#include "load_cell.h"
#include "system.h"
#include "motor.h"
#include "NU32.h"


#define MAX_ERROR_INT 10000
#define LOOP_RATE_HZ 200
#define LOOP_TIMER_PRESCALAR 16

#define K_DENOM 1000


// --- Private Variables --- //

static volatile int desired_force_g = 0;

static volatile int kp_num = 2000;
static volatile int ki_num = 1500;

static int error_int = 0;

static int holdTorqueTuneIndex = 0;
static torque_tune_data holdTorqueTuneBuffer[200];


// --- Control Loop Function --- //

static int torque_control_loop(int load_cell_g)
{
	int error = desired_force_g - load_cell_g;

	error_int += error;
	if( error_int > MAX_ERROR_INT )
		error_int = MAX_ERROR_INT;
	else if( error_int < -MAX_ERROR_INT )
		error_int = -MAX_ERROR_INT;

	int u_new = ((kp_num*error)/K_DENOM) + ((ki_num*error_int)/K_DENOM);

	motor_mA_set(u_new);
	
	holdTorqueTuneBuffer[holdTorqueTuneIndex].load_cell_g = load_cell_g;
	holdTorqueTuneBuffer[holdTorqueTuneIndex].error = error;
	holdTorqueTuneBuffer[holdTorqueTuneIndex].error_int = error_int;
	holdTorqueTuneBuffer[holdTorqueTuneIndex].current_mA = u_new;

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
		break;

	case TUNE_TORQUE_GAINS:
	{
		unsigned int start = _CP0_GET_COUNT();

		torque_control_loop(load_cell_read_grams());

		unsigned int end = _CP0_GET_COUNT();
		
		holdTorqueTuneBuffer[holdTorqueTuneIndex].loop_exe_time_ms =  end - start;
		holdTorqueTuneBuffer[holdTorqueTuneIndex].loop_exe_time_ms *= 25.0;
		holdTorqueTuneBuffer[holdTorqueTuneIndex].loop_exe_time_ms /= 1000000.0;  // Put it in ms
		holdTorqueTuneBuffer[holdTorqueTuneIndex].timestamp = start;
		
		++holdTorqueTuneIndex;

		if( holdTorqueTuneIndex >= 200 )
		{
			system_set_state(IDLE);
			motor_mA_set(0);
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

void torque_control_set_desired_force(int force_g)
{
	desired_force_g = force_g;
}

void torque_control_set_gains(float kp_new, float ki_new)
{
	int new_kp_num = (int)(kp_new * (float)(K_DENOM));
	int new_ki_num = (int)(ki_new * (float)(K_DENOM));
	
	__builtin_disable_interrupts();

	kp_num = new_kp_num;
	ki_num = new_ki_num;
	
	__builtin_enable_interrupts();
}

void torque_control_get_gains(float* p, float* i)
{
	*p = ((float)(kp_num) / (float)(K_DENOM));
	*i = ((float)(ki_num) / (float)(K_DENOM));
}

unsigned char* torque_control_get_raw_tune_buffer()
{
	return (unsigned char*)(&holdTorqueTuneBuffer[0]);
}

