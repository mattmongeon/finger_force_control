#ifndef INCLUDED_PIC_COMM_H
#define INCLUDED_PIC_COMM_H

#include <string>
#include <termios.h>
#include <iostream>
#include <string.h>


// This class handles communicating with the PIC32 (NU32) over a serial connection.
class cPicSerial
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//
	
	cPicSerial();
	~cPicSerial();

	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//
	
	// Opens the serial port for communicating with the PIC.  The serial port is
	// hard-coded to /dev/ttyUSB0 and all of its communication settings are
	// hard-coded since it is expecting the NU32.
	//
	// Return - true if the serial port is successfully opened, false otherwise.
	bool OpenSerialPort();

	
	// Closes the serial port used for communicating with the PIC.
	void CloseSerialPort();

	
	// Template function for reading a particular type of data from the PIC.
	//
	// Params:
	// cmd - the string command used for requesting data from the PIC.  Needs to
	//       be a single character followed by '\n'.
	//
	// Returns - the value requested from the PIC.
	template<typename T>
	T ReadValueFromPic(const std::string& cmd) const
	{
		WriteCommandToPic(cmd);

		unsigned char buffer[sizeof(T)];
		ReadFromPic(buffer, sizeof(T));

		T retVal = reinterpret_cast<T&>(buffer);
		return retVal;
	}

	template<typename T>
	void WriteValueToPic(const std::string& cmd, T value) const
	{
		WriteCommandToPic(cmd);

		unsigned char buffer[sizeof(T)+1];
		memcpy(buffer, &value, sizeof(T));
		buffer[sizeof(T)] = '\n';
		WriteToPic(buffer, sizeof(T)+1);
	}
	
	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Sends the parameter command to the PIC.
	//
	// Params:
	// cmd - the string command used for requesting data from the PIC.  Needs to
	//       be a single character followed by '\n'.
	//
	// Return - true if sent successfully over serial, false otherwise.
	bool WriteCommandToPic(const std::string& cmd) const;


	bool WriteToPic(unsigned char* buffer, int numBytes) const;

	
	// Reads data from the PIC over the serial connection.
	//
	// Params:
	// buffer - the data buffer that will hold the data to be read.
	// numBytes - the number of bytes to be read from the PIC.
	//
	// Return - true if serial communication was successful, false otherwise.
	bool ReadFromPic(unsigned char* buffer, int numBytes) const;


	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The handle used for identifying the serial port.
	int mPortHandle;
	
};



#endif // INCLUDED_PIC_COMM_H

