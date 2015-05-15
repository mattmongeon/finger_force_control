#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "adc.h"
#include "utils.h"


extern volatile int adc_data_ready;


int main()
{
	NU32_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init

	NU32_LED1 = 0;
	NU32_LED2 = 0;
	
	wait_usec(500000);

	NU32_LED1 = 1;
	NU32_LED2 = 1;


	
	int init_retVal = init_adc();
	

	int errorLEDs = 0;
	errorLEDs = (init_retVal != 0);
	NU32_LED1 = 0;
	NU32_LED2 = (errorLEDs != 0);

	_CP0_SET_COUNT(0);
	
	while(1)
	{
		/* if( !NU32_LED2 && adc_data_ready ) */
		/* 	NU32_LED2 = 1; */

		if( (init_retVal < 0) && (_CP0_GET_COUNT() >= (NU32_SYS_FREQ/8ul)) )
		{
			_CP0_SET_COUNT(0);
			NU32_LED1 = !NU32_LED1;
			if( errorLEDs )
				NU32_LED2 = !NU32_LED2;
		}
		
		/* NU32_ReadUART1(buffer, BUF_SIZE); */
		/* NU32_LED2 = 1; */

		/* sprintf(buffer, "%d\r\n", isense_ticks()); */
		/* NU32_WriteUART1(buffer); */		
	}

	return 0;
}
