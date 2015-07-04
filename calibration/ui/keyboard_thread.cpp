#include "keyboard_thread.h"
#include <string>
#include <iostream>


cKeyboardThread* cKeyboardThread::mpInstance = 0;


////////////////////////////////////////////////////////////////////////////////
//  Singleton Functions
////////////////////////////////////////////////////////////////////////////////

cKeyboardThread* cKeyboardThread::Instance()
{
	if( mpInstance == 0 )
		mpInstance = new cKeyboardThread();

	return mpInstance;
}

////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cKeyboardThread::cKeyboardThread()
:   mDetectEnterPress(false),
	mQuitRequested(false),
	mLoopRunning(true)
{
	pthread_create(&mKeyboardThreadHandle, NULL, ThreadFunc, NULL);
}

////////////////////////////////////////////////////////////////////////////////

cKeyboardThread::~cKeyboardThread()
{
	mLoopRunning = false;
	pthread_join(mKeyboardThreadHandle, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cKeyboardThread::StartDetection()
{
	mDetectEnterPress = true;
}

////////////////////////////////////////////////////////////////////////////////

bool cKeyboardThread::QuitRequested()
{
	if(mQuitRequested)
	{
		mQuitRequested = false;
		return true;
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Threaded Function
////////////////////////////////////////////////////////////////////////////////

void* cKeyboardThread::ThreadFunc(void* pIn)
{
	while(mpInstance->mLoopRunning)
	{
		if( mpInstance->mDetectEnterPress )
		{
			// Just wait for the user to press q+ENTER.
			std::string input;
			std::cin >> input;
			if( input == "q" )
			{
				mpInstance->mQuitRequested = true;
				mpInstance->mDetectEnterPress = false;
			}
		}
	}

	return NULL;
}

