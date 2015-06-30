#include <iostream>
#include <string>
#include <vector>
#include "pic_serial.h"
#include "load_cell.h"
#include "biotac.h"
#include "utils.h"
#include "stopwatch.h"
#include "keyboard_thread.h"
#include <pthread.h>
#include <plplot/plplot.h>


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
	std::cout << "a:  Calibrate BioTac" << std::endl;
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
	std::cout << "z:  Continuous raw load cell" << std::endl;
	std::cout << std::endl;
}


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
	
	std::cout << "Constructing device instances" << std::endl;
	cLoadCell loadCell(&picSerial);
	cBioTac biotac(&picSerial);
	

	// --- Miscellaneous --- //
	
	std::cout << "Setting up thread for keyboard input" << std::endl;
	cKeyboardThread keyboardThread;
	

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
			keyboardThread.StartDetection();
			stopwatch.Start();
			
			while(true)
			{
				std::cout << nUtils::CLEAR_CONSOLE << std::flush;
				std::cout << "Read continuously from BioTac\r\n";
				std::cout << "Enter q + ENTER to quit\r\n";
				std::cout << "\r\n";
				biotac.ReadSingle();

				stopwatch.Reset();
				while(stopwatch.GetElapsedTime_ms() < 100.0)
				{
					;
				}

				if(keyboardThread.QuitRequested())
					break;
			}

			break;
		}

		case 's':
		{
			cStopwatch stopwatch;
			keyboardThread.StartDetection();
			stopwatch.Start();

			while(true)
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

				if(keyboardThread.QuitRequested())
					break;
			}

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
			picSerial.WriteCommandToPic(nUtils::WILDCARD);
			
			keyboardThread.StartDetection();

			std::cout << nUtils::CLEAR_CONSOLE << std::flush;
			std::cout << "Read continuously from load cell\r\n";
			std::cout << "Enter q+ENTER to quit\r\n";
			std::cout << "\r\n";
			std::cout << "Waiting for start...\r\n";
			std::cout << "Waiting for loop frequency...\r\n";
			std::cout << "Waiting for ticks...\r\n";
			std::cout << "Waiting for exe time...\r\n";
			std::cout << "Waiting for possible frequency...\r\n";
			std::cout << "Waiting for ADC...\r\n";
			std::cout << std::flush;

			uint32_t prevStart = 0;
			while(true)
			{
				std::cout << nUtils::PREV_LINE << nUtils::PREV_LINE << nUtils::PREV_LINE << nUtils::PREV_LINE << nUtils::PREV_LINE << nUtils::PREV_LINE;

				int adc_value = 0;
				uint32_t ticks = 0;
				uint32_t start = 0;
				picSerial.ReadFromPic( reinterpret_cast<unsigned char*>(&start), sizeof(uint32_t) );
				picSerial.ReadFromPic( reinterpret_cast<unsigned char*>(&ticks), sizeof(uint32_t) );
				picSerial.ReadFromPic( reinterpret_cast<unsigned char*>(&adc_value), sizeof(int) );
				
				float exe_time_ms = (ticks * 25.0) / 1000000.0;
				float loopFreq_hz = ((start - prevStart) * 25.0) / 1000000000.0;
				loopFreq_hz = 1.0 / loopFreq_hz;
				std::cout << nUtils::CLEAR_LINE << "Start: " << start << "\r\n";
				std::cout << nUtils::CLEAR_LINE << "Loop Freq: " << loopFreq_hz << " Hz\r\n";
				std::cout << nUtils::CLEAR_LINE << "Ticks:  " << ticks << "\r\n";
				std::cout << nUtils::CLEAR_LINE << "Exe Time:  " << exe_time_ms  << " ms\r\n";
				std::cout << nUtils::CLEAR_LINE << "Possible Freq:  " << 1.0 / (exe_time_ms / 1000.0) << " Hz\r\n";
				std::cout << nUtils::CLEAR_LINE << "ADC:  " << adc_value << "\r\n";
				std::cout << std::flush;

				prevStart = start;

				if(keyboardThread.QuitRequested())
					break;
			}

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

