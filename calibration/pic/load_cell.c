#include "NU32.h"
#include "load_cell.h"
#include "utils.h"
#include "uart.h"
#include "system.h"


////////////////////////////////////////////////////////////////////////////////
//  File-Local Declarations
////////////////////////////////////////////////////////////////////////////////

// --- Variables --- //

static volatile unsigned long adc_value_timestamp = 0;
static volatile int adc_value = 0;


// --- Constants --- //

#define LOOP_RATE_HZ  500
#define LOOP_TIMER_PRESCALAR  64


////////////////////////////////////////////////////////////////////////////////
//  Interrupt Functions
////////////////////////////////////////////////////////////////////////////////

// Timer 1 interrupt for timing reading the ADC.
void __ISR(_TIMER_1_VECTOR, IPL5SOFT) adc_timer_interrupt()
{
	unsigned long start = _CP0_GET_COUNT();
	
	AD1CHSbits.CH0SA = 2;  // Sample AN0
	AD1CON1bits.SAMP = 1;

	// Wait enough time for sampling.
	wait_nsec(250);

	// Turn off the sampling bit.
	AD1CON1bits.SAMP = 0;

	// Start the conversion process.  The PIC will tell us when it is done
	// by setting the DONE bit to 1.
	while( !AD1CON1bits.DONE )
	{
		;
	}

	// Read in the ADC value.
	adc_value = ADC1BUF0;

	unsigned long end = _CP0_GET_COUNT();

	unsigned long time = end - start;

	if( system_get_state() == LOAD_CELL_CONTINUOUS_READ )
	{
		uart1_send_packet( (unsigned char*)(&start), sizeof(unsigned long) );
		uart1_send_packet( (unsigned char*)(&time), sizeof(unsigned long) );
		uart1_send_packet( (unsigned char*)(&adc_value), sizeof(int) );

		int load_cell_val = load_cell_read_grams();
		uart1_send_packet( (unsigned char*)(&load_cell_val), sizeof(int) );
	}

	IFS0CLR = 0x10;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void load_cell_init()
{
	// --- Configure ADC --- //

	AD1PCFGbits.PCFG2 = 0;  // Use AN2
	AD1CON3bits.ADCS = 2;
	AD1CON1bits.ADON = 1;
	
	
	// --- Configure Timer 1 --- //

	PR1 = ((NU32_SYS_FREQ / LOOP_RATE_HZ) / LOOP_TIMER_PRESCALAR) - 1;
	T1CON = 0x0020;
	TMR1 = 0;
	
	
	// --- Set Up Timer 1 Interrupt --- //
	
	IPC1SET = 0x14;     // priority 5, subpriority 0
	IFS0CLR = 0x10;  // clear the interrupt flag
	IEC0SET = 0x10;  // Enable the interrupt

	
	// --- Start Controller Timer --- //

	T1CONSET = 0x8040;
}

////////////////////////////////////////////////////////////////////////////////

int load_cell_read_grams()
{
	float f = 8.56463 + (1.03208 * load_cell_raw_value());

	int retVal = (int)f;
	if( retVal >= 0 )
		return retVal;
	else
		return 0;
}

////////////////////////////////////////////////////////////////////////////////

int load_cell_raw_value()
{
	return adc_value;
}

