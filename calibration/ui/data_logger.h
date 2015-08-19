#ifndef INCLUDED_DATA_LOGGER_H
#define INCLUDED_DATA_LOGGER_H

#include <boost/thread/thread.hpp>
#include <string>
#include <queue>
#include <fstream>


// Writes data to file in a binary format.
class cDataLogger
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Opens the output file and starts the real-time thread.
	cDataLogger(const std::string& fileName = "");

	
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
	void LogDataBuffer(T* pData, int size)
	{
		sData data;
		data.mpData = new unsigned char[size];
		memcpy(data.mpData, reinterpret_cast<unsigned char*>(pData), size);
		data.mNumBytes = size;

		mDataMutex.lock();
		mOutputQueue.push(data);
		mDataMutex.unlock();
	}


	// Writes the paramter data to file.
	//
	// Params:
	// data - The data to be written.
	template<typename T>
	void LogData(T data)
	{
		LogDataBuffer<T>(&data, sizeof(T));
	}

	
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
	static void ThreadFunc(cDataLogger* pThis);


	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The file for writing the data. 
	std::ofstream mOutFile;

	// The handle to the worker thread.
	boost::thread mThread;

	// Mutex protecting access to the data queue.
	boost::mutex mDataMutex;
	
	// True if the real-time thread is still running.
	bool mLoopRunning;

	// A queue containing data to be written to file.
	std::queue<sData> mOutputQueue;
};


#endif  // INCLUDED_DATA_LOGGER_H
