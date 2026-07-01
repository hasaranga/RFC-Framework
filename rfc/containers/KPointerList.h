
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

#include "../core/CoreModule.h"
#include <malloc.h>
#include <functional>

/**
	Helper base class for thread safety - only contains critical section when needed
*/
template<bool IsThreadSafe>
struct KThreadSafetyBase
{
	// Empty base class when thread safety is not needed. ("empty base optimization")
};

template<>
struct KThreadSafetyBase<true>
{
	CRITICAL_SECTION criticalSection;

	KThreadSafetyBase() noexcept
	{
		::InitializeCriticalSection(&criticalSection);
	}

	~KThreadSafetyBase() noexcept
	{
		::DeleteCriticalSection(&criticalSection);
	}
};

/**
	Holds a resizable list of pointers with small buffer optimization.
	Thread safety is determined at compile time via template parameter.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	@param T The pointer type to store
	@param SmallBufferSize Number of items to store in stack buffer before allocating heap memory
	@param IsThreadSafe Compile-time flag for thread safety

	e.g. @code
	KButton btn1;
	KPointerList<KButton*, 8, false> btnList; // 8 items in small buffer, not thread safe
	KPointerList<KButton*, 8, true> threadSafeBtnList; // 8 items in small buffer, thread safe
	btnList.addPointer(&btn1);
	btnList.addPointer(&btn1);
	@endcode
*/
template<class T, int SmallBufferSize, bool IsThreadSafe>
class KPointerList : private KThreadSafetyBase<IsThreadSafe>
{
protected:
	int itemCount; // current element count in the list
	int roomCount; // maximum element count
	T* list;
	T smallBuffer[SmallBufferSize]; // Stack-allocated small buffer
	bool usingSmallBuffer;

	void resetToSmallBuffer() noexcept
	{
		usingSmallBuffer = true;
		list = smallBuffer;
		roomCount = SmallBufferSize;
		itemCount = 0;
	}

	// Thread safety helper methods
	inline void enterCriticalSectionIfNeeded() noexcept
	{
		if constexpr (IsThreadSafe)
		{
			::EnterCriticalSection(&this->criticalSection);
		}
	}

