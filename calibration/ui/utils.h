#ifndef INCLUDED_UTILS_H
#define INCLUDED_UTILS_H


#include <string>


namespace nUtils
{
    // --- Linux-Specific Console Commands --- //

	const std::string PREV_LINE = "\e[A";
	const std::string CLEAR_CONSOLE = "\033[2J\033[1;1H";


	// --- PIC Commands --- //

	const std::string READ_RAW_LOAD_CELL = "r\n";
	const std::string READ_LOAD_CELL = "l\n";
	const std::string SET_PWM = "p\n";
	const std::string READ_BIOTAC = "b\n";


	// --- Terminal Helper Functions --- //

	char GetMenuSelection();


	// --- Time Helper Functions --- //

	double GetSysTime_ms();
}

#endif  // INCLUDED_UTILS_H

