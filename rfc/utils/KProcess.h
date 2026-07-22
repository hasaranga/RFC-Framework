
/*
    Copyright (C) 2013-2026 CrownSoft
  
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
#include <psapi.h>
#include <shellapi.h>

class KProcess
{
public:
	// bufferCharCount should be MAX_PATH
	// buffer is valid only if return value is true
	static bool getProcessPathByID(DWORD processId, LPWSTR buffer, DWORD bufferCharCount) noexcept
	{
		if (buffer == NULL || bufferCharCount == 0)
			return false;

		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
		if (hProcess == NULL)
			return false;

		if (::QueryFullProcessImageNameW(hProcess, 0, buffer, &bufferCharCount))
		{
			::CloseHandle(hProcess);
			return true;
		}

		::CloseHandle(hProcess);
		return false;
	}

	static bool runAsAdmin(HWND hwnd, const wchar_t* applicationPath, const wchar_t* cmdArgs) noexcept
	{
		SHELLEXECUTEINFOW ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
		ShExecInfo.fMask = SEE_MASK_DEFAULT;
		ShExecInfo.hwnd = hwnd;
		ShExecInfo.lpVerb = L"runas";
		ShExecInfo.lpFile = applicationPath;
		ShExecInfo.lpParameters = cmdArgs;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL;
		return ::ShellExecuteExW(&ShExecInfo) != FALSE;
	}
};
