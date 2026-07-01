
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


#include "KTrackBar.h"
#include "KGUIProc.h"
#include <commctrl.h>

KTrackBar::KTrackBar(bool showTicks, bool vertical) noexcept : KComponent(false)
{
	rangeMin = 0;
	rangeMax = 100;
	value = 0;
	this->vertical = vertical;

	compLWidth = 100;
	compLHeight = 25;

	compDwStyle = (WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS) | 
		(showTicks ? TBS_AUTOTICKS : TBS_NOTICKS) | 
		(vertical ? TBS_VERT : TBS_HORZ);

	compDwExStyle = WS_EX_WINDOWEDGE;

	compClassName.assignStaticText(TXT_WITH_LEN("msctls_trackbar32"));
}

void KTrackBar::setRange(int min, int max) noexcept
{
	rangeMin = min;
	rangeMax = max;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));	
}

void KTrackBar::setValue(int value) noexcept
{
	this->value = value;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)(vertical ? (100 - value) : value));
}

void KTrackBar::_onChange() noexcept
{
	value = (int)::SendMessageW(compHWND, TBM_GETPOS, 0, 0);
	value = vertical ? (100 - value) : value;
	if(onChange)
		onChange(this, value);
}

int KTrackBar::getValue() noexcept
{
	return value;
}

bool KTrackBar::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) noexcept
{
	if( (msg == WM_HSCROLL) || (msg == WM_VSCROLL) )
	{
		const int nScrollCode = (int)LOWORD(wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || 
			(TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || 
			(TB_PAGEDOWN == nScrollCode) || (TB_THUMBPOSITION == nScrollCode)) // its trackbar!
		{
			this->_onChange();
			*result = 0;
			return true;
		}
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

void KTrackBar::afterCreated() noexcept
{
	::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(rangeMin, rangeMax));
	::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)(vertical ? (100 - value) : value));
	__super::afterCreated();
}

KTrackBar::~KTrackBar() noexcept {}

