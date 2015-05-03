#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "i2c.h"

int main()
{
	NU32_Startup();  // cache on, min flash wait, interrupts on, LED/button init, UART init
	NU32_LED1 = 1;
	NU32_LED2 = 1;
	__builtin_disable_interrupts();

	i2c_setup();
	
	__builtin_enable_interrupts();

	while(1)
	{
		/* NU32_ReadUART1(buffer, BUF_SIZE); */
		/* NU32_LED2 = 1; */

		/* sprintf(buffer, "%d\r\n", isense_ticks()); */
		/* NU32_WriteUART1(buffer); */		
	}

	return 0;
}