	inline void leaveCriticalSectionIfNeeded() noexcept
	{
		if constexpr (IsThreadSafe)
		{
			::LeaveCriticalSection(&this->criticalSection);
		}
	}

public:
	/**
		Constructs PointerList object.
		Thread safety is determined at compile time via template parameter.
	*/
	KPointerList() noexcept
	{
		resetToSmallBuffer();
		// Critical section initialization is handled by base class constructor
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool add(T pointer) noexcept
	{
		enterCriticalSectionIfNeeded();

		if (roomCount >= (itemCount + 1)) // no need reallocation. room count is enough!
		{
			list[itemCount] = pointer;
			itemCount++;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // require reallocation!
		{
			if (usingSmallBuffer)
			{
				// Switch from small buffer to heap buffer
				roomCount += SmallBufferSize;
				T* newList = (T*)::malloc(roomCount * sizeof(T));

				if (newList)
				{
					// Copy from small buffer to heap buffer
					::memcpy(newList, smallBuffer, SmallBufferSize * sizeof(T));

					list = newList;
					usingSmallBuffer = false;

					list[itemCount] = pointer;
					itemCount++;

					leaveCriticalSectionIfNeeded();
					return true;
				}
				else // memory allocation failed!
				{
					roomCount -= SmallBufferSize;
					leaveCriticalSectionIfNeeded();
					return false;
				}
			}
			else
			{
				// Already using heap buffer, just reallocate
				roomCount += SmallBufferSize;
				void* retVal = ::realloc((void*)list, roomCount * sizeof(T));
				if (retVal)
				{
					list = (T*)retVal;
					list[itemCount] = pointer;
					itemCount++;

					leaveCriticalSectionIfNeeded();
					return true;
				}
				else // memory allocation failed!
				{
					roomCount -= SmallBufferSize;
					leaveCriticalSectionIfNeeded();
					return false;
				}
			}
		}
	}

	/**
		Get pointer at index.
		@returns 0 if index is out of range!
	*/
	T get(const int index) noexcept
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount)) // checks for valid range!
		{
			T object = list[index];
			leaveCriticalSectionIfNeeded();
			return object;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return nullptr;
		}
	}

	/**
		Get pointer at index.
		@returns 0 if index is out of range!
	*/
	T operator[](const int index) noexcept
	{
		return get(index);
	}

	/**
		Replace pointer of given index with new pointer
		@returns false if index is out of range!
	*/
	bool set(const int index, T pointer) noexcept
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			list[index] = pointer;
			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return false;
		}
	}

	/**
		Remove pointer of given index
		@returns false if index is out of range!
	*/
	bool remove(const int index) noexcept
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			// Shift all elements after 'index' one position to the left
			for (int i = index; i < itemCount - 1; i++)
			{
				list[i] = list[i + 1];
			}
			itemCount--;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return false;
		}
	}

	bool remove(T pointer) noexcept
	{
		enterCriticalSectionIfNeeded();

		bool retVal = false;
		const int index = getIndex(pointer);
		if (index != -1)
			retVal = remove(index);

		leaveCriticalSectionIfNeeded();
		return retVal;
	}

	/**
		Removes all pointers from the list! Falls back to small buffer.
	*/
	void removeAll() noexcept
	{
		enterCriticalSectionIfNeeded();

		if (!usingSmallBuffer)
			::free((void*)list);

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list. Falls back to small buffer.
	*/
	void deleteAll() noexcept
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			T object = list[i];
			delete object;
		}

		if (!usingSmallBuffer)
			::free((void*)list);

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate through all pointers in the list with thread synchronization.
	 * The entire iteration is protected by critical section if thread safety is enabled.
	 * @param func Function/lambda to call for each pointer in the list
	*/
	void forEach(std::function<void(T)> func) noexcept
	{
		enterCriticalSectionIfNeeded();
		for (int i = 0; i < itemCount; i++) 
		{
			func(list[i]);
		}
		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with index access. Useful when you need the index as well.
	 * @param func Function/lambda that takes (pointer, index) as parameters
	*/
	void forEachWithIndex(std::function<void(T, int)> func) noexcept
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			func(list[i], i);
		}

		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with early termination support.
	 * @param func Function/lambda that returns bool (true = continue, false = stop)
	 * @returns true if iteration completed, false if stopped early
	 * Example: Early termination (find first disabled button)
		KButton* foundBtn = nullptr;
		buttonList.forEachUntil([&foundBtn](KButton* btn) -> bool {
			if (btn && !btn->isEnabled()) {
				foundBtn = btn;
				return false; // Stop iteration
			}
			return true; // Continue
		});
	*/
	bool forEachUntil(std::function<bool(T)> func) noexcept
	{
		enterCriticalSectionIfNeeded();

		bool completed = true;
		for (int i = 0; i < itemCount; i++)
		{
			if (!func(list[i]))
			{
				completed = false;
				break;
			}
		}

		leaveCriticalSectionIfNeeded();
		return completed;
	}

	bool forEachUntilWithIndex(std::function<bool(T,int)> func) noexcept
	{
		enterCriticalSectionIfNeeded();

		bool completed = true;
		for (int i = 0; i < itemCount; i++)
		{
			if (!func(list[i], i))
			{
				completed = false;
				break;
			}
		}

		leaveCriticalSectionIfNeeded();
		return completed;
	}

	/**
		Finds the index of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int getIndex(T pointer) noexcept
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			if (list[i] == pointer)
			{
				leaveCriticalSectionIfNeeded();
				return i;
			}
		}

		leaveCriticalSectionIfNeeded();
		return -1;
	}

	/**
		@returns item count in the list
	*/
	int size() noexcept
	{
		return itemCount;
	}

	/**
		@returns whether the list is currently using the small buffer optimization
	*/
	bool isUsingSmallBuffer() const noexcept
	{
		return usingSmallBuffer;
	}

	/**
		@returns the size of the small buffer
	*/
	static constexpr int getSmallBufferSize() noexcept
	{
		return SmallBufferSize;
	}

	/**
		@returns whether this instance is thread-safe (compile-time constant)
	*/
	static constexpr bool isThreadSafeInstance() noexcept
	{
		return IsThreadSafe;
	}

	/** Destructs PointerList object.*/
	~KPointerList() noexcept
	{
		if (!usingSmallBuffer)
			::free((void*)list);

		// Critical section cleanup is handled by base class destructor
	}

private:
	RFC_LEAK_DETECTOR(KPointerList)
};
