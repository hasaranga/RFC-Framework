
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

#ifndef _KQUEUE_H_
#define _KQUEUE_H_

template<class T>
class KQueueNode {
public:
	T data;
	KQueueNode* next;
};

// Queue implemented using a linked list. Can hold unlimited number of items. (assumes T is a pointer type which is allocated using new)
template<class T, bool IsThreadSafe>
class KPointerQueue : private KThreadSafetyBase<IsThreadSafe>
{
protected:
	KQueueNode<T>* firstNode;
	KQueueNode<T>* lastNode;

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
	KPointerQueue()
	{
		firstNode = nullptr;
		lastNode = nullptr;
	}

	void push(T value)
	{
		KQueueNode<T>* newNode = new KQueueNode<T>();
		newNode->data = value;
		newNode->next = nullptr;

		enterCriticalSectionIfNeeded();

		if (firstNode == nullptr)
		{
			firstNode = newNode;
			lastNode = newNode;
		}
		else
		{
			lastNode->next = newNode;
			lastNode = newNode;
		}

		leaveCriticalSectionIfNeeded();
	}

	T pop()
	{
		enterCriticalSectionIfNeeded();

		if (firstNode == nullptr)
		{
			leaveCriticalSectionIfNeeded();
			return nullptr;
		}

		T value = firstNode->data;

		// remove the item
		KQueueNode<T>* tmp = firstNode;
		firstNode = firstNode->next;
		if (firstNode == nullptr) // we had only one item
			lastNode = nullptr;

		delete tmp;

		leaveCriticalSectionIfNeeded();
		return value;
	}

	// calls desctructor of all the T objects in the queue. also clear the queue.
	void deleteAllObjects()
	{
		enterCriticalSectionIfNeeded();

		if (firstNode == nullptr)
		{
			leaveCriticalSectionIfNeeded();
			return;
		}

		KQueueNode<T>* nextNode = firstNode;
		while (nextNode)
		{
			KQueueNode<T>* tmp = nextNode;
			nextNode = nextNode->next;

			delete tmp->data;
			delete tmp;
		}

		firstNode = nullptr;
		lastNode = nullptr;

		leaveCriticalSectionIfNeeded();
	}

	~KPointerQueue()
	{
		// delete all nodes

		if (firstNode == nullptr)
			return;

		KQueueNode<T>* nextNode = firstNode;
		while (nextNode)
		{
			KQueueNode<T>* tmp = nextNode;
			nextNode = nextNode->next;
			delete tmp;
		}
	}
};

#endif
