
/*
	Copyright (C) 2013-2026 CrownSoft

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
	This class holds a COM pointer which is automatically released when this object goes
	out of scope.
*/
template<class T>
class KScopedComPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedComPointer() noexcept
	{
		object = nullptr;
	}

	inline KScopedComPointer(T* object) noexcept
	{
		this->object = object;

		if (this->object)
			this->object->AddRef();
	}

	KScopedComPointer(KScopedComPointer& objectToTransferFrom) noexcept
	{
		object = objectToTransferFrom.object;

		if (object)
			object->AddRef();
	}

	bool isNull() noexcept
	{
		return (object == nullptr);
	}

	/** 
		Removes the current COM object from this KScopedComPointer without releasing it.
		This will return the current object, and set the KScopedComPointer to a null pointer.
	*/
	T* detach() noexcept
	{ 
		T* o = object; 
		object = nullptr;
		return o; 
	}

	~KScopedComPointer() noexcept
	{
		if (object)
			object->Release();

		object = nullptr;
	}

	// pointer to pointer
	void** asVoidPP() noexcept
	{
		return (void**)&object;
	}

	// pointer to pointer
	T** getAddressOf() noexcept
	{
		return &object;
	}

	inline T** operator&() noexcept { return &object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline operator T*() const noexcept { return object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline T& operator*() const noexcept { return *object; }

	/** Lets you access methods and properties of the object that this KScopedComPointer refers to. */
	inline T* operator->() const noexcept { return object; }

};

