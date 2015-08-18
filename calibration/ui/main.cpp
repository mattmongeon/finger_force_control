#include "pic_serial.h"
#include "load_cell.h"
#include "biotac.h"
#include "utils.h"
#include "stopwatch.h"
#include "keyboard_thread.h"
#include "file_plotter.h"
#include "function_fit_nn.h"
#include "function_fit_nls.h"
#include "function_fit_force_terms.h"
#include "file_utils.h"
#include "data_file_reader.h"
#include "electrode_tdc_compensator.h"
#include "data_file_editor.h"
#include "motor.h"
#include "file_system.h"
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <ceres/ceres.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <map>
#include <fstream>


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
	std::cout << "b:  BioTac operations" << std::endl;
	std::cout << "f:  File operations" << std::endl;
	std::cout << "l:  Load cell operations" << std::endl;
	std::cout << "m:  Motor operations" << std::endl;
	std::cout << "n:  Train Neural network" << std::endl;
	std::cout << "o:  Test Neural Network" << std::endl;
	std::cout << "v:  Send force trajectory" << std::endl;
	std::cout << "q:  Quit" << std::endl;
	std::cout << "4:  Test TDC vs Electrode curve" << std::endl;
	std::cout << std::endl;
}


int getFileSelection(const std::vector<std::string>& files)
{
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		std::cout << i+1 << ": " << files[i] << std::endl;
	}
				
	bool goodSelection = false;
	while(!goodSelection)
	{
		std::cout << std::endl;
		std::cout << "Selection: " << std::flush;

		size_t selection = 0;
		std::cin >> selection;
		selection -= 1;
		std::cout << std::endl << std::endl;
					
		if( (selection < files.size()) && (selection >= 0) )
		{
			goodSelection = true;
			return selection;
		}
		else
		{
			std::cout << "The entry \'" << selection+1 << "\' is invalid." << std::endl;
		}
	}

	return -1;
}


