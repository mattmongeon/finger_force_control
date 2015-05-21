#include "pic_serial.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>


cPicSerial::cPicSerial()
{
	mPortHandle = -1;
}

cPicSerial::~cPicSerial()
{
	if( mPortHandle != -1 )
		CloseSerialPort();
}

bool cPicSerial::OpenSerialPort()
{
	termios tio;
	memset(&tio, 0, sizeof(tio));

	// Setting characters to 8N1
	tio.c_cflag &= ~PARENB;
	tio.c_cflag &= ~CSTOPB;
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= CS8;

	// Enable hardware flow control
	tio.c_cflag |= CRTSCTS;

	// Use raw input
	tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	// Use raw output
	tio.c_oflag &= ~OPOST;

	tio.c_cc[VINTR] = 021;  // Interrupt - set to CTRL+C
	tio.c_cc[VQUIT] = 023;  // Interrupt - set to CTRL+Z
	tio.c_cc[VMIN] = 1;     // Minimum read 1 character
	tio.c_cc[VTIME] = 100; // Timeout of 10 seconds
	
	mPortHandle = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if( mPortHandle == -1 )
		return false;

	cfsetospeed(&tio, B230400);
	cfsetispeed(&tio, B230400);

	tcsetattr(mPortHandle, TCSANOW, &tio);
	
	return true;
}

void cPicSerial::CloseSerialPort()
{
	if( mPortHandle != -1 )
	{
		close(mPortHandle);
		mPortHandle = -1;
	}
}

bool cPicSerial::WriteCommandToPic(const std::string& cmd) const
{
	return WriteToPic((unsigned char*)(cmd.data()), 2);
	// return write(mPortHandle, cmd.data(), 2) == 2;
}

bool cPicSerial::WriteToPic(unsigned char* buffer, int numBytes) const
{
	return write(mPortHandle, buffer, numBytes) == numBytes;
}

bool cPicSerial::ReadFromPic(unsigned char* buffer, int numBytes) const
{
	int numRead = 0;
	while( numRead < numBytes )
	{
		int n = read(mPortHandle, &(buffer[numRead]), numBytes - numRead);
		if( n > 0 )
		{
			numRead += n;
		}
	}

	return (numRead == numBytes);
}
