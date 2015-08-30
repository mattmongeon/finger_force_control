#include "pic_serial.h"
#include "utils.h"
#include "stopwatch.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>


#ifdef WIN32
#include <windows.h>
#define UART_PORT_ID "COM3"
#else
#include <termios.h>
#define UART_PORT_ID "/dev/ttyUSB0"
#endif


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cPicSerial::cPicSerial()
	: mIO(), mpPort(0)
{
}

////////////////////////////////////////////////////////////////////////////////

cPicSerial::~cPicSerial()
{
	CloseSerialPort();
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

bool cPicSerial::OpenSerialPort()
{
	try
	{
		if( mpPort == 0 )
		{
			mpPort = new boost::asio::serial_port(mIO, UART_PORT_ID);
			mpPort->set_option(boost::asio::serial_port_base::baud_rate(230400));
			mpPort->set_option(boost::asio::serial_port_base::character_size(8));
			mpPort->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
			mpPort->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
			mpPort->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::hardware));
		}
	}
	catch(...)
	{
		delete mpPort;
		mpPort = 0;
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void cPicSerial::CloseSerialPort()
{
	delete mpPort;
	mpPort = 0;
}

////////////////////////////////////////////////////////////////////////////////

bool cPicSerial::WriteCommandToPic(const std::string& cmd) const
{
	return WriteToPic((unsigned char*)(cmd.data()), 2);
}

////////////////////////////////////////////////////////////////////////////////

bool cPicSerial::WriteToPic(unsigned char* buffer, unsigned int numBytes) const
{
	return boost::asio::write( *mpPort,
							   boost::asio::buffer(buffer, numBytes) ) == numBytes;
}

////////////////////////////////////////////////////////////////////////////////

bool cPicSerial::ReadFromPic(unsigned char* buffer, int numBytes) const
{
	int numRead = 0;
	while( numRead < numBytes )
	{
		int n = boost::asio::read( *mpPort,
								   boost::asio::buffer(&(buffer[numRead]), numBytes - numRead),
								   boost::asio::transfer_at_least(1) );

		if( n > 0 )
		{
			numRead += n;
		}
	}

	return (numRead == numBytes);
}

////////////////////////////////////////////////////////////////////////////////

void cPicSerial::DiscardIncomingData(int wait_ms) const
{
	if( wait_ms > 0 )
	{
		cStopwatch timer;
		timer.Start();
		while( timer.GetElapsedTime_ms() < wait_ms )
		{
			;
		}
	}

	FlushPort();
}

////////////////////////////////////////////////////////////////////////////////

bool cPicSerial::CheckDataAvailable() const
{
#ifdef WIN32
	return true;
#else
	int bytes = 0;
	ioctl( mpPort->native_handle(), FIONREAD, &bytes );
	return (bytes != 0);
#endif
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cPicSerial::FlushPort() const
{
#ifdef WIN32
	PurgeComm( mpPort->native_handle(),
			   PURGE_RXABORT || PURGE_RXCLEAR || PURGE_TXABORT || PURGE_TXCLEAR);
#else
	tcflush( mpPort->native_handle(), TCIFLUSH );
#endif
}

