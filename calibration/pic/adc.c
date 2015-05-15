#include "NU32.h"
#include "adc.h"
#include "utils.h"
#include "i2c_master_int.h"
#include <i2c/plib_i2c.h>


#define ADC_ADDRESS   (0x2A << 1)  // Shift left because of the R/W bit
#define ADC_I2C_BUS   I2C_ID_1
#define ADC_I2C_BAUD  50000

#define PU_CTRL_ADDR  0x00
#define CTRL2_ADDR    0x02
#define ADC_REGISTERS_ADDR 0x15


volatile int adc_data_ready = 0;


void __ISR(_EXTERNAL_2_VECTOR, IPL5SOFT) adc_auto_read()
{
	// PORTEbits.RE9
    // Read from the ADC
	if( PORTEbits.RE9 )
		adc_data_ready = 1;

    IFS0CLR = 0x800;
}


void master_start()
{
	while( PLIB_I2C_ReceiverOverflowHasOccurred(ADC_I2C_BUS) )
	{
		PLIB_I2C_ReceiverOverflowClear(ADC_I2C_BUS);
	}

	while( PLIB_I2C_TransmitterOverflowHasOccurred(ADC_I2C_BUS) )
	{
		PLIB_I2C_TransmitterOverflowClear(ADC_I2C_BUS);
	}

	while( PLIB_I2C_ArbitrationLossHasOccurred(ADC_I2C_BUS) )
	{
		;
	}
	
	PLIB_I2C_MasterStart(ADC_I2C_BUS);

	while(!PLIB_I2C_StartWasDetected(ADC_I2C_BUS))
	{
		;
	}
}


void master_restart()
{
	while( PLIB_I2C_ReceiverOverflowHasOccurred(ADC_I2C_BUS) )
	{
		PLIB_I2C_ReceiverOverflowClear(ADC_I2C_BUS);
	}

	while( PLIB_I2C_TransmitterOverflowHasOccurred(ADC_I2C_BUS) )
	{
		PLIB_I2C_TransmitterOverflowClear(ADC_I2C_BUS);
	}

	while( PLIB_I2C_ArbitrationLossHasOccurred(ADC_I2C_BUS) )
	{
		;
	}
	
	PLIB_I2C_MasterStartRepeat(ADC_I2C_BUS);
}


void transmit_byte(uint8_t byte)
{
	while( PLIB_I2C_ReceiverOverflowHasOccurred(ADC_I2C_BUS) )
	{
		PLIB_I2C_ReceiverOverflowClear(ADC_I2C_BUS);
	}

	while( PLIB_I2C_TransmitterOverflowHasOccurred(ADC_I2C_BUS) )
	{
		PLIB_I2C_TransmitterOverflowClear(ADC_I2C_BUS);
	}

	PLIB_I2C_TransmitterByteSend(ADC_I2C_BUS, byte);

	while( !PLIB_I2C_TransmitterByteHasCompleted(ADC_I2C_BUS) && PLIB_I2C_TransmitterIsBusy(ADC_I2C_BUS) )
	{
		;
	}

	while( !PLIB_I2C_TransmitterByteWasAcknowledged(ADC_I2C_BUS) )
	{
		;
	}
}


uint8_t read_one_byte()
{
    while( PLIB_I2C_ReceiverOverflowHasOccurred(ADC_I2C_BUS) )
    {
        PLIB_I2C_ReceiverOverflowClear(ADC_I2C_BUS);
    }
	
    PLIB_I2C_MasterReceiverClock1Byte(ADC_I2C_BUS);

	while( !PLIB_I2C_ReceivedByteIsAvailable(ADC_I2C_BUS) )
	{
		;
	}

	uint8_t byte = PLIB_I2C_ReceivedByteGet(ADC_I2C_BUS);

    while( !PLIB_I2C_MasterReceiverReadyToAcknowledge(ADC_I2C_BUS) )
    {
		;
    }

	// Send NACK
	PLIB_I2C_ReceivedByteAcknowledge(ADC_I2C_BUS, false);
	while(!PLIB_I2C_ReceiverByteAcknowledgeHasCompleted(ADC_I2C_BUS))
	{
		;
	}

	return byte;
}


void stop_transmission()
{
	while(!PLIB_I2C_BusIsIdle(ADC_I2C_BUS))
	{
		;
	}

	while(PLIB_I2C_TransmitterOverflowHasOccurred(ADC_I2C_BUS))
	{
		PLIB_I2C_TransmitterOverflowClear(ADC_I2C_BUS);
	}

	PLIB_I2C_MasterStop(ADC_I2C_BUS);

	while(!PLIB_I2C_StopWasDetected(ADC_I2C_BUS))
	{
		;
	}
}


void transmit_single_register(uint8_t reg_addr, uint8_t data)
{
	while( !PLIB_I2C_BusIsIdle(ADC_I2C_BUS) )
	{
		;
	}


	// --- Start Master Transmit --- //
	
	master_start();


	// --- Send Address Byte --- //

	transmit_byte(ADC_ADDRESS);


	// --- Transmit Configuration Data --- //

	// First the register address
	transmit_byte(reg_addr);

	// Now the data
	transmit_byte(data);

	// --- Stop Transmission --- //

	stop_transmission();
}


void transmit_burst_data( uint8_t start_reg_addr, uint8_t* pBuffer, int num_bytes )
{
	while( !PLIB_I2C_BusIsIdle(ADC_I2C_BUS) )
	{
		;
	}


	// --- Start Master Transmit --- //
	
	master_start();


	// --- Send Address Byte --- //

	transmit_byte(ADC_ADDRESS);


	// --- Transmit Configuration Data --- //

	// First the register address
	transmit_byte(start_reg_addr);

	// Now the data buffer
	int i = 0;
	while( i < num_bytes )
	{
		transmit_byte(pBuffer[i]);
		++i;
	}

	// --- Stop Transmission --- //

	stop_transmission();
}


uint8_t receive_single_register(uint8_t reg_addr)
{
	while( !PLIB_I2C_BusIsIdle(ADC_I2C_BUS) )
	{
		;
	}


	// --- Start Master Transmit --- //
	
	master_start();


	// --- Send Address Byte --- //

	transmit_byte(ADC_ADDRESS);


	// --- Specify Register --- //

	transmit_byte(reg_addr);


	// --- Redo Start --- //

	master_restart();


	// --- Send Address With R Bit --- //

	transmit_byte(ADC_ADDRESS | 0x01);


	// --- Receive Data --- //

	uint8_t byte = read_one_byte();

	
	// --- Stop Transmission --- //

	stop_transmission();

	return byte;
}


int init_adc()
{
	i2c_master_setup();
	

	// --- Initialize The ADC --- //

	i2c_write_byte(PU_CTRL_ADDR, 0x01);
	i2c_write_byte(PU_CTRL_ADDR, 0x02);
	
	// The documentation says to wait about 200 microseconds, but we will wait for 400
	// just to be safe.
	wait_usec(400);

	// Check the PUR bit
	unsigned char readMe = 0;
	i2c_write_read(PU_CTRL_ADDR, NULL, 0, &readMe, 1);
	if( !(readMe & 0x08) )
		return -1;

	// It is powered up and ready.  Now do the rest of the initialization.
	i2c_write_byte(PU_CTRL_ADDR, 0x3E);
	/* i2c_write_byte(CTRL2_ADDR, 0x30); */
	/* i2c_write_byte(ADC_REGISTERS_ADDR, 0x30); */


	// --- Initialize Notification Pin --- //
	
	// Take in the notification pin
	INTCONSET = 0x4;       // step 3: INT2 triggers on rising edge
	IPC2SET = 0x17 << 24;  // step 4: Set priority to 5, subpriority to 3
	IFS0CLR = 0x800;       // step 5: clear the interrupt flag.
	IEC0SET = 0x800;       // step 6: enable INT2 interrupt
	
	return 0;
}


int check_data_available()
{
	//i2c_write_read(PU_CTRL_ADDR, 
}
