
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

#include "KTransparentBitmap.h"

KTransparentBitmap::KTransparentBitmap(void* data, int width, int height, int stride)
{
	if ((width * sizeof(unsigned int)) != stride) // this should not happen! no padding in 32bpp data.
	{
		this->createEmptyBitmap(width, height);
		return;
	}

	this->width = width;
	this->height = height;

	hdcMem = ::CreateCompatibleDC(NULL);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = ::CreateDIBSection(hdcMem, &bmi,
		DIB_RGB_COLORS, &pvBits,
		NULL, 0);

	::memcpy(pvBits, data, height * width * sizeof(unsigned int));

	// the color format for each pixel is 0xaarrggbb  
	/*
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			((unsigned int*)pvBits)[x + y * width] = ((unsigned int*)data)[x + y * width];
	*/

	hbmPrev = (HBITMAP)::SelectObject(hdcMem, hbm);

}

KTransparentBitmap::KTransparentBitmap(int width, int height)
{
	this->createEmptyBitmap(width, height);
}

void KTransparentBitmap::createEmptyBitmap(int width, int height)
{
	this->width = width;
	this->height = height;

	hdcMem = ::CreateCompatibleDC(NULL);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = ::CreateDIBSection(hdcMem, &bmi,
		DIB_RGB_COLORS, &pvBits,
		NULL, 0);

	::ZeroMemory(pvBits, height * width * sizeof(unsigned int));

	hbmPrev = (HBITMAP)::SelectObject(hdcMem, hbm);
}

void KTransparentBitmap::releaseResources()
{
	if (hdcMem == 0)
		return;

	::SelectObject(hdcMem, hbmPrev);
	::DeleteObject(hbm);
	::DeleteDC(hdcMem);
}

bool KTransparentBitmap::hitTest(int x, int y)
{
	if (pvBits == nullptr)
		return false;

	if ((x < 0) || (y < 0) || (x > (width - 1)) || (y > (height - 1)))
		return false;

	unsigned int pixelColor = ((unsigned int*)pvBits)[x + y * width];
	return ((pixelColor >> 24) == 0xff);
}

unsigned int KTransparentBitmap::getPixel(int x, int y)
{
	if (pvBits == nullptr)
		return 0;

	if ((x < 0) || (y < 0) || (x > (width - 1)) || (y > (height - 1)))
		return 0;

	return ((unsigned int*)pvBits)[x + y * width];
}

int KTransparentBitmap::getWidth()
{
	return width;
}

int KTransparentBitmap::getHeight()
{
	return height;
}

void KTransparentBitmap::resize(int width, int height)
{
	this->releaseResources();
	this->createEmptyBitmap(width, height);
}

HDC KTransparentBitmap::getDC()
{
	return hdcMem;
}

void KTransparentBitmap::draw(HDC destHdc, int destX, int destY, BYTE alpha)
{
	this->draw(destHdc, destX, destY, width, height, 0, 0, width, height, alpha);
}

void KTransparentBitmap::draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha)
{
	this->draw(destHdc, destX, destY, destWidth, destHeight, 0, 0, width, height, alpha);
}

void KTransparentBitmap::draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha)
{
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
	::AlphaBlend(destHdc, destX, destY, destWidth, destHeight,
		hdcMem, srcX, srcY, srcWidth, srcHeight, bf);
}

KTransparentBitmap::~KTransparentBitmap()
{
	this->releaseResources();
}


