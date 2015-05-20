#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "adc.h"
#include "utils.h"
#include "i2c_master_int.h"


extern volatile int adc_data_ready;
extern volatile int adc_value;


int main()
{
	NU32_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init

	NU32_LED1 = 1;
	NU32_LED2 = 1;
	
	wait_usec(500000);

	NU32_LED1 = 1;
	NU32_LED2 = 0;

	
	int init_retVal = init_adc();

	__builtin_disable_interrupts();
	
	LCD_Setup();
	isense_init();
	motor_init();
	
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

	int i = 0;
	char buffer[20];
	while(1)
	{
		NU32_ReadUART1(buffer, 20);
		LCD_Clear();
		LCD_WriteString("Received:  ");
		LCD_WriteChar(buffer[0]);
		LCD_Move(1,0);

		switch(buffer[0])
		{
		case 'l':
		{
			LCD_WriteString("Raw Load Cell");
			
			while(!adc_data_ready)
			{
				;
			}

			// Send the value
			int toSend = adc_value;
			memset(buffer, 0, sizeof(buffer));
			memcpy(&(buffer[0]), &toSend, 4);
			NU32_WriteUART1(buffer);

			break;
		}

		case 'q':
		{
			LCD_WriteString("Quit!");
			break;
		}

		default:
			break;
		};
	}

	return 0;
}
