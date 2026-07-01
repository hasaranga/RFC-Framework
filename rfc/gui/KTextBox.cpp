
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

#include "KTextBox.h"
#include "KGUIProc.h"

KTextBox::KTextBox(bool readOnly) noexcept : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("EDIT"));

	compLWidth = 100;
	compLHeight = 20;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL;

	if(readOnly)
		compDwStyle = compDwStyle | ES_READONLY;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

void KTextBox::clear() noexcept
{
	this->compText = KString();
	if (compHWND)
		::SetWindowTextW(compHWND, L"");
}

KString KTextBox::getText() noexcept
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
			compText = KString(text, KStringBehaviour::FREE_ON_DESTROY);
		}else
		{
			compText = KString();
		}
	}
	return compText;
}

KTextBox::~KTextBox() noexcept {}