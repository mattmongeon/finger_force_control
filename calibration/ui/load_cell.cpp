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

void cLoadCell::DisplayMenu()
{
	nUtils::ClearConsole();
	
	bool keepGoing = true;
	while(keepGoing)
	{
		std::cout << "Load Cell Function Menu" << std::endl;
		std::cout << "-----------------------" << std::endl;
		std::cout << std::endl;
		std::cout << "a: Calibrate" << std::endl;
		std::cout << "b: Read load cell once" << std::endl;
		std::cout << "c: Continuously read load cell" << std::endl;
		std::cout << "q: Go back to main menu" << std::endl;
		std::cout << std::endl;

		switch(nUtils::GetMenuSelection())
		{
		case 'a':
			RunCalibrationRoutine();
			break;

		case 'b':
			ReadSingle();
			break;

		case 'c':
			ReadContinuous();
			break;

		case 'q':
			keepGoing = false;
			break;

		default:
			std::cout << "Invalid selection!  Please choose again." << std::endl << std::endl;
			break;
		}
	}

	nUtils::ClearConsole();
}

////////////////////////////////////////////////////////////////////////////////
//  Load Cell Operation Functions
////////////////////////////////////////////////////////////////////////////////

void cLoadCell::RunCalibrationRoutine()
{
	nUtils::ClearConsole();
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
			std::cout << "Get value";
			nUtils::ConsoleMoveCursorUpLines(1);
			std::cout << "\r" << std::endl;
			int weight_g = 0;
			std::cout << "Enter amount of weight (g):  " << std::flush;
			std::cin >> weight_g;
			std::cout << std::endl << "Reading and averaging 20 values..." << std::endl;
			int raw = 0;
			for( int i = 0; i < 20; ++i )
			{
				mpPicSerial->WriteCommandToPic(nUtils::READ_RAW_LOAD_CELL);
				raw += mpPicSerial->ReadValueFromPic<int>();
			}
			raw /= 20;
			std::cout << "Avg raw ADC value:  " << raw << std::endl;
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
	mpPicSerial->WriteCommandToPic(nUtils::READ_LOAD_CELL);
	std::cout << "Current load cell reading:  " << mpPicSerial->ReadValueFromPic<int>() << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cLoadCell::ReadContinuous()
{
	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)", "", "", "", 2500.0, 0.0, 10.0);

	
	// --- Start Gathering Data --- //
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::LOAD_CELL_READ_CONTINUOUS);
		
	cKeyboardThread::Instance()->StartDetection();

	nUtils::ClearConsole();
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
		nUtils::ConsoleMoveCursorUpLines(7);
		
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

		nUtils::ClearCurrentLine();
		std::cout << "Start: " << start << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "Loop Freq: " << loopFreq_hz << " Hz\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "Ticks:	" << ticks << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "Exe Time:	 " << exe_time_ms	<< " ms\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "Possible Freq:	" << 1.0 / (exe_time_ms / 1000.0) << " Hz\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "ADC:	 " << adc_value << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "Calibrated Value:	 " << load_cell_value << "\r\n";
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

