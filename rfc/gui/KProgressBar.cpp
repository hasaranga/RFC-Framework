
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

#include "KProgressBar.h"
#include "KGUIProc.h"
#include <commctrl.h>

KProgressBar::KProgressBar(bool smooth, bool vertical) noexcept : KComponent(false)
{
	value = 0;

	compClassName.assignStaticText(TXT_WITH_LEN("msctls_progress32"));

	compLWidth = 100;
	compLHeight = 20;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS;
	compDwExStyle = WS_EX_WINDOWEDGE;

	if(smooth)
		compDwStyle = compDwStyle | PBS_SMOOTH;

	if(vertical)
		compDwStyle = compDwStyle | PBS_VERTICAL;
}

int KProgressBar::getValue() noexcept
{
	return value;
}

void KProgressBar::setValue(int value) noexcept
{
	this->value=value;

	if(compHWND)
		::SendMessageW(compHWND, PBM_SETPOS, value, 0);
}

void KProgressBar::afterCreated() noexcept
{
	::SendMessageW(compHWND, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
	::SendMessageW(compHWND, PBM_SETPOS, value, 0); // set current value!
	__super::afterCreated();
}

KProgressBar::~KProgressBar() noexcept {}
