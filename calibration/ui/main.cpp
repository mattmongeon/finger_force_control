#include <iostream>
#include <string>
#include "pic_serial.h"
#include "load_cell.h"
#include "utils.h"


void printValues( double loadCellForce, double biotacForce, int sampleNum )
{
	std::cout << nUtils::PREV_LINE << nUtils::PREV_LINE << "\r" << std::flush;
	std::cout << "Sample " << sampleNum << std::endl;
	std::cout << "Load Cell Force:  " << loadCellForce << " kg" << std::endl;
	std::cout << "Biotac Force:  " << biotacForce << " kg" << std::flush;
}


void printMenu()
{
	std::cout << "Function Menu" << std::endl;
	std::cout << "-------------" << std::endl;
	std::cout << std::endl;
	std::cout << "c:  Calibrate load cell" << std::endl;
	std::cout << "l:  Read load cell" << std::endl;
	std::cout << "p:  Set motor PWM duty cycle" << std::endl;
	std::cout << "q:  Quit" << std::endl;
	std::cout << std::endl;
}


int main(int argc, char** argv)
{
	std::cout << nUtils::CLEAR_CONSOLE << std::flush;
	
	std::cout << "Opening serial port to communicate with PIC..." << std::endl;

	cPicSerial picSerial;
	if( !picSerial.OpenSerialPort() )
	{
		std::cout << "Error opening serial port!" << std::endl;
		return 0;
	}

	std::cout << "Serial port opened and ready!" << std::endl;
	std::cout << std::endl;

	std::cout << "Constructing load cell instance" << std::endl;
	cLoadCell loadCell(&picSerial);

	std::cout << "Ready to begin!" << std::endl;


	// --- Main State Machine --- //
	
	bool keepGoing = true;
	while(keepGoing)
	{
		printMenu();
		switch(nUtils::GetMenuSelection())
		{
		case 'c':
		{
			loadCell.RunCalibrationRoutine();
			break;
		}

		case 'l':
		{
			std::cout << "Current load cell reading:  " << loadCell.ReadLoadCell_grams() << std::endl;
			break;
		}

		case 'p':
		{
			std::cout << "PWM value as integer percentage:  " << std::flush;
			int pwm = 0;
			std::cin >> pwm;
			picSerial.WriteValueToPic<char>(nUtils::SET_PWM, static_cast<char>(pwm));
			std::cout << "Sent!" << std::endl;
			break;
		}

		case 'q':
		{
			std::cout << "Exiting..." << std::endl;
			keepGoing = false;
			break;
		}
		}
	}

	
	// --- Release Resources --- //

	std::cout << "Releasing resources..." << std::endl;
	picSerial.CloseSerialPort();


	// --- Finished --- //

	std::cout << "Finished.  Goodbye!" << std::endl;
	
	return 0;
}

