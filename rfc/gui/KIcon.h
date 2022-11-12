
/*
	Copyright (C) 2013-2022 CrownSoft
  
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

public:
	KIcon();

	/**
		Loads icon from resource
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads icon from file
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool LoadFromFile(const KString& filePath);

	/**
		Returns icon handle
	*/
	HICON GetHandle();

	operator HICON()const;

	virtual ~KIcon();

private:
	RFC_LEAK_DETECTOR(KIcon)
};

