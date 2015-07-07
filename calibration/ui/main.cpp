#include <iostream>
#include <string>
#include <vector>
#include "pic_serial.h"
#include "load_cell.h"
#include "biotac.h"
#include "utils.h"
#include "stopwatch.h"
#include "keyboard_thread.h"
#include "real_time_plot.h"
#include <pthread.h>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <cmath>
#include <cstdio>


static bool noSerial = false;
static bool help = false;


// Takes in the command line options given to main and parses them to appropriately configure the program.
//
// Params:
// argc - The number of command-line arguments including the program name.
// argv - The vector of command-line argument strings including the program name at index 0.
void parseCommandLineOptions(int argc, char** argv)
{
	for( int i = 1; i < argc; ++i )
	{
		if( strcmp("-N", argv[i]) == 0 )
		{
			noSerial = true;
		}
		else if( strcmp("-h", argv[i]) == 0 )
		{
			help = true;
		}
		else if( strcmp("--help", argv[i]) == 0 )
		{
			help = true;
		}
		else
		{
			std::cout << "Unknown command-line option \'" << argv[i] << "\'!  Exiting." << std::endl;
			exit(0);
		}
	}
}


// Prints the main menu of possible functions for this program.
void printMenu()
{
	std::cout << "Function Menu" << std::endl;
	std::cout << "-------------" << std::endl;
	std::cout << std::endl;
	std::cout << "a:  Calibrate BioTac" << std::endl;
	std::cout << "b:  Single read from BioTac" << std::endl;
	std::cout << "c:  Calibrate load cell" << std::endl;
	std::cout << "f:  Hold force" << std::endl;
	std::cout << "g:  Set torque controller gains" << std::endl;
	std::cout << "h:  Get torque controller gains" << std::endl;
	std::cout << "l:  Read load cell" << std::endl;
	std::cout << "m:  Read uncalibrated load cell" << std::endl;
	std::cout << "p:  Set motor PWM duty cycle" << std::endl;
	std::cout << "r:  Continuously read from BioTac" << std::endl;
	std::cout << "s:  Continuously read from load cell" << std::endl;
	std::cout << "t:  Tune torque gains" << std::endl;
	std::cout << "q:  Quit" << std::endl;
	std::cout << "z:  Continuous raw load cell" << std::endl;
	std::cout << std::endl;
}


int main(int argc, char** argv)
{
	std::cout << nUtils::CLEAR_CONSOLE << std::flush;

	
	// --- Read Options --- //
	
	parseCommandLineOptions(argc, argv);
	

	// --- Check Help --- //

	if( help )
	{
		std::cout << "Usage: biotac_cal [OPTION]" << std::endl;
		std::cout << std::endl;
		std::cout << "This is the user-interface portion of the setup used for calibrating the BioTac by Syntouch." << std::endl;
		std::cout << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "-h --help\tShow this information" << std::endl;
		std::cout << "-N\t\tSkip opening serial connection" << std::endl;
		
		return 0;
	}

	
	// --- Serial Communication --- //
	
	cPicSerial picSerial;
	if( !noSerial )
	{
		std::cout << "Opening serial port to communicate with PIC..." << std::endl;

		if( !picSerial.OpenSerialPort() )
		{
			std::cout << "Error opening serial port!" << std::endl;
			return 0;
		}

		std::cout << "Serial port opened and ready!" << std::endl;
		std::cout << std::endl;
	}
	else
	{
		std::cout << "No serial mode set.  Skipping opening serial connection." << std::endl;
		std::cout << std::endl;
	}


	// --- Devices --- //
	
	std::cout << "Constructing device instances" << std::endl;
	cLoadCell loadCell(&picSerial);
	cBioTac biotac(&picSerial);
	

	// --- Miscellaneous --- //
	
	std::cout << "Setting up thread for keyboard input" << std::endl;
	cKeyboardThread::Instance();
	

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
		case 'a':
		{
			biotac.RecordCalibrationRun();
			break;
		}
		
		case 'b':
		{
			biotac.ReadSingle();
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
			float k[2];
			
			std::cout << "Enter Kp:  ";
			std::cin >> k[0];

			std::cout << "Enter Ki:  ";
			std::cin >> k[1];

			picSerial.WriteCommandToPic(nUtils::SET_TORQUE_CTRL_GAINS);
			picSerial.WriteToPic(reinterpret_cast<unsigned char*>(&k), sizeof(k));
		}

		case 'h':
		{
			picSerial.WriteCommandToPic(nUtils::GET_TORQUE_CTRL_GAINS);
			
			unsigned char buffer[20];
			picSerial.ReadFromPic(buffer, 8);

			float* pF = reinterpret_cast<float*>(&buffer);
			std::cout << "Kp:  " << pF[0] << std::endl;
			std::cout << "Ki:  " << pF[1] << std::endl;
			std::cout << std::endl;
			
			break;
		}

		case 'l':
		{
			loadCell.ReadSingle();
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
			biotac.ReadContinuous();
			break;
		}

		case 's':
		{
			loadCell.ReadContinuous();
			break;
		}

		case 't':
		{
			loadCell.TuneForceHolding();
			break;
		}

		case 'q':
		{
			std::cout << "Exiting..." << std::endl;
			keepGoing = false;
			break;
		}

		case 'z':
		{
			std::cout << "CURRENTLY DOING NOTHING WITH z" << std::endl;
			break;
		}

		case '1':
		{
			cKeyboardThread::Instance()->StartDetection();
			
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 1953125;

			double dt = 1.953125;
			double t = 0;

			cStopwatch timer;

			int n = 0;
			while(true)
			{
				// Wait about 2 ms, which corresponds to 512 Hz.
				nanosleep( &ts, NULL );

				timer.Start();

				t = n * dt;
				double newY = sin( t * 3.14 / 18 );

				double time_ms = timer.GetElapsedTime_ms();
				std::cout << "New data:  " << newY << std::endl;
				std::cout << "Number:  " << n << std::endl;
				std::cout << "Exe Time (ms):  " << time_ms << std::endl;
				std::cout << "Frequency (Hz):  " << 1/(time_ms / 1000.0) << std::endl;
				std::cout << std::endl;
				timer.StopAndReset();
				++n;

				if(cKeyboardThread::Instance()->QuitRequested())
					break;
			}
			
			break;
		}

		default:
			break;
		}
	}

	
	// --- Release Resources --- //

	std::cout << "Releasing resources..." << std::endl;
	picSerial.CloseSerialPort();


	// --- Finished --- //

	std::cout << "Finished.  Goodbye!" << std::endl;
	
	return 0;
}

