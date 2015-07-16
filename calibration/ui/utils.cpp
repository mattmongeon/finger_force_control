#include "utils.h"
#include <iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#endif


namespace
{
#ifdef WIN32
	COORD Win32GetCurrentConsoleCoords()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &csbi );
		return csbi.dwCursorPosition;
	}

	COORD Win32GetCurrentConsoleWindowSize()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &csbi );
		return csbi.dwMaximumWindowSize;
	}

	void Win32SetConsoleCoords(COORD xy)
	{
		SetConsoleCursorPosition( GetStdHandle(STD_OUTPUT_HANDLE), xy );
	}

	void Win32ClearCurrentLine()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo( consoleHandle, &csbi );

		csbi.dwCursorPosition.X = 0;
		DWORD count;
		FillConsoleOutputCharacter(consoleHandle, (TCHAR)' ', csbi.dwMaximumWindowSize.X, csbi.dwCursorPosition, &count);

		// Move the cursor to the beginning of the line.
		Win32SetConsoleCoords( csbi.dwCursorPosition );
	}
#else
	const std::string PREV_LINE = "\e[A";
	const std::string CLEAR_LINE = "\e[2K";
	const std::string CLEAR_CONSOLE = "\033[2J\033[1;1H";
#endif
}


void nUtils::ClearConsole()
{
#ifdef WIN32
	system("cls");
#else
	std::cout << CLEAR_CONSOLE << std::flush;
#endif
}

void nUtils::ConsoleMoveCursorUpLines( int numLines )
{
#ifdef WIN32
	COORD currentXY = Win32GetCurrentConsoleCoords();
	currentXY.Y -= numLines;
	Win32SetConsoleCoords(currentXY);
#else
	for( int i = 0; i < numLines; ++i )
	{
		std::cout << PREV_LINE;
	}

	std::cout << std::flush;
#endif
}

void nUtils::ClearCurrentLine()
{
#ifdef WIN32
	Win32ClearCurrentLine();
#else
	std::cout << CLEAR_LINE << std::flush;
#endif
}


char nUtils::GetMenuSelection()
{
	std::string selection;
	std::cout << "Selection:  " << std::flush;
	std::cin >> selection;
	return selection[0];
}

