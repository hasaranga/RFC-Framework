
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

#include "KLabel.h"
#include "KGUIProc.h"
#include "KGraphics.h"

KLabel::KLabel() noexcept : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("STATIC"));
	compText.assignStaticText(TXT_WITH_LEN("Label"));

	compLWidth = 100;
	compLHeight = 25;

	autoResize = false;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KLabel::resizeToTextSize() noexcept
{
	if (compText.isNotEmpty())
	{
		const int dpi = compFontRef.getCurrentDPI();
		RECT rect = KGraphics::calculateTextSize(compText, compFontRef.getFontHandle());
		// AUTOSIZE_EXTRA_GAP is already a logical value.
		setSize(KDPIUtility::toLogical(rect.right, dpi) + AUTOSIZE_EXTRA_GAP,
			KDPIUtility::toLogical(rect.bottom, dpi));
	}
	else // text is empty
	{
		setSize(20, 25);
	}

	repaint(); // to fix bug when text contain one space character and os repaint only resized area.
}

void KLabel::enableAutoResize(bool enable) noexcept
{
	autoResize = enable;

	if(autoResize)
		resizeToTextSize();
}

void KLabel::setText(const KString& compText) noexcept
{
	KComponent::setText(compText);

	if (autoResize)
		resizeToTextSize();
}

void KLabel::setFontType(const KFontType& fontType) noexcept
{
	__super::setFontType(fontType);

	if (autoResize)
		resizeToTextSize();
}

KLabel::~KLabel() noexcept {}