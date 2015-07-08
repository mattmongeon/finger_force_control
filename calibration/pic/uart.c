#include "uart.h"
#include "NU32.h"

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void uart1_send_packet(unsigned char* pData, int numBytes)
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

////////////////////////////////////////////////////////////////////////////////

void uart1_read_packet(unsigned char* pData, int numBytes)
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
