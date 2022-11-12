
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

#include "KButton.h"
#include "KButtonListener.h"
#include "KGUIProc.h"

KButton::KButton() : KComponent(false)
{
	listener = 0;

	compClassName.AssignStaticText(TXT_WITH_LEN("BUTTON"));
	compText.AssignStaticText(TXT_WITH_LEN("Button"));

	compWidth = 100;
	compHeight = 30;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KButton::SetListener(KButtonListener *listener)
{
	this->listener = listener;
}

KButtonListener* KButton::GetListener()
{
	return listener;
}

void KButton::OnPress()
{
	if(listener)
		listener->OnButtonPress(this);
}

bool KButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
	{
		this->OnPress();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KButton::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

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

KButton::~KButton()
{
}