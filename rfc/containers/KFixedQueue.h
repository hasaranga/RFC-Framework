
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

#include "../core/CoreModule.h"
#include "KPointerList.h"

// automatically overwrites oldest messages when full.
// circular buffer implementation (efficient, no dynamic allocation)
template <typename T, size_t QueueSize, bool IsThreadSafe>
class KFixedQueue : private KThreadSafetyBase<IsThreadSafe>
{
protected:
	// Queue storage
	T queue[QueueSize];

	// Queue indices
	int iQueueHead;
	int iQueueTail;

	// Thread safety helper methods
	inline void enterCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::EnterCriticalSection(&this->criticalSection);
		}
	}

	inline void leaveCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::LeaveCriticalSection(&this->criticalSection);
		}
	}

public:
	KFixedQueue() :iQueueHead(0), iQueueTail(0) {}

	// Delete copy constructor and assignment operator
	KFixedQueue(const KFixedQueue&) = delete;
	KFixedQueue& operator=(const KFixedQueue&) = delete;

	// Queue operations
	void enqueue(const T& data)
	{
		enterCriticalSectionIfNeeded();

		// Store the message at the head position
		queue[iQueueHead] = data;

		// Move head forward (circular)
		int nextHead = (iQueueHead + 1) % QueueSize;

		// If queue is full, move tail forward to overwrite oldest message
		if (nextHead == iQueueTail)
			iQueueTail = (iQueueTail + 1) % QueueSize;

		iQueueHead = nextHead;
		leaveCriticalSectionIfNeeded();
	}

	bool dequeue(T& data)
	{
		enterCriticalSectionIfNeeded();

		bool result = false;

		// Check if queue has messages
		if (iQueueHead != iQueueTail) {
			// Read message from tail position
			data = queue[iQueueTail];

			// Move tail forward (circular)
			iQueueTail = (iQueueTail + 1) % QueueSize;

			result = true;
		}

		leaveCriticalSectionIfNeeded();
		return result;
	}

	// Queue state
	bool isEmpty() 
	{
		enterCriticalSectionIfNeeded();
		bool empty = (iQueueHead == iQueueTail);
		leaveCriticalSectionIfNeeded();
		return empty;
	}

	bool isFull()
	{
		enterCriticalSectionIfNeeded();
		bool full = ((iQueueHead + 1) % QueueSize == iQueueTail);
		leaveCriticalSectionIfNeeded();
		return full;
	}

	int getCount()
	{
		enterCriticalSectionIfNeeded();

		int count;
		if (iQueueHead >= iQueueTail)
		{
			count = iQueueHead - iQueueTail;
		}
		else
		{
			count = QueueSize - iQueueTail + iQueueHead;
		}

		leaveCriticalSectionIfNeeded();
		return count;
	}

	void clear()
	{
		enterCriticalSectionIfNeeded();
		iQueueHead = 0;
		iQueueTail = 0;
		leaveCriticalSectionIfNeeded();
	}
};