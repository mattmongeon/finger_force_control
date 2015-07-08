#ifndef INCLUDED_SPI_H
#define INCLUDED_SPI_H

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// Initializes SPI communication and configures it for use by the BioTac.
void spi_init();


// Controls the slave select pin used in SPI communication.
//
// Params:
// enabled - Set to 1 to enable the slave select line, 0 to disable.
void spi_comm_enable(int enabled);


// Performs the task of transferring data over the SPI line.  Transmits the
// parameter data and returns data received on the line.
//
// Params:
// out - The data to be transmitted.
//
// Return - The data received in this SPI burst.
unsigned short spi_comm(unsigned short out);


#endif  // INCLUDED_SPI_H

