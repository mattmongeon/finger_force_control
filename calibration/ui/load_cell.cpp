#include "load_cell.h"
#include "utils.h"
#include "pic_serial.h"
#include "keyboard_thread.h"
#include "real_time_plot.h"
#include <vector>
#include <utility>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cLoadCell::cLoadCell(const cPicSerial* picSerial)
	: mpPicSerial( picSerial )
{
	
}

////////////////////////////////////////////////////////////////////////////////

cLoadCell::~cLoadCell()
{
	mpPicSerial = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cLoadCell::RunCalibrationRoutine()
{
	std::cout << nUtils::CLEAR_CONSOLE << std::flush;
	std::cout << "Load cell calibration" << std::endl;
	std::cout << "---------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "This routine reads raw ADC values from the PIC that represents the" << std::endl;
	std::cout << "voltage output by the load cell." << std::endl;
	std::cout << std::endl;
	std::cout << "n:  Get new calibration point" << std::endl;
	std::cout << "q:  Exit calibration" << std::endl;
	std::cout << std::endl;

	std::vector< std::pair<int, int> > calValues;
	
	bool keepGoing = true;
	while(keepGoing)
	{
		switch(nUtils::GetMenuSelection())
		{
		case 'n':
		{
			std::cout << "Get value" << nUtils::PREV_LINE << "\r" << std::endl;
			int weight_g = 0;
			std::cout << "Enter amount of weight (g):  " << std::flush;
			std::cin >> weight_g;
			std::cout << std::endl << "Reading and averaging 20 values..." << std::endl;
			int raw = 0;
			for( int i = 0; i < 20; ++i )
			{
				raw += mpPicSerial->ReadValueFromPic<int>(nUtils::READ_RAW_LOAD_CELL);
			}
			raw /= 20;
			std::cout << "Avg value:  " << raw << std::endl;
			calValues.push_back( std::pair<int, int>(weight_g, raw) );
			break;
		}

		case 'q':
		{
			std::cout << std::endl;
			std::cout << "Calibration values:" << std::endl;
			std::cout << std::endl;
			std::cout << "Weights\t\tLoad Cell Readings" << std::endl;
			for( std::size_t i = 0; i < calValues.size(); ++i )
			{
				std::cout << calValues[i].first << "\t\t" << calValues[i].second << std::endl;
			}
			std::cout << std::endl;
			std::cout << "Press ENTER to continue" << std::endl;
			std::cin.get();
			keepGoing = false;
			break;
		}

		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void cLoadCell::ReadSingle()
{
	std::cout << "Current load cell reading:  " << mpPicSerial->ReadValueFromPic<int>(nUtils::READ_LOAD_CELL) << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cLoadCell::ReadContinuous()
{
	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)");

	
	// --- Start Gathering Data --- //
			
	mpPicSerial->WriteCommandToPic(nUtils::LOAD_CELL_READ_CONTINUOUS);
			
	cKeyboardThread::Instance()->StartDetection();

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
	std::cout << "Waiting for load cell...\r\n";
	std::cout << std::flush;

	uint32_t prevStart = 0;
	while(true)
	{
		std::cout << nUtils::PREV_LINE << nUtils::PREV_LINE
				  << nUtils::PREV_LINE << nUtils::PREV_LINE
				  << nUtils::PREV_LINE << nUtils::PREV_LINE
				  << nUtils::PREV_LINE;

		int adc_value = 0;
		uint32_t ticks = 0;
		uint32_t start = 0;
		int load_cell_value = 0;
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&start), sizeof(uint32_t) );
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&ticks), sizeof(uint32_t) );
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&adc_value), sizeof(int) );
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&load_cell_value), sizeof(int) );

		plotter.AddDataPoint(load_cell_value);
				
		float exe_time_ms = (ticks * 25.0) / 1000000.0;
		float loopFreq_hz = ((start - prevStart) * 25.0) / 1000000000.0;
		loopFreq_hz = 1.0 / loopFreq_hz;
		std::cout << nUtils::CLEAR_LINE << "Start: " << start << "\r\n";
		std::cout << nUtils::CLEAR_LINE << "Loop Freq: " << loopFreq_hz << " Hz\r\n";
		std::cout << nUtils::CLEAR_LINE << "Ticks:	" << ticks << "\r\n";
		std::cout << nUtils::CLEAR_LINE << "Exe Time:  " << exe_time_ms	<< " ms\r\n";
		std::cout << nUtils::CLEAR_LINE << "Possible Freq:	" << 1.0 / (exe_time_ms / 1000.0) << " Hz\r\n";
		std::cout << nUtils::CLEAR_LINE << "ADC:	 " << adc_value << "\r\n";
		std::cout << nUtils::CLEAR_LINE << "Calibrated Value:  " << load_cell_value << "\r\n";
		std::cout << std::flush;

		prevStart = start;

		if(cKeyboardThread::Instance()->QuitRequested())
		{
			mpPicSerial->WriteCommandToPic(nUtils::STOP_ACTIVITY);
			mpPicSerial->DiscardIncomingData();
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void cLoadCell::TuneForceHolding()
{
	// --- Preparations --- //

	// Ahead of time we will set up the things to be used during real-time processing so we can move fast.
	torque_tune_data rxData;
	torque_tune_data stopCondition;
	memset(&stopCondition, 0, sizeof(torque_tune_data));
	std::vector<torque_tune_data> tuneData;
	

	// --- Start The Process --- //
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::TUNE_TORQUE_GAINS);

	std::cout << "Enter force to hold (g):  ";

	int force;
	std::cin >> force;

	// Do this one next.  It is really annoying to create it before sending the force, because it pops up
	// a window right in the way.
	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)", "", "", "", 200.0);

	mpPicSerial->WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );

	std::cout << "Waiting for tuning results..." << std::endl;

	while(true)
	{
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&rxData), sizeof(torque_tune_data) );

		if( memcmp(&rxData, &stopCondition, sizeof(torque_tune_data)) != 0 )
		{
			plotter.AddDataPoint(rxData.load_cell_g);
			tuneData.push_back(rxData);
		}
		else
		{
			break;
		}
	}


	// --- Print Results --- //
	
	std::cout << "Tuning results received!" << std::endl;
	std::cout << "Load Cell\tError\t\tError Integral\t\tPWM\t\tTimestamp\t\tExe Time (ms)\t\tFrequency (Hz)" << std::endl;
	unsigned int prevTimestamp = 0;
	for( size_t i = 0; i < tuneData.size(); ++i )
	{
		std::cout << tuneData[i].load_cell_g << "\t\t"
				  << tuneData[i].error << "\t\t"
				  << tuneData[i].error_int << "\t\t"
				  << tuneData[i].pwm << "\t\t"
				  << tuneData[i].timestamp << "\t\t"
				  << tuneData[i].loop_exe_time_ms << "\t\t"
				  << 1.0 / ((tuneData[i].timestamp - prevTimestamp) * 25.0 / 1000000000.0) << "\t\t"
				  << std::endl;

		prevTimestamp = tuneData[i].timestamp;
	}


	std::cout << std::endl;


	// Just in case...
	mpPicSerial->DiscardIncomingData(0);
}

