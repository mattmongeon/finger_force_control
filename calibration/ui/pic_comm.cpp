#include "pic_comm.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>


cPicComm::cPicComm()
{
	mPortHandle = -1;
}

cPicComm::~cPicComm()
{

}

bool cPicComm::OpenSerialPort()
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

void cPicComm::CloseSerialPort()
{
	if( mPortHandle != -1 )
	{
		close(mPortHandle);
		mPortHandle = -1;
	}
}

int cPicComm::ReadRawLoadCellValue()
{
	WriteCommandToPic(nUtils::READ_RAW_LOAD_CELL);

	std::cout << "Reading from PIC" << std::endl;
	unsigned char buffer[4];
	ReadFromPic(buffer, 3);
	buffer[3] = 0;

	int retVal = reinterpret_cast<int&>(buffer);
	return retVal;
}

bool cPicComm::WriteCommandToPic(const std::string& cmd)
{
	write(mPortHandle, cmd.data(), 2);
	return 1;
}

bool cPicComm::ReadFromPic(unsigned char* buffer, int numBytes)
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
