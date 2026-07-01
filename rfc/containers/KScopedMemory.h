
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

#include "KReleaseTypes.h"

/**
	This class holds a memory which is automatically freed when this object goes
	out of scope.

	Once a memory has been passed to a KScopedMemory, it will make sure that the memory
	gets freed when the KScopedMemory is deleted. Using the KScopedMemory on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	allocated memory.

	If you use the assignment operator to assign a different memory to a
	KScopedMemory, the old one will be automatically freed.

	If you need to get a memory block out of a KScopedMemory without it being freed, you
	can use the detach() method.

	e.g. @code
	KScopedMemory<float*> a = (float*)malloc(512 * sizeof(float));
	KScopedMemory<float*> b( (float*)malloc(512 * sizeof(float)) ); // prevents extra assignment!
	@endcode
*/
template<class T, class ReleaseMethod = KReleasePtrUsingFree<T>>
class KScopedMemory
{
private:
	T memoryBlock;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedMemory() noexcept
	{
		memoryBlock = nullptr;
	}

	inline KScopedMemory(T memoryBlock) noexcept
	{
		this->memoryBlock = memoryBlock;
	}

	KScopedMemory(KScopedMemory& memoryBlockToTransferFrom) noexcept
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = nullptr;
	}

	bool isNull() noexcept
	{
		return (memoryBlock == nullptr);
	}

	/** 
		Removes the current memory block from this KScopedMemory without freeing it.
		This will return the current memory block, and set the KScopedMemory to a null pointer.
	*/
	T detach() noexcept
	{ 
		T m = memoryBlock;
		memoryBlock = nullptr;
		return m; 
	}

	~KScopedMemory() noexcept
	{
		if (memoryBlock)
			ReleaseMethod::release(memoryBlock);
	}

	/** 
		Changes this KScopedMemory to point to a new memory block.

		If this KScopedMemory already points to a memory, that memory
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedMemory& operator= (T const newMemoryBlock) noexcept
	{
		if (memoryBlock != newMemoryBlock)
		{
			T const oldMemoryBlock = memoryBlock;
			memoryBlock = newMemoryBlock;

			if (oldMemoryBlock)
				ReleaseMethod::release(oldMemoryBlock);
		}

		return *this;
	}

	// returns the internal pointer
	inline T get() noexcept { return memoryBlock; }

	// returns address of internal pointer. (pointer to pointer)
	inline T* getAddressOf() noexcept { return &memoryBlock; }

	inline void** asVoidPP() noexcept { return (void**)&memoryBlock; }
};

