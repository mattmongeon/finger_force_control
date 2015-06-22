#include <iostream>
#include <string>
#include <vector>
#include "pic_serial.h"
#include "load_cell.h"
#include "utils.h"
#include "stopwatch.h"
#include <pthread.h>
#include <plplot/plplot.h>


static bool detectEnterPress = false;
static bool stopLoop = false;
static bool stopThread = false;


void* keyboardThread(void* pIn)
{
	while(!stopThread)
	{
		if( detectEnterPress )
		{
			// Just wait for the user to press q+ENTER.
			std::string input;
			std::cin >> input;
			if( input == "q" )
			{
				stopLoop = true;
				detectEnterPress = false;
			}
		}
	}

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
	std::cout << "f:  Hold force" << std::endl;
	std::cout << "g:  Set torque controller gains" << std::endl;
	std::cout << "h:  Get torque controller gains" << std::endl;
	std::cout << "l:  Read load cell" << std::endl;
	std::cout << "p:  Set motor PWM duty cycle" << std::endl;
	std::cout << "r:  Continuously read from BioTac" << std::endl;
	std::cout << "s:  Continuously read from load cell" << std::endl;
	std::cout << "t:  Tune torque gains" << std::endl;
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

struct sTorqueTuneData
{
	int mLoadCell_g;
	int mError;
	int mErrorInt;
	unsigned int mTimeStamp;
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
	pthread_create(&keyboardThreadHandle, NULL, keyboardThread, NULL);
	

	// --- Main State Machine --- //

	std::cout << "Ready to begin!" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;


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
			break;
		}
			
		case 'c':
		{
			loadCell.RunCalibrationRoutine();
			break;
		}

		case 'f':
		{
			picSerial.WriteCommandToPic(nUtils::HOLD_FORCE);

			std::cout << "Enter force to hold (g):  ";

			int force;
			std::cin >> force;

			picSerial.WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );
			break;
		}

		case 'g':
		{
			float k[3];
			
			std::cout << "Enter Kp:  ";
			std::cin >> k[0];

			std::cout << "Enter Ki:  ";
			std::cin >> k[1];

			std::cout << "Enter Kd:  ";
			std::cin >> k[2];

			picSerial.WriteCommandToPic(nUtils::SET_TORQUE_CTRL_GAINS);
			picSerial.WriteToPic(reinterpret_cast<unsigned char*>(&k), sizeof(k));
		}

		case 'h':
		{
			picSerial.WriteCommandToPic(nUtils::GET_TORQUE_CTRL_GAINS);
			
			unsigned char buffer[20];
			picSerial.ReadFromPic(buffer, 12);

			float* pF = reinterpret_cast<float*>(&buffer);
			std::cout << "Kp:  " << pF[0] << std::endl;
			std::cout << "Ki:  " << pF[1] << std::endl;
			std::cout << "kd:  " << pF[2] << std::endl;
			std::cout << std::endl;
			
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
			detectEnterPress = true;
			stopwatch.Start();
			
			while(!stopLoop)
			{
				std::cout << nUtils::CLEAR_CONSOLE << std::flush;
				std::cout << "Read continuously from BioTac\r\n";
				std::cout << "Enter q + ENTER to quit\r\n";
				std::cout << "\r\n";
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

			break;
		}

		case 's':
		{
			cStopwatch stopwatch;
			stopLoop = false;
			detectEnterPress = true;
			stopwatch.Start();

			while(!stopLoop)
			{
				std::cout << nUtils::CLEAR_CONSOLE << std::flush;
				std::cout << "Read continuously from load cell\r\n";
				std::cout << "Enter q+ENTER to quit\r\n";
				std::cout << "\r\n";
				std::cout << "Force:  " << loadCell.ReadLoadCell_grams() << " g\r\n";
				std::cout << std::endl;

				stopwatch.Reset();
				while(stopwatch.GetElapsedTime_ms() < 100.0)
				{
					;
				}
			}

			break;
		}

		case 't':
		{
			picSerial.WriteCommandToPic(nUtils::TUNE_TORQUE_GAINS);

			std::cout << "Enter force to hold (g):  ";

			int force;
			std::cin >> force;

			picSerial.WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );

			std::cout << "Waiting for tuning results..." << std::endl;

			sTorqueTuneData tuneData[100];
			picSerial.ReadFromPic( reinterpret_cast<unsigned char*>(&tuneData), sizeof(sTorqueTuneData)*100 );

			std::cout << "Tuning results received!" << std::endl;
			std::cout << "Load Cell\t\tError\t\tError Integral\t\tTimestamp" << std::endl;
			for( int i = 0; i < 100; ++i )
			{
				std::cout << tuneData[i].mLoadCell_g << "\t\t" << tuneData[i].mError << "\t\t" << tuneData[i].mErrorInt << "\t\t" << tuneData[i].mTimeStamp << std::endl;
			}

			std::cout << std::endl;


			// --- Plot Results --- //

			plinit();
			plenv(0, 100, 0, 300, 0, 0);
			pllab("Measurement", "Force (g)", "Trial results");
			
			std::vector<PLFLT> x;
			for( int i = 0; i < 100; ++i )
			{
				x.push_back(i);
			}

			std::vector<PLFLT> y;
			for( int i = 0; i < 100; ++i )
			{
				y.push_back(tuneData[i].mLoadCell_g);
			}

			plline(100, &(x[0]), &(y[0]));

			plend();
			
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

	stopThread = true;
	pthread_join(keyboardThreadHandle, NULL);


	// --- Finished --- //

	std::cout << "Finished.  Goodbye!" << std::endl;
	
	return 0;
}

