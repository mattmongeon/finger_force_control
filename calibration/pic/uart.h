#ifndef INCLUDED_UART_H
#define INCLUDED_UART_H


int uart1_send_packet(unsigned char* pData, int numBytes);
int uart1_read_packet(unsigned char* pData, int numBytes);


#endif  // INCLUDED_UART_H

