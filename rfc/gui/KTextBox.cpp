
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

#include "KTextBox.h"
#include "KGUIProc.h"

KTextBox::KTextBox(bool readOnly) : KComponent(false)
{
	compClassName.AssignStaticText(TXT_WITH_LEN("EDIT"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL;

	if(readOnly)
		compDwStyle = compDwStyle | ES_READONLY;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

KString KTextBox::GetText()
{
	if(compHWND)
	{
		const int length = ::GetWindowTextLengthW(compHWND);
		if(length)
		{
			const int size = (length + 1) * sizeof(wchar_t);
			wchar_t *text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(compHWND, text, size);
			compText = KString(text, KString::FREE_TEXT_WHEN_DONE);
		}else
		{
			compText = KString();
		}
	}
	return compText;
}


bool KTextBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register EDIT class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KTextBox::~KTextBox()
{
}