
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

#include "KMenuButton.h"
#include "KGraphics.h"
#include <commctrl.h>

KMenuButton::KMenuButton() noexcept : 
	arrowFontRef(KFontType(L"Webdings", 18, false, false, false, false), USER_DEFAULT_SCREEN_DPI)
{
	buttonMenu = nullptr;
	glyphChar = nullptr;
	glyphLeft = 6;
}	

KMenuButton::~KMenuButton() noexcept {}

void KMenuButton::setMenu(KMenu* buttonMenu) noexcept
{
	this->buttonMenu = buttonMenu;
}

void KMenuButton::setGlyph(const wchar_t* glyphChar, 
	const KFontType& glyphFontType, COLORREF glyphColor, int glyphLeft) noexcept
{
	this->glyphChar = glyphChar;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	glyphFontRef.update(glyphFontType, compFontRef.getCurrentDPI());
	this->repaint();
}

void KMenuButton::setDPI(int newDPI) noexcept
{
	glyphFontRef.update(newDPI);
	arrowFontRef.update(newDPI);

	__super::setDPI(newDPI);
}

void KMenuButton::_onPress() noexcept
{
	if (buttonMenu)
	{
		const int dpi = KDPIUtility::getWindowDPI(compHWND);
		POINT point = { KDPIUtility::toPhysical(getX(),dpi), KDPIUtility::toPhysical(getY(),dpi)};
		::ClientToScreen(compParentHWND, &point); // get screen cordinates

		::TrackPopupMenu(buttonMenu->getMenuHandle(), 
			TPM_LEFTBUTTON, point.x, point.y + KDPIUtility::toPhysical(getHeight(), dpi),
			0, compParentHWND, NULL);
	}
}

bool KMenuButton::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) noexcept
{
	if (msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW) lParam;

			*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				*result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				const int dpi = KDPIUtility::getWindowDPI(compHWND);

				const RECT rc = lpNMCD->rc;
				KGraphics::draw3dVLine(lpNMCD->hdc, rc.right - ::MulDiv(22, dpi, USER_DEFAULT_SCREEN_DPI),
					rc.top + ::MulDiv(6, dpi, USER_DEFAULT_SCREEN_DPI),
					rc.bottom - ::MulDiv(12, dpi, USER_DEFAULT_SCREEN_DPI)); // draw line

				const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFontRef.getFontHandle());
				const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - ::MulDiv(18, dpi, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphChar) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFontRef.getFontHandle());
					::SetTextColor(lpNMCD->hdc, bDisabled ? GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + ::MulDiv(glyphLeft, dpi, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				*result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}

	return KButton::eventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}