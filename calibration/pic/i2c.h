#ifndef INCLUDED_I2C_H
#define INCLUDED_I2C_H


void i2c_initialize(int baudrate);
void i2c_write(char deviceaddress, char registeraddress, char value);
char i2c_readonebyte(int ack);
int i2c_sendonebyte(char value);
int i2c_startevent();
int i2c_repeatstartevent();
void i2c_stopevent();
void i2c_read(char writeaddress, char registeraddress, char * value);
void i2c_burstread(char writeaddress, char firstregisteraddress, int * value);


#endif // INCLUDED_I2C_H

