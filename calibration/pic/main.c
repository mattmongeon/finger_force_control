#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "utils.h"
#include "load_cell.h"
#include "motor.h"
#include "biotac.h"
#include "torque_control.h"


extern volatile int adc_data_ready;
extern volatile int adc_value;


static int uart1_send_packet(unsigned char* pData, int numBytes)
{
	int i = 0;
	for( ; i < numBytes; ++i )
	{
		while(U1STAbits.UTXBF)
		{
			;  // Wait until TX buffer is not full.
		}

		U1TXREG = *pData;
		++pData;
	}
}


static int uart1_read_packet(unsigned char* pData, int numBytes)
{
	int i = 0;
	while(numBytes-i > 0)
	{
		if(U1STAbits.URXDA)
		{
			pData[i] = U1RXREG;
			++i;
		}
	}
}


int main()
{
	NU32_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init

	NU32_LED1 = 1;
	NU32_LED2 = 1;
	
	wait_usec(500000);

	NU32_LED1 = 1;
	NU32_LED2 = 0;

	wait_usec(500000);
	
	
	__builtin_disable_interrupts();
	
	LCD_Setup();	
	int init_retVal = load_cell_init();
	isense_init();
	motor_init();
	biotac_init();
	torque_control_init();
	
	__builtin_enable_interrupts();


	LCD_Clear();
	LCD_WriteString("Initialized!");

	wait_usec(500000);

	NU32_LED1 = 0;
	NU32_LED2 = 0;

	wait_usec(500000);
	

	int errorLEDs = (init_retVal != 0);
	NU32_LED1 = 0;
	NU32_LED2 = (errorLEDs != 0);


	char buffer[50];
	biotac_data biotac;

	_CP0_SET_COUNT(0);

	while(1)
	{
		NU32_ReadUART1(buffer, 20);

		/*
		LCD_Clear();
		LCD_WriteString("Received:  ");
		LCD_WriteChar(buffer[0]);
		LCD_Move(1,0);
		*/
		
		switch(buffer[0])
		{
		case 'b':
		{
			read_biotac(&biotac);
			uart1_send_packet( (unsigned char*)(&biotac), sizeof(biotac_data) );
			break;
		}

		case 'f':
		{
			// --- Hold Commanded Force --- //
			
			int force;
			uart1_read_packet( (unsigned char*)(&force), sizeof(int) );
			torque_control_set_desired_force(force);

			system_set_state(HOLD_FORCE);

			// Let the motor try following the torque
			while(system_get_state() == HOLD_FORCE)
			{
				;
			}

			// Send the recorded data to the PC.
			
			
			break;
		}
		
		case 'g':
		{
			float k[2];
			uart1_read_packet( (unsigned char*)(&k), sizeof(k) );
			torque_control_set_gains(k[0], k[1]);
			break;
		}

		case 'h':
		{
			float k[2];
			torque_control_get_gains(&(k[0]), &(k[1]));
			uart1_send_packet( (unsigned char*)(&k), sizeof(k) );
			break;
		}
			
		case 'l':
		{
			int load_cell_g = load_cell_read_grams();
			uart1_send_packet( (unsigned char*)(&load_cell_g), 4 );
			break;
		}

		case 'p':
		{
			NU32_ReadUART1(buffer, 50);
			char pwm = buffer[0];
			if( pwm != 0 )
			{
				system_set_state(DIRECT_PWM);
			}
			else
			{
				system_set_state(IDLE);
			}
			
			motor_duty_cycle_pct_set(pwm);
			
			break;
		}
		
		case 'q':
		{
			LCD_WriteString("Quit!");
			break;
		}

		case 'r':
		{
			int load_cell_raw = load_cell_raw_value();
			uart1_send_packet( (unsigned char*)(&load_cell_raw), 4 );
			break;
		}

		case 't':
		{
			// --- Hold Commanded Tuning Force --- //

			LCD_Clear();
			LCD_WriteString("Tuning force...");
			
			int force;
			uart1_read_packet( (unsigned char*)(&force), sizeof(int) );
			torque_control_set_desired_force(force);

			system_set_state(TUNE_TORQUE_GAINS);

			// Let the motor try following the torque
			while(system_get_state() == TUNE_TORQUE_GAINS)
			{
				;
			}

			LCD_Clear();
			LCD_WriteString("Done tuning force");
			
			// Send the recorded data to the PC.
			unsigned char* pBuff = torque_control_get_raw_tune_buffer();

			unsigned long start = _CP0_GET_COUNT();
			uart1_send_packet( pBuff, sizeof(torque_tune_data)*200 );
			unsigned long end = _CP0_GET_COUNT();

			float t = end - start;
			t *= 25.0;
			t /= 1000000.0;
			char b[20];
			sprintf(b, "Tx: %f", t);
			LCD_Move(1,0);
			LCD_WriteString(b);
			LCD_Move(0,0);

			// LCD_Clear();
			LCD_WriteString("Tuning data sent");
			
			break;
		}

		default:
			break;
		};
	}

	return 0;
}
