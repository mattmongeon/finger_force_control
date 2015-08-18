#include "biotac.h"
#include "pic_serial.h"
#include "utils.h"
#include "keyboard_thread.h"
#include "data_logger.h"
#include "real_time_plot.h"
#include "function_fit_nls.h"
#include "function_fit_force_terms.h"
#include "electrode_tdc_compensator.h"
#include "biotac_force_curve.h"
#include "file_utils.h"
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

void cBioTac::DisplayMenu()
{
	nUtils::ClearConsole();
	
	bool keepGoing = true;
	while(keepGoing)
	{
		std::cout << "BioTac Function Menu" << std::endl;
		std::cout << "--------------------" << std::endl;
		std::cout << std::endl;
		std::cout << "a: Record force hold for time" << std::endl;
		std::cout << "b: Single read" << std::endl;
		std::cout << "c: Test BioTac force sensing" << std::endl;
		std::cout << "d: Continuously read from BioTac" << std::endl;
		std::cout << "e: Calibrate BioTac with trajectory" << std::endl;
		std::cout << "f: Train electrode compensators" << std::endl;
		std::cout << "g: Train force vector coefficients" << std::endl;
		std::cout << "h: Train all force terms" << std::endl;
		std::cout << "q: Go back to main menu" << std::endl;
		std::cout << std::endl;

		switch(nUtils::GetMenuSelection())
		{
		case 'a':
			RecordForceHold();
			break;

		case 'b':
			ReadSingle();
			break;

		case 'c':
			RecordBioTacForceTest();
			break;

		case 'd':
			ReadContinuous();
			break;

		case 'e':
			RecordForceTrajectory();
			break;

		case 'f':
			TrainElectrodeCompenstors();
			break;

		case 'g':
			TrainForceVectorCoefficients();
			break;

		case 'h':
			TrainAllForceTerms();
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
//  BioTac Operation Functions
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
	// --- Check If We Are Recording To File --- //
	
	bool recording = false;
	nUtils::ClearConsole();
	std::cout << "Record session? [y/n]: " << std::flush;
	std::string choice;
	std::cin >> choice;
	recording = (choice == "y") || (choice == "Y");


	// --- Set Up --- //

	cDataLogger* pLogger = NULL;
	if( recording )
	{
		nUtils::ClearConsole();
		std::cout << "Enter file name (just press ENTER for default name): " << std::flush;
		std::cin.clear();

		// Ignore leftover '\n' characters so that we don't always automatically end up with a
		// blank file name every time.
		std::cin.ignore(1);
		std::string fileName;
		std::getline( std::cin, fileName );

		std::cout << "length:  " << fileName.size() << std::endl;
		std::cout << "name:    " << fileName << std::endl;
		
		pLogger = new cDataLogger(fileName);
	}

	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)", "", "", "", 2500.0);
	
	
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


	// --- Read Data --- //
	
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

		if( pLogger != NULL )
			pLogger->LogDataBuffer(&data, sizeof(biotac_tune_data));
		
		plotter.AddDataPoint(data.mLoadCell_g);
		
		prevTimestamp = data.mTimestamp;
		
		if(cKeyboardThread::Instance()->QuitRequested())
		{
			mpPicSerial->WriteCommandToPic(nUtils::STOP_ACTIVITY);
			mpPicSerial->DiscardIncomingData();
			break;
		}
	}

	if( pLogger != NULL )
		delete pLogger;
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::RecordForceHold()
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

void cBioTac::RecordForceTrajectory()
{
	// --- Preparations --- //

	// Ahead of time we will set up the things to be used during real-time processing.
	biotac_tune_data rxData;
	biotac_tune_data stopCondition;
	memset(&stopCondition, 0, sizeof(biotac_tune_data));
	std::vector<biotac_tune_data> tuneData;
	cDataLogger logger;
	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)", "", "", "", 500.0);

	
	// --- Start The Process --- //
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::BIOTAC_CAL_TRAJECTORY);

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

