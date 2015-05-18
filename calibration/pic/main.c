#include "NU32.h"  // config bits, constants, funcs for startup and UART
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

	NU32_LED1 = 0;
	NU32_LED2 = 0;

	
	int init_retVal = init_adc();
	

	int errorLEDs = (init_retVal != 0);
	NU32_LED1 = 0;
	NU32_LED2 = (errorLEDs != 0);

	
	_CP0_SET_COUNT(0);

	int i = 0;
	while(1)
	{
		if( errorLEDs && (_CP0_GET_COUNT() >= (NU32_SYS_FREQ/8ul)) )
		{
			_CP0_SET_COUNT(0);
			NU32_LED1 = !NU32_LED1;
			NU32_LED2 = !NU32_LED2;
		}

		if( check_data_available() )
		{
			int adc = read_adc();
			wait_usec(100000);
			NU32_LED1 = adc & 0x1;
			NU32_LED2 = adc & 0x2;
		}
		
		
		/* NU32_ReadUART1(buffer, BUF_SIZE); */
		/* NU32_LED2 = 1; */

		/* sprintf(buffer, "%d\r\n", isense_ticks()); */
		/* NU32_WriteUART1(buffer); */		
	}

	return 0;
}
