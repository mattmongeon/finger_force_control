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
	std::cout << "z:  Continuous raw load cell" << std::endl;
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
	int mCurrent_mA;
	unsigned int mTimeStamp;
	float mLoopExeTime_ms;
};


enum enumPLplotColor
{
	enumPLplotColor_BLACK = 0,
	enumPLplotColor_RED,
	enumPLplotColor_YELLOW,
	enumPLplotColor_GREEN,
	enumPLplotColor_AQUAMARINE,
	enumPLplotColor_PINK,
	enumPLplotColor_WHEAT,
	enumPLplotColor_GREY,
	enumPLplotColor_BROWN,
	enumPLplotColor_BLUE,
	enumPLplotColor_BLUE_VIOLET,
	enumPLplotColor_CYAN,
	enumPLplotColor_TURQUOISE,
	enumPLplotColor_MAGENTA,
	enumPLplotColor_SALMON,
	enumPLplotColor_WHITE
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

			sTorqueTuneData tuneData[200];
			picSerial.ReadFromPic( reinterpret_cast<unsigned char*>(&tuneData), sizeof(sTorqueTuneData)*200 );

			std::cout << "Tuning results received!" << std::endl;
			std::cout << "Load Cell\tError\t\tError Integral\t\tCurrent (mA)\t\tTimestamp\t\tExe Time (ms)" << std::endl;
			for( int i = 0; i < 200; ++i )
			{
				std::cout << tuneData[i].mLoadCell_g << "\t\t" << tuneData[i].mError << "\t\t" << tuneData[i].mErrorInt << "\t\t" << tuneData[i].mCurrent_mA << "\t\t" << tuneData[i].mTimeStamp << "\t\t" << tuneData[i].mLoopExeTime_ms << std::endl;
			}

			std::cout << std::endl;


			// --- Plot Results --- //

			std::vector<PLFLT> x;
			for( int i = 0; i < 200; ++i )
			{
				x.push_back(i);
			}

			PLFLT yMax = -1000000.0;
			PLFLT yMin = 1000000.0;
			std::vector<PLFLT> y;
			for( int i = 0; i < 200; ++i )
			{
				yMax = std::max<PLFLT>(yMax, tuneData[i].mLoadCell_g);
				yMin = std::min<PLFLT>(yMin, tuneData[i].mLoadCell_g);
				y.push_back(tuneData[i].mLoadCell_g);
			}

			plsdev("xcairo");
			plscolbg(255, 255, 255);
			plinit();

			plscolbg(0,0,0);  // The first call to plscolbg() edits the value stored for color index 0.  Set it back to black.
			plcol0(enumPLplotColor_BLACK);
			plenv(0, 200, yMin, yMax, 0, 0);
			pllab("Measurement", "Force (g)", "Trial results");

			plcol0(enumPLplotColor_BLUE);
			plline(200, &(x[0]), &(y[0]));

			plend();
			
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
			
			stopLoop = false;
			detectEnterPress = true;

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
			while(!stopLoop)
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
			}

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

