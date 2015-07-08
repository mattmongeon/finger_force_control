#include "data_logger.h"
#include <sstream>
#include <ctime>
#include <cstring>
#include <iostream>
#include <sys/stat.h>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cDataLogger::cDataLogger()
{
	struct stat st = {0};
	if( stat("./data", &st) == -1)
	{
		mkdir("./data", 0777);
	}
	
	time_t curTime = time(NULL);
	tm* t = localtime(&curTime);
	std::ostringstream s;
	s << "./data/data_" << t->tm_year + 1900 << "_"
	  << t->tm_mon + 1 << "_"
	  << t->tm_mday + 1 << "_"
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
	if( !mOutputQueue.empty() )
	{
		std::cout << "Data logger still has data to write to file.  Finishing..." << std::endl;
		while(!mOutputQueue.empty())
		{
			;
		}

		std::cout << "Finished writing points to file!" << std::endl;
	}
	
	mLoopRunning = false;
	pthread_join(mThreadHandle, NULL);
	pthread_mutex_destroy(&mDataMutex);
	
	mOutFile.close();
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
