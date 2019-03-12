
/*
	RFC - KScopedGdiObject.h
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

#ifndef _RFC_KSCOPED_GDI_OBJECT_H_
#define _RFC_KSCOPED_GDI_OBJECT_H_

#include <windows.h>

/**
	This class holds a gdi object which is automatically freed when this object goes
	out of scope.

	If you need to get a gdi object out of a KScopedGdiObject without it being freed, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedGdiObject<HBRUSH> a = ::CreateSolidBrush(RGB(255, 255, 255)); // slow
	KScopedGdiObject<HBRUSH> b(::CreateSolidBrush(RGB(255, 0, 0))); // fast
	@endcode,
*/
template<class T>
class KScopedGdiObject
{
private:
	T gdiObject;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedGdiObject()
	{
		gdiObject = 0;
	}

	inline KScopedGdiObject(T gdiObject)
	{
		this->gdiObject = gdiObject;
	}

	KScopedGdiObject(KScopedGdiObject& gdiObjectToTransferFrom)
	{
		this->gdiObject = gdiObjectToTransferFrom.gdiObject;
		gdiObjectToTransferFrom.gdiObject = 0;
	}

	/** 
		Removes the current gdi object from this KScopedGdiObject without freeing it.
		This will return the current gdi object, and set the KScopedGdiObject to a null value.
	*/
	T Detach()
	{ 
		T g = gdiObject;
		gdiObject = 0;
		return g; 
	}

	~KScopedGdiObject()
	{
		if (gdiObject)
			::DeleteObject(gdiObject);
	}

	/** 
		Changes this KScopedGdiObject to point to a new gdi object.

		If this KScopedGdiObject already points to a gdi object, that object
		will first be freed.

		The object that you pass in may be a zero.
	*/
	KScopedGdiObject& operator= (T const newGdiObject)
	{
		if (gdiObject != newGdiObject)
		{
			T const oldgdiObject = gdiObject;
			gdiObject = newGdiObject;

			if (oldgdiObject)
				::DeleteObject(oldgdiObject);
		}

		return *this;
	}

	/** Returns the gdi object that this KScopedGdiObject refers to. */
	inline operator T() const { return gdiObject; }

};

#endif