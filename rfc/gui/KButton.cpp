
/*
    RFC - KButton.cpp
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

#include "KButton.h"
#include "../rfc.h"

KButton::KButton()
{
	listener = 0;

	compClassName = STATIC_TXT("BUTTON");

	this->SetText(STATIC_TXT("Button"));
	this->SetSize(100, 30);
	this->SetPosition(0, 0);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP);
	this->SetExStyle(WS_EX_WINDOWEDGE);
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

bool KButton::CreateComponent()
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this); // we dont need to register BUTTON class!

	if(compHWND)
	{
		if (compFont != KFont::GetDefaultFont())
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(this->IsVisible())
			this->SetVisible(true);
		return true;
	}
	return false;
}

KButton::~KButton()
{
}