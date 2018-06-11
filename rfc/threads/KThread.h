
/*
	RFC - KThread.h
	Copyright (C) 2013-2018 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/

#ifndef _RFC_KTHREAD_H_
#define _RFC_KTHREAD_H_

#include "../config.h"
#include <windows.h>

/**
	Encapsulates a thread.

	Subclasses derive from KThread and implement the Run() method, in which they
	do their business. The thread can then be started with the StartThread() method
	and controlled with various other methods.

	Run() method implementation might be like this
	@code
	virtual void Run()
	{
		while(!threadShouldStop)
		{
			// your code goes here...
		}
		isThreadRunning=false;	
	}
	@endcode

*/
class RFC_API KThread
{
protected:
	HANDLE handle;
	volatile bool isThreadRunning;
	volatile bool threadShouldStop;

public:
	KThread();

	/**
		Sets thread handle.
	*/
	virtual void SetHandle(HANDLE handle);

	/**
		Returns handle of the thread
	*/
	virtual HANDLE GetHandle();

	/**
		Override this method in your class.
	*/
	virtual void Run();

	/**
		Starts thread
	*/
	virtual bool StartThread();

	/**
		Another thread can signal this thread should stop. 
	*/
	virtual void ThreadShouldStop();

	/**
		@returns true if thread is still running
	*/
	virtual bool IsThreadRunning();

	/**
		Calling thread is not return until this thread finish.
	*/
	virtual void WaitUntilThreadFinish();

	/**
		Sleeps calling thread to given micro seconds.
	*/
	static void uSleep(int waitTime);

	virtual ~KThread();
};

#endif