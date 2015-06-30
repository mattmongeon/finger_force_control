#include "NU32.h"
#include "load_cell.h"
#include "utils.h"
#include "uart.h"


static volatile unsigned long adc_value_timestamp = 0;
static volatile int adc_value = 0;
static volatile int enable_continuous = 0;


#define LOOP_RATE_HZ  320
#define LOOP_TIMER_PRESCALAR  16


/*
Notes:
- Transmit a timestamp for the beginning of the ADC loop
  o In the UI take the difference between timestamps during gain tuning to ensure
    the loop really is running regularly.
  o Update the execution time of each loop in the console.
  o Update the time between ADC loop ticks.
- Try doing the full BioTac calibration functionality.

- Figure out why my loop is triggering faster than I think I have it set for.
  o The ticks tell me it is triggering at 512 Hz even though my numbers say 320.  
- Fix the slight ADC noise.
  o Average the ADC value over a few readings internal to the loop.
  o Double-check all of the loop frequency and execution stuff to ensure we are still ok.
  o Make sure to test it all during gain tuning since that is currently most intensive.
- Debug why the loop is getting stuck
  o Most likely it is due to the UART Tx buffer loop.  When the UI is constantly receiving, it
    never gets stuck.  This tells me sometimes the Tx buffer gets stuck.
*/


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

	if( enable_continuous )
	{
		LCD_Clear();
		char b[20];
		sprintf(b, "Ticks: %d", time);
		LCD_WriteString(b);

		uart1_send_packet( (unsigned char*)(&start), sizeof(unsigned long) );
		uart1_send_packet( (unsigned char*)(&time), sizeof(unsigned long) );
		uart1_send_packet( (unsigned char*)(&adc_value), sizeof(int) );
	}

	IFS0CLR = 0x10;
}


void load_cell_init()
{
	// --- Configure ADC --- //

	AD1PCFGbits.PCFG2 = 0;  // Use AN2
	AD1CON3bits.ADCS = 2;
	AD1CON1bits.ADON = 1;
	
	
	// --- Configure Timer 1 --- //

	PR1 = ((NU32_SYS_FREQ / LOOP_RATE_HZ) / LOOP_TIMER_PRESCALAR) - 1;
	T1CON = 0x0040;
	TMR1 = 0;
	
	
	// --- Set Up Timer 1 Interrupt --- //
	
	IPC1SET = 0x14;     // priority 5, subpriority 0
	IFS0CLR = 0x10;  // clear the interrupt flag
	IEC0SET = 0x10;  // Enable the interrupt

	
	// --- Start Controller Timer --- //

	T1CONSET = 0x8000;
}


int load_cell_read_grams()
{
	float f = 1301.1 - (1.938 * load_cell_raw_value());

	int retVal = (int)f;
	if( retVal >= 0 )
		return retVal;
	else
		return 0;
}


int load_cell_raw_value()
{
	return adc_value;
}

void load_cell_continuous_raw(int enable)
{
	enable_continuous = enable;
}

