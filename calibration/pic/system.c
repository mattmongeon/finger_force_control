#include "system.h"


static volatile state_t system_state = IDLE;


state_t system_get_state()
{
	return system_state;
}

void system_set_state(state_t state)
{
	__builtin_disable_interrupts();

	system_state = state;

	__builtin_enable_interrupts();
}