int main(int argc, char** argv)
{
	nUtils::ClearConsole();
	
	
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
	cMotor motor(&picSerial);
	cFileSystem fileSystem;
	

	// --- Miscellaneous --- //
	
	std::cout << "Setting up thread for keyboard input" << std::endl;
	cKeyboardThread::Instance();
	google::InitGoogleLogging("biotac_cal");
	

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
			biotac.DisplayMenu();
			break;
		}
			
		case 'f':
		{
			fileSystem.DisplayMenu();
			break;
		}
			
		case 'l':
		{
			loadCell.DisplayMenu();
			break;
		}
		
		case 'm':
		{
			motor.DisplayMenu();
			break;
		}

		case 'n':
		{
			std::vector<std::string> v = nFileUtils::GetFilesInDirectory("./data", ".dat");

			for( std::size_t i = 0; i < v.size(); ++i )
			{
				v[i].insert(0, "./data/");
			}
			
			cFunctionFitNN nn;
			nn.TrainAgainstDataFiles(v);

			nn.SaveNeuralNetwork("first.net");
			
			break;
		}

		case 'o':
		{
			cFunctionFitNN nn;
			nn.LoadNeuralNetwork("first.net");

			std::vector<std::string> files = nFileUtils::GetFilesInDirectory("./data/test", ".dat");

			if( !files.empty() )
			{
				int selection = getFileSelection(files);

				if( selection != -1 )
				{
					std::string path("./data/test/");
					path += files[selection];
					std::cout << "Opening \'" << files[selection] << "\'" << std::endl;
					std::vector<std::string> f;
					f.push_back(path);
					nn.TestAgainstDataFiles(f);
				}
				else
				{
					std::cout << "There was an error choosing the file!" << std::endl;
				}
			}
			else
			{
				std::cout << std::endl << "There are no files in the \'data\' directory." << std::endl << std::endl;
				std::cout << std::endl;
			}
			
			break;
		}

		case 'q':
		{
			std::cout << "Exiting..." << std::endl;
			keepGoing = false;
			break;
		}

		case 'v':
		{
			std::cout << std::endl;
			std::cout << "Choose waveform:" << std::endl;
			std::cout << "1: Sine" << std::endl;
			std::cout << "2: Square" << std::endl;
			char selection = nUtils::GetMenuSelection();

			std::cout << std::endl;
			std::cout << "Enter maximum force (g): " << std::flush;
			int force = 0;
			std::cin >> force;

			picSerial.WriteCommandToPic(nUtils::SEND_FORCE_TRAJECTORY);
			picSerial.WriteValueToPic<int>(5);
			
			switch(selection)
			{
			case '1':
			{
				double pi = 3.14159265358979323846264338;
				for( double i = 0.0; i < 500.0; ++i )
				{
					double val = static_cast<double>(force/2) - static_cast<double>(force/2) * cos( 3 * i * pi / 500.0 );
					picSerial.WriteValueToPic<int>(static_cast<int>(val));
				}
				break;
			}

			case '2':
			{
				for( int i = 0; i < 100; ++i )
				{
					picSerial.WriteValueToPic<int>(force);
				}

				for( int i = 0; i < 100; ++i )
				{
					picSerial.WriteValueToPic<int>(static_cast<int>(0));
				}
				
				for( int i = 0; i < 100; ++i )
				{
					picSerial.WriteValueToPic<int>(force);
				}

				for( int i = 0; i < 100; ++i )
				{
					picSerial.WriteValueToPic<int>(static_cast<int>(0));
				}

				for( int i = 0; i < 100; ++i )
				{
					picSerial.WriteValueToPic<int>(force);
				}
				
				break;
			}

			default:
				break;
			}
			
			break;
		}

		case '9':
		{
			cBioTacForceCurve curve("./coefficients.bio");
			
			// --- Load TDC vs Electrode Coefficients From File --- //
			
			std::vector<cElectrodeTdcCompensator> compensators;
			compensators.clear();
			std::ifstream file("./data/tdc_electrodes/tdc_electrode_curve.coeff", std::ios::binary | std::ios::in);
			for( std::size_t i = 0; i < 19; ++i )
			{
				compensators.push_back( cElectrodeTdcCompensator(file) );
			}
			

			std::string fileName = nFileUtils::GetFileSelectionInDirectory("./data", ".dat");

			cDataFileReader reader(fileName.c_str());
			std::ofstream outFile("./out.csv");
			std::vector<biotac_tune_data> data = reader.GetData();
			for( std::size_t i = 0; i < data.size(); ++i )
			{
				for( std::size_t compIndex = 1; compIndex <= compensators.size(); ++compIndex )
				{
					double electrode = 0.0;
					switch(compIndex)
					{
					case 1: electrode = data[i].mData.e1; break;
					case 2: electrode = data[i].mData.e2; break;
					case 3: electrode = data[i].mData.e3; break;
					case 4: electrode = data[i].mData.e4; break;
					case 5: electrode = data[i].mData.e5; break;
					case 6: electrode = data[i].mData.e6; break;
					case 7: electrode = data[i].mData.e7; break;
					case 8: electrode = data[i].mData.e8; break;
					case 9: electrode = data[i].mData.e9; break;
					case 10: electrode = data[i].mData.e10; break;
					case 11: electrode = data[i].mData.e11; break;
					case 12: electrode = data[i].mData.e12; break;
					case 13: electrode = data[i].mData.e13; break;
					case 14: electrode = data[i].mData.e14; break;
					case 15: electrode = data[i].mData.e15; break;
					case 16: electrode = data[i].mData.e16; break;
					case 17: electrode = data[i].mData.e17; break;
					case 18: electrode = data[i].mData.e18; break;
					case 19: electrode = data[i].mData.e19; break;
					default: break;
					}
					
					double error = compensators[compIndex-1].CompensateTdc(data[i].mData.tdc, data[i].mData.pdc, electrode);
					outFile << error << ", ";
				}

				outFile << data[i].mData.tac << ", " << data[i].mData.tdc << ", "
						<< data[i].mData.pac << ", " << data[i].mData.pdc << std::endl;
			}

			outFile.close();

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

