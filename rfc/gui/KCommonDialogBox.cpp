
/*
	RFC - KCommonDialogBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif 

#include "KCommonDialogBox.h"
#include "../utils/KRegistry.h"
#include "../io/KDirectory.h"

const wchar_t* const RFC_OSD_REG_LOCATION = L"Software\\CrownSoft\\RFC\\OSD";

bool KCommonDialogBox::ShowOpenFileDialog(KWindow *window, 
	const KString& title, 
	const wchar_t* filter, 
	KString *fileName, 
	bool saveLastLocation, 
	const wchar_t* dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!	
	wchar_t *buff = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	buff[0] = 0;
 
	if (saveLastLocation)
	{
		KString lastLocation;
		KRegistry::ReadString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.GetLength() > 0)
			::wcscpy(buff, (const wchar_t*)lastLocation);
	}

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetOpenFileNameW(&ofn))
	{
		KString path(buff, KString::FREE_TEXT_WHEN_DONE);
		*fileName = path;

		if (saveLastLocation)
		{
			KRegistry::CreateKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::WriteString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, path);
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

bool KCommonDialogBox::ShowSaveFileDialog(KWindow *window, 
	const KString& title, 
	const wchar_t* filter, 
	KString *fileName,
	bool saveLastLocation,
	const wchar_t* dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!
	wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	buff[0] = 0;

	if (saveLastLocation)
	{
		KString lastLocation;
		KRegistry::ReadString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.GetLength() > 0)
			::wcscpy(buff, (const wchar_t*)lastLocation);
	}

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetSaveFileNameW(&ofn))
	{
		KString path(buff, KString::FREE_TEXT_WHEN_DONE);
		*fileName = path;

		if (saveLastLocation)
		{
			KRegistry::CreateKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::WriteString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, path);
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}