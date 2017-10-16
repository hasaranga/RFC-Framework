
/*
	RFC - KTrackBar.cpp
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

#include "../rfc.h"
#include "KTrackBar.h"


KTrackBar::KTrackBar(bool showTicks, bool vertical)
{
	listener = 0;
	rangeMin = 0;
	rangeMax = 100;
	value = 0;

	this->SetSize(100, 25);
	this->SetPosition(0, 0);
	this->SetStyle(WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS);
	this->SetExStyle(WS_EX_WINDOWEDGE);

	this->SetStyle(compDwStyle | (showTicks ? TBS_AUTOTICKS : TBS_NOTICKS));
	this->SetStyle(compDwStyle | (vertical ? TBS_VERT : TBS_HORZ));

	compClassName = KString(TRACKBAR_CLASSW, KString::STATIC_TEXT_DO_NOT_FREE);
}

void KTrackBar::SetRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));	
}

void KTrackBar::SetValue(int value)
{
	this->value = value;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);
}

void KTrackBar::OnChange()
{
	value = (int)::SendMessageW(compHWND, TBM_GETPOS, 0, 0);
	if(listener)
		listener->OnTrackBarChange(this);
}

void KTrackBar::SetListener(KTrackBarListener *listener)
{
	this->listener = listener;
}

int KTrackBar::GetValue()
{
	return value;
}

bool KTrackBar::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if( (msg == WM_HSCROLL) || (msg == WM_VSCROLL) )
	{
		int nScrollCode = (int)LOWORD(wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || (TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || (TB_PAGEDOWN == nScrollCode) ) // its trackbar!
		{
			this->OnChange();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KTrackBar::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register TRACKBAR_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(rangeMin, rangeMax));	
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

KTrackBar::~KTrackBar()
{
}

