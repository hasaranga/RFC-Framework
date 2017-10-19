
/*
	RFC - KGraphics.cpp
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

#include "KGraphics.h"

KGraphics::KGraphics(){}

KGraphics::~KGraphics(){}

void KGraphics::Draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	KGraphics::Draw3dRect(hdc, startX, startY, 2, height, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	KGraphics::Draw3dRect(hdc, startX, startY, width, 2, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void KGraphics::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	KGraphics::FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void KGraphics::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	KGraphics::FillSolidRect(hdc, &rect, color);
}

void KGraphics::FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}