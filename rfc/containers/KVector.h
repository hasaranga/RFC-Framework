
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
#include "KPointerList.h"
#include <malloc.h>
#include <utility>

/**
	Holds a resizable list of classes with small buffer optimization.
	item removing can be expensive if T's move is expensive.
	Thread safety is determined at compile time via template parameter.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	@param T The class type to store. T should implement copy/move constructor, (move)assign & compare operators.
	@param SmallBufferSize Number of items to store in stack buffer before allocating heap memory
	@param IsThreadSafe Compile-time flag for thread safety

	e.g. @code
	KString str1, str2;
	KVector<KString, 8, false> strList; // 8 items in small buffer, not thread safe
	strList.add(str1);
	strList.add(str2);
	@endcode
*/
template<class T, int SmallBufferSize, bool IsThreadSafe>
class KVector : private KThreadSafetyBase<IsThreadSafe>
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
		Constructs KVector object.
		Thread safety is determined at compile time via template parameter.
	*/
	KVector() noexcept
	{
		resetToSmallBuffer();
		// Critical section initialization is handled by base class constructor
	}

	/**
		Copy constructor
	*/
	KVector(const KVector& other) noexcept
	{
		resetToSmallBuffer();

		// other may be concurrently mutated by another thread, so its own lock must be held while we read it.
		KVector& src = const_cast<KVector&>(other);
		src.enterCriticalSectionIfNeeded();

		// If other has more items than our small buffer can hold
		if (other.itemCount > SmallBufferSize)
		{
			roomCount = other.roomCount;
			list = new T[roomCount];
			usingSmallBuffer = false;
		}

		// Copy items
		itemCount = other.itemCount;
		for (int i = 0; i < itemCount; i++)
		{
			list[i] = other.list[i];
		}

		src.leaveCriticalSectionIfNeeded();
	}

	/**
		Assignment operator
	*/
	KVector& operator=(const KVector& other) noexcept
	{
		if (this == &other)
			return *this;

		// other may be concurrently mutated by another thread, so its own lock must be held while we read it.
		// Lock in a consistent address order to avoid deadlocking against a concurrent reverse assignment (other = *this).
		KVector& src = const_cast<KVector&>(other);
		if (this < &src)
		{
			enterCriticalSectionIfNeeded();
			src.enterCriticalSectionIfNeeded();
		}
		else
		{
			src.enterCriticalSectionIfNeeded();
			enterCriticalSectionIfNeeded();
		}

		// Clean up current data
		if (!usingSmallBuffer)
		{
			delete[] list;
		}

		resetToSmallBuffer();

		// If other has more items than our small buffer can hold
		if (other.itemCount > SmallBufferSize)
		{
			roomCount = other.roomCount;
			list = new T[roomCount];
			usingSmallBuffer = false;
		}

		// Copy items
		itemCount = other.itemCount;
		for (int i = 0; i < itemCount; i++)
		{
			list[i] = other.list[i]; // copy
		}

		leaveCriticalSectionIfNeeded();
		src.leaveCriticalSectionIfNeeded();
		return *this;
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool add(const T& item) noexcept
	{
		enterCriticalSectionIfNeeded();

		if (roomCount >= (itemCount + 1)) // no need reallocation. room count is enough!
		{
			list[itemCount] = item; // copy
			itemCount++;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // require reallocation!
		{
			int newRoomCount = roomCount + SmallBufferSize;
			T* newList = new T[newRoomCount];

			// Copy existing items to new buffer
			for (int i = 0; i < itemCount; i++)
			{
				newList[i] = std::move(list[i]);
			}

			// Add the new item
			newList[itemCount] = item;
			itemCount++;

			// Free old buffer if it was heap allocated
			if (!usingSmallBuffer)
				delete[] list;

			// Update to use new buffer
			list = newList;
			roomCount = newRoomCount;
			usingSmallBuffer = false;

			leaveCriticalSectionIfNeeded();
			return true;
		}
	}

	T get(const int index) noexcept
	{
		if constexpr (IsThreadSafe)
		{
			// Thread-safe path: need to copy under lock
			enterCriticalSectionIfNeeded();

			if ((0 <= index) && (index < itemCount))
			{
				T object(list[index]); // make a copy under lock
				leaveCriticalSectionIfNeeded();
				return object;
			}
			else
			{
				leaveCriticalSectionIfNeeded();
				return T();
			}
		}
		else
		{
			// Non-thread-safe path: direct return, no copy
			if ((0 <= index) && (index < itemCount))
			{
				return list[index]; // single copy on return
			}
			else
			{
				return T();
			}
		}
	}

	// avoids extra copy
	bool get(const int index, T& outItem) noexcept
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount)) // checks for valid range!
		{
			outItem = list[index];
			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			outItem = T();
			return false;
		}
	}

	T operator[](const int index) noexcept
	{
		return get(index);
	}

	/**
		@returns false if index is out of range!
	*/
	bool set(const int index, const T& item) noexcept
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			list[index] = item;
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
		Remove item of given index
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
				list[i] = std::move(list[i + 1]);
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

	bool removeItem(const T& item) noexcept
	{
		enterCriticalSectionIfNeeded();

		bool retVal = false;
		const int index = getIndex(item);
		if (index != -1)
			retVal = remove(index);

		leaveCriticalSectionIfNeeded();
		return retVal;
	}

	/**
		Removes all items from the list! Falls back to small buffer.
	*/
	void removeAll() noexcept
	{
		enterCriticalSectionIfNeeded();

		if (!usingSmallBuffer)
			delete[] list;

		// smallBuffer items are intentionally left as-is: forcing a default-construct + assign here would pay
		// T's construction cost on every removeAll() even when nothing needs it. They're released whenever
		// the slot is next overwritten by add()/set(), or when the KVector itself is destroyed.

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
		Finds the index of the first item which matches the item passed in.
		@returns -1 if not found!
	*/
	int getIndex(const T& item) noexcept
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			if (list[i] == item)
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
	int size() const noexcept
	{
		return itemCount;
	}

	/**
	 * Safely iterate through all items in the list with thread synchronization.
	 * The entire iteration is protected by critical section if thread safety is enabled.
	 * @param func Function/lambda to call for each item in the list
	*/
	void forEach(std::function<void(T&)> func) noexcept
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
	 * @param func Function/lambda that takes (item, index) as parameters
	*/
	void forEachWithIndex(std::function<void(T&, int)> func) noexcept
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			func(list[i], i);
		}

		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with early termination support and index.
	 * @param func Function/lambda that returns bool (true = continue, false = stop)
	 * @returns true if iteration completed, false if stopped early
	*/
	bool forEachUntilWithIndex(std::function<bool(T&, int)> func) noexcept
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
	 * Safely iterate with early termination support.
	 * @param func Function/lambda that returns bool (true = continue, false = stop)
	 * @returns true if iteration completed, false if stopped early
	*/
	bool forEachUntil(std::function<bool(T&)> func) noexcept
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

	/**
	 * Safely iterate backwards through all items in the list with thread synchronization.
	 * Safe for removing items during iteration since indices aren't affected by removals.
	 * @param func Function/lambda to call for each item in the list (from last to first)
	*/
	void forEachReverse(std::function<void(T&)> func) noexcept
	{
		enterCriticalSectionIfNeeded();
		for (int i = itemCount - 1; i >= 0; i--)
		{
			func(list[i]);
		}
		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate backwards with index access.
	 * Safe for removing items during iteration.
	 * @param func Function/lambda that takes (item, index) as parameters
	*/
	void forEachReverseWithIndex(std::function<void(T&, int)> func) noexcept
	{
		enterCriticalSectionIfNeeded();
		for (int i = itemCount - 1; i >= 0; i--)
		{
			func(list[i], i);
		}
		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate backwards with early termination support and index.
	 * Safe for removing items during iteration.
	 * @param func Function/lambda that returns bool (true = continue, false = stop)
	 * @returns true if iteration completed, false if stopped early
	*/
	bool forEachReverseUntilWithIndex(std::function<bool(T&, int)> func) noexcept
	{
		enterCriticalSectionIfNeeded();

		bool completed = true;
		for (int i = itemCount - 1; i >= 0; i--)
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

	/** Destructs KVector object.*/
	~KVector() noexcept
	{
		if (!usingSmallBuffer)
			delete[] list;

		// Critical section cleanup is handled by base class destructor
	}

private:
	RFC_LEAK_DETECTOR(KVector)
};