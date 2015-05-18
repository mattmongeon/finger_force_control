#include "i2c.h"
#include "NU32.h"


//Turn on I2C Module and calibrate for standard operation.
void i2c_initialize(int baudrate)
{
	I2C1BRG = baudrate;
	
    // Enables the I2C module and configures SDA2 (A3) and SCL2 (A2) as serial pins
	I2C1CONbits.ON = 1;
}


//This function sends one byte via I2C. It should only be called after a
//start event.
int i2c_sendonebyte(char value)
{
	int success = 0;
	
    //check to see that the transmitter buffer is empty
    if(!I2C1STATbits.TBF)
	{
		//Load the transmit register with the value to transmit
		I2C1TRN = value;

		//Check to ensure there was not a bus collision or a write collision
		if(I2C1STATbits.BCL | I2C1STATbits.IWCOL)
		{
			success = -1;
		}
		
		//wait until transmission status is cleared at the end of transmisssion
		while(I2C1STATbits.TRSTAT)
		{
			;
		}
		
		//report error if transmit buffer was already full
    }
	else
	{
		success = -1;
    }

    //Report if byte not acknowledged. This could indicate that the wrong byte
    //was sent or the SCLK and SDA lines were not hooked up correctly.
    if (I2C1STATbits.ACKSTAT)
	{
		success = -1;
    }

	return success;
}

//This function reads one byte via I2C. It should only be used after a start
//event and the slave device have already been addressed as per I2C protocol
char i2c_readonebyte(int ack)
{
    //Enable recieve
    I2C1CONbits.RCEN = 1;
    //Report if recieve events overlap.
    if(I2C1STATbits.I2COV)
	{
		// Clear the overflow
		I2C1STATbits.I2COV = 0;
    }

    //Wait until recieve register is full.
    while(!I2C1STATbits.RBF)
	{
		;
    }
	
    //Save recieved data to variable. Automatically clears RCEN and RBF
	char retVal = I2C1RCV;

	if( ack )
	{
		I2C1CONbits.ACKDT = 0;
		I2C1CONbits.ACKEN = 1;
	}

	return retVal;
}

//Initiates a start event, master pulls SDA low hwile SCLK is high
int i2c_startevent()
{
	int success = 0;
	
    //Check that the last event was a stop event which means the bus is idle
    if(!I2C1STATbits.S)
	{
	    //initiate start event
	    I2C1CONbits.SEN = 1;
        //check for bus collisions
		if (I2C1STATbits.BCL)
		{
			success = -1;
        }
		
		//wait until the end of the start event clears the start enable bit
		while(I2C1CONbits.SEN)
		{
	        ;
        }
    }
    else
	{
		success = -1;
    }

	return success;
}

//Initiates a repeat start event
int i2c_repeatstartevent()
{
	int success = -1;
	
	//Initiates a repeat start event, master pulls SDA low
    I2C1CONbits.RSEN = 1;
    if (I2C1STATbits.BCL)
	{
		success = -1;
    }
	
    //wait until the end of the repeatstart event clears the repeat
    //start enable bit
    while(I2C1CONbits.RSEN)
	{
		;
    }

	return success;
}

//Initiates a stop event to end transmission and leave bus idle
void i2c_stopevent()
{
    //Initiates a stop event, master sets SDA high on a high SCLK
    I2C1CONbits.PEN = 1;

    //wait for stop event enable to be cleared by completion of stop event
    while(I2C1CONbits.PEN)
	{
		;
    }
}

//This function initiates a full transmission to write a single byte to
//a register of a slave device via I2C. The device address is the 7 bit
//address of the slave device with a 0 as the least significant bit.
void i2c_write(char deviceaddress, char registeraddress, char value)
{
    i2c_startevent();

    i2c_sendonebyte(deviceaddress);
    i2c_sendonebyte(registeraddress);
    i2c_sendonebyte(value);

    i2c_stopevent();
}

//This function initiates a full transmission to read a single byte from
//a register of a slave device via I2C.The device address is the 7 bit
//address of the slave device with a 0 as the least significant bit.
void i2c_read(char writeaddress, char registeraddress, char * value)
{
    char readaddress = writeaddress + 0x01;
    i2c_startevent();
	i2c_sendonebyte(writeaddress);
	i2c_sendonebyte(registeraddress);

    i2c_repeatstartevent();
	i2c_sendonebyte(readaddress);
	*value = i2c_readonebyte(0);
	i2c_stopevent();
}

