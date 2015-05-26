#ifndef INCLUDED_SPI_H
#define INCLUDED_SPI_H


void spi_init();


void spi_comm_enable(int enabled);


unsigned short spi(unsigned short out);


#endif  // INCLUDED_SPI_H

