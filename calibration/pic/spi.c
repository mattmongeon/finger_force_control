#include "spi.h"
#include "NU32.h"


void spi_init()
{
	SPI1CON = 0;
	SPI1BRG = 39;   // 1 MHz
	SPI1STATbits.SPIROV = 0; // Clear overflow flag

	SPI1CONbits.MODE32 = 0;  // 16-bit mode
	SPI1CONbits.CKP = 0;     // Clock is active high, idle low
	SPI1CONbits.CKE = 0;     // Serial data changes on idle to low
	SPI1CONbits.MSTEN = 1;   // Enable Master Mode
	SPI1CONbits.SMP = 1;
	SPI1CONbits.ON = 1;

	TRISACLR = 0x0001;
}


void spi_comm_enable(int enabled)
{
	LATAbits.LATA0 = (enabled == 0);
}


unsigned short spi(unsigned short out)
{
	SPI1BUF = out;
	while(!SPI1STATbits.SPIRBF)
	{
		;
	}
	
	return SPI1BUF;
}

