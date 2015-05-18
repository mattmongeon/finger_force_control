#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "adc.h"
#include "utils.h"
#include "i2c_master_int.h"


extern volatile int adc_data_ready;


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
	__builtin_enable_interrupts();


	LCD_Clear();
	LCD_WriteString("LCD ready!");

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
		if( errorLEDs && (_CP0_GET_COUNT() >= (NU32_SYS_FREQ/8ul)) )
		{
			LCD_WriteString("Error!");
			_CP0_SET_COUNT(0);
			NU32_LED1 = !NU32_LED1;
			NU32_LED2 = !NU32_LED2;
		}
		else if( adc_data_ready )
		{
			adc_data_ready = 0;
			int adc = read_adc();
			LCD_Clear();
			LCD_Move(0,0);
			LCD_WriteString("Sensor:");
			LCD_Move(1,0);
			sprintf(buffer, "%d", adc);
			LCD_WriteString(buffer);
		}
		
		
		/* NU32_ReadUART1(buffer, BUF_SIZE); */
		/* NU32_LED2 = 1; */

		/* sprintf(buffer, "%d\r\n", isense_ticks()); */
		/* NU32_WriteUART1(buffer); */		
	}

	return 0;
}
