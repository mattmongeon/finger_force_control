#include "data_logger.h"
#include "file_utils.h"
#include "../common/biotac_comm.h"
#include <sstream>
#include <ctime>
#include <cstring>
#include <iostream>
#include <iomanip>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cDataLogger::cDataLogger(const std::string& fileName)
{
	nFileUtils::CreateDirectory("./data");
	
	time_t curTime = time(NULL);
	tm* t = localtime(&curTime);
	std::ostringstream s;
	if( fileName == "" )
	{
		s << "./data/data_" << t->tm_year + 1900 << "_"
		  << std::setw(2) << std::setfill('0')
		  << t->tm_mon + 1 << "_"
		  << std::setw(2) << std::setfill('0')
		  << t->tm_mday << "_"
		  << std::setw(2) << std::setfill('0')
		  << t->tm_hour << "_"
		  << std::setw(2) << std::setfill('0')
		  << t->tm_min << "_"
		  << std::setw(2) << std::setfill('0')
		  << t->tm_sec << ".dat";
		s << std::flush;
	}
	else
	{
		s << "./data/" << fileName << ".dat" << std::flush;
	}
	
	mOutFile.open(s.str().c_str(), std::ios::binary | std::ios::out);

	
	mLoopRunning = true;
	mThread = boost::thread(ThreadFunc, this);
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
	mThread.join();
	
	mOutFile.close();
}

////////////////////////////////////////////////////////////////////////////////
//  Threaded Functions
////////////////////////////////////////////////////////////////////////////////

void cDataLogger::ThreadFunc(cDataLogger* pThis)
{
	sData data;
	bool writeData = false;
	while(pThis->mLoopRunning)
	{
		pThis->mDataMutex.lock();
		if( !pThis->mOutputQueue.empty() )
		{
			writeData = true;
			data = pThis->mOutputQueue.front();
			pThis->mOutputQueue.pop();
		}
		pThis->mDataMutex.unlock();

		
		if( writeData )
		{
			writeData = false;
			pThis->mOutFile.write(reinterpret_cast<const char*>(data.mpData), data.mNumBytes);
			delete data.mpData;
		}
	}
}
