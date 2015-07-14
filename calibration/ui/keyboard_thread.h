#ifndef INCLUDED_KEYBOARD_THREAD_H
#define INCLUDED_KEYBOARD_THREAD_H


#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


// Maintains a thread that monitors keyboard input to check for when the user
// presses q+ENTER.  It relies on RAII to start and stop the thread with the
// constructor and destructor.
class cKeyboardThread
{
public:

	//--------------------------------------------------------------------------//
	//--------------------------  SINGLETON FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Returns the only possible instance to this class.
	//
	// Return - A pointer to the only existing instance of this class.
	static cKeyboardThread* Instance();

	
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
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Starts the keyboard monitoring thread.
	cKeyboardThread();

	// Destructor.  Stops and cleans up the keyboard monitoring thread.
	~cKeyboardThread();


	//--------------------------------------------------------------------------//
	//--------------------------  THREADED FUNCTIONS  --------------------------//
	//--------------------------------------------------------------------------//

	// The threaded function that monitors the keyboard.
	static void ThreadFunc();
	
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The pointer to the instance used by this singleton.
	static cKeyboardThread* mpInstance;
	
	// Set to true to detect when q+ENTER is pressed.
	bool mDetectEnterPress;
	
	// Set to true if the user has entered q+ENTER.
	bool mQuitRequested;

	// True if the monitoring thread is still running.
	bool mLoopRunning;

	// The thread used for monitoring the keyboard.
	boost::thread mKeyboardThread;
};


#endif  // INCLUDED_KEYBOARD_THREAD_H


