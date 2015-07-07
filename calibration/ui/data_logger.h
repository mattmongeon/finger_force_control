#ifndef INCLUDED_DATA_LOGGER_H
#define INCLUDED_DATA_LOGGER_H

#include <queue>
#include <fstream>
#include <pthread.h>


// Writes data to file in a binary format.  The file contains a header which gives
// the names of the variables being written and their sizes in bytes. 
class cDataLogger
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Opens the output file and starts the real-time thread.
	cDataLogger();

	
	// Destructor.  Stops and cleans up the real-time thread, and closes the
	// output file.
	~cDataLogger();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Takes a pointer to a data buffer and the size of the buffer and writes the
	// data to file.
	//
	// Params:
	// pData - A pointer to the data buffer.
	// size - The number of bytes to write to file.
	template<typename T>
	void LogDataBuffer(T* pData, int size);


	// Writes the paramter data to file.
	//
	// Params:
	// data - The data to be written.
	template<typename T>
	void LogData(T data);

	
private:

	//--------------------------------------------------------------------------//
	//----------------------------  NESTED STRUCTS  ----------------------------//
	//--------------------------------------------------------------------------//

	// Contains the data to be written to file.
	struct sData
	{
		unsigned char* mpData;
		int mNumBytes;
	};
	
	
	//--------------------------------------------------------------------------//
	//--------------------------  THREADED FUNCTIONS  --------------------------//
	//--------------------------------------------------------------------------//

	// The threaded function that writes the data to file.  The parameter is
	// expected to be a pointer to the instance of this class.
	static void* ThreadFunc(void* pIn);


	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The file for writing the data. 
	std::ofstream mOutFile;

	// The handle to the POSIX thread.
	pthread_t mThreadHandle;

	// Mutex protecting access to the data queue.
	pthread_mutex_t mDataMutex;
	
	// True if the real-time thread is still running.
	bool mLoopRunning;

	// A queue containing data to be written to file.
	std::queue<sData> mOutputQueue;
};


#endif  // INCLUDED_DATA_LOGGER_H