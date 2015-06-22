#include "NU32.h"
#include "load_cell.h"
#include "i2c.h"

#define ADC_ADDRESS   (0x2A << 1)
#define ADC_I2C_BUS   I2C_ID_1
#define ADC_I2C_BAUD  50000

#define PU_CTRL_ADDR  0x00
#define CTRL2_ADDR    0x02
#define ADC_REGISTERS_ADDR 0x15
#define I2C_CONTROL_ADDR 0x11
#define ADCO_B2_ADDR  0x12


static volatile unsigned long adc_value_timestamp = 0;
static volatile int adc_value = 0;
static int sample_rate_hz = 20;


static int read_adc()
{
	char value1 = 0, value2 = 0, value3 = 0;
	i2c_read(ADC_ADDRESS, ADCO_B2_ADDR, &value1);
	i2c_read(ADC_ADDRESS, ADCO_B2_ADDR + 1, &value2);
	i2c_read(ADC_ADDRESS, ADCO_B2_ADDR + 2, &value3);
	return (value1<<16) + (value2<<8) + (value3);
}


static void __ISR(_EXTERNAL_2_VECTOR, IPL5SOFT) adc_auto_read()
{
	if( PORTEbits.RE9 )
	{
		adc_value_timestamp = _CP0_GET_COUNT();
		adc_value = read_adc();
	}

    IFS0CLR = 0x800;
}


int load_cell_init()
{
	// --- Initialize Notification Pin --- //
	
	// Take in the notification pin
	INTCONSET = 0x4;       // step 3: INT2 triggers on rising edge
	IPC2SET = 0x17 << 24;  // step 4: Set priority to 5, subpriority to 3
	IFS0CLR = 0x800;       // step 5: clear the interrupt flag.
	IEC0SET = 0x800;       // step 6: enable INT2 interrupt
	

	// --- Initialize I2C --- //
	
	i2c_initialize(90);
	

	// --- Initialize The ADC --- //

	i2c_write(ADC_ADDRESS, PU_CTRL_ADDR, 0x01);
	i2c_write(ADC_ADDRESS, PU_CTRL_ADDR, 0x02);
	
	// The documentation says to wait about 200 microseconds, but we will wait for 400
	// just to be safe.
	wait_usec(400);

	// Check the PUR bit
	unsigned char readMe = 0;
	i2c_read(ADC_ADDRESS, PU_CTRL_ADDR, &readMe);
	if( !(readMe & 0x08) )
		return -1;

	// It is powered up and ready.  Now do the rest of the initialization.
	i2c_write(ADC_ADDRESS, PU_CTRL_ADDR, 0x3E);
	i2c_write(ADC_ADDRESS, CTRL2_ADDR, 0x10);  // 20 samples per second

	return 0;
}


int load_cell_read_grams()
{
	float f = 1289.34 - (0.000178688 * load_cell_raw_value());

	int retVal = (int)f;
	if( retVal >= 0 )
		return retVal;
	else
		return 0;
}


int load_cell_raw_value()
{
	static unsigned long last_timestamp = 0;

	// Wait for a unique value.
	while(last_timestamp == adc_value_timestamp)
	{
		;
	}

	
	__builtin_disable_interrupts();
	
	int retVal = adc_value;
	last_timestamp = adc_value_timestamp;

	__builtin_enable_interrupts();

	
	return retVal;
}

int load_cell_sample_rate_hz()
{
	return sample_rate_hz;
}

