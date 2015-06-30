#include "keyboard_thread.h"
#include <string>
#include <iostream>


cKeyboardThread* cKeyboardThread::mpThis = NULL;


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cKeyboardThread::cKeyboardThread()
:   mDetectEnterPress(false),
	mQuitRequested(false),
	mLoopRunning(true)
{
	mpThis = this;
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
	while(mpThis->mLoopRunning)
	{
		if( mpThis->mDetectEnterPress )
		{
			// Just wait for the user to press q+ENTER.
			std::string input;
			std::cin >> input;
			if( input == "q" )
			{
				mpThis->mQuitRequested = true;
				mpThis->mDetectEnterPress = false;
			}
		}
	}

	return NULL;
}

