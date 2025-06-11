
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

#define KIPC_USER_MESSAGE 1000
#define KIPC_CLOSE_CLIENT_MESSAGE 10

#define KIPC_SHARED_MEM_POSTFIX			L"_shared_mem"
#define KIPC_SVR_DATA_READY_POSTFIX		L"_svr_data_ready"
#define KIPC_CLNT_DATA_READY_POSTFIX	L"_clnt_data_ready"
#define KIPC_CLNT_STARTED_POSTFIX		L"_clnt_started"
#define KIPC_SVR_LIVE_POSTFIX			L"svr_live"
#define KIPC_CLNT_LIVE_POSTFIX			L"clnt_live"

#define KIPC_CLOSE_CLIENT_TIMEOUT 500

// fast inter-process method calling mechanism.
// do not return dataBuffer as output from proxy class. instead, make a copy of required memory then return it.
// because dataBuffer will be modified on next call.
// when passing bufer or string, first send its size first. so the receiver can easily process the data.
class KIPCServer
{
protected:
	unsigned int dataPosition;
	unsigned char* dataBuffer;

	HANDLE hMapFile;
	HANDLE serverDataReadyEvent;
	HANDLE clientDataReadyEvent;
	HANDLE clientStartedEvent;
	HANDLE serverLiveMutex;
	HANDLE clientLiveMutex;

	CRITICAL_SECTION csGuardMapFile;

	void resetFields()
	{
		hMapFile = NULL;
		serverDataReadyEvent = NULL;
		clientDataReadyEvent = NULL;
		clientStartedEvent = NULL;
		serverLiveMutex = NULL;
		clientLiveMutex = NULL;
		dataBuffer = nullptr;
		dataPosition = 0;
	}

	void closeClient(int timeout)
	{
		this->acquireLock();

		int messageID = KIPC_CLOSE_CLIENT_MESSAGE;
		this->resetDataPosition();
		this->addParam(&messageID, sizeof(int));
		this->dispatchCall(timeout);

		this->releaseLock();
	}

public:
	KIPCServer()
	{

	}

	bool init(const wchar_t* bridgeID, unsigned int sharedMemSize)
	{
		this->resetFields();
		::InitializeCriticalSection(&csGuardMapFile);

		wchar_t nameBuffer[128];
		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_SHARED_MEM_POSTFIX);

		hMapFile = ::CreateFileMappingW(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			sharedMemSize,         // maximum object size (low-order DWORD)
			nameBuffer);                 // name of mapping object

		if (hMapFile == NULL)
			return false;

		dataBuffer = (unsigned char*)::MapViewOfFile(hMapFile,   // handle to map object
			FILE_MAP_ALL_ACCESS, // read/write permission
			0,
			0,
			sharedMemSize);

		if (dataBuffer == nullptr)
		{
			::CloseHandle(hMapFile);
			hMapFile = NULL;
			return false;
		}

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_SVR_DATA_READY_POSTFIX);
		serverDataReadyEvent = ::CreateEventW(NULL, FALSE, FALSE, nameBuffer);

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_CLNT_DATA_READY_POSTFIX);
		clientDataReadyEvent = ::CreateEventW(NULL, FALSE, FALSE, nameBuffer);

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_CLNT_STARTED_POSTFIX);
		clientStartedEvent = ::CreateEventW(NULL, FALSE, FALSE, nameBuffer);

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_SVR_LIVE_POSTFIX);
		serverLiveMutex = ::CreateMutexW(NULL, TRUE, nameBuffer);

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_CLNT_LIVE_POSTFIX);
		clientLiveMutex = ::CreateMutexW(NULL, FALSE, nameBuffer);
		
		return true;
	}

	// only disconnect if Init returns true
	void disconnect(bool closeClient = true, int timeout = KIPC_CLOSE_CLIENT_TIMEOUT)
	{
		if (closeClient)
			this->closeClient(timeout);

		::ReleaseMutex(serverLiveMutex);
		::CloseHandle(serverLiveMutex);
		::CloseHandle(clientLiveMutex);
		::CloseHandle(serverDataReadyEvent);
		::CloseHandle(clientDataReadyEvent);
		::CloseHandle(clientStartedEvent);
		::UnmapViewOfFile((LPCVOID)dataBuffer);
		::CloseHandle(hMapFile);

		::DeleteCriticalSection(&csGuardMapFile);
	}

	bool waitForClientAvailable(DWORD timeout = INFINITE)
	{
		return (::WaitForSingleObject(clientStartedEvent, timeout) == WAIT_OBJECT_0);
	}

	bool isClientAlive()
	{
		DWORD retVal = ::WaitForSingleObject(clientLiveMutex, 0);
		if ((retVal == WAIT_OBJECT_0) || (retVal == WAIT_ABANDONED) || (retVal == WAIT_FAILED)) // client crashed or closed.
		{
			::ReleaseMutex(clientLiveMutex);
			return false;
		}

		return true;
	}

	void acquireLock()
	{
		::EnterCriticalSection(&csGuardMapFile);
	}

	void releaseLock()
	{
		::LeaveCriticalSection(&csGuardMapFile);
	}

	// returns dataPosition of the param
	unsigned int addParam(void* buffer, unsigned int size)
	{
		unsigned int lastPos = dataPosition;

		::memcpy(&dataBuffer[dataPosition], buffer, size);
		dataPosition += size;

		return lastPos;
	}

	// returns false if failed to respond within the given time.
	bool dispatchCall(DWORD timeout, bool processMessages = false, bool ignoreMessageValid = false, UINT ignoreMessage = 0)
	{
		::ResetEvent(clientDataReadyEvent);
		::SetEvent(serverDataReadyEvent);

		if(!processMessages)
			return (::WaitForSingleObject(clientDataReadyEvent, timeout) == WAIT_OBJECT_0);

		while (true)
		{
			MSG msg;
			while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if ((ignoreMessageValid) && (msg.message == ignoreMessage))
					continue;

				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}

			if (!this->isClientAlive())
				break;

			// if this thread sent msgs to caller thread before calling following function and after the above while block, those msgs will not be processed until new msg arrived. 
			// if there is no new msgs, there will be a deadlock! that's why we have a timeout! after the timeout, any pending msgs will be processed and continue...
			DWORD dwRet = ::MsgWaitForMultipleObjects(1, &clientDataReadyEvent, FALSE, 10, QS_ALLINPUT);

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

	void resetDataPosition()
	{
		dataPosition = 0;
	}

	unsigned int getCurrentDataPosition()
	{
		return dataPosition;
	}

	unsigned char* getDataBuffer()
	{
		return dataBuffer;
	}

	virtual ~KIPCServer()
	{

	}
};

