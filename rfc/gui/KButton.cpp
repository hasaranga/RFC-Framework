
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

#include "KButton.h"
#include "KGUIProc.h"

KButton::KButton() : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("BUTTON"));
	compText.assignStaticText(TXT_WITH_LEN("Button"));

	compWidth = 100;
	compHeight = 30;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KButton::_onPress()
{
	if(onClick)
		onClick(this);
}

bool KButton::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
	{
		this->_onPress();

		*result = 0;
		return true;
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KButton::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KButton::~KButton()
{
}