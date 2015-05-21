#ifndef INCLUDED_PIC_COMM_H
#define INCLUDED_PIC_COMM_H

#include <string>
#include <termios.h>


class cPicComm
{
public:

	struct sPicData
	{

	};

	
public:

	cPicComm();
	~cPicComm();

	bool OpenSerialPort();
	void CloseSerialPort();

	// Reads the raw ADC value that represents the load cell's output voltage.
	int ReadRawLoadCellValue();

	int ReadLoadCellValue_grams();
	
	
private:

	bool WriteCommandToPic(const std::string& cmd);

	bool ReadFromPic(unsigned char* buffer, int numBytes);
	
	int mPortHandle;
	
};



#endif // INCLUDED_PIC_COMM_H

