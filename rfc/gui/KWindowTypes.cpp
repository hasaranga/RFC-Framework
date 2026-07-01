
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

#include "KWindowTypes.h"


KHotPluggedDialog::KHotPluggedDialog() noexcept {}


void KHotPluggedDialog::onClose() noexcept
{
	::EndDialog(compHWND, 0);
}

void KHotPluggedDialog::onDestroy() noexcept {}

KHotPluggedDialog::~KHotPluggedDialog() noexcept {}


KOverlappedWindow::KOverlappedWindow() noexcept
{
	compText.assignStaticText(TXT_WITH_LEN("KOverlapped Window"));
	compDwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
}

KOverlappedWindow::~KOverlappedWindow() noexcept {}


KFrame::KFrame() noexcept
{
	compText.assignStaticText(TXT_WITH_LEN("KFrame"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
}

KFrame::~KFrame() noexcept {}



KDialog::KDialog() noexcept
{
	compText.assignStaticText(TXT_WITH_LEN("KDialog"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN;
	compDwExStyle = WS_EX_DLGMODALFRAME;
}

KDialog::~KDialog() noexcept {}



KToolWindow::KToolWindow() noexcept
{
	compText.assignStaticText(TXT_WITH_LEN("KTool Window"));
	compDwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN;
	compDwExStyle = WS_EX_TOOLWINDOW;
}

KToolWindow::~KToolWindow() noexcept {}

