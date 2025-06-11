
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

#include <windows.h>
#include "KLeakDetector.h"

/**
	This class holds reference counted heap memory which is allocated using malloc.
	when ref count reach zero, the memory will be released using ::free.
*/
template<class T>
class KRefCountedMemory
{
private:
	~KRefCountedMemory() {}

protected:
	volatile LONG refCount;

public:
	T buffer;

	KRefCountedMemory(T buffer) : refCount(1), buffer(buffer) {}
	
	/**
		Make sure to call this method if you construct new KRefCountedMemory or keep reference to another KRefCountedMemory object.
	*/
	void addReference()
	{
		::InterlockedIncrement(&refCount);
	}

	/**
		Make sure to call this method if you clear reference to KRefCountedMemory object. 
		it will release allocated memory for string if ref count is zero.
	*/
	void releaseReference()
	{
		const LONG res = ::InterlockedDecrement(&refCount);
		if (res == 0)
		{
			if (buffer)
				::free(buffer);

			delete this;
		}
	}

private:
	RFC_LEAK_DETECTOR(KRefCountedMemory)
};


