
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

#pragma once
#include "../core/CoreModule.h"

class KGraphics
{
public:
	KGraphics() noexcept;

	virtual ~KGraphics() noexcept;

	static void draw3dVLine(HDC hdc, Physical startX, Physical startY, Physical height) noexcept;

	static void draw3dHLine(HDC hdc, Physical startX, Physical startY, Physical width) noexcept;

	static void draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight) noexcept;

	static void draw3dRect(HDC hdc, Physical x, Physical y, Physical cx, Physical cy, COLORREF clrTopLeft, COLORREF clrBottomRight) noexcept;

	static void fillSolidRect(HDC hdc, Physical x, Physical y, Physical cx, Physical cy, COLORREF color) noexcept;

	static void fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color) noexcept;

	// destX and destY are the left top location to place the src image on destination.
	// rotation angle in degrees.
	// does not support alpha channel.
	static void drawHDCRotated(HDC srcHDC, Physical srcX, Physical srcY, Physical srcWidth, Physical srcHeight,
		HDC destHDC, Physical destX, Physical destY, float rotation) noexcept;

	// make sure to use matching HFONT for current dpi.
	// returns physical values.
	static RECT calculateTextSize(const wchar_t* text, HFONT hFont) noexcept;

	// make sure to use matching HFONT for current dpi. width is physical.
	// returns a physical value.
	static int calculateTextHeight(const wchar_t* text, HFONT hFont, int width) noexcept;

	// This function sets the alpha channel to 255 without affecting any of the color channels.
	// hdc is a memory DC with a 32bpp bitmap selected into it.
	// can be use to fix 32bit bitmap alpha which is destroyed by the gdi operations.
	static void makeBitmapOpaque(HDC hdc, Physical x, Physical y, Physical cx, Physical cy) noexcept;

	// hdc is a memory DC with a 32bpp bitmap selected into it.
	// This function sets the alpha channel without affecting any of the color channels.
	static void setBitmapAlphaChannel(HDC hdc, Physical x, Physical y, Physical cx, Physical cy, BYTE alpha) noexcept;

private:
	RFC_LEAK_DETECTOR(KGraphics)
};

