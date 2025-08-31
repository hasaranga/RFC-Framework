
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
#include "plutosvg/plutosvg.h"

// uses modified plutosvg. (removed font support, image element support & image saving)
class KSVGImage
{
protected:
	plutosvg::plutosvg_document_t* document = nullptr;
	plutosvg::plutovg_surface_t* surface = nullptr;

public:
	KSVGImage() {}

	bool loadFromFile(const char* filePath)
	{
		document = plutosvg::plutosvg_document_load_from_file(filePath, -1, -1);
		return document != NULL;
	}

	bool loadFromData(const char* data, int length)
	{
		document = plutosvg::plutosvg_document_load_from_data(data, length, -1, -1, 0, 0);
		return document != NULL;
	}

	// can call multiple times
	void rasterize(int width, int height)
	{
		if (document == NULL)
			return;

		if (surface)
		{
			plutosvg::plutovg_surface_destroy(surface);
			surface = nullptr;
		}

		surface = plutosvg::plutosvg_document_render_to_surface(document, nullptr, width, height, nullptr, nullptr, nullptr);
	}

	// can call multiple times
	void rasterize(int newDPI = USER_DEFAULT_SCREEN_DPI)
	{
		if (document == NULL)
			return;

		unsigned int width = (unsigned int)plutosvg::plutosvg_document_get_width(document);
		unsigned int height = (unsigned int)plutosvg::plutosvg_document_get_height(document);
		unsigned int newWidth = (unsigned int)MulDiv((uint32_t)width, newDPI, USER_DEFAULT_SCREEN_DPI);
		unsigned int newHeight = (unsigned int)MulDiv((uint32_t)height, newDPI, USER_DEFAULT_SCREEN_DPI);
		this->rasterize(newWidth, newHeight);
	}

	bool isImageRasterized()
	{
		return surface != NULL;
	}

	bool isDocumentLoaded()
	{
		return document != NULL;
	}

	// data format is premultiplied gdi 0xaarrggbb or gdiplus PixelFormat32bppPARGB or 
	// direct2d {DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED}
	void getImageData(int* width, int* height, unsigned char** data, int* stride)
	{
		if (surface == NULL)
			return;

		if(width)
			*width = surface->width;

		if(height)
			*height = surface->height;

		if(data)
			*data = surface->data;

		if(stride)
			*stride = surface->stride;
	}

	// free the generated pixel data. does not free the document.
	// you need to call rasterize method again to generate the image data.
	void freeRasterData()
	{
		if (surface)
		{
			plutosvg::plutovg_surface_destroy(surface);
			surface = nullptr;
		}
	}

	~KSVGImage()
	{
		if (surface)
			plutosvg::plutovg_surface_destroy(surface);

		if (document)
			plutosvg::plutosvg_document_destroy(document);
	}

private:
	RFC_LEAK_DETECTOR(KSVGImage)
};
