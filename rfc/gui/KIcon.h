
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
	Can be use to load icon from file or resource.
*/
class KIcon
{
protected:
	HICON hIcon;
	WORD resourceID;

public:
	KIcon();

	// does not load. to be use with getScaledIcon method.
	void setResource(WORD resourceID);

	// this method scales down a larger image instead of scaling up a smaller image.
	// can be use for high-dpi requirements.
	// must destroy returned icon by calling DestroyIcon
	HICON getScaledIcon(int size);

	/**
		Loads icon from resource with default size given by the system
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads icon from file with default size given by the system
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool loadFromFile(const KString& filePath);

	/**
		Returns icon handle
	*/
	HICON getHandle();

	operator HICON()const;

	virtual ~KIcon();

private:
	RFC_LEAK_DETECTOR(KIcon)
};

