#include "load_cell.h"
#include "utils.h"
#include "pic_serial.h"
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

int cLoadCell::ReadLoadCell_grams()
{
	return mpPicSerial->ReadValueFromPic<int>(nUtils::READ_LOAD_CELL);
}

