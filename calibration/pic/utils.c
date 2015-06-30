#include "utils.h"
#include "NU32.h"

#define TICKS_PER_USEC 40  // 1 / (25 ns / 1 tick) * (1 us / 1000 ns)

void wait_usec(unsigned long time_usec)
{
	unsigned long start = _CP0_GET_COUNT();
	while((_CP0_GET_COUNT() - start) < (time_usec * TICKS_PER_USEC))
	{
		;
	}
}

void wait_nsec(unsigned long time_nsec)
{
	unsigned long start = _CP0_GET_COUNT();
	while((_CP0_GET_COUNT() - start) < time_nsec)
	{
		;
	}
}
