
/*
    RFC - KCommonDialogBox.cpp
    Copyright (C) 2013-2017 CrownSoft
  
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

#include"KCommonDialogBox.h"

bool KCommonDialogBox::ShowOpenFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName)
{
	// assumes MAX_PATH * 2 is enough!

	OPENFILENAMEW ofn;
	wchar_t *buff = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );

	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags =  OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	ofn.lpstrTitle = (const wchar_t*)title;

	if(::GetOpenFileNameW(&ofn))
	{
		*fileName=KString(buff);
		::free(buff);

		return true;
	}else
	{
		::free(buff);
		return false;
	}
}

bool KCommonDialogBox::ShowSaveFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName)
{
	// assumes MAX_PATH * 2 is enough!

	OPENFILENAMEW ofn;
	wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));

	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags =  OFN_HIDEREADONLY;
	ofn.lpstrTitle = (const wchar_t*)title;

	if(::GetSaveFileNameW(&ofn))
	{
		*fileName=KString(buff);
		::free(buff);

		return true;
	}else
	{
		::free(buff);
		return false;
	}
}