#include "spi.h"
#include "NU32.h"


void spi_init()
{
	SPI4CON = 0;
	SPI4BRG = 39;   // 1 MHz
	SPI4STATbits.SPIROV = 0; // Clear overflow flag

	SPI4CONbits.MODE16 = 1;  // 16-bit mode
	SPI4CONbits.CKP = 0;     // Clock is active high, idle low
	SPI4CONbits.CKE = 1;     // Serial output changes on active to idle transition (falling edge)
	SPI4CONbits.MSTEN = 1;   // Enable Master Mode
	SPI4CONbits.SMP = 1;
	SPI4CONbits.ON = 1;

	TRISACLR = 0x0001;
}


void spi_comm_enable(int enabled)
{
	LATAbits.LATA0 = (enabled == 0);
}


unsigned short spi_comm(unsigned short out)
{
	SPI4BUF = out;
	while(!SPI4STATbits.SPIRBF)
	{
		;
	}
	
	return SPI4BUF;
}

