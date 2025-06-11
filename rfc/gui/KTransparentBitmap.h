
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

#pragma once

#include "../core/CoreModule.h"

#pragma comment(lib, "Msimg32.lib") // AlphaBlend

/**
	Can be use to create 32bit bitmap image from data.
*/
class KTransparentBitmap
{
protected:
	int width;
	int height;
	HDC hdcMem;
	HBITMAP hbm;
	HBITMAP hbmPrev;
	void* pvBits;

	void releaseResources();
	void createEmptyBitmap(int width, int height);

public:
	// data must be in 0xaarrggbb format with premultiplied alpha.
	// stride must be equal to width * 4
	KTransparentBitmap(void* data, int width, int height, int stride);

	// creates a transparent empty image
	KTransparentBitmap(int width, int height);

	// color format: 0xaarrggbb
	unsigned int getPixel(int x, int y);

	bool hitTest(int x, int y);

	int getWidth();

	int getHeight();

	// also clears the content
	void resize(int width, int height);

	// use AlphaBlend to draw
	// standard gdi drawing commands may not work with the returned hdc. (content has premultiplied alpha)
	// copy to secondary hdc using AlphaBlend or use gdi+ with PixelFormat32bppPARGB
	HDC getDC();

	void draw(HDC destHdc, int destX, int destY, BYTE alpha = 255);

	void draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha = 255);

	// can copy/scale specific part of the image
	void draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha = 255);

	virtual ~KTransparentBitmap();

private:
	RFC_LEAK_DETECTOR(KTransparentBitmap)
};