class KIPCClient
{
protected:
	unsigned int dataPosition;
	unsigned char* dataBuffer;

	HANDLE hMapFile;
	HANDLE serverDataReadyEvent;
	HANDLE clientDataReadyEvent;
	HANDLE clientStartedEvent;
	HANDLE serverLiveMutex;
	HANDLE clientLiveMutex;

	void resetFields()
	{
		hMapFile = NULL;
		serverDataReadyEvent = NULL;
		clientDataReadyEvent = NULL;
		clientStartedEvent = NULL;
		serverLiveMutex = NULL;
		clientLiveMutex = NULL;
		dataBuffer = nullptr;
		dataPosition = 0;
	}

public:
	KIPCClient()
	{

	}

	bool Init(const wchar_t* bridgeID, unsigned int sharedMemSize)
	{
		this->resetFields();

		wchar_t nameBuffer[128];
		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_SHARED_MEM_POSTFIX);

		hMapFile = ::OpenFileMappingW(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			nameBuffer);               // name of mapping object

		if (hMapFile == NULL)
			return false;

		dataBuffer = (unsigned char*)::MapViewOfFile(hMapFile, // handle to map object
			FILE_MAP_ALL_ACCESS,  // read/write permission
			0,
			0,
			sharedMemSize);

		if (dataBuffer == NULL)
		{
			this->disconnect();
			return false;
		}

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_SVR_DATA_READY_POSTFIX);
		serverDataReadyEvent = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, nameBuffer);
		if (serverDataReadyEvent == NULL)
		{
			this->disconnect();
			return false;
		}

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_CLNT_DATA_READY_POSTFIX);
		clientDataReadyEvent = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, nameBuffer);
		if (clientDataReadyEvent == NULL)
		{
			this->disconnect();
			return false;
		}

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_CLNT_STARTED_POSTFIX);
		clientStartedEvent = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, nameBuffer);
		if (clientStartedEvent == NULL)
		{
			this->disconnect();
			return false;
		}

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_SVR_LIVE_POSTFIX);
		serverLiveMutex = ::OpenMutexW(SYNCHRONIZE, FALSE, nameBuffer);
		if (serverLiveMutex == NULL)
		{
			this->disconnect();
			return false;
		}

		::wcscpy_s(nameBuffer, 128, bridgeID);
		::wcscat_s(nameBuffer, 128, KIPC_CLNT_LIVE_POSTFIX);
		clientLiveMutex = ::OpenMutexW(SYNCHRONIZE, FALSE, nameBuffer);
		if (clientLiveMutex == NULL)
		{
			this->disconnect();
			return false;
		}

		::WaitForSingleObject(clientLiveMutex, 0); // own the mutex

		return true;
	}

	bool isServerAlive()
	{
		DWORD retVal = ::WaitForSingleObject(serverLiveMutex, 0);
		if ((retVal == WAIT_OBJECT_0) || (retVal == WAIT_ABANDONED) || (retVal == WAIT_FAILED)) // server crashed or closed without informing the client.
		{
			::ReleaseMutex(serverLiveMutex);
			return false;
		}

		return true;
	}

	void informClientStarted()
	{
		::SetEvent(clientStartedEvent);
	}

	// do not disconnect if Init returns false
	void disconnect()
	{
		if (clientLiveMutex)
		{
			::ReleaseMutex(clientLiveMutex);
			::CloseHandle(clientLiveMutex);
		}

		if(serverLiveMutex)
			::CloseHandle(serverLiveMutex);

		if (serverDataReadyEvent)
			::CloseHandle(serverDataReadyEvent);

		if (clientDataReadyEvent)
			::CloseHandle(clientDataReadyEvent);

		if (clientStartedEvent)
			::CloseHandle(clientStartedEvent);

		if(dataBuffer)
			::UnmapViewOfFile((LPCVOID)dataBuffer);

		if (hMapFile)
			::CloseHandle(hMapFile);
	}

	// returns false if timeout or error occured.
	bool waitForMessages(DWORD timeout = INFINITE)
	{
		return (::WaitForSingleObject(serverDataReadyEvent, timeout) == WAIT_OBJECT_0);
	}

	unsigned char* getDataBuffer()
	{
		return dataBuffer;
	}

	bool isQuitMessageReceived()
	{
		int messageID = *((int*)dataBuffer);
		return (messageID == KIPC_CLOSE_CLIENT_MESSAGE);
	}

	void informClientProcessedMessage()
	{
		::SetEvent(clientDataReadyEvent);
	}

	virtual ~KIPCClient()
	{

	}
};
