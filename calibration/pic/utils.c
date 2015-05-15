#include "utils.h"
#include "NU32.h"

#define TICKS_PER_USEC  ((NU32_SYS_FREQ / 1000000ul) / 2)

void wait_usec(unsigned long time_usec)
{
	unsigned long start = _CP0_GET_COUNT();
	while(_CP0_GET_COUNT() - start < (time_usec * TICKS_PER_USEC))
	{
		;
	}
}
