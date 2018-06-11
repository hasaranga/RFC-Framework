
/*
	RFC - KToolTip.cpp
	Copyright (C) 2013-2018 CrownSoft
  
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

#include "../rfc.h"
#include "KToolTip.h"

KToolTip::KToolTip()
{
	attachedCompHWND = 0;
	compClassName = KString(TOOLTIPS_CLASSW, KString::STATIC_TEXT_DO_NOT_FREE);

	this->SetStyle(WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX);
}

KToolTip::~KToolTip()
{
}

void KToolTip::AttachToComponent(KWindow *parentWindow, KComponent *attachedComponent)
{
	compParentHWND = parentWindow->GetHWND();
	attachedCompHWND = attachedComponent->GetHWND();

	compHWND = ::CreateWindowExW(0, compClassName, NULL, compDwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, compParentHWND, NULL, KPlatformUtil::GetInstance()->GetAppHInstance(), 0);

	if (compHWND)
	{
		::SetWindowPos(compHWND, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		::AttachRFCPropertiesToHWND(compHWND, (KComponent*)this);

		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = compText;

		SendMessageW(compHWND, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}
}

bool KToolTip::CreateComponent(bool requireInitialMessages)
{
	return false;
}

void KToolTip::SetText(const KString& compText)
{
	this->compText = compText;
	if (compHWND)
	{
		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = compText;
		toolInfo.hinst = KPlatformUtil::GetInstance()->GetAppHInstance();

		SendMessageW(compHWND, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}

