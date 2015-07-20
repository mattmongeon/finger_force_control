#ifndef INCLUDED_UTILS_H
#define INCLUDED_UTILS_H


#include <string>


namespace nUtils
{
    // --- Console Commands --- //

	void ClearConsole();

	void ConsoleMoveCursorUpLines( int numLines );

	void ClearCurrentLine();
	

	// --- PIC Commands --- //

	const std::string BIOTAC_CAL_SINGLE = "a\n";
	const std::string READ_BIOTAC = "b\n";
	const std::string HOLD_FORCE = "f\n";
	const std::string SET_TORQUE_CTRL_GAINS = "g\n";
	const std::string GET_TORQUE_CTRL_GAINS = "h\n";
	const std::string READ_LOAD_CELL = "l\n";
	const std::string READ_RAW_LOAD_CELL = "m\n";
	const std::string SET_PWM = "p\n";
	const std::string BIOTAC_READ_CONTINUOUS = "r\n";
	const std::string LOAD_CELL_READ_CONTINUOUS = "s\n";
	const std::string TUNE_TORQUE_GAINS = "t\n";
	const std::string BIOTAC_CAL_TRAJECTORY = "u\n";
	const std::string SEND_FORCE_TRAJECTORY = "v\n";

	const std::string WILDCARD = "z\n";

	const std::string STOP_ACTIVITY = "q\n";
	

	// --- Color Enum for PLplot --- //
	
	enum enumPLplotColor
	{
		enumPLplotColor_BLACK = 0,
		enumPLplotColor_RED,
		enumPLplotColor_YELLOW,
		enumPLplotColor_GREEN,
		enumPLplotColor_AQUAMARINE,
		enumPLplotColor_PINK,
		enumPLplotColor_WHEAT,
		enumPLplotColor_GREY,
		enumPLplotColor_BROWN,
		enumPLplotColor_BLUE,
		enumPLplotColor_BLUE_VIOLET,
		enumPLplotColor_CYAN,
		enumPLplotColor_TURQUOISE,
		enumPLplotColor_MAGENTA,
		enumPLplotColor_SALMON,
		enumPLplotColor_WHITE
	};
	

	// --- Terminal Helper Functions --- //

	char GetMenuSelection();
}

#endif  // INCLUDED_UTILS_H

