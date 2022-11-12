
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

/**
	This class holds a pointer to CRITICAL_SECTION which is automatically released when this object goes
	out of scope.
*/
class KScopedCriticalSection
{
private:
	CRITICAL_SECTION *criticalSection;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedCriticalSection(CRITICAL_SECTION *criticalSection)
	{
		this->criticalSection = criticalSection;
		::EnterCriticalSection(criticalSection);
	}

	// does not call LeaveCriticalSection
	CRITICAL_SECTION* Detach()
	{ 
		CRITICAL_SECTION *c = criticalSection;
		criticalSection = NULL;
		return c; 
	}

	~KScopedCriticalSection()
	{
		if (criticalSection)
			::LeaveCriticalSection(criticalSection);
	}

	inline operator CRITICAL_SECTION*() const { return criticalSection; }

};

