#include "file_system.h"
#include "file_plotter.h"
#include "utils.h"
#include "file_utils.h"
#include "data_file_editor.h"
#include "data_file_reader.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFileSystem::cFileSystem()
{

}

////////////////////////////////////////////////////////////////////////////////

cFileSystem::~cFileSystem()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cFileSystem::DisplayMenu()
{
	nUtils::ClearConsole();
	
	bool keepGoing = true;
	while(keepGoing)
	{
		std::cout << "File Function Menu" << std::endl;
		std::cout << "--------------------" << std::endl;
		std::cout << std::endl;
		std::cout << "a: Plot data file" << std::endl;
		std::cout << "b: Edit files" << std::endl;
		std::cout << "c: Print file to CSV" << std::endl;
		std::cout << "q: Go back to main menu" << std::endl;
		std::cout << std::endl;
		
		switch(nUtils::GetMenuSelection())
		{
		case 'a':
			PlotFile();
			break;

		case 'b':
			EditFiles();
			break;

		case 'c':
			PrintFileToCSV();
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
//  File Operation Functions
////////////////////////////////////////////////////////////////////////////////

void cFileSystem::PlotFile()
{
	std::string file = nFileUtils::GetFileSelectionInDirectory("./data", ".dat");

	cFilePlotter f(file);
}

////////////////////////////////////////////////////////////////////////////////

void cFileSystem::EditFiles()
{
	cDataFileEditor("./data/tdc_electrodes/full_heat_up.dat", "./data/tdc_electrodes/full_heat_up.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/cooling_down_from_high.dat", "./data/tdc_electrodes/cooling_down_from_high.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/data_2015_07_30_09_44_43.dat", "./data/tdc_electrodes/data_2015_07_30_09_44_43.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/data_2015_08_04_15_27_33.dat", "./data/tdc_electrodes/data_2015_08_04_15_27_33.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/data_2015_08_04_16_15_04.dat", "./data/tdc_electrodes/data_2015_08_04_16_15_04.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/heating_up.dat", "./data/tdc_electrodes/heating_up.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/high_temp.dat", "./data/tdc_electrodes/high_temp.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/zero1.dat", "./data/tdc_electrodes/zero1.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/zero2.dat", "./data/tdc_electrodes/zero2.dat");
	// cDataFileEditor("./data/tdc_electrodes/original/zero3.dat", "./data/tdc_electrodes/zero3.dat");
}

////////////////////////////////////////////////////////////////////////////////

void cFileSystem::PrintFileToCSV()
{
	std::ofstream out("out.csv");

	std::string file = nFileUtils::GetFileSelectionInDirectory("./data", ".dat");
	cDataFileReader outFile(file);
	std::vector<biotac_tune_data> data(outFile.GetData());
				
	for( std::size_t i = 0; i < data.size(); ++i )
	{
		out << data[i].mData.e1 << ", " << data[i].mData.e2 << ", "
			<< data[i].mData.e3 << ", " << data[i].mData.e4 << ", "
			<< data[i].mData.e5 << ", " << data[i].mData.e6 << ", "
			<< data[i].mData.e7 << ", " << data[i].mData.e8 << ", "
			<< data[i].mData.e9 << ", " << data[i].mData.e10 << ", "
			<< data[i].mData.e11 << ", " << data[i].mData.e12 << ", "
			<< data[i].mData.e13 << ", " << data[i].mData.e14 << ", "
			<< data[i].mData.e15 << ", " << data[i].mData.e16 << ", "
			<< data[i].mData.e17 << ", " << data[i].mData.e18 << ", "
			<< data[i].mData.e19 << ", " << data[i].mData.tac << ", "
			<< data[i].mData.tdc << ", " << data[i].mData.pac << ", "
			<< data[i].mData.pdc << std::endl;
	}

	out.close();
}

