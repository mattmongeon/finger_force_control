#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "utils.h"
#include "load_cell.h"
#include "motor.h"
#include "biotac.h"


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


int main()
{
	NU32_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init

	NU32_LED1 = 1;
	NU32_LED2 = 1;
	
	wait_usec(500000);

	NU32_LED1 = 1;
	NU32_LED2 = 0;

	
	__builtin_disable_interrupts();
	
	int init_retVal = load_cell_init();
	LCD_Setup();
	isense_init();
	motor_init();
	biotac_init();
	
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

	
	_CP0_SET_COUNT(0);

	char buffer[50];
	biotac_data biotac;
	while(1)
	{
		NU32_ReadUART1(buffer, 20);
		LCD_Clear();
		LCD_WriteString("Received:  ");
		LCD_WriteChar(buffer[0]);
		LCD_Move(1,0);

		switch(buffer[0])
		{
		case 'b':
		{
			LCD_WriteString("Read BioTac");
			read_biotac(&biotac);
			LCD_Move(1,0);
			sprintf(buffer, "%d", biotac.e1);
			LCD_WriteString(&(buffer[0]));
			uart1_send_packet( (unsigned char*)(&biotac), sizeof(biotac_data) );
			break;
		}
			
		case 'l':
		{
			LCD_WriteString("Load Cell");
			int load_cell_g = load_cell_read_grams();
			uart1_send_packet( (unsigned char*)(&load_cell_g), 4 );
			break;
		}

		case 'p':
		{
			NU32_ReadUART1(buffer, 50);
			char pwm = buffer[0];
			motor_state_set(PWM);
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
			LCD_WriteString("Raw Load Cell");
			int load_cell_raw = load_cell_raw_value();
			uart1_send_packet( (unsigned char*)(&load_cell_raw), 4 );
			break;
		}

		default:
			break;
		};
	}

	return 0;
}
