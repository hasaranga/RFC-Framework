
/*
    RFC - KPointerList.h
    Copyright (C) 2013-2017 CrownSoft
  
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


#ifndef _RFC_KPOINTERLIST_H_
#define _RFC_KPOINTERLIST_H_

#include <malloc.h>
#include <windows.h>
#include "../config.h"

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma warning(disable:4311)
	#pragma warning(disable:4312)
#endif


/**
	Holds a resizable list of pointers.
	To make all the methods thread-safe, pass true for isThreadSafe parameter of constructor.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	e.g. @code
	KButton btn1;
	KPointerList<KButton*> btnList(1024,true);
	btnList.AddPointer(&btn1);
	btnList.AddPointer(&btn1);
	@endcode
*/
template<class T>
class KPointerList
{
protected:
	int size;
	int roomCount;
	int roomIncrement;
	void* list;

	CRITICAL_SECTION criticalSection;
	bool isThreadSafe;

public:
	/**
		Constructs PointerList object.
		@param roomIncrement initial and reallocation size of internal memory block in DWORDS
		@param isThreadSafe make all the methods thread-safe
	*/
	KPointerList(int roomIncrement = 1024, bool isThreadSafe = false) // 1024*4=4096 = default alignment!
	{
		roomCount = roomIncrement;
		this->roomIncrement = roomIncrement;
		this->isThreadSafe = isThreadSafe;
		size = 0;
		list = ::malloc(roomCount * PTR_SIZE);
		
		if(isThreadSafe)
		{
			::InitializeCriticalSection(&criticalSection);
		}
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool AddPointer(T pointer)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection); // thread safe!
		}

		if(roomCount >= (size + 1) ) // no need reallocation. coz room count is enough!
		{
			*(NATIVE_INT*)((NATIVE_INT)list + (size * PTR_SIZE)) = (NATIVE_INT)pointer;

			size++;
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return true;
		}else // require reallocation!
		{
			roomCount += roomIncrement;
			void* retVal = ::realloc(list, roomCount * PTR_SIZE); 
			if(retVal)
			{
				list = retVal;

				*(NATIVE_INT*)((NATIVE_INT)list + (size * PTR_SIZE)) = (NATIVE_INT)pointer;

				size++;
				if(isThreadSafe)
				{
					::LeaveCriticalSection(&criticalSection);
				}
				return true;
			}else // memory allocation failed!
			{
				if(isThreadSafe)
				{
					::LeaveCriticalSection(&criticalSection);
				}
				return false;
			}
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T GetPointer(int id)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		if( (0 <= id) & (id < size) ) // checks for valid range!
		{	
			T object = (T)(*(NATIVE_INT*)((NATIVE_INT)list + (id * PTR_SIZE)));
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return object;
		}else // out of range!
		{
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return 0;
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T operator[](const int id)
	{
		return GetPointer(id);
	}

	/**
		Replace pointer of given id with new pointer
		@returns false if id is out of range!
	*/
	bool SetPointer(int id, T pointer)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		if( (0 <= id) & (id < size) )
		{			
			*(NATIVE_INT*)((NATIVE_INT)list + (id * PTR_SIZE)) = (NATIVE_INT)pointer;
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return true;
		}else // out of range!
		{
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return false;
		}
	}

	/**
		Remove pointer of given id
		@returns false if id is out of range!
	*/
	bool RemovePointer(int id)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		if( (0 <= id) & (id < size) )
		{	
			int newRoomCount = (((size - 1) / roomIncrement) + 1) * roomIncrement;
			void* newList = ::malloc(newRoomCount * PTR_SIZE);

			for(register int i = 0, j = 0; i < size; i++)
			{
				if(i != id)
				{
					*(NATIVE_INT*)((NATIVE_INT)newList + (j*PTR_SIZE)) = *(NATIVE_INT*)((NATIVE_INT)list + (i * PTR_SIZE));
					j++;
				}	
			}
			::free(list); // free old list!
			list = newList;
			roomCount = newRoomCount;
			size--;
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return true;

		}else // out of range!
		{
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return false;
		}

	}

	/**
		Clears the list!
	*/
	void RemoveAll()// remove all pointers from list!
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		::free(list);
		roomCount = roomIncrement;
		list = ::malloc(roomCount * PTR_SIZE);
		size = 0;

		if(isThreadSafe)
		{
			::LeaveCriticalSection(&criticalSection);
		}
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list.
	*/
	void DeleteAll()
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		for(int i = 0; i < size; i++)
		{
			T object = (T)(*(NATIVE_INT*)((NATIVE_INT)list + (i * PTR_SIZE)));
			delete object;
		}

		::free(list);
		roomCount = roomIncrement;
		list = ::malloc(roomCount * PTR_SIZE);
		size = 0;

		if(isThreadSafe)
		{
			::LeaveCriticalSection(&criticalSection);
		}
	}

	/**
		Finds the id of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int GetID(T pointer)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		for(register int i = 0; i < size; i++)
		{
			if (*(NATIVE_INT*)((NATIVE_INT)list + (i*PTR_SIZE)) == (NATIVE_INT)pointer)
			{
				if(isThreadSafe)
				{
					::LeaveCriticalSection(&criticalSection);
				}
				return i;
			}
		}

		if(isThreadSafe)
		{
			::LeaveCriticalSection(&criticalSection);
		}
		return -1;
	}

	/**
		@returns item count in the list
	*/
	int GetSize()
	{
		return size;
	}

	/** Destructs PointerList object.*/
	~KPointerList()
	{
		::free(list);
		if(isThreadSafe)
		{
			::DeleteCriticalSection(&criticalSection);
		}
	}

};

#endif