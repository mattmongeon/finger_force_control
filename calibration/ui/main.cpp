#include <iostream>
#include <string>
#include "pic_serial.h"
#include "load_cell.h"
#include "utils.h"
#include "stopwatch.h"
#include <pthread.h>


static bool stopLoop = false;


void* keyboardThread(void* pIn)
{
	// Just wait for the user to press ENTER.
	std::cin.ignore();

	stopLoop = true;

	return NULL;
}


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
	std::cout << "b:  Single read from BioTac" << std::endl;
	std::cout << "c:  Calibrate load cell" << std::endl;
	std::cout << "l:  Read load cell" << std::endl;
	std::cout << "p:  Set motor PWM duty cycle" << std::endl;
	std::cout << "r:  Continuously read from BioTac" << std::endl;
	std::cout << "q:  Quit" << std::endl;
	std::cout << std::endl;
}


struct sBioTacData
{
	unsigned short e1;
	unsigned short e2;
	unsigned short e3;
	unsigned short e4;
	unsigned short e5;
	unsigned short e6;
	unsigned short e7;
	unsigned short e8;
	unsigned short e9;
	unsigned short e10;
	unsigned short e11;
	unsigned short e12;
	unsigned short e13;
	unsigned short e14;
	unsigned short e15;
	unsigned short e16;
	unsigned short e17;
	unsigned short e18;
	unsigned short e19;

	unsigned short pac;
	unsigned short pdc;

	unsigned short tac;
	unsigned short tdc;
};


int main(int argc, char** argv)
{
	std::cout << nUtils::CLEAR_CONSOLE << std::flush;


	// --- Serial Communication --- //
	
	std::cout << "Opening serial port to communicate with PIC..." << std::endl;

	cPicSerial picSerial;
	if( !picSerial.OpenSerialPort() )
	{
		std::cout << "Error opening serial port!" << std::endl;
		return 0;
	}

	std::cout << "Serial port opened and ready!" << std::endl;
	std::cout << std::endl;


	// --- Devices --- //
	
	std::cout << "Constructing load cell instance" << std::endl;
	cLoadCell loadCell(&picSerial);


	// --- Miscellaneous --- //
	
	std::cout << "Setting up thread for keyboard input" << std::endl;

	pthread_t keyboardThreadHandle;
	

	// --- Main State Machine --- //

	std::cout << "Ready to begin!" << std::endl;
	
	bool keepGoing = true;
	while(keepGoing)
	{
		printMenu();
		switch(nUtils::GetMenuSelection())
		{
		case 'b':
		{
			std::cout << "Read from BioTac (e1):" << std::endl;
			sBioTacData d = picSerial.ReadValueFromPic<sBioTacData>(nUtils::READ_BIOTAC);
			std::cout << "E1:  " << d.e1 << std::endl;
			std::cout << "PAC:  " << d.pac << std::endl;
			std::cout << "PDC:  " << d.pdc << std::endl;
			std::cout << "TAC:  " << d.tac << std::endl;
			std::cout << "TDC:  " << d.tdc << std::endl;
			std::cout << std::endl;
			break;
		}
			
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

		case 'r':
		{
			cStopwatch stopwatch;
			stopLoop = false;
			pthread_create(&keyboardThreadHandle, NULL, keyboardThread, NULL);
			stopwatch.Start();
			while(!stopLoop)
			{
				std::cout << nUtils::CLEAR_CONSOLE << std::flush;
				std::cout << "Read continuously from BioTac" << std::endl;
				std::cout << "Press ENTER to quit" << std::endl;
				std::cout << std::endl;
				sBioTacData d = picSerial.ReadValueFromPic<sBioTacData>(nUtils::READ_BIOTAC);
				std::cout << "E1:  " << d.e1 << "\r\n";
				std::cout << "E2:  " << d.e2 << "\r\n";
				std::cout << "E3:  " << d.e3 << "\r\n";
				std::cout << "E4:  " << d.e4 << "\r\n";
				std::cout << "E5:  " << d.e5 << "\r\n";
				std::cout << "E6:  " << d.e6 << "\r\n";
				std::cout << "E7:  " << d.e7 << "\r\n";
				std::cout << "E8:  " << d.e8 << "\r\n";
				std::cout << "E9:  " << d.e9 << "\r\n";
				std::cout << "E10:  " << d.e10 << "\r\n";
				std::cout << "E11:  " << d.e11 << "\r\n";
				std::cout << "E12:  " << d.e12 << "\r\n";
				std::cout << "E13:  " << d.e13 << "\r\n";
				std::cout << "E14:  " << d.e14 << "\r\n";
				std::cout << "E15:  " << d.e15 << "\r\n";
				std::cout << "E16:  " << d.e16 << "\r\n";
				std::cout << "E17:  " << d.e17 << "\r\n";
				std::cout << "E18:  " << d.e18 << "\r\n";
				std::cout << "E19:  " << d.e19 << "\r\n";
				std::cout << "PAC:  " << d.pac << "\r\n";
				std::cout << "PDC:  " << d.pdc << "\r\n";
				std::cout << "TAC:  " << d.tac << "\r\n";
				std::cout << "TDC:  " << d.tdc << "\r\n";
				std::cout << std::endl;
				stopwatch.Reset();
				while(stopwatch.GetElapsedTime_ms() < 100.0)
				{
					;
				}
			}
			stopwatch.Stop();
			stopLoop = false;
			pthread_join(keyboardThreadHandle, NULL);
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

