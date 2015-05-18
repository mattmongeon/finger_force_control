#include "isense.h"
#include "NU32.h"

#define SAMPLE_TIME 10


void isense_init()
{
	AD1PCFGbits.PCFG0 = 0;  // Use AN0
	AD1CON3bits.ADCS = 2;
	AD1CON1bits.ADON = 1;
}


int isense_read_single()
{
	unsigned int elapsed = 0, finish_time = 0;
	AD1CHSbits.CH0SA = 0;  // Sample AN0
	AD1CON1bits.SAMP = 1;
	
	// Wait enough time to sample the voltage.
	finish_time = _CP0_GET_COUNT() + SAMPLE_TIME;
	while( _CP0_GET_COUNT() < finish_time )
	{
		;
	}

	// Start the conversion process.
	AD1CON1bits.SAMP = 0;
	while( !AD1CON1bits.DONE )
	{
		;
	}

	return ADC1BUF0;
}

int isense_ticks()
{
	int i = 0;
	int val = 0;
	for( ; i < 50; ++i )
	{
		val += isense_read_single();
	}

	return val / 50;
}

int isense_mA()
{
	// Using y = mx + b with x being ticks.  Linear model basd on a model fit
	// using Mathematica using several current measurements.
	int mA = isense_ticks();
	mA *= 102209;
	mA /= 500;
	mA -= 103007;
	mA /= 100;
	
    return mA;
}
