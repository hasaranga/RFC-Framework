
/*
	RFC - KScopedHandle.h
	Copyright (C) 2013-2019 CrownSoft

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

#ifndef _RFC_KSCOPED_HANDLE_H_
#define _RFC_KSCOPED_HANDLE_H_

#include <windows.h>

/**
	This class holds a handle which is automatically closed when this object goes
	out of scope.
*/
class KScopedHandle
{
private:
	HANDLE handle;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedHandle()
	{
		handle = 0;
	}

	KScopedHandle(HANDLE handle)
	{
		this->handle = handle;
	}

	HANDLE Detach()
	{
		HANDLE h = handle;
		handle = 0;
		return h;
	}

	KScopedHandle& operator= (HANDLE newHandle)
	{
		if (handle != newHandle)
		{
			HANDLE oldHandle = handle;
			handle = newHandle;

			if (oldHandle)
				::CloseHandle(oldHandle);
		}

		return *this;
	}

	bool IsNull()
	{
		return (handle == 0);
	}

	~KScopedHandle()
	{
		if (handle)
			::CloseHandle(handle);
	}

	inline operator HANDLE() const { return handle; }

	inline HANDLE* operator&() { return &handle; }

};

#endif