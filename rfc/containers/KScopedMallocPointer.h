
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


/**
	This class holds a pointer which is allocated using malloc and it will automatically freed when this object goes
	out of scope. 
*/
template<class PointerType>
class KScopedMallocPointer
{
private:
	PointerType* pointer;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedMallocPointer()
	{
		pointer = nullptr;
	}

	inline KScopedMallocPointer(PointerType* pointer)
	{
		this->pointer = pointer;
	}

	KScopedMallocPointer(KScopedMallocPointer& pointerToTransferFrom)
	{
		this->pointer = pointerToTransferFrom.pointer;
		pointerToTransferFrom.pointer = nullptr;
	}

	bool IsNull()
	{
		return (pointer == nullptr);
	}

	/** 
		Removes the current pointer from this KScopedMallocPointer without freeing it.
		This will return the current pointer, and set the KScopedMallocPointer to a null pointer.
	*/
	PointerType* Detach()
	{ 
		PointerType* m = pointer;
		pointer = nullptr;
		return m; 
	}

	~KScopedMallocPointer()
	{
		if (pointer)
			::free(pointer);
	}

	/** 
		Changes this KScopedMallocPointer to point to a new pointer.

		If this KScopedMallocPointer already holds a pointer, that pointer
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedMallocPointer& operator= (PointerType* const newPointer)
	{
		if (pointer != newPointer)
		{
			PointerType* const oldPointer = pointer;
			pointer = newPointer;

			if (oldPointer)
				::free(oldPointer);
		}

		return *this;
	}

	inline PointerType** operator&() { return &pointer; }

	/** Returns the pointer that this KScopedMallocPointer refers to. */
	inline operator PointerType*() const { return pointer; }

	/** Returns the pointer that this KScopedMallocPointer refers to. */
	inline PointerType& operator*() const { return *pointer; }

};

