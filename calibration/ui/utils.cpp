#include "utils.h"
#include <iostream>
#include <string>
#include <time.h>


char nUtils::GetMenuSelection()
{
	std::string selection;
	std::cout << "Selection:  " << std::flush;
	std::cin >> selection;
	return selection[0];
}

double nUtils::GetSysTime_ms()
{
	struct timespec ts;
	clock_gettime( CLOCK_REALTIME, &ts );
	double time_ms = static_cast<double>(ts.tv_nsec) / 1000000.0;
	time_ms += static_cast<double>(ts.tv_sec) * 1000.0;
	return time_ms;
}
