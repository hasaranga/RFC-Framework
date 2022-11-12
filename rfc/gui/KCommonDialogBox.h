
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

#pragma once

#include "../core/CoreModule.h"
#include "KWindow.h"

// generates filter text for KFILE_FILTER("Text Files", "txt") as follows: L"Text Files\0*.txt\0"
#define KFILE_FILTER(desc, ext) L##desc L"\0*." L##ext L"\0"

class KCommonDialogBox
{
public:
	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool ShowOpenFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName, bool saveLastLocation = false, const wchar_t* dialogGuid = 0);

	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool ShowSaveFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName, bool saveLastLocation = false, const wchar_t* dialogGuid = 0);
};

