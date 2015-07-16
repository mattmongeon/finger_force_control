#ifndef INCLUDED_STOPWATCH_H
#define INCLUDED_STOPWATCH_H


#include <boost/timer/timer.hpp>


class cStopwatch
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cStopwatch();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Starts the stopwatch keeping time.  No effect if the stopwatch is already
	// timing.
	void Start();

	// Stops the stopwatch from keeping time.
	void Stop();

	// Returns the elapsed time of the stopwatch.  Returns 0 if not timing.
	//
	// Return - elapsed time in milliseconds.
	double GetElapsedTime_ms() const;

	
private:

	// The actual timer object used.
	boost::timer::cpu_timer mTimer;
};


#endif // INCLUDED_STOPWATCH_H
