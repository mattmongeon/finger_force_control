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
	std::cout << "a:  Calibrate BioTac" << std::endl;
	std::cout << "b:  Single read from BioTac" << std::endl;
	std::cout << "c:  Calibrate load cell" << std::endl;
	std::cout << "d:  List files in \'data\' directory" << std::endl;
	std::cout << "f:  Hold force" << std::endl;
	std::cout << "g:  Set torque controller gains" << std::endl;
	std::cout << "h:  Get torque controller gains" << std::endl;
	std::cout << "l:  Read load cell" << std::endl;
	std::cout << "n:  Train Neural network" << std::endl;
	std::cout << "o:  Test Neural Network" << std::endl;
	std::cout << "p:  Set motor PWM duty cycle" << std::endl;
	std::cout << "r:  Continuously read from BioTac" << std::endl;
	std::cout << "s:  Continuously read from load cell" << std::endl;
	std::cout << "t:  Tune torque gains" << std::endl;
	std::cout << "u:  Calibrate BioTac with trajectory" << std::endl;
	std::cout << "v:  Send force trajectory" << std::endl;
	std::cout << "q:  Quit" << std::endl;
	std::cout << "z:  Continuous raw load cell" << std::endl;
	std::cout << "2:  Fit curve for TDC vs Electrodes" << std::endl;
	std::cout << "3:  Fit curve for force vector coefficients" << std::endl;
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

		case 'd':
		{
			std::cout << "Select the file using its list index." << std::endl;
			std::cout << std::endl;
			std::cout << "Files:" << std::endl;
			std::cout << "------" << std::endl;
			std::cout << std::endl;

			std::vector<std::string> files = nFileUtils::GetFilesInDirectory("./data", ".dat");

			if( !files.empty() )
			{
				int selection = getFileSelection(files);

				if( selection != -1 )
				{
					std::string path("./data/");
					path += files[selection];
					std::cout << "Opening \'" << files[selection] << "\'" << std::endl;
					cFilePlotter f(path);
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

		case 'p':
		{
			std::cout << "PWM value as integer percentage:  " << std::flush;
			int pwm = 0;
			std::cin >> pwm;
			picSerial.WriteCommandToPic(nUtils::SET_PWM);
			picSerial.WriteValueToPic<char>(static_cast<char>(pwm));
			std::cout << "Sent!" << std::endl;
			break;
		}

		case 'q':
		{
			std::cout << "Exiting..." << std::endl;
			keepGoing = false;
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

		case 'u':
		{
			biotac.RecordCalWithTrajectory();
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

		case 'z':
		{
			std::cout << "CURRENTLY DOING NOTHING WITH z" << std::endl;
			break;
		}

		case '1':
		{
			std::map< int, std::vector<uint16_t> > e1, e2, e3, e4, e5,
				                                   e6, e7, e8, e9, e10,
				                                   e11, e12, e13, e14, e15,
				                                   e16, e17, e18, e19;
			
			std::string path = "./data/tdc_electrodes/data_2015_07_30_09_44_43.dat";

			cDataFileReader reader(path);
			std::vector<biotac_tune_data> data = reader.GetData();
			std::size_t numDataPoints = data.size();
			for( std::size_t dataIndex = 0; dataIndex < numDataPoints; ++dataIndex )
			{
				e1[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e1);
				e2[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e2);
				e3[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e3);
				e4[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e4);
				e5[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e5);
				e6[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e6);
				e7[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e7);
				e8[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e8);
				e9[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e9);
				e10[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e10);
				e11[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e11);
				e12[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e12);
				e13[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e13);
				e14[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e14);
				e15[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e15);
				e16[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e16);
				e17[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e17);
				e18[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e18);
				e19[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e19);
			}


			// --- E1 --- //
			
			std::cout << "E1 values:" << std::endl;
			std::cout << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e1.begin(); it != e1.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl;
			std::cout << std::endl << std::endl;
			

			// --- E2 --- //
			
			std::cout << "E2 values:" << std::endl;
			std::cout << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e2.begin(); it != e2.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl;
			std::cout << std::endl << std::endl;
			
			
			// --- E3 --- //
			
			std::cout << "E3 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e3.begin(); it != e3.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;


			// --- E4 --- //
			
			std::cout << "E4 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e4.begin(); it != e4.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E5 --- //
			
			std::cout << "E5 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e5.begin(); it != e5.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E6 --- //
			
			std::cout << "E6 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e6.begin(); it != e6.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E7 --- //
			
			std::cout << "E7 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e7.begin(); it != e7.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E9 --- //
			
			std::cout << "E9 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e9.begin(); it != e9.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E10 --- //
			
			std::cout << "E10 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e10.begin(); it != e10.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E11 --- //
			
			std::cout << "E11 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e11.begin(); it != e11.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E12 --- //
			
			std::cout << "E12 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e12.begin(); it != e12.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E13 --- //
			
			std::cout << "E13 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e13.begin(); it != e13.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E14 --- //
			
			std::cout << "E14 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e14.begin(); it != e14.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E15 --- //
			
			std::cout << "E15 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e15.begin(); it != e15.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E16 --- //
			
			std::cout << "E16 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e16.begin(); it != e16.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E17 --- //
			
			std::cout << "E17 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e17.begin(); it != e17.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E18 --- //
			
			std::cout << "E18 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e18.begin(); it != e18.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			// --- E19 --- //
			
			std::cout << "E19 values:" << std::endl << "{";
			for( std::map< int, std::vector<uint16_t> >::iterator it = e19.begin(); it != e19.end(); ++it )
			{
				unsigned long long sum = 0;
				for( std::size_t i = 0; i < it->second.size(); ++i )
					sum += it->second[i];

				std::cout << "{" << it->first << "," << (static_cast<double>(sum) / static_cast<double>(it->second.size())) << "},";
			}

			std::cout << "}" << std::endl << std::endl << std::endl;
			
			
			break;
		}

		case '2':
		{
			// --- Train For Fit --- //
			
			cFunctionFitNLS fit;
			std::vector<std::string> files;
			files.push_back("./data/tdc_electrodes/cold_start.dat");
			// files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
			files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
			files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
			files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
			files.push_back("./data/tdc_electrodes/heating_up.dat");
			files.push_back("./data/tdc_electrodes/high_temp.dat");
			// files.push_back("./data/tdc_electrodes/zero1.dat");
			// files.push_back("./data/tdc_electrodes/zero2.dat");
			// files.push_back("./data/tdc_electrodes/zero3.dat");
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
			fit.TestAgainstDataFiles(files, compensators);


			// --- Write Coefficients To File --- //
			
			std::ofstream file("./data/tdc_electrodes/tdc_electrode_curve.coeff", std::ios::binary | std::ios::out);
			for( std::size_t i = 0; i < compensators.size(); ++i )
			{
				compensators[i].SaveCoefficientsToFile(file);
			}
			
			break;
		}

		case '3':
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
			
			cBioTacForceCurve curve = ffTerms.TrainAgainstDataFiles(files);


			// --- Test Fit --- //
			
			files.clear();
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
			
			break;
		}


		case '4':
		{
			cBioTacForceCurve curve("./coefficients.bio");

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

			std::vector<std::string> files;
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
			
			
			// curve.PlotElectrodeCurveAgainstFileData(file, electrode);

			break;
		}

		case '5':
		{
			// --- Edit Files --- //
			
			cDataFileEditor("./data/data_2015_08_10_12_25_24.dat", "./data/data_2015_08_10_12_25_24.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/cooling_down_from_high.dat", "./data/tdc_electrodes/cooling_down_from_high.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/data_2015_07_30_09_44_43.dat", "./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/data_2015_08_04_15_27_33.dat", "./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/data_2015_08_04_16_15_04.dat", "./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/heating_up.dat", "./data/tdc_electrodes/heating_up.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/high_temp.dat", "./data/tdc_electrodes/high_temp.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/zero1.dat", "./data/tdc_electrodes/zero1.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/zero2.dat", "./data/tdc_electrodes/zero2.dat");
			// cDataFileEditor("./data/tdc_electrodes/original/zero3.dat", "./data/tdc_electrodes/zero3.dat");
			
			break;
		}

		case '6':
		{
			// --- Print To CSV --- //
			
			cDataFileReader f1("./data/tdc_electrodes/cold_start.dat");
			// cDataFileReader f2("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");

			std::vector<biotac_tune_data> data1(f1.GetData());
			// std::vector<biotac_tune_data> data2(f2.GetData());

			std::ofstream out1("e1.csv");
			std::ofstream out2("e2.csv");
			std::ofstream out3("e3.csv");
			std::ofstream out4("e4.csv");
			std::ofstream out5("e5.csv");
			std::ofstream out6("e6.csv");
			std::ofstream out7("e7.csv");
			std::ofstream out8("e8.csv");
			std::ofstream out9("e9.csv");
			std::ofstream out10("e10.csv");
			std::ofstream out11("e11.csv");
			std::ofstream out12("e12.csv");
			std::ofstream out13("e13.csv");
			std::ofstream out14("e14.csv");
			std::ofstream out15("e15.csv");
			std::ofstream out16("e16.csv");
			std::ofstream out17("e17.csv");
			std::ofstream out18("e18.csv");
			std::ofstream out19("e19.csv");
			
			for( std::size_t i = 0; i < data1.size(); ++i )
			{
				out1 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e1 << std::endl;

				out2 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e2 << std::endl;

				out3 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e3 << std::endl;

				out4 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e4 << std::endl;

				out5 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e5 << std::endl;

				out6 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e6 << std::endl;

				out7 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e7 << std::endl;

				out8 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e8 << std::endl;

				out9 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e9 << std::endl;

				out10 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e10 << std::endl;

				out11 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e11 << std::endl;

				out12 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e12 << std::endl;

				out13 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e13 << std::endl;

				out14 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e14 << std::endl;

				out15 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e15 << std::endl;

				out16 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e16 << std::endl;

				out17 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e17 << std::endl;

				out18 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e18 << std::endl;

				out19 << data1[i].mData.tac << ", " << data1[i].mData.tdc << ", "
					 << data1[i].mData.pac << ", " << data1[i].mData.pdc << ", "
					 << data1[i].mData.e19 << std::endl;
			}

			/*
			for( std::size_t i = 0; i < data2.size(); ++i )
			{
				out1 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e1 << std::endl;
				
				out2 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e2 << std::endl;
				
				out3 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e3 << std::endl;
				
				out4 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e4 << std::endl;
				
				out5 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e5 << std::endl;
				
				out6 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e6 << std::endl;
				
				out7 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e7 << std::endl;
				
				out8 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e8 << std::endl;
				
				out9 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e9 << std::endl;
				
				out10 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e10 << std::endl;
				
				out11 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e11 << std::endl;
				
				out12 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e12 << std::endl;
				
				out13 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e13 << std::endl;
				
				out14 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e14 << std::endl;
				
				out15 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e15 << std::endl;
				
				out16 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e16 << std::endl;
				
				out17 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e17 << std::endl;
				
				out18 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e18 << std::endl;
				
				out19 << data2[i].mData.tac << ", " << data2[i].mData.tdc << ", "
					<< data2[i].mData.pac << ", " << data2[i].mData.pdc << ", "
					<< data2[i].mData.e19 << std::endl;	
			}
			*/

			out1.close();
			out2.close();
			out3.close();
			out4.close();
			out5.close();
			out6.close();
			out7.close();
			out8.close();
			out9.close();
			out10.close();
			out11.close();
			out12.close();
			out13.close();
			out14.close();
			out15.close();
			out16.close();
			out17.close();
			out18.close();
			out19.close();
			
			break;
		}

		case '7':
		{
			// --- Train For Fit --- //
			
			cFunctionFitNLS fit;
			std::vector<std::string> files;
			files.push_back("./data/tdc_electrodes/cold_start.dat");
			files.push_back("./data/tdc_electrodes/cooling_down_from_high.dat");
			files.push_back("./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
			files.push_back("./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
			files.push_back("./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
			files.push_back("./data/tdc_electrodes/heating_up.dat");
			files.push_back("./data/tdc_electrodes/high_temp.dat");
			// files.push_back("./data/tdc_electrodes/zero1.dat");
			// files.push_back("./data/tdc_electrodes/zero2.dat");
			// files.push_back("./data/tdc_electrodes/zero3.dat");
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
			
			
			// curve.PlotElectrodeCurveAgainstFileData(file, electrode);
			
			break;
		}

		case '8':
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
			

			// --- Test Force Fit --- //
			
			cFunctionFitForceTerms ffTerms(compensators);

			// std::vector<std::string> files;
			std::vector<std::string> files = nFileUtils::GetFilesInDirectory("./data", ".dat");
			for( std::size_t i = 0; i < files.size(); ++i )
			{
				files[i].insert(0, "./data/");
			}
			
			files.push_back("./data/test/data_2015_07_30_16_35_24.dat");
			files.push_back("./data/test/data_2015_07_29_14_44_14.dat");
			files.push_back("./data/test/data_2015_07_23_09_57_11.dat");
			// files.push_back("./data/data_2015_07_23_09_50_24.dat");
			// files.push_back("./data/data_2015_07_23_09_55_36.dat");
			// files.push_back("./data/data_2015_07_23_09_55_48.dat");
			ffTerms.TestAgainstDataFiles(files, curve);

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
			

			std::string fileName = nFileUtils::GetFileSelectionInDirectory("./data/tdc_electrodes", ".dat");
			fileName.insert(0, "./data/tdc_electrodes/");

			cDataFileReader reader(fileName.c_str());
			std::ofstream outFile("./out.csv");
			std::vector<biotac_tune_data> data = reader.GetData();
			for( std::size_t i = 0; i < data.size(); ++i )
			{
				for( std::size_t compIndex = 0; compIndex < compensators.size(); ++compIndex )
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
					
					double error = compensators[compIndex].CompensateTdc(data[i].mData.tdc, data[i].mData.pdc, electrode);
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

