#include "load_cell_cal.h"
#include "utils.h"
#include "pic_comm.h"
#include <iostream>


void calibrateLoadCell(cPicComm& picComm)
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

	bool keepGoing = true;
	while(keepGoing)
	{
		switch(nUtils::GetMenuSelection())
		{
		case 'n':
			
			std::cout << "Get value" << nUtils::PREV_LINE << "\r" << std::endl;
			std::cout << "Value:  " << picComm.ReadRawLoadCellValue() << std::endl;
			break;

		case 'q':
			keepGoing = false;
			return;

		default:
			break;
		}
	}
}

