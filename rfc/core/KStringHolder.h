
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

#include <windows.h>
#include "KLeakDetector.h"

/**
	This class holds reference counted string.
*/
class KStringHolder
{
	volatile LONG refCount;
	char *a_text; // ansi version

	#ifndef RFC_NO_SAFE_ANSI_STR
	CRITICAL_SECTION cs_a_text; // to guard ansi string creation
	#endif

public:
	wchar_t *w_text; // unicode version
	int count; // character count

	KStringHolder(wchar_t *w_text, int count);

	~KStringHolder();

	/**
		Make sure to call this method if you contruct new KStringHolder or keep reference to another KStringHolder object.
	*/
	void AddReference();

	/**
		Make sure to call this method if you clear reference to KStringHolder object. it will release allocated memory for string.
	*/
	void ReleaseReference();

	/**
		ANSI version available only when needed.
	*/
	const char* GetAnsiVersion(UINT codePage = CP_UTF8);

private:
	RFC_LEAK_DETECTOR(KStringHolder)
};


