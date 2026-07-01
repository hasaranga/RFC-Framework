
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

#pragma comment(lib, "Msimg32.lib") // AlphaBlend

/**
	Can be use to create 32bit bitmap image from data.
	all values are physical values.
*/
class KTransparentBitmap
{
protected:
	Physical width;
	Physical height;
	HDC hdcMem;
	HBITMAP hbm;
	HBITMAP hbmPrev;
	void* pvBits;

	void releaseResources() noexcept;
	void createEmptyBitmap(Physical width, Physical height) noexcept;

public:
	// data must be in 0xaarrggbb format with premultiplied alpha.
	// stride must be equal to width * 4.
	// data will be copied to internal buffer.
	KTransparentBitmap(void* data, Physical width, Physical height, int stride) noexcept;

	// creates a transparent empty image
	KTransparentBitmap(Physical width, Physical height) noexcept;

	// color format: 0xaarrggbb
	unsigned int getPixel(Physical x, Physical y) noexcept;

	bool hitTest(Physical x, Physical y) noexcept;

	Physical getWidth() noexcept;

	Physical getHeight() noexcept;

	// also clears the content
	void resize(Physical width, Physical height) noexcept;

	// use AlphaBlend to draw
	// standard gdi drawing commands may not work with the returned hdc. (content has premultiplied alpha)
	// copy to secondary hdc using AlphaBlend or use gdi+ with PixelFormat32bppPARGB
	HDC getDC() noexcept;

	void draw(HDC destHdc, Physical destX, Physical destY, BYTE alpha = 255) noexcept;

	void draw(HDC destHdc, Physical destX, Physical destY, Physical destWidth, Physical destHeight, BYTE alpha = 255) noexcept;

	// can copy/scale specific part of the image
	void draw(HDC destHdc, Physical destX, Physical destY, Physical destWidth, Physical destHeight,
		Physical srcX, Physical srcY, Physical srcWidth, Physical srcHeight, BYTE alpha = 255) noexcept;

	virtual ~KTransparentBitmap() noexcept;

private:
	RFC_LEAK_DETECTOR(KTransparentBitmap)
};



