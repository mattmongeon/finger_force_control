#ifndef INCLUDED_KEYBOARD_THREAD_H
#define INCLUDED_KEYBOARD_THREAD_H


#include <pthread.h>


// Maintains a thread that monitors keyboard input to check for when the user
// presses q+ENTER.  It relies on RAII to start and stop the thread with the
// constructor and destructor.
class cKeyboardThread
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Starts the keyboard monitoring thread.
	cKeyboardThread();

	// Destructor.  Stops and cleans up the keyboard monitoring thread.
	~cKeyboardThread();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Starts detection of q+ENTER.
	void StartDetection();
	
	// Returns true if this thread has detected that the user has pressed q+ENTER.
	// As soon as this function is called and returns true, the detection will be
	// reset so that further calls to this function will return false until the
	// user enters q+ENTER again.
	//
	// Return - true if q+ENTER has been entered, false otherwise.
	bool QuitRequested();

	
private:

	//--------------------------------------------------------------------------//
	//--------------------------  THREADED FUNCTIONS  --------------------------//
	//--------------------------------------------------------------------------//

	// The threaded function that monitors the keyboard.  The parameter is not
	// used.  This function returns nothing.
	static void* ThreadFunc(void* pIn);
	
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// Set to true to detect when q+ENTER is pressed.
	bool mDetectEnterPress;
	
	// Set to true if the user has entered q+ENTER.
	bool mQuitRequested;

	// True if the monitoring thread is still running.
	bool mLoopRunning;

	// The handle to the POSIX thread.
	pthread_t mKeyboardThreadHandle;

	// Pointer to this so it can be used in the threaded function.
	static cKeyboardThread* mpThis;
};


#endif  // INCLUDED_KEYBOARD_THREAD_H


