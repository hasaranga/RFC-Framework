
/*
    RFC - KProgressBar.cpp
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

#include "KProgressBar.h"
#include "../rfc.h"

KProgressBar::KProgressBar(bool smooth, bool vertical)
{
	value = 0;

	compClassName = KString(PROGRESS_CLASSW, KString::STATIC_TEXT_DO_NOT_FREE);

	this->SetPosition(0, 0);
	this->SetSize(100, 20);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS);
	this->SetExStyle(WS_EX_WINDOWEDGE);

	if(smooth)
		this->SetStyle(compDwStyle | PBS_SMOOTH);

	if(vertical)
		this->SetStyle(compDwStyle | PBS_VERTICAL);
}

int KProgressBar::GetValue()
{
	return value;
}

void KProgressBar::SetValue(int value)
{
	this->value=value;

	if(compHWND)
		::SendMessageW(compHWND, PBM_SETPOS, value, 0);
}

bool KProgressBar::CreateComponent()
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this); // we dont need to register PROGRESS_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(compHWND, PBM_SETPOS, value, 0); // set current value!

		::EnableWindow(compHWND, compEnabled);

		if(this->IsVisible())
			this->SetVisible(true);
		return true;
	}
	return false;	
}

KProgressBar::~KProgressBar()
{
}
