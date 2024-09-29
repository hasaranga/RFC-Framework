
/*
	Copyright (C) 2013-2022 CrownSoft
  
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

unsigned __stdcall RFCThread_Proc(void* lpParameter)
{
	if (lpParameter == nullptr) // for safe!
		return 0;

	KThread* thread = (KThread*)lpParameter;
	thread->Run();

	return 0;
}

bool CreateRFCThread(KThread* thread)
{
	if (thread)
	{
		// create thread in suspended state. so we can set the handle field.
		HANDLE handle = (HANDLE)::_beginthreadex(NULL, 0, RFCThread_Proc, thread, CREATE_SUSPENDED, NULL);
		//HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);

		if (handle)
		{
			thread->SetHandle(handle);
			::ResumeThread(handle);

			return true;
		}
	}
	return false;
}

KThread::KThread()
{
	handle = 0; 
	runnable = nullptr;
	threadShouldStop = false;
}

void KThread::SetHandle(HANDLE handle)
{
	this->handle = handle;
}

void KThread::SetRunnable(KRunnable* runnable)
{
	this->runnable = runnable;
}

HANDLE KThread::GetHandle()
{
	return handle;
}

KThread::operator HANDLE()const
{
	return handle;
}

bool KThread::ShouldRun()
{
	return !threadShouldStop;
}

void KThread::Run()
{
	if (runnable)
		runnable->Run(this);
}

bool KThread::IsThreadRunning()
{
	if (handle)
	{
		const DWORD result = ::WaitForSingleObject(handle, 0);
		return (result != WAIT_OBJECT_0);
	}

	return false;
}

void KThread::ThreadShouldStop()
{
	threadShouldStop = true;
}

DWORD KThread::WaitUntilThreadFinish(bool pumpMessages)
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

bool KThread::StartThread()
{
	threadShouldStop = false;

	if (handle) // close old handle
	{
		::CloseHandle(handle);
		handle = 0;
	}

	return ::CreateRFCThread(this);
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
	if (handle)
		::CloseHandle(handle);
}
