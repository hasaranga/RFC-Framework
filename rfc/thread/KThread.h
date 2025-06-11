
/*
	Copyright (C) 2013-2025 CrownSoft
  
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

#pragma once

#include "../core/CoreModule.h"
#include "KRunnable.h"
#include <functional>

/**
	Encapsulates a thread.

	Method1:
	Subclasses derive from KThread and implement the run() method, in which they
	do their business. The thread can then be started with the startThread() method
	and controlled with various other methods.

	run() method implementation might be like this
	@code
	virtual void run()
	{
		while(shouldRun())
		{
			// your code goes here...
		}
	}
	@endcode

	Method2:
	Subclasses derive from KRunnable and implement the run method. 

	Run method implementation might be like this
	@code
	virtual void run(KThread* thread)
	{
		while(thread->shouldRun())
		{
			// your code goes here...
		}
	}
	@endcode

	Method3:
	Assign lambda to onRun property.

	Run method implementation might be like this
	@code
	myThread.onRun = [this](KThread* thread){
		while(thread->shouldRun())
		{
			// your code goes here...
		}
	};
	@endcode
*/
class KThread
{
protected:
	HANDLE handle;
	volatile bool shouldStop;
	KRunnable* runnable;

public:
	std::function<void(KThread*)> onRun;

	KThread();

	/**
		Sets thread handle.
	*/
	virtual void setHandle(HANDLE handle);

	/**
		Sets runnable object for this thread.
	*/
	virtual void setRunnable(KRunnable* runnable);

	/**
		Returns handle of the thread
	*/
	virtual HANDLE getHandle();

	operator HANDLE()const;

	/**
		Override this method in your class.
	*/
	virtual void run();

	/**
		Starts thread
	*/
	virtual bool startThread();

	/**
		Another thread can signal this thread should stop. 
	*/
	virtual void threadShouldStop();

	/**
		@returns true if thread should run
	*/
	virtual bool shouldRun();

	/**
		@returns true if thread is still running
	*/
	virtual bool isThreadRunning();

	/**
		Caller will not return until this thread finish.
		Set pumpMessages to true to enable message processing for caller. It will help to avoid deadlocks if the caller is a gui thread!
	*/
	virtual DWORD waitUntilThreadFinish(bool pumpMessages = false);

	/**
		Sleeps calling thread to given micro seconds.
	*/
	static void uSleep(int waitTime);

	virtual ~KThread();

private:
	RFC_LEAK_DETECTOR(KThread)
};

