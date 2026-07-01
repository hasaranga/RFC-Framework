
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

#include"KMenuBar.h"

KMenuBar::KMenuBar() noexcept
{
	hMenu = ::CreateMenu();
}

void KMenuBar::addMenu(const KString& text, KMenu* menu) noexcept
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, 
		(UINT_PTR)menu->getMenuHandle(), text);
}

void KMenuBar::addToWindow(KWindow* window) noexcept
{
	HWND hwnd = window->getHWND();
	K_ASSERT(hwnd != NULL, "window is not created");
	::SetMenu(hwnd, hMenu);
}

KMenuBar::~KMenuBar() noexcept
{
	::DestroyMenu(hMenu);
}