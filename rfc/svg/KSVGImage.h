
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
#include "lunasvg/lunasvg.h"

class KSVGImage
{
protected:
	std::unique_ptr<lunasvg::Document> document;
	lunasvg::Bitmap lunaBitmap;

public:
	KSVGImage() {}

	bool loadFromFile(const std::string& path)
	{
		document = lunasvg::Document::loadFromFile(path);
		if (document == nullptr)
			return false;

		return true;
	}

	bool loadFromText(const std::string& svgText)
	{
		document = lunasvg::Document::loadFromData(svgText);
		if (document == nullptr)
			return false;

		return true;
	}

	void rasterize()
	{
		this->rasterize((unsigned int)document->width(), (unsigned int)document->height());
	}

	// can call multiple times
	void rasterize(unsigned int width, unsigned int height)
	{
		lunaBitmap = document->renderToBitmap(width, height);
	}

	// can call multiple times
	void rasterize(int newDPI)
	{
		const unsigned int newWidth = (unsigned int)MulDiv((uint32_t)document->width(), newDPI, USER_DEFAULT_SCREEN_DPI);
		const unsigned int newHeight = (unsigned int)MulDiv((uint32_t)document->height(), newDPI, USER_DEFAULT_SCREEN_DPI);
		this->rasterize(newWidth, newHeight);
	}

	// data format is premultiplied gdi 0xaarrggbb or gdiplus PixelFormat32bppPARGB or 
	// direct2d {DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED}
	void getImageData(int* width, int* height, BYTE** data, int* stride)
	{
		*width = (int)lunaBitmap.width();
		*height = (int)lunaBitmap.height();
		*data = lunaBitmap.data();
		*stride = (int)lunaBitmap.stride();
	}

	virtual ~KSVGImage()
	{

	}

private:
	RFC_LEAK_DETECTOR(KSVGImage)
};