void cBioTac::RecordBioTacForceTest()
{
	// --- Preparations --- //

	// Ahead of time we will set up the things to be used during real-time processing.
	biotac_tune_data rxData;
	biotac_tune_data stopCondition;
	memset(&stopCondition, 0, sizeof(biotac_tune_data));
	std::vector<biotac_tune_data> tuneData;
	std::vector<int> biotacForceData;
	//cDataLogger logger;

	
	// --- Start The Process --- //
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::BIOTAC_TRACK_FORCE);

	std::cout << "Enter number of seconds for this run (s):  " ;
	int seconds = 0;
	std::cin >> seconds;
	mpPicSerial->WriteValueToPic(seconds);
	
	
	std::cout << "Enter calibration force (g):  ";
	int force;
	std::cin >> force;

	cRealTimePlot plotter("BioTac Force", "Sample", "Force (g)", "Force (g)", "", "", "", seconds * 100.0);

	// Now start the process.
	mpPicSerial->WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );

	std::cout << "Waiting for tuning results..." << std::endl;

	while(true)
	{
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&rxData), sizeof(biotac_tune_data) );

		if( memcmp(&rxData, &stopCondition, sizeof(biotac_tune_data)) != 0 )
		{
			int biotacForce_g = 0;
			mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&biotacForce_g), sizeof(int) );
			
			// logger.LogDataBuffer(&rxData, sizeof(biotac_tune_data));
			plotter.AddDataPoint(biotacForce_g);
			tuneData.push_back(rxData);
			biotacForceData.push_back(biotacForce_g);
		}
		else
		{
			break;
		}
	}


	// --- Print Results --- //

	std::cout << "Results received!" << std::endl << std::endl;

	std::cout << "Load cell\tBioTac Force\tReference\tTDC\tPDC" << std::endl;
	for( size_t i = 0; i < tuneData.size(); ++i )
	{
		std::cout << tuneData[i].mLoadCell_g << "\t\t" << biotacForceData[i] << "\t\t" << tuneData[i].mReference_g << "\t\t"
				  << tuneData[i].mData.tdc << "\t\t" << tuneData[i].mData.pdc << std::endl;
	}

	std::cout << std::endl;


	// Just in case...
	mpPicSerial->DiscardIncomingData(0);
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::TrainElectrodeCompenstors()
{
	// --- Train For Fit --- //
			
	cFunctionFitNLS fit;
	std::vector<std::string> files;
	files.push_back("./data/tdc_electrodes/cold_start.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	// files.push_back("./data/tdc_electrodes/high_temp.dat");
	// files.push_back("./data/tdc_electrodes/zero1.dat");
	// files.push_back("./data/tdc_electrodes/zero2.dat");
	// files.push_back("./data/tdc_electrodes/zero3.dat");
	files.push_back("./data/tdc_electrodes/unforced_already_warm.dat");
	files.push_back("./data/tdc_electrodes/cold_soak_to_hot.dat");
	files.push_back("./data/tdc_electrodes/full_heat_up.dat");
	files.push_back("./data/tdc_electrodes/reheat_after_heat_and_cooldown.dat");

	std::vector<cElectrodeTdcCompensator> compensators = fit.TrainAgainstDataFiles(files);


	// --- Test The Fit --- //
			
	files.clear();
	files.push_back("./data/tdc_electrodes/cold_start.dat");
	files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	files.push_back("./data/tdc_electrodes/heating_up.dat");
	files.push_back("./data/tdc_electrodes/high_temp.dat");
	files.push_back("./data/tdc_electrodes/zero1.dat");
	files.push_back("./data/tdc_electrodes/zero2.dat");
	files.push_back("./data/tdc_electrodes/zero3.dat");
	files.push_back("./data/tdc_electrodes/unforced_already_warm.dat");
	files.push_back("./data/tdc_electrodes/cold_soak_to_hot.dat");
	files.push_back("./data/tdc_electrodes/full_heat_up.dat");
	files.push_back("./data/tdc_electrodes/reheat_after_heat_and_cooldown.dat");
	fit.TestAgainstDataFiles(files, compensators);


	// --- Write Coefficients To File --- //
			
	std::ofstream file("./data/tdc_electrodes/tdc_electrode_curve.coeff", std::ios::binary | std::ios::out);
	for( std::size_t i = 0; i < compensators.size(); ++i )
	{
		compensators[i].SaveCoefficientsToFile(file);
	}
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::TrainForceVectorCoefficients()
{
	// --- Load TDC vs Electrode Coefficients From File --- //
			
	std::vector<cElectrodeTdcCompensator> compensators;
	compensators.clear();
	std::ifstream file("./data/tdc_electrodes/tdc_electrode_curve.coeff", std::ios::binary | std::ios::in);
	for( std::size_t i = 0; i < 19; ++i )
	{
		compensators.push_back( cElectrodeTdcCompensator(file) );
	}
			

	// --- Train Fit For Force Equation --- //
			
	cFunctionFitForceTerms ffTerms(compensators);
	std::vector<std::string> files = nFileUtils::GetFilesInDirectory("./data", ".dat");
	for( std::size_t i = 0; i < files.size(); ++i )
		files[i].insert(0, "./data/");

	files.push_back("./data/tdc_electrodes/cold_start.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	files.push_back("./data/tdc_electrodes/high_temp.dat");
			
	cBioTacForceCurve curve = ffTerms.TrainAgainstDataFiles(files);


	// --- Test Fit --- //
			
	files.clear();
	files.push_back("./data/tdc_electrodes/cold_start.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	files.push_back("./data/tdc_electrodes/high_temp.dat");
	files.push_back("./data/tdc_electrodes/unforced_already_warm.dat");
	files.push_back("./data/test/data_2015_07_30_16_35_24.dat");
	files.push_back("./data/test/data_2015_07_29_14_44_14.dat");
	files.push_back("./data/test/data_2015_07_23_09_57_11.dat");
	// files.push_back("./data/zero_force_01.dat");
	// files.push_back("./data/zero_force_02.dat");
	// files.push_back("./data/data_2015_07_23_09_50_24.dat");
	// files.push_back("./data/data_2015_07_23_09_55_36.dat");
	// files.push_back("./data/data_2015_07_23_09_55_48.dat");
	ffTerms.TestAgainstDataFiles(files, curve);


	// --- Write Trained Curve To File --- //

	curve.SaveFitTermsToFile("./coefficients.bio");
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::TrainAllForceTerms()
{
	// --- Train For Fit --- //
			
	cFunctionFitNLS fit;
	std::vector<std::string> files;
	files.push_back("./data/tdc_electrodes/cold_start.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	// files.push_back("./data/tdc_electrodes/high_temp.dat");
	// files.push_back("./data/tdc_electrodes/zero1.dat");
	// files.push_back("./data/tdc_electrodes/zero2.dat");
	// files.push_back("./data/tdc_electrodes/zero3.dat");
	files.push_back("./data/tdc_electrodes/unforced_already_warm.dat");
	files.push_back("./data/tdc_electrodes/cold_soak_to_hot.dat");
	files.push_back("./data/tdc_electrodes/full_heat_up.dat");
	files.push_back("./data/tdc_electrodes/reheat_after_heat_and_cooldown.dat");
	std::vector<cElectrodeTdcCompensator> compensators = fit.TrainAgainstDataFiles(files);


	// --- Write Coefficients To File --- //
			
	std::ofstream outFile("./data/tdc_electrodes/tdc_electrode_curve.coeff", std::ios::binary | std::ios::out);
	for( std::size_t i = 0; i < compensators.size(); ++i )
	{
		compensators[i].SaveCoefficientsToFile(outFile);
	}

			
	// --- Train Fit For Force Equation --- //
			
	cFunctionFitForceTerms ffTerms(compensators);
	files = nFileUtils::GetFilesInDirectory("./data", ".dat");
	for( std::size_t i = 0; i < files.size(); ++i )
		files[i].insert(0, "./data/");

	files.push_back("./data/tdc_electrodes/cold_start.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	files.push_back("./data/tdc_electrodes/high_temp.dat");
			
	cBioTacForceCurve curve = ffTerms.TrainAgainstDataFiles(files);


	// --- Write Trained Curve To File --- //

	curve.SaveFitTermsToFile("./coefficients.bio");


	// --- Test Fit --- //

	std::cout << "Testing TDC/Electrode fit" << std::endl;
	files.clear();
	files.push_back("./data/tdc_electrodes/cold_start.dat");
	files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	files.push_back("./data/tdc_electrodes/heating_up.dat");
	files.push_back("./data/tdc_electrodes/high_temp.dat");
	files.push_back("./data/tdc_electrodes/zero1.dat");
	files.push_back("./data/tdc_electrodes/zero2.dat");
	files.push_back("./data/tdc_electrodes/zero3.dat");
	fit.TestAgainstDataFiles(files, compensators);
			
	std::cout << "Testing Force fit" << std::endl;
	files.clear();
	files.push_back("./data/test/data_2015_07_30_16_35_24.dat");
	files.push_back("./data/test/data_2015_07_29_14_44_14.dat");
	files.push_back("./data/test/data_2015_07_23_09_57_11.dat");
	// files.push_back("./data/zero_force_01.dat");
	// files.push_back("./data/zero_force_02.dat");
	files.push_back("./data/data_2015_07_23_09_50_24.dat");
	files.push_back("./data/data_2015_07_23_09_55_36.dat");
	files.push_back("./data/data_2015_07_23_09_55_48.dat");
	files.push_back("./data/tdc_electrodes/cold_start.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	files.push_back("./data/tdc_electrodes/high_temp.dat");
	ffTerms.TestAgainstDataFiles(files, curve);

			
	// --- Plot Results --- //
			
	nUtils::ClearConsole();
	int electrode = 0;
	while( true )
	{
		std::cout << "Select electrode: " << std::flush;
		std::cin >> electrode;
		if( (electrode >= 1) && (electrode <= 19) )
		{
			break;
		}
		else
		{
			std::cout << "Invalid selection: " << electrode << "!" << std::endl;
			std::cout << "Selection must be in the range 1-19" << std::endl;
			std::cout << std::endl;
		}
	}

	files.clear();
	// files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	// files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
	// files.push_back("./data/tdc_electrodes/heating_up.dat");
	// files.push_back("./data/tdc_electrodes/high_temp.dat");
	// files.push_back("./data/tdc_electrodes/zero1.dat");
	// files.push_back("./data/tdc_electrodes/zero2.dat");
	// files.push_back("./data/tdc_electrodes/zero3.dat");

	std::string file = nFileUtils::GetFileSelectionInDirectory("./data/tdc_electrodes", ".dat");
	files.push_back(file);
			
	curve.PlotElectrodeCurveAgainstFileData(files, electrode);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

biotac_data cBioTac::Read() const
{
	mpPicSerial->WriteCommandToPic(nUtils::READ_BIOTAC);
	return mpPicSerial->ReadValueFromPic<biotac_data>();
}

