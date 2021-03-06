#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "utils.h"
#include "load_cell.h"
#include "motor.h"
#include "biotac.h"
#include "torque_control.h"
#include "uart.h"
#include "system.h"


extern volatile int adc_data_ready;
extern volatile int adc_value;


int main()
{
	NU32_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init

	NU32_LED1 = 1;
	NU32_LED2 = 1;
	
	__builtin_disable_interrupts();
	
	LCD_Setup();	
	load_cell_init();
	motor_init();
	biotac_init();
	torque_control_init();
	
	__builtin_enable_interrupts();


	LCD_Clear();
	LCD_WriteString("Initialized!");

	NU32_LED1 = 0;
	NU32_LED2 = 0;


	char buffer[50];
	biotac_data biotac;

	_CP0_SET_COUNT(0);

	while(1)
	{
		NU32_ReadUART1(buffer, 20);

		switch(buffer[0])
		{
		case 'a':
		{
			// --- BioTac Calibration --- //

			int seconds;
			uart1_read_packet( (unsigned char*)(&seconds), sizeof(int) );
			if( seconds > 0 )
			{
				biotac_set_time_length(seconds);
			}
			else
			{
				// Provide our own default value.
				biotac_set_time_length(2);
			}
			
			int force;
			uart1_read_packet( (unsigned char*)(&force), sizeof(int) );
			biotac_set_desired_testing_force_g(force);
			
			system_set_state(BIOTAC_CAL_SINGLE);
			
			while(system_get_state() == BIOTAC_CAL_SINGLE)
			{
				;
			}

			break;
		}

		case 'b':
		{
			// --- Read BioTac Data --- //
			
			read_biotac(&biotac);
			uart1_send_packet( (unsigned char*)(&biotac), sizeof(biotac_data) );
			break;
		}

		case 'e':
		{
			// --- Track Force With BioTac Calibration --- //
			
			int seconds;
			uart1_read_packet( (unsigned char*)(&seconds), sizeof(int) );
			if( seconds > 0 )
			{
				biotac_set_time_length(seconds);
			}
			else
			{
				// Provide our own default value.
				biotac_set_time_length(2);
			}
			
			int force;
			uart1_read_packet( (unsigned char*)(&force), sizeof(int) );
			torque_control_set_desired_force(force);
			
			system_set_state(BIOTAC_TRACK);

			while(system_get_state() == BIOTAC_TRACK)
			{
				;
			}
			
			break;
		}

		case 'f':
		{
			// --- Hold Commanded Force --- //
			
			int force;
			uart1_read_packet( (unsigned char*)(&force), sizeof(int) );
			torque_control_set_desired_force(force);

			if( force != 0 )
			{
				system_set_state(HOLD_FORCE);
			}
			else
			{
				system_set_state(IDLE);
			}

			break;
		}
		
		case 'g':
		{
			// --- Set Gains --- //
			
			float k[2];
			uart1_read_packet( (unsigned char*)(&k), sizeof(k) );
			torque_control_set_gains(k[0], k[1]);
			break;
		}

		case 'h':
		{
			// --- Get Gains --- //
			
			float k[2];
			torque_control_get_gains(&(k[0]), &(k[1]));
			uart1_send_packet( (unsigned char*)(&k), sizeof(k) );
			break;
		}
			
		case 'l':
		{
			// --- Read Load Cell -- //
			
			int load_cell_g = load_cell_read_grams();
			uart1_send_packet( (unsigned char*)(&load_cell_g), 4 );
			break;
		}

		case 'm':
		{
			// --- Read Raw (Uncalibrated) Load Cell --- //
			
			int load_cell_raw = load_cell_raw_value();
			uart1_send_packet( (unsigned char*)(&load_cell_raw), 4 );
			break;
		}

		case 'p':
		{
			// --- Set PWM Directly --- //

			char pwm = 0;
			uart1_read_packet( (unsigned char*)(&pwm), sizeof(char) );

			if( pwm != 0 )
			{
				motor_duty_cycle_pct_set(pwm);
				system_set_state(DIRECT_PWM);
			}
			else
			{
				motor_duty_cycle_pct_set(pwm);
				system_set_state(IDLE);
			}
			
			break;
		}

		case 'r':
		{
			// --- Continuous Read From BioTac --- //

			system_set_state(BIOTAC_CONTINUOUS_READ);

			while(1)
			{
				NU32_ReadUART1(buffer, 20);
				if( buffer[0] == 'q' )
				{
					system_set_state(IDLE);
					break;
				}
			}
			
			break;
		}

		case 's':
		{
			// --- Read Load Cell Continuously --- //

			system_set_state(LOAD_CELL_CONTINUOUS_READ);

			while(1)
			{
				NU32_ReadUART1(buffer, 20);
				if( buffer[0] == 'q' )
				{
					system_set_state(IDLE);
					break;
				}
			}
			
			break;
		}
		
		case 't':
		{
			// --- Hold Commanded Tuning Force --- //

			int seconds;
			uart1_read_packet( (unsigned char*)(&seconds), sizeof(int) );
			if( seconds > 0 )
			{
				torque_control_set_time_length(seconds);
			}
			else
			{
				// Provide our own default value.
				torque_control_set_time_length(2);
			}
			
			int force;
			uart1_read_packet( (unsigned char*)(&force), sizeof(int) );
			torque_control_set_desired_force(force);

			system_set_state(TUNE_TORQUE_GAINS);

			// Let the motor try following the torque
			while(system_get_state() == TUNE_TORQUE_GAINS)
			{
				;
			}

			break;
		}

		case 'u':
		{
			// --- BioTac Calibration With Trajectory --- //

			system_set_state(BIOTAC_CAL_TRAJECTORY);
			
			while(system_get_state() == BIOTAC_CAL_TRAJECTORY)
			{
				;
			}
			
			break;
		}

		case 'v':
		{
			biotac_receive_force_trajectory();
			
			break;
		}

		case 'z':
		{
			system_set_state(LOAD_CELL_CONTINUOUS_READ);

			while(1)
			{
				NU32_ReadUART1(buffer, 20);
				if( buffer[0] == 'q' )
				{
					system_set_state(IDLE);
					break;
				}
			}
			
			break;
		}

		default:
			break;
		};
	}

	return 0;
}
