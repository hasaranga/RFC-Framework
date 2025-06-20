
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


#include "KThread.h"
#include <process.h>

unsigned __stdcall _RFCThread_Proc(void* lpParameter)
{
	if (lpParameter == nullptr) // for safe!
		return 0;

	KThread* thread = (KThread*)lpParameter;
	thread->run();

	// handle will be closed at destructor or next "start" call.

	return 0;
}

bool KThread::createRFCThread()
{
	// create thread in suspended state. so we can set the handle field.
	HANDLE threadHandle = (HANDLE)::_beginthreadex(NULL, 0, _RFCThread_Proc, (KThread*)this, CREATE_SUSPENDED, NULL);
	//HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);

	if (threadHandle)
	{
		handle = threadHandle;
		::ResumeThread(handle);

		return true;
	}
	return false;
}

KThread::KThread() : stopRequestedFlag(false), handle(0), runnable(nullptr) {}

void KThread::setRunnable(KRunnable* runnable)
{
	this->runnable = runnable;
}

HANDLE KThread::getHandle()
{
	return handle;
}

KThread::operator HANDLE()const
{
	return handle;
}

bool KThread::isRunningAllowed()
{
	const bool mustStop = stopRequestedFlag.load(std::memory_order_relaxed);
	if (mustStop)
		std::atomic_thread_fence(std::memory_order_acquire);

	return !mustStop;
}

void KThread::run()
{
	if (runnable)
		runnable->run(this);
	else if (onRun)
		onRun(this);
}

bool KThread::isRunning()
{
	if (handle)
	{
		const DWORD result = ::WaitForSingleObject(handle, 0);
		if (result == WAIT_FAILED)
			return false;
		return (result != WAIT_OBJECT_0);
	}

	return false;
}

void KThread::shouldStop()
{
	stopRequestedFlag.store(true, std::memory_order_release);
}

DWORD KThread::waitUntilThreadFinish(bool pumpMessages)
{
	if (!pumpMessages)
		return ::WaitForSingleObject(handle, INFINITE);
	
	while (true)
	{
		MSG msg;
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		// if this thread sent msgs to caller thread before calling following function and after the above while block, those msgs will not be processed until new msg arrived. 
		// if there is no new msgs, there will be a deadlock! that's why we have a timeout! after the timeout, any pending msgs will be processed and continue...
		DWORD dwRet = ::MsgWaitForMultipleObjects(1, &handle, FALSE, 200, QS_ALLINPUT);

		if (dwRet == WAIT_OBJECT_0) // thread finished
		{
			return true;
		}
		else if ((dwRet == (WAIT_OBJECT_0 + 1)) || (dwRet == WAIT_TIMEOUT)) // window message or timeout
		{
			continue;
		}
		else // failure
		{
			break;
		}
	}

	return false;
}

bool KThread::start()
{
	if (isRunning())
		return false;

	if (handle) // close old stopped handle
	{
		::CloseHandle(handle);
		handle = 0;
	}

	stopRequestedFlag.store(false, std::memory_order_release);
	return createRFCThread();
}

void KThread::uSleep(int waitTime)
{
	__int64 time1 = 0, time2 = 0, freq = 0;

	::QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	::QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		::QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < ((waitTime * freq) / 1000000));
}

KThread::~KThread()
{
	if (isRunning())
	{
		#ifdef _DEBUG
		MessageBoxW(0, L"Thread object was destroyed while the associated thread was still running!", L"Error", MB_ICONERROR);
		#endif
		shouldStop();
		::WaitForSingleObject(handle, 1000); // wait for one second
	}

	// close old stopped handle
	if (handle)
		::CloseHandle(handle);
}
