#ifndef INCLUDED_I2C_H
#define INCLUDED_I2C_H


typedef volatile unsigned char* volatile buffer_t;


void i2c_setup();


// initiate an i2c write read operation at the given address. You can optionally only read or only write by passing zero lengths for the reading or writing
// this will not return until the transaction is complete.  returns false on error
int i2c_write_read(unsigned int addr, const buffer_t write, unsigned int wlen, const buffer_t read, unsigned int rlen );


#endif // INCLUDED_I2C_H

