#include "data_logger.h"
#include <sstream>
#include <ctime>
#include <cstring>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cDataLogger::cDataLogger()
{
	time_t curTime = time(NULL);
	tm* t = localtime(&curTime);
	std::ostringstream s;
	s << "data_" << t->tm_year + 1900 << "_"
	  << t->tm_mon << "_"
	  << t->tm_mday << "_"
	  << t->tm_hour << "_"
	  << t->tm_min << "_"
	  << t->tm_sec << ".dat";
	
	mOutFile.open(s.str().c_str(), std::ios::binary | std::ios::out);

	mLoopRunning = true;
	pthread_mutex_init(&mDataMutex, NULL);
	pthread_create(&mThreadHandle, NULL, ThreadFunc, (void*)this);
}

////////////////////////////////////////////////////////////////////////////////

cDataLogger::~cDataLogger()
{
	// It is possible there is still some data left to be written.  Sit here and
	// spin until it is written to file.
	while(!mOutputQueue.empty())
	{
		;
	}
	
	mLoopRunning = false;
	pthread_join(mThreadHandle, NULL);
	pthread_mutex_destroy(&mDataMutex);
	
	mOutFile.close();
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void cDataLogger::LogDataBuffer(T* pData, int size)
{
	sData data;
	data.mpData = new unsigned char[size];
	memcpy(data.mpData, reinterpret_cast<unsigned char*>(pData), size);
	data.mNumBytes = size;

	pthread_mutex_lock(&mDataMutex);
	mOutputQueue.push(data);
	pthread_mutex_unlock(&mDataMutex);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void cDataLogger::LogData(T data)
{
	sData d;
	data.mpData = new unsigned char[sizeof(T)];
	memcpy(d.mpData, reinterpret_cast<unsigned char*>(&data), sizeof(T));
	data.mNumBytes = sizeof(T);

	pthread_mutex_lock(&mDataMutex);
	mOutputQueue.push(d);
	pthread_mutex_unlock(&mDataMutex);
}

////////////////////////////////////////////////////////////////////////////////
//  Threaded Functions
////////////////////////////////////////////////////////////////////////////////

void* cDataLogger::ThreadFunc(void* pIn)
{
	cDataLogger* pThis = reinterpret_cast<cDataLogger*>(pIn);

	sData data;
	bool writeData = false;
	while(pThis->mLoopRunning)
	{
		pthread_mutex_lock(&(pThis->mDataMutex));
		if( !pThis->mOutputQueue.empty() )
		{
			writeData = true;
			data = pThis->mOutputQueue.front();
			pThis->mOutputQueue.pop();
		}
		pthread_mutex_unlock(&(pThis->mDataMutex));

		
		if( writeData )
		{
			writeData = false;
			pThis->mOutFile.write(reinterpret_cast<const char*>(data.mpData), data.mNumBytes);
			delete data.mpData;
		}
	}

	return NULL;
}
