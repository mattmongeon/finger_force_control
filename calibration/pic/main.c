#include "NU32.h"  // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "utils.h"
#include "load_cell.h"


extern volatile int adc_data_ready;
extern volatile int adc_value;


static int uart1_send_int(int val)
{
	unsigned char* p = (unsigned char*)(&val);
	int i = 0;
	for( ; i < 4; ++i )
	{
		while(U1STAbits.UTXBF)
		{
			;  // Wait until TX buffer is not full.
		}

		U1TXREG = *p;
		++p;
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
			LCD_WriteString("Load Cell");
			uart1_send_int( load_cell_read_grams() );
			break;
		}
			
		case 'r':
		{
			LCD_WriteString("Raw Load Cell");
			uart1_send_int( load_cell_raw_value() );
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
