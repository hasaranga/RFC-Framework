
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

#include <malloc.h>
#include <Objbase.h>

template<class StructType>
class KReleaseUsingFree
{
public:
	static void Release(StructType* structPtr)
	{
		::free(structPtr);
	}
};

template<class StructType>
class KReleaseUsingTaskMemFree
{
public:
	static void Release(StructType* memory)
	{
		::CoTaskMemFree(memory);
	}
};

/**
	This class holds a pointer to the struct which is automatically freed when this object goes
	out of scope. 
*/
template<class StructType, class ReleaseMethod = KReleaseUsingFree<StructType>>
class KScopedStructPointer
{
private:
	StructType* structPointer;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedStructPointer()
	{
		structPointer = nullptr;
	}

	inline KScopedStructPointer(StructType* structPointer)
	{
		this->structPointer = structPointer;
	}

	KScopedStructPointer(KScopedStructPointer& structPointerToTransferFrom)
	{
		this->structPointer = structPointerToTransferFrom.structPointer;
		structPointerToTransferFrom.structPointer = nullptr;
	}

	bool IsNull()
	{
		return (structPointer == nullptr);
	}

	/** 
		Removes the current struct pointer from this KScopedStructPointer without freeing it.
		This will return the current struct pointer, and set the KScopedStructPointer to a null pointer.
	*/
	StructType* Detach()
	{ 
		StructType* m = structPointer;
		structPointer = nullptr;
		return m; 
	}

	~KScopedStructPointer()
	{
		if (structPointer)
			ReleaseMethod::Release(structPointer);
	}

	/** 
		Changes this KScopedStructPointer to point to a new struct.

		If this KScopedStructPointer already points to a struct, that struct
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedStructPointer& operator= (StructType* const newStructPointer)
	{
		if (structPointer != newStructPointer)
		{
			StructType* const oldStructPointer = structPointer;
			structPointer = newStructPointer;

			if (oldStructPointer)
				ReleaseMethod::Release(oldStructPointer);
		}

		return *this;
	}

	inline StructType** operator&() { return &structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline operator StructType*() const { return structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline StructType& operator*() const { return *structPointer; }

	/** Lets you access properties of the struct that this KScopedStructPointer refers to. */
	inline StructType* operator->() const { return structPointer; }

};

