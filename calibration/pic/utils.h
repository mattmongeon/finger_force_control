#ifndef INCLUDED_UTILS_H
#define INCLUDED_UTILS_H


#include "NU32.h"


////////////////////////////////////////////////////////////////////////////////
//  Macros
////////////////////////////////////////////////////////////////////////////////

#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE

// Write a message to the status line and wait a bit.
#define DEBUG_MSG(msg, wait)  \
	LCD_Move(1,0);			  \
	LCD_WriteString((msg));	  \
							  \
    if( (wait) != 0 )		  \
	{						  \
		int i = 0;			  \
		while( i < 10000000 ) \
			++i;			  \
	}

#else

// Generates no code if DEBUG_ENABLE is not defined.
#define DEBUG_MSG(msg, wait) msg; wait;

#endif

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// This function performs a spin wait for the specified number of microseconds.
//
// Params:
// time_usec - The number of microseconds to wait in place.
void wait_usec(unsigned long time_usec);

// This function performs a spin wait for the specified number of nanoseconds.
// This timer has 25 nanoseconds resolution.
//
// Params:
// time_nsec - The number of nanoseconds to wait in place.
void wait_nsec(unsigned long time_nsec);


#endif // INCLUDED_UTILS_H

