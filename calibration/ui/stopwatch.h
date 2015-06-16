#ifndef INCLUDED_STOPWATCH_H
#define INCLUDED_STOPWATCH_H


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

	// Resets the stopwatch to 0 ms elapsed time.
	void Reset();

	// Stops the stopwatch from timing and resets the elapsed time to 0 ms.
	void StopAndReset();

	// Returns the elapsed time of the stopwatch.  Returns 0 if not timing.
	//
	// Return - elapsed time in milliseconds.
	double GetElapsedTime_ms() const;

	
private:

	// Marks the time the stopwatch started timing.
	double mStartTime;
	
	// Keeps track of all elapsed time in ms.
	double mPrevElapsedTime_ms;

	// Specifies whether the timer is currently running or not.
	bool mStarted;
};


#endif // INCLUDED_STOPWATCH_H
