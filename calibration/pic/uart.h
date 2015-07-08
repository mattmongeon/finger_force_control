#ifndef INCLUDED_UART_H
#define INCLUDED_UART_H


////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// Takes in a data buffer and transmits it over UART1 as set up by the NU32 library.
//
// Params:
// pData - A pointer to the data buffer.
// numBytes - The number of bytes to be transmitted.
void uart1_send_packet(unsigned char* pData, int numBytes);


// Takes in a pointer to a data buffer and a number of expected bytes and reads
// data from the UART1 line and copies it into the buffer.
//
// Params:
// pData - A pointer to the data buffer to be filled.
// numBytes - The number of bytes expected to be received.
void uart1_read_packet(unsigned char* pData, int numBytes);


#endif  // INCLUDED_UART_H

