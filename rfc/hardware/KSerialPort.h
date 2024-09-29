
/*
	Copyright (C) 2013-2024 CrownSoft

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

#include <windows.h>

class KSerialPort
{
protected:
	HANDLE hPort;

public:
	KSerialPort()
	{
		hPort = NULL;
	}

	virtual ~KSerialPort() {}

	// portText can be COM2 etc...
	// set waitForInitialize to true if connect to arduino board
	bool Open(const char* portText, bool waitForInitialize = false)
	{
		DCB dcb;
		hPort = ::CreateFileA(portText, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (!::GetCommState(hPort, &dcb))
		{
			hPort = NULL;
			return false;
		}

		dcb.BaudRate = CBR_115200;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		if (!::SetCommState(hPort, &dcb)) // this will restart & re-initialize the arduino
		{
			::CloseHandle(hPort);
			hPort = NULL;
			return false;
		}

		if (waitForInitialize)
			::Sleep(1000); // give some time to restart the arduino

		return true;
	}

	bool Write(const char* data, int len)
	{
		DWORD bytesWritten = 0;
		::WriteFile(hPort, data, len, &bytesWritten, NULL);
		return (bytesWritten == len);
	}

	// reading will not wait.
	// call this after open.
	void SetReadTimeOuts()
	{
		COMMTIMEOUTS cto;
		::GetCommTimeouts(hPort, &cto);

		cto.ReadIntervalTimeout = 1;
		cto.ReadTotalTimeoutConstant = 1;
		cto.ReadTotalTimeoutMultiplier = 1;

		::SetCommTimeouts(hPort, &cto);
	}

	// reads a single byte
	bool Read(char* data)
	{
		DWORD bytesRead = 0;
		::ReadFile(hPort, data, 1, &bytesRead, NULL);
		return (bytesRead == 1);
	}

	void Close()
	{
		if (hPort)
			::CloseHandle(hPort);
	}
};
