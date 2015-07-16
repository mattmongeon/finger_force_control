#include "biotac.h"
#include "pic_serial.h"
#include "utils.h"
#include "keyboard_thread.h"
#include "data_logger.h"
#include "real_time_plot.h"
#include <iostream>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cBioTac::cBioTac(const cPicSerial* picSerial)
	: mpPicSerial( picSerial )
{
	
}

////////////////////////////////////////////////////////////////////////////////

cBioTac::~cBioTac()
{
	mpPicSerial = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cBioTac::ReadSingle() const
{
	std::cout << "Read from BioTac:" << std::endl;
	biotac_data d = Read();
	std::cout << "E1:   " << d.e1 << "\r\n";
	std::cout << "E2:   " << d.e2 << "\r\n";
	std::cout << "E3:   " << d.e3 << "\r\n";
	std::cout << "E4:   " << d.e4 << "\r\n";
	std::cout << "E5:   " << d.e5 << "\r\n";
	std::cout << "E6:   " << d.e6 << "\r\n";
	std::cout << "E7:   " << d.e7 << "\r\n";
	std::cout << "E8:   " << d.e8 << "\r\n";
	std::cout << "E9:   " << d.e9 << "\r\n";
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
	std::cout << std::flush;
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::ReadContinuous() const
{
	cKeyboardThread::Instance()->StartDetection();

	nUtils::ClearConsole();
	std::cout << "Read continuously from BioTac\r\n";
	std::cout << "Enter q+ENTER to quit\r\n";
	std::cout << "\r\n";
	std::cout << "Waiting for E1...\r\n";
	std::cout << "Waiting for E2...\r\n"; 	
	std::cout << "Waiting for E3...\r\n"; 	
	std::cout << "Waiting for E4...\r\n"; 	
	std::cout << "Waiting for E5...\r\n"; 	
	std::cout << "Waiting for E6...\r\n"; 	
	std::cout << "Waiting for E7...\r\n"; 	
	std::cout << "Waiting for E8...\r\n"; 	
	std::cout << "Waiting for E9...\r\n"; 	
	std::cout << "Waiting for E10...\r\n";
	std::cout << "Waiting for E11...\r\n";
	std::cout << "Waiting for E12...\r\n";
	std::cout << "Waiting for E13...\r\n";
	std::cout << "Waiting for E14...\r\n";
	std::cout << "Waiting for E15...\r\n";
	std::cout << "Waiting for E16...\r\n";
	std::cout << "Waiting for E17...\r\n";
	std::cout << "Waiting for E18...\r\n";
	std::cout << "Waiting for E19...\r\n";
	std::cout << "Waiting for PAC...\r\n";
	std::cout << "Waiting for PDC...\r\n";
	std::cout << "Waiting for TAC...\r\n";
	std::cout << "Waiting for TDC...\r\n";
	std::cout << "Waiting for load cell...\r\n";
	std::cout << "Waiting for loop actual frequency...\r\n";
	std::cout << std::flush;
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::BIOTAC_READ_CONTINUOUS);

	unsigned int prevTimestamp = 0;
	while(true)
	{
		biotac_tune_data data;
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&data), sizeof(biotac_tune_data) );

		nUtils::ConsoleMoveCursorUpLines(25);

		nUtils::ClearCurrentLine();
		std::cout << "E1:  " << data.mData.e1 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E2:  " << data.mData.e2 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E3:  " << data.mData.e3 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E4:  " << data.mData.e4 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E5:  " << data.mData.e5 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E6:  " << data.mData.e6 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E7:  " << data.mData.e7 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E8:  " << data.mData.e8 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E9:  " << data.mData.e9 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E10: " << data.mData.e10 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E11: " << data.mData.e11 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E12: " << data.mData.e12 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E13: " << data.mData.e13 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E14: " << data.mData.e14 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E15: " << data.mData.e15 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E16: " << data.mData.e16 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E17: " << data.mData.e17 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E18: " << data.mData.e18 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "E19: " << data.mData.e19 << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "PAC: " << data.mData.pac << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "PDC: " << data.mData.pdc << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "TAC: " << data.mData.tac << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "TDC: " << data.mData.tdc << "\r\n";
		nUtils::ClearCurrentLine();
		std::cout << "Load cell:  " << data.mLoadCell_g << "\r\n";

		float freq = 1.0 / ((data.mTimestamp - prevTimestamp) * 25.0 / 1000000000.0);
		nUtils::ClearCurrentLine();
		std::cout << "Actual frequency (Hz):  " << freq << "\r\n";
		std::cout << std::flush;

		prevTimestamp = data.mTimestamp;
		
		if(cKeyboardThread::Instance()->QuitRequested())
		{
			mpPicSerial->WriteCommandToPic(nUtils::STOP_ACTIVITY);
			mpPicSerial->DiscardIncomingData();
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::RecordCalibrationRun()
{
	// --- Preparations --- //

	// Ahead of time we will set up the things to be used during real-time processing.
	biotac_tune_data rxData;
	biotac_tune_data stopCondition;
	memset(&stopCondition, 0, sizeof(biotac_tune_data));
	std::vector<biotac_tune_data> tuneData;
	cDataLogger logger;

	
	// --- Start The Process --- //
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::BIOTAC_CAL_SINGLE);

	std::cout << "Enter number of seconds for this run (s):  " ;
	int seconds = 0;
	std::cin >> seconds;
	mpPicSerial->WriteValueToPic(seconds);
	
	
	std::cout << "Enter calibration force (g):  ";
	int force;
	std::cin >> force;

	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)", "", "", "", seconds * 100.0);

	// Now start the process.
	mpPicSerial->WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );

	std::cout << "Waiting for tuning results..." << std::endl;

	while(true)
	{
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&rxData), sizeof(biotac_tune_data) );

		if( memcmp(&rxData, &stopCondition, sizeof(biotac_tune_data)) != 0 )
		{
			logger.LogDataBuffer(&rxData, sizeof(biotac_tune_data));
			plotter.AddDataPoint(rxData.mLoadCell_g);
			tuneData.push_back(rxData);
		}
		else
		{
			break;
		}
	}


	// --- Print Results --- //

	std::cout << "Results received!" << std::endl << std::endl;

	std::cout << "Load cell\tBioTac PDC\tBioTac PAC\tBioTac TDC\tBioTac TAC\tTimestamp" << std::endl;
	for( size_t i = 0; i < tuneData.size(); ++i )
	{
		std::cout << tuneData[i].mLoadCell_g << "\t\t" << tuneData[i].mData.pdc << "\t\t" << tuneData[i].mData.pac << "\t\t"
				  << tuneData[i].mData.tdc << "\t\t" << tuneData[i].mData.tac << "\t\t" << tuneData[i].mTimestamp << std::endl;
	}

	std::cout << std::endl;


	// Just in case...
	mpPicSerial->DiscardIncomingData(0);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

biotac_data cBioTac::Read() const
{
	mpPicSerial->WriteCommandToPic(nUtils::READ_BIOTAC);
	return mpPicSerial->ReadValueFromPic<biotac_data>();
}

