
/*
	Copyright (C) 2013-2023 CrownSoft

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

#ifndef _KQUEUE_H_
#define _KQUEUE_H_

template<class T>
class KQueueNode {
public:
	T data;
	KQueueNode* next;
};

// Queue implemented using a linked list. Can hold unlimited number of items. (assumes T is a pointer type which is allocated using new)
template<class T>
class KPointerQueue
{
protected:
	KQueueNode<T>* firstNode;
	KQueueNode<T>* lastNode;

	CRITICAL_SECTION criticalSection;
	volatile bool isThreadSafe;

public:
	KPointerQueue(const bool isThreadSafe = true)
	{
		firstNode = nullptr;
		lastNode = nullptr;

		this->isThreadSafe = isThreadSafe;
		if (isThreadSafe)
			::InitializeCriticalSection(&criticalSection);
	}

	virtual void Push(T value)
	{
		KQueueNode<T>* newNode = new KQueueNode<T>();
		newNode->data = value;
		newNode->next = nullptr;

		if (isThreadSafe)
			::EnterCriticalSection(&criticalSection);

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

		if (isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	virtual T Pop()
	{
		if (isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if (firstNode == nullptr)
		{
			if (isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return nullptr;
		}

		T value = firstNode->data;

		// remove the item
		KQueueNode<T>* tmp = firstNode;
		firstNode = firstNode->next;
		if (firstNode == nullptr) // we had only one item
			lastNode = nullptr;

		delete tmp;

		if (isThreadSafe)
			::LeaveCriticalSection(&criticalSection);

		return value;
	}

	// calls desctructor of all the T objects in the queue. also clear the queue.
	virtual void DeleteAllObjects()
	{
		if (isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if (firstNode == nullptr)
		{
			if (isThreadSafe)
				::LeaveCriticalSection(&criticalSection);
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

		if (isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	virtual ~KPointerQueue()
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

		if (isThreadSafe)
			::DeleteCriticalSection(&criticalSection);
	}
};

#endif
