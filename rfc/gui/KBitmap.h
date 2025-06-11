
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

/**
	Can be use to load bitmap image from file or resource.
*/
class KBitmap
{
protected:
	HBITMAP hBitmap;

public:
	KBitmap();

	/**
		Loads bitmap image from resource
		@param resourceID resource ID of image
		@returns false if image load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads bitmap image from file
		@param filePath path to image
		@returns false if image load fails
	*/
	bool loadFromFile(const KString& filePath);

	// does not scale
	void drawOnHDC(HDC hdc, int x, int y, int width, int height);

	/**
		Returns bitmap handle
	*/
	HBITMAP getHandle();

	/**
		Returns bitmap handle
	*/
	operator HBITMAP()const;

	virtual ~KBitmap();

private:
	RFC_LEAK_DETECTOR(KBitmap)
};



