#include "utils.h"
#include <iostream>
#include <string>


char nUtils::GetMenuSelection()
{
	std::string selection;
	std::cout << "Selection:  " << std::flush;
	std::cin >> selection;
	return selection[0];
}
