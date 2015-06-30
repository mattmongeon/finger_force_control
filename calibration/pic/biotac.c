#include "biotac.h"
#include "system.h"
#include "spi.h"
#include "utils.h"
#include "uart.h"
#include "load_cell.h"
#include "NU32.h"


#define LOOP_RATE_HZ 100
#define LOOP_TIMER_PRESCALAR 16


static int numCalibrationSamples = 0;


// --- BioTac Sampling Commands --- //

#define BIOTAC_SAMPLE_PAC (unsigned short)(0x8001)
#define BIOTAC_SAMPLE_PDC (unsigned short)(0x8301) 
#define BIOTAC_SAMPLE_TAC (unsigned short)(0x8501) 
#define BIOTAC_SAMPLE_TDC (unsigned short)(0x8601) 
#define BIOTAC_SAMPLE_E1  (unsigned short)(0xA201) 
#define BIOTAC_SAMPLE_E2  (unsigned short)(0xA401) 
#define BIOTAC_SAMPLE_E3  (unsigned short)(0xA701) 
#define BIOTAC_SAMPLE_E4  (unsigned short)(0xA801) 
#define BIOTAC_SAMPLE_E5  (unsigned short)(0xAB01) 
#define BIOTAC_SAMPLE_E6  (unsigned short)(0xAD01) 
#define BIOTAC_SAMPLE_E7  (unsigned short)(0xAE01) 
#define BIOTAC_SAMPLE_E8  (unsigned short)(0xB001) 
#define BIOTAC_SAMPLE_E9  (unsigned short)(0xB301) 
#define BIOTAC_SAMPLE_E10 (unsigned short)(0xB501) 
#define BIOTAC_SAMPLE_E11 (unsigned short)(0xB601) 
#define BIOTAC_SAMPLE_E12 (unsigned short)(0xB901) 
#define BIOTAC_SAMPLE_E13 (unsigned short)(0xBA01) 
#define BIOTAC_SAMPLE_E14 (unsigned short)(0xBC01) 
#define BIOTAC_SAMPLE_E15 (unsigned short)(0xBF01) 
#define BIOTAC_SAMPLE_E16 (unsigned short)(0xC101) 
#define BIOTAC_SAMPLE_E17 (unsigned short)(0xC201) 
#define BIOTAC_SAMPLE_E18 (unsigned short)(0xC401) 
#define BIOTAC_SAMPLE_E19 (unsigned short)(0xC701) 


// This is used for sending out over the MOSI line when reading
// from the BioTac.
#define BIOTAC_DUMMY_DATA (unsigned short)(0x0100)


#define READ_BIOTAC_SAMPLE(cmd, var)				 \
	/* Request data */								 \
	spi_comm_enable(1);								 \
	spi_comm((cmd));								 \
	spi_comm_enable(0);								 \
													 \
	/* Let the BioTac fill in its data buffer */	 \
	wait_usec(50);									 \
													 \
	/* Read data from BioTac */						 \
	spi_comm_enable(1);								 \
	(var) = spi_comm(BIOTAC_DUMMY_DATA);			 \
	spi_comm_enable(0);								 \
													 \
	/* The data seems to be big-endian */			 \
	(var) = ((var) >> 9) + (((var) & 0x00FF) >> 3);	 \
													 \
	wait_usec(10);


void __ISR(_TIMER_5_VECTOR, IPL4SOFT) biotac_reader_int()
{
	static biotac_data data;
	
	switch(system_get_state())
	{
	case BIOTAC_CAL_SINGLE:
	{
		// Read a single BioTac reading and associated load cell reading
		// and transmit it.
		unsigned long time_stamp = _CP0_GET_COUNT();
		read_biotac(&data);
		int load_cell = load_cell_read_grams();

		uart1_send_packet((unsigned char*)(&time_stamp), sizeof(unsigned long));
		uart1_send_packet((unsigned char*)(&data), sizeof(biotac_data));
		uart1_send_packet((unsigned char*)(&load_cell), sizeof(int));

		++numCalibrationSamples;
		if( numCalibrationSamples > 200 )
		{
			// Signal end of data stream to the UI.
			
			memset(&time_stamp, 0x00, sizeof(unsigned long));
			uart1_send_packet((unsigned char*)(&time_stamp), sizeof(unsigned long));

			memset(&data, 0x00, sizeof(biotac_data));
			uart1_send_packet((unsigned char*)(&data), sizeof(biotac_data));

			memset(&load_cell, 0x00, sizeof(int));
			uart1_send_packet((unsigned char*)(&load_cell), sizeof(int));
			
			system_set_state(IDLE);
		}
		
		break;
	}

	default:
		numCalibrationSamples = 0;
		break;
	}
	
	IFS0CLR = 1 << 20;
}

void biotac_init()
{
	spi_init();

	
	// --- Set Up Timer 3 Interrupt --- //

	PR5 = ((NU32_SYS_FREQ / LOOP_RATE_HZ) / LOOP_TIMER_PRESCALAR) - 1;
	T5CON = 0x0040;
	TMR5 = 0;

	IPC5SET = 0x11;     // priority 4, subpriority 1
	IFS0CLR = 1 << 20;  // Clear the interrupt flag
	IEC0SET = 1 << 20;  // Enable the interrupt

	T5CONSET = 0x8000;
}


void read_biotac(biotac_data* pData)
{
	if( pData == 0 )
		return;

	memset(pData, 0, sizeof(biotac_data));
	
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E1,  pData->e1  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E2,  pData->e2  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E3,  pData->e3  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E4,  pData->e4  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E5,  pData->e5  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E6,  pData->e6  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E7,  pData->e7  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E8,  pData->e8  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E9,  pData->e9  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E10, pData->e10 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E11, pData->e11 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E12, pData->e12 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E13, pData->e13 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E14, pData->e14 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E15, pData->e15 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E16, pData->e16 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E17, pData->e17 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E18, pData->e18 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E19, pData->e19 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PDC, pData->pdc )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_TAC, pData->tac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_TDC, pData->tdc )
}

