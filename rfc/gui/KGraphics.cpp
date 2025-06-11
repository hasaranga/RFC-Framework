
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

#include "KGraphics.h"

KGraphics::KGraphics(){}

KGraphics::~KGraphics(){}

void KGraphics::draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	KGraphics::draw3dRect(hdc, startX, startY, 2, 
		height, ::GetSysColor(COLOR_BTNSHADOW), 
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	KGraphics::draw3dRect(hdc, startX, startY, width, 
		2, ::GetSysColor(COLOR_BTNSHADOW), 
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::draw3dRect(hdc, lpRect->left, lpRect->top, 
		lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 
		clrTopLeft, clrBottomRight);
}

void KGraphics::draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::fillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	KGraphics::fillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	KGraphics::fillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	KGraphics::fillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void KGraphics::fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	KGraphics::fillSolidRect(hdc, &rect, color);
}

void KGraphics::fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	const COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

RECT KGraphics::calculateTextSize(const wchar_t* text, HFONT hFont)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = {0, 0, 0, 0};

	::DrawTextW(hDC, text, ::lstrlenW(text), &sz, DT_CALCRECT | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz;
}

int KGraphics::calculateTextHeight(const wchar_t* text, HFONT hFont, int width)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = { 0, 0, width, 0 };

	::DrawTextW(hDC, text, ::lstrlenW(text), &sz, 
		DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT);

	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz.bottom;
}

// https://devblogs.microsoft.com/oldnewthing/20210915-00/?p=105687
void KGraphics::makeBitmapOpaque(HDC hdc, int x, int y, int cx, int cy)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD bitmapBits = { 0x00, 0x00, 0x00, 0xFF };

	::StretchDIBits(hdc, x, y, cx, cy,
		0, 0, 1, 1, &bitmapBits, &bi,
		DIB_RGB_COLORS, SRCPAINT);
}

void KGraphics::setBitmapAlphaChannel(HDC hdc, int x, int y, int cx, int cy, BYTE alpha)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	if (alpha != 255) {
		RGBQUAD zeroAlpha = { 0xFF, 0xFF, 0xFF, 0x00 };
		::StretchDIBits(hdc, x, y, cx, cy,
			0, 0, 1, 1, &zeroAlpha, &bi,
			DIB_RGB_COLORS, SRCAND);
	}

	RGBQUAD alphaOnly = { 0x00, 0x00, 0x00, alpha };
	::StretchDIBits(hdc, x, y, cx, cy,
		0, 0, 1, 1, &alphaOnly, &bi,
		DIB_RGB_COLORS, SRCPAINT);
}