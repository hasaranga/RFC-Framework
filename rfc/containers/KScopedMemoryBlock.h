
/*
	RFC - KScopedMemoryBlock.h
	Copyright (C) 2013-2018 CrownSoft

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

#ifndef _RFC_KSCOPED_MEMORY_BLOCK_H_
#define _RFC_KSCOPED_MEMORY_BLOCK_H_

#include "../config.h"
#include <malloc.h>

/**
	This class holds a memory which is automatically freed when this object goes
	out of scope.

	Once a memory has been passed to a KScopedMemoryBlock, it will make sure that the memory
	gets freed when the KScopedMemoryBlock is deleted. Using the KScopedMemoryBlock on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	allocated memory.

	A KScopedMemoryBlock can be used in pretty much the same way that you'd use a normal pointer
	to a memory block. If you use the assignment operator to assign a different memory to a
	KScopedMemoryBlock, the old one will be automatically freed.

	If you need to get a memory block out of a KScopedClassPointer without it being freed, you
	can use the Release() method.

	(Inspired from JUCE ScopedPointer class)

	e.g. @code
	KScopedMemoryBlock<float*> a = (float*)malloc(512 * sizeof(float));
	@endcode
*/
template<class T>
class KScopedMemoryBlock
{
private:
	T memoryBlock;

public:
	inline KScopedMemoryBlock()
	{
		memoryBlock = 0;
	}

	inline KScopedMemoryBlock(T memoryBlock)
	{
		this->memoryBlock = memoryBlock;
	}

	KScopedMemoryBlock(KScopedMemoryBlock& memoryBlockToTransferFrom)
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = 0;
	}

	/** 
		Removes the current memory block from this KScopedMemoryBlock without freeing it.
		This will return the current memory block, and set the KScopedMemoryBlock to a null pointer.
	*/
	T Release()
	{ 
		T m = memoryBlock;
		memoryBlock = 0;
		return m; 
	}

	~KScopedMemoryBlock()
	{
		if (memoryBlock)
			::free(memoryBlock);
	}

	/** 
		Changes this KScopedMemoryBlock to point to a new memory block.

		If this KScopedMemoryBlock already points to a memory, that memory
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedMemoryBlock& operator= (T const newMemoryBlock)
	{
		if (memoryBlock != newMemoryBlock)
		{
			T const oldMemoryBlock = memoryBlock;
			memoryBlock = newMemoryBlock;

			if (oldMemoryBlock)
				::free(oldMemoryBlock);
		}

		return *this;
	}

	/** Returns the memory block that this KScopedMemoryBlock refers to. */
	inline operator T() const { return memoryBlock; }

};

#endif