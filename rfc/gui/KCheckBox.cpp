
/*
    RFC - KCheckBox.cpp
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

#include "KCheckBox.h"
#include "../rfc.h"

KCheckBox::KCheckBox()
{
	checked = false;

	this->SetText(STATIC_TXT("CheckBox"));
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_NOTIFY | WS_TABSTOP);
}

bool KCheckBox::CreateComponent()
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this); // we dont need to register BUTTON class!

	if(compHWND)
	{
		if (compFont != KFont::GetDefaultFont())
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);

		::EnableWindow(compHWND, compEnabled);

		if(this->IsVisible())
			this->SetVisible(true);
		return true;
	}
	return false;
}

void KCheckBox::OnPress()
{
	if(::SendMessageW(compHWND, BM_GETCHECK, 0, 0) == BST_CHECKED)
		checked = true;
	else
		checked = false;

	if(listener)
		listener->OnButtonPress(this);
}

bool KCheckBox::IsChecked()
{
	return checked;
}

void KCheckBox::SetCheckedState(bool state)
{
	checked = state;

	if(compHWND)
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
}

KCheckBox::~KCheckBox()
{
}