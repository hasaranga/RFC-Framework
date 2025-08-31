
/*
	Copyright (C) 2013-2025 CrownSoft
  
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

#include "KLabel.h"
#include "KGUIProc.h"
#include "KGraphics.h"

KLabel::KLabel() : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("STATIC"));
	compText.assignStaticText(TXT_WITH_LEN("Label"));

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	autoResize = false;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KLabel::resizeToTextSize()
{
	if (compText.isNotEmpty())
	{
		RECT rect = KGraphics::calculateTextSize(compText, compFont->getFontHandle());
		this->setSize(rect.right + AUTOSIZE_EXTRA_GAP, rect.bottom);
	}
	else // text is empty
	{
		this->setSize(20, 25);
	}

	this->repaint(); // to fix bug when text contain one space character and os repaint only resized area.
}

void KLabel::enableAutoResize(bool enable)
{
	autoResize = enable;

	if(autoResize)
		this->resizeToTextSize();
}

void KLabel::setText(const KString& compText)
{
	KComponent::setText(compText);

	if (autoResize)
		this->resizeToTextSize();
}

void KLabel::setFont(KFont* compFont)
{
	KComponent::setFont(compFont);

	if (autoResize)
		this->resizeToTextSize();
}

void KLabel::setDPI(int newDPI)
{
	if (newDPI == compDPI)
		return;

	int oldDPI = compDPI;
	compDPI = newDPI;

	this->compX = ::MulDiv(compX, newDPI, oldDPI);
	this->compY = ::MulDiv(compY, newDPI, oldDPI);

	if (!compFont->isDefaultFont())
		compFont->setDPI(newDPI);
	
	if (compText.isNotEmpty() && autoResize)
	{
		RECT rect = KGraphics::calculateTextSize(compText, compFont->getFontHandle());
		this->compWidth = rect.right + AUTOSIZE_EXTRA_GAP;
		this->compHeight = rect.bottom;
	}
	else
	{
		this->compWidth = ::MulDiv(compWidth, newDPI, oldDPI);
		this->compHeight = ::MulDiv(compHeight, newDPI, oldDPI);
	}

	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compX, compY, compWidth, 
			compHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);

		if ((!compFont->isDefaultFont()) && (compDwStyle & WS_CHILD))
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0));
	}
}

bool KLabel::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register Label class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KLabel::~KLabel()
{
}