
// ========== RFC Generator v1.0 - 2022-11-12 12:16 PM ==========

#ifndef _RFC_H_
#define _RFC_H_ 

#define AMALGAMATED_VERSION


// =========== KListBoxListener.h ===========

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

class KListBox;

class KListBoxListener
{
public:
	KListBoxListener();

	virtual ~KListBoxListener();

	virtual void OnListBoxItemSelect(KListBox *listBox);

	virtual void OnListBoxItemDoubleClick(KListBox *listBox);
};

// =========== KModuleManager.h ===========

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

typedef bool (*RFCModuleInitFunc)();
typedef void (*RFCModuleFreeFunc)();

#define MAX_RFC_MODULE_COUNT 2

class KModuleManager {
public:
	static bool RegisterRFCModule(int index, RFCModuleInitFunc initFunc, RFCModuleFreeFunc freeFunc);
	static RFCModuleInitFunc* RFCModuleInitFuncList();
	static RFCModuleFreeFunc* RFCModuleFreeFuncList();
};

#define REGISTER_RFC_MODULE(index, ModuleObjectType) \
static bool ModuleObjectType##_Registered = KModuleManager::RegisterRFCModule( index , ModuleObjectType##::RFCModuleInit, ModuleObjectType##::RFCModuleFree);

// =========== KLeakDetector.h ===========

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

#include <windows.h>
#include <string.h>  
#include <stdlib.h>  

/**
	Embedding an instance of this class inside another class can be used as a
	low-overhead way of detecting leaked instances.

	This class keeps an internal static count of the number of instances that are
	active, so that when the app is shutdown and the static destructors are called,
	it can check whether there are any left-over instances that may have been leaked.

	To use it, simply declare RFC_LEAK_DETECTOR(YourClassName) inside a private section
	of the class declaration.

	(ref: LeakedObjectDetector class of JUCE)

	@code
	class MyClass
	{
	public:
		MyClass();
		void method1();

	private:
		RFC_LEAK_DETECTOR (MyClass)
	};
	@endcode
*/
template <class T>
class KLeakDetector
{
public:
	KLeakDetector()
	{
		::InterlockedIncrement(&GetCounter().numObjects);
	}

	KLeakDetector(const KLeakDetector&)
	{
		::InterlockedIncrement(&GetCounter().numObjects);
	}

	~KLeakDetector()
	{
		::InterlockedDecrement(&GetCounter().numObjects);
	}

private:
	class LeakCounter
	{
	public:
		LeakCounter()
		{
			numObjects = 0;
		}

		~LeakCounter()
		{
			if (numObjects > 0) // If you hit this, then you've leaked one or more objects of the type specified by the template parameter
			{
				char textBuffer[128];
				::strcpy_s(textBuffer, "Leaked objects detected : ");

				char intBuffer[16];
				::_itoa_s((int)numObjects, intBuffer, 10);

				::strcat_s(textBuffer, intBuffer);
				::strcat_s(textBuffer, " instance(s) of class ");
				::strcat_s(textBuffer, GetLeakedClassName());

				::MessageBoxA(0, textBuffer, "Warning", MB_ICONWARNING);
			}
		}

		volatile long numObjects;
	};

	static const char* GetLeakedClassName()
	{
		return T::rfc_GetLeakedClassName();
	}

	static LeakCounter& GetCounter()
	{
		static LeakCounter counter;
		return counter;
	}
};

#ifdef _DEBUG
	#define RFC_LEAK_DETECTOR(Class) \
			friend class KLeakDetector<Class>; \
			KLeakDetector<Class> rfc_leakDetector; \
			static const char* rfc_GetLeakedClassName() { return #Class; }
#else 
	#define RFC_LEAK_DETECTOR(Class)
#endif


// =========== KDPIUtility.h ===========

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

#include <windows.h>
#include <shellscalingapi.h>


typedef HRESULT(WINAPI* KGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
typedef BOOL(WINAPI* KSetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
typedef HRESULT(STDAPICALLTYPE* KSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
typedef BOOL (WINAPI* KSetProcessDPIAware)(VOID);
typedef DPI_AWARENESS_CONTEXT (WINAPI* KSetThreadDpiAwarenessContext) (DPI_AWARENESS_CONTEXT dpiContext);

/*
MIXEDMODE_ONLY:	on win10 - all windows are scaled according to the dpi and the mixed mode windows are scaled by the system. 
OS lower than win10 - all windows are scaled by the system.
STANDARD_MODE:	all windows are scaled according to the dpi. cannot have mixed mode windows.
UNAWARE_MODE:	all windows are scaled by the system.
*/

enum class KDPIAwareness
{
	MIXEDMODE_ONLY, // win10 only, app is not dpi aware on other os
	STANDARD_MODE, // win7 or higher
	UNAWARE_MODE
};

class KDPIUtility
{
public:		
	static KGetDpiForMonitor pGetDpiForMonitor;
	static KSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext;
	static KSetProcessDpiAwareness pSetProcessDpiAwareness;
	static KSetProcessDPIAware pSetProcessDPIAware;
	static KSetThreadDpiAwarenessContext pSetThreadDpiAwarenessContext;

	static void InitDPIFunctions();

	static WORD GetWindowDPI(HWND hWnd);

	static void MakeProcessDPIAware(KDPIAwareness dpiAwareness);
};


// =========== KComboBoxListener.h ===========

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

class KComboBox;

class KComboBoxListener
{
public:
	KComboBoxListener();

	virtual ~KComboBoxListener();

	virtual void OnComboBoxItemSelect(KComboBox *comboBox);
};


// =========== KMenuItemListener.h ===========

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


class KMenuItem;

class KMenuItemListener
{
public:
	KMenuItemListener();

	virtual ~KMenuItemListener();

	virtual void OnMenuItemPress(KMenuItem *menuItem);
};


// =========== KTrackBarListener.h ===========

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

class KTrackBar;

class KTrackBarListener
{
public:
	KTrackBarListener();

	virtual ~KTrackBarListener();

	virtual void OnTrackBarChange(KTrackBar *trackBar);
};


// =========== Architecture.h ===========

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

#ifdef _WIN64
	#define RFC64
	#define RFC_PTR_SIZE 8
	#define RFC_NATIVE_INT __int64
#else
	#define RFC32
	#define RFC_PTR_SIZE 4
	#define RFC_NATIVE_INT int
#endif


// =========== KGridViewListener.h ===========

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

class KGridView;

class KGridViewListener
{
public:
	KGridViewListener();

	virtual ~KGridViewListener();

	virtual void OnGridViewItemSelect(KGridView *gridView);

	virtual void OnGridViewItemRightClick(KGridView *gridView);

	virtual void OnGridViewItemDoubleClick(KGridView *gridView);
};

// =========== KScopedStructPointer.h ===========

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
		structPointer = NULL;
	}

	inline KScopedStructPointer(StructType* structPointer)
	{
		this->structPointer = structPointer;
	}

	KScopedStructPointer(KScopedStructPointer& structPointerToTransferFrom)
	{
		this->structPointer = structPointerToTransferFrom.structPointer;
		structPointerToTransferFrom.structPointer = NULL;
	}

	bool IsNull()
	{
		return (structPointer == NULL);
	}

	/** 
		Removes the current struct pointer from this KScopedStructPointer without freeing it.
		This will return the current struct pointer, and set the KScopedStructPointer to a null pointer.
	*/
	StructType* Detach()
	{ 
		StructType* m = structPointer;
		structPointer = NULL;
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


// =========== KScopedMallocPointer.h ===========

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
		pointer = NULL;
	}

	inline KScopedMallocPointer(PointerType* pointer)
	{
		this->pointer = pointer;
	}

	KScopedMallocPointer(KScopedMallocPointer& pointerToTransferFrom)
	{
		this->pointer = pointerToTransferFrom.pointer;
		pointerToTransferFrom.pointer = NULL;
	}

	bool IsNull()
	{
		return (pointer == NULL);
	}

	/** 
		Removes the current pointer from this KScopedMallocPointer without freeing it.
		This will return the current pointer, and set the KScopedMallocPointer to a null pointer.
	*/
	PointerType* Detach()
	{ 
		PointerType* m = pointer;
		pointer = NULL;
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


// =========== KScopedHandle.h ===========

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

#include <windows.h>

/**
	This class holds a handle which is automatically closed when this object goes
	out of scope.
*/
class KScopedHandle
{
private:
	HANDLE handle;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedHandle()
	{
		handle = 0;
	}

	KScopedHandle(HANDLE handle)
	{
		this->handle = handle;
	}

	HANDLE Detach()
	{
		HANDLE h = handle;
		handle = 0;
		return h;
	}

	KScopedHandle& operator= (HANDLE newHandle)
	{
		if (handle != newHandle)
		{
			HANDLE oldHandle = handle;
			handle = newHandle;

			if (oldHandle)
				::CloseHandle(oldHandle);
		}

		return *this;
	}

	bool IsNull()
	{
		return (handle == 0);
	}

	~KScopedHandle()
	{
		if (handle)
			::CloseHandle(handle);
	}

	inline operator HANDLE() const { return handle; }

	inline HANDLE* operator&() { return &handle; }

};


// =========== KScopedGdiObject.h ===========

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

#include <windows.h>

/**
	This class holds a gdi object which is automatically freed when this object goes
	out of scope.

	If you need to get a gdi object out of a KScopedGdiObject without it being freed, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedGdiObject<HBRUSH> a = ::CreateSolidBrush(RGB(255, 255, 255)); // slow
	KScopedGdiObject<HBRUSH> b(::CreateSolidBrush(RGB(255, 0, 0))); // fast
	@endcode,
*/
template<class T>
class KScopedGdiObject
{
private:
	T gdiObject;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedGdiObject()
	{
		gdiObject = 0;
	}

	inline KScopedGdiObject(T gdiObject)
	{
		this->gdiObject = gdiObject;
	}

	KScopedGdiObject(KScopedGdiObject& gdiObjectToTransferFrom)
	{
		this->gdiObject = gdiObjectToTransferFrom.gdiObject;
		gdiObjectToTransferFrom.gdiObject = 0;
	}

	/** 
		Removes the current gdi object from this KScopedGdiObject without freeing it.
		This will return the current gdi object, and set the KScopedGdiObject to a null value.
	*/
	T Detach()
	{ 
		T g = gdiObject;
		gdiObject = 0;
		return g; 
	}

	~KScopedGdiObject()
	{
		if (gdiObject)
			::DeleteObject(gdiObject);
	}

	/** 
		Changes this KScopedGdiObject to point to a new gdi object.

		If this KScopedGdiObject already points to a gdi object, that object
		will first be freed.

		The object that you pass in may be a zero.
	*/
	KScopedGdiObject& operator= (T const newGdiObject)
	{
		if (gdiObject != newGdiObject)
		{
			T const oldgdiObject = gdiObject;
			gdiObject = newGdiObject;

			if (oldgdiObject)
				::DeleteObject(oldgdiObject);
		}

		return *this;
	}

	/** Returns the gdi object that this KScopedGdiObject refers to. */
	inline operator T() const { return gdiObject; }

};


// =========== KScopedCriticalSection.h ===========

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

#include <windows.h>

/**
	This class holds a pointer to CRITICAL_SECTION which is automatically released when this object goes
	out of scope.
*/
class KScopedCriticalSection
{
private:
	CRITICAL_SECTION *criticalSection;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedCriticalSection(CRITICAL_SECTION *criticalSection)
	{
		this->criticalSection = criticalSection;
		::EnterCriticalSection(criticalSection);
	}

	// does not call LeaveCriticalSection
	CRITICAL_SECTION* Detach()
	{ 
		CRITICAL_SECTION *c = criticalSection;
		criticalSection = NULL;
		return c; 
	}

	~KScopedCriticalSection()
	{
		if (criticalSection)
			::LeaveCriticalSection(criticalSection);
	}

	inline operator CRITICAL_SECTION*() const { return criticalSection; }

};


// =========== KScopedComPointer.h ===========

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

#include <windows.h>

/**
	This class holds a COM pointer which is automatically released when this object goes
	out of scope.
*/
template<class T>
class KScopedComPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedComPointer()
	{
		object = NULL;
	}

	inline KScopedComPointer(T* object)
	{
		this->object = object;

		if (this->object)
			this->object->AddRef();
	}

	KScopedComPointer(KScopedComPointer& objectToTransferFrom)
	{
		object = objectToTransferFrom.object;

		if (object)
			object->AddRef();
	}

	bool IsNull()
	{
		return (object == NULL);
	}

	/** 
		Removes the current COM object from this KScopedComPointer without releasing it.
		This will return the current object, and set the KScopedComPointer to a null pointer.
	*/
	T* Detach()
	{ 
		T* o = object; 
		object = NULL; 
		return o; 
	}

	~KScopedComPointer()
	{
		if (object)
			object->Release();

		object = NULL;
	}

	inline T** operator&() { return &object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this KScopedComPointer refers to. */
	inline T* operator->() const { return object; }

};


// =========== KScopedClassPointer.h ===========

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

#include <windows.h>

/**
	This class holds a pointer which is automatically deleted when this object goes
	out of scope.

	Once a pointer has been passed to a KScopedClassPointer, it will make sure that the pointer
	gets deleted when the KScopedClassPointer is deleted. Using the KScopedClassPointer on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	created objects.

	A KScopedClassPointer can be used in pretty much the same way that you'd use a normal pointer
	to an object. If you use the assignment operator to assign a different object to a
	KScopedClassPointer, the old one will be automatically deleted.

	Important note: The class is designed to hold a pointer to an object, NOT to an array!
	It calls delete on its payload, not delete[], so do not give it an array to hold!

	If you need to get a pointer out of a KScopedClassPointer without it being deleted, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedClassPointer<MyClass> a = new MyClass(); // slow
	a->myMethod();
	a = new MyClass(); // old object will be deleted
	KScopedClassPointer<MyClass> b( new MyClass() ); // fast
	@endcode

*/
template<class T>
class KScopedClassPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedClassPointer()
	{
		object = 0;
	}

	inline KScopedClassPointer(T* object)
	{
		this->object = object;
	}

	KScopedClassPointer(KScopedClassPointer& objectToTransferFrom)
	{
		this->object = objectToTransferFrom.object;
		objectToTransferFrom.object = 0;
	}

	bool IsNull()
	{
		return (object == NULL);
	}

	/** 
		Removes the current object from this KScopedClassPointer without deleting it.
		This will return the current object, and set the KScopedClassPointer to a null pointer.
	*/
	T* Detach()
	{ 
		T* o = object; 
		object = 0; 
		return o; 
	}

	~KScopedClassPointer()
	{
		if (object)
			delete object;
	}

	/** 
		Changes this KScopedClassPointer to point to a new object.

		If this KScopedClassPointer already points to an object, that object
		will first be deleted.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedClassPointer& operator= (T* const newObject)
	{
		if (object != newObject)
		{
			T* const oldObject = object;
			object = newObject;

			if (oldObject)
				delete oldObject;
		}

		return *this;
	}

	/** Returns the object that this KScopedClassPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this KScopedClassPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this KScopedClassPointer refers to. */
	inline T* operator->() const { return object; }
};

// =========== KTimerListener.h ===========

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

class KTimer;

class KTimerListener
{
public:
	KTimerListener();

	virtual ~KTimerListener();

	virtual void OnTimer(KTimer *timer);
};


// =========== KScopedMemoryBlock.h ===========

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

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedMemoryBlock<float*> a = (float*)malloc(512 * sizeof(float)); // slow
	KScopedMemoryBlock<float*> b( (float*)malloc(512 * sizeof(float)) ); // fast
	@endcode
*/
template<class T>
class KScopedMemoryBlock
{
private:
	T memoryBlock;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedMemoryBlock()
	{
		memoryBlock = NULL;
	}

	inline KScopedMemoryBlock(T memoryBlock)
	{
		this->memoryBlock = memoryBlock;
	}

	KScopedMemoryBlock(KScopedMemoryBlock& memoryBlockToTransferFrom)
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = NULL;
	}

	bool IsNull()
	{
		return (memoryBlock == NULL);
	}

	/** 
		Removes the current memory block from this KScopedMemoryBlock without freeing it.
		This will return the current memory block, and set the KScopedMemoryBlock to a null pointer.
	*/
	T Detach()
	{ 
		T m = memoryBlock;
		memoryBlock = NULL;
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


// =========== KButtonListener.h ===========

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

class KButton;

class KButtonListener
{
public:
	KButtonListener();

	virtual ~KButtonListener();

	virtual void OnButtonPress(KButton *button);
};


// =========== KStringHolder.h ===========

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

#include <windows.h>

/**
	This class holds reference counted string.
*/
class KStringHolder
{
	volatile LONG refCount;
	char *a_text; // ansi version

	#ifndef RFC_NO_SAFE_ANSI_STR
	CRITICAL_SECTION cs_a_text; // to guard ansi string creation
	#endif

public:
	wchar_t *w_text; // unicode version
	int count; // character count

	KStringHolder(wchar_t *w_text, int count);

	~KStringHolder();

	/**
		Make sure to call this method if you contruct new KStringHolder or keep reference to another KStringHolder object.
	*/
	void AddReference();

	/**
		Make sure to call this method if you clear reference to KStringHolder object. it will release allocated memory for string.
	*/
	void ReleaseReference();

	/**
		ANSI version available only when needed.
	*/
	const char* GetAnsiVersion(UINT codePage = CP_UTF8);

private:
	RFC_LEAK_DETECTOR(KStringHolder)
};



// =========== KString.h ===========

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


#include <windows.h>
#include <string.h>
#include <malloc.h>

/**
	Using a reference-counted internal representation, these strings are fast and efficient. <BR>
	KString was optimized to use with unicode strings. So, use unicode strings instead of ansi. <BR>
	KString does not support for multiple zero terminated strings. <BR>

	Optimization tips: <BR>
	use CONST_TXT macro when using statically typed text. <BR>
	use constructor instead of assignment (if can). <BR>
	use "Append" method instead of "+" operator. <BR>
	use "AppendStaticText" method instead of "Append" if you are using statically typed text. <BR>
	use "CompareWithStaticText" method instead of "Compare" if you are comparing statically typed text. <BR>
	define RFC_NO_SAFE_ANSI_STR if your app is not casting KString to ansi string within multiple threads. <BR>

	@code
	KString result1 = str1 + L"1234"; // slow
	KString result2 = str1 + CONST_TXT("1234"); // fast
	KString result3( str1 + CONST_TXT("1234") ); // more fast
	KString result4( str1.Append(CONST_TXT("1234")) ); // bit more fast
	KString result5( str1.AppendStaticText(TXT_WITH_LEN("1234")) ); // that's all you can have ;-)
	@endcode
*/
class KString
{
protected:
	mutable KStringHolder *stringHolder; // for empty string: stringHolder=0 && isStaticText=false
	bool isZeroLength; // true if empty string or staticText, stringHolder are zero length
	mutable bool isStaticText; // staticText & staticTextLength are valid only if this field is true. stringHolder can be zero even this filed is false.
	wchar_t *staticText;
	int staticTextLength;

	void ConvertToRefCountedStringIfStatic()const; // generates StringHolder object from static text

public:

	enum TextTypes
	{
		STATIC_TEXT_DO_NOT_FREE = 1,
		FREE_TEXT_WHEN_DONE = 2,
		USE_COPY_OF_TEXT = 3,
	};

	/**
		Constructs an empty string
	*/
	KString();

	/**
		Constructs copy of another string
	*/
	KString(const KString& other);

	/**
		Constructs String object using ansi string
	*/
	KString(const char* const text, UINT codePage = CP_UTF8);

	/**
		Constructs String object using unicode string
	*/
	KString(const wchar_t* const text, unsigned char behaviour = USE_COPY_OF_TEXT, int length = -1);

	/**
		Constructs String object using integer
	*/
	KString(const int value, const int radix = 10);

	/**
		Constructs String object using float
		@param compact   removes ending decimal zeros if true
	*/
	KString(const float value, const int numDecimals, bool compact = false);

	/** 
		Replaces this string's contents with another string.
	*/
	const KString& operator= (const KString& other);

	/** 
		Replaces this string's contents with unicode string. 
	*/
	const KString& operator= (const wchar_t* const other);


	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const KString& stringToAppend);

	/** 
		Appends a unicode string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const wchar_t* const textToAppend);
	/**
		Returns ansi version of this string
	*/
	operator const char*()const;

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const;

	/**
		Returns unicode version of this string
	*/
	operator wchar_t*()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const char operator[](const int index)const;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	KString Append(const KString& otherString)const;

	/**
		Appends a statically typed string to begining or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to begining if false
		@returns			the concatenated string
	*/
	KString AppendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void AssignStaticText(const wchar_t* const text, int length);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	KString SubString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool CompareIgnoreCase(const KString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool Compare(const KString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool CompareWithStaticText(const wchar_t* const text)const;

	/**
		Compare first character with given unicode character
	*/
	bool StartsWithChar(wchar_t character)const;

	/**
		Compare first character with given ansi character
	*/
	bool StartsWithChar(char character)const;

	/**
		Compare last character with given unicode character
	*/
	bool EndsWithChar(wchar_t character)const;

	/**
		Compare last character with given ansi character
	*/
	bool EndsWithChar(char character)const;

	/**
		Check if string is quoted or not
	*/
	bool IsQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t GetCharAt(int index)const;

	/**
		Returns number of characters in string
	*/
	int GetLength()const;

	/**
		Returns true if string is empty
	*/
	bool IsEmpty()const;

	bool IsNotEmpty()const;

	/**
		Returns value of string
	*/
	int GetIntValue()const;

	/** 
		Returns an upper-case version of this string.
	*/
	KString ToUpperCase()const;

	/** 
		Returns an lower-case version of this string. 
	*/
	KString ToLowerCase()const;

	// free returned buffer when done.
	static char* ToAnsiString(const wchar_t* text);
	static wchar_t* ToUnicodeString(const char* text);

	virtual ~KString();

private:
	RFC_LEAK_DETECTOR(KString)
};

const KString operator+ (const char* const string1, const KString& string2);

const KString operator+ (const wchar_t* const string1, const KString& string2);

const KString operator+ (const KString& string1, const KString& string2);

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) KString(L##X, KString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X))

// do not make a copy + do not free + calculate length
#define STATIC_TXT(X) KString(L##X, KString::STATIC_TEXT_DO_NOT_FREE, -1)

// do not make a copy + free when done + calculate length
#define BUFFER_TXT(X) KString(X, KString::FREE_TEXT_WHEN_DONE, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, KString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)

// =========== KApplication.h ===========

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

#include <windows.h>

/**
	Derive your application object from this class!
	Override Main method and put your application code there.
	Use START_RFC_APPLICATION macro somewhere in a cpp file to declare an instance of this class.

	e.g. @code
	class MyApp : public KApplication
	{
		public:
			MyApp(){}
			~MyApp(){}

			int Main(KString **argv,int argc)
			{
				// your app code goes here...
				return 0;
			}
	};

	START_RFC_APPLICATION(MyApp)
	@endcode
*/
class KApplication
{
public:

	/**
		Use this field to get HINSTANCE of your application.
		(This field will set when calling InitRFC function.)
	*/
	static HINSTANCE hInstance;

	static KDPIAwareness dpiAwareness;
	static bool dpiAwareAPICalled;

	/** 
		Constructs an KApplication object.
	*/
	KApplication();

	/**
		Called before InitRFC function call. Use this method to modify each module InitParams.
		Do not call framework APIs here. Only modify InitParams.
	*/
	virtual void ModifyModuleInitParams();

	/** 
		Called when the application starts.
		Put your application code here and if you create a window, 
		then make sure to call DoMessagePump method before you return.

		@param argv array of command-line arguments! access them like this KString* arg1=argv[0];
		@param argc number of arguments
	*/
	virtual int Main(KString **argv, int argc);

	/**
		Return false if your application is single instance only.
		Single instance applications must implement "GetApplicationID" method.
	*/
	virtual bool AllowMultipleInstances();

	/**
		This method will be called if the application is single instance only and another instance is already running.
		("Main" method will not be called.)
	*/
	virtual int AnotherInstanceIsRunning(KString **argv, int argc);

	/**
		Unique id of your application which is limited to MAX_PATH characters.
		Single instance applications must implement this method.
	*/
	virtual const wchar_t* GetApplicationID();

	static void MessageLoop(bool handleTabKey = true);

	/** 
		Destructs an Application object.
	*/
	virtual ~KApplication();

private:
	RFC_LEAK_DETECTOR(KApplication)
};


// =========== Core.h ===========

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

#include <windows.h>

void InitRFC();
void DeInitRFC();

#define START_RFC_APPLICATION(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	AppClass* application = new AppClass(); \
	application->ModifyModuleInitParams(); \
	::InitRFC(); \
	int argc = 0; \
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc); \
	KString **str_argv = (KString**)::malloc(argc * RFC_PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new KString(args[i], KString::STATIC_TEXT_DO_NOT_FREE);} \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(str_argv, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->Main(str_argv, argc); \
		}else{ \
			retVal = application->AnotherInstanceIsRunning(str_argv, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	for(int i = 0; i < argc; i++){delete str_argv[i];} \
	::DeInitRFC(); \
	::free((void*)str_argv); \
	::GlobalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	AppClass* application = new AppClass(); \
	application->ModifyModuleInitParams(); \
	::InitRFC(); \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->Main(0, 0); \
		}else{ \
			retVal = application->AnotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	::DeInitRFC(); \
	return retVal; \
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef STYLE_MANIFEST_DEFINED
		#ifndef RFC_NO_MANIFEST
			#define STYLE_MANIFEST_DEFINED
			#ifdef RFC64
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#else
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#endif
		#endif
	#endif
#endif

// =========== CoreModule.h ===========

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

#include <windows.h>

class CoreModuleInitParams {
public:
	/**
		If you are in EXE, then hInstance is HINSTANCE provided by WinMain. Or you can use zero.
		If you are in DLL, then hInstance is HINSTANCE provided by DllMain or HMODULE of the DLL. Or you can use zero.
		If you are in Console app, then use zero.
		The defalut value is zero.
	*/
	static HINSTANCE hInstance;

	static bool initCOMAsSTA; // initializes COM as STA. default value is true
	static KDPIAwareness dpiAwareness; // default value is UNAWARE_MODE
};

// =========== KIcon.h ===========

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


/**
	Can be use to load icon from file or resource.
*/
class KIcon
{
protected:
	HICON hIcon;

public:
	KIcon();

	/**
		Loads icon from resource
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads icon from file
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool LoadFromFile(const KString& filePath);

	/**
		Returns icon handle
	*/
	HICON GetHandle();

	operator HICON()const;

	virtual ~KIcon();

private:
	RFC_LEAK_DETECTOR(KIcon)
};


// =========== KGraphics.h ===========

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

class KGraphics
{
public:
	KGraphics();

	virtual ~KGraphics();

	static void Draw3dVLine(HDC hdc, int startX, int startY, int height);

	static void Draw3dHLine(HDC hdc, int startX, int startY, int width);

	static void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color);

	static void FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color);

	static RECT CalculateTextSize(const wchar_t *text, HFONT hFont);

	static int CalculateTextHeight(wchar_t* text, HFONT hFont, int width);

private:
	RFC_LEAK_DETECTOR(KGraphics)
};


// =========== KTransparentBitmap.h ===========

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


#pragma comment(lib, "Msimg32.lib") // AlphaBlend

/**
	Can be use to create 32bit bitmap image from data.
*/
class KTransparentBitmap
{
protected:
	int width;
	int height;
	HDC hdcMem;
	HBITMAP hbm;
	HBITMAP hbmPrev;
	void* pvBits;

	void ReleaseResources();
	void CreateEmptyBitmap(int width, int height);

public:
	// data must be in 0xaarrggbb format with premultiplied alpha.
	// stride must be equal to width * 4
	KTransparentBitmap(void* data, int width, int height, int stride);

	// creates a transparent empty image
	KTransparentBitmap(int width, int height);

	// color format: 0xaarrggbb
	unsigned int GetPixel(int x, int y);

	bool HitTest(int x, int y);

	int GetWidth();

	int GetHeight();

	// also clears the content
	void Resize(int width, int height);

	// use AlphaBlend to draw
	// standard gdi drawing commands may not work with the returned hdc. (content has premultiplied alpha)
	// copy to secondary hdc using AlphaBlend or use gdi+ with PixelFormat32bppPARGB
	HDC GetDC();

	void Draw(HDC destHdc, int destX, int destY, BYTE alpha = 255);

	void Draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha = 255);

	// can copy/scale specific part of the image
	void Draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha = 255);

	virtual ~KTransparentBitmap();

private:
	RFC_LEAK_DETECTOR(KTransparentBitmap)
};




// =========== KFont.h ===========

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


/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class KFont
{
private:
	static KFont* defaultInstance;

protected:
	HFONT hFont;
	bool customFont;
	KString fontFace;
	int fontSizeFor96DPI;
	int currentDPI;
	bool isBold;
	bool isItalic;
	bool isUnderline;
	bool isAntiAliased;

public:
	/**
		Constructs default system Font object.
	*/
	KFont();

	/**
		Constructs custom font object.
		sizeFor96DPI = 14
		antiAliased = false, 
		requiredDPI = USER_DEFAULT_SCREEN_DPI
	*/
	KFont(const KString& face, int sizeFor96DPI, bool bold, 
		bool italic, bool underline, bool antiAliased, int requiredDPI);

	virtual void SetDPI(int newDPI);

	/**
		If you want to use system default font, then use this static method. Do not delete returned object!
	*/
	static KFont* GetDefaultFont();

	// deletes the default font if it already created. for internal use only!
	static void DeleteDefaultFont();

	virtual bool IsDefaultFont();

	/**
		Loads font from a file. make sure to call RemoveFont when done.
	*/
	static bool LoadFont(const KString& path);

	static void RemoveFont(const KString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT GetFontHandle();

	operator HFONT()const;

	virtual ~KFont();

private:
	RFC_LEAK_DETECTOR(KFont)
};


// =========== KMenuItem.h ===========

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


class KMenuItemListener;

class KMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	KMenuItemListener *listener;
	KString itemText;
	bool enabled;
	bool checked;

public:
	KMenuItem();

	virtual void AddToMenu(HMENU hMenu);

	virtual bool IsChecked();

	virtual void SetCheckedState(bool state);

	virtual bool IsEnabled();

	virtual void SetEnabled(bool state);

	virtual void SetText(const KString& text);

	virtual KString GetText();

	virtual UINT GetItemID();

	virtual HMENU GetMenuHandle();

	virtual void SetListener(KMenuItemListener *listener);

	virtual KMenuItemListener* GetListener();

	virtual void OnPress();

	virtual ~KMenuItem();

private:
	RFC_LEAK_DETECTOR(KMenuItem)
};


// =========== KTime.h ===========

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


// __int64 is not defined in mingw.
#ifdef __MINGW32__
	#include <stdint.h>
	typedef int64_t _int64;
#endif

class KTime
{
public:

	KTime(){}

	// Returns time difference in units of 100 us.
	static _int64 Delta100us(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		union timeunion {
			FILETIME fileTime;
			ULARGE_INTEGER ul;
		};

		timeunion ft1;
		timeunion ft2;

		SystemTimeToFileTime(&time1, &ft1.fileTime);
		SystemTimeToFileTime(&time2, &ft2.fileTime);

		return ft2.ul.QuadPart - ft1.ul.QuadPart;
	}

	// Returns time difference in seconds.
	static _int64 DeltaSeconds(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		return (Delta100us(time1, time2) / 10000000);
	}

	// Returns time difference in minutes.
	static _int64 DeltaMinutes(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		return (DeltaSeconds(time1, time2) / 60);
	}

	// Returns time difference in hours.
	static _int64 DeltaHours(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		return (DeltaMinutes(time1, time2) / 60);
	}

	static void GetNow(SYSTEMTIME* time, const bool isLocalTime = true)
	{
		if (isLocalTime)
			::GetLocalTime(time);
		else
			::GetSystemTime(time);
	}

	virtual ~KTime(){}

private:
	RFC_LEAK_DETECTOR(KTime)
};


// =========== KPerformanceCounter.h ===========

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


class KPerformanceCounter
{
protected:
	double pcFreq;
	__int64 counterStart;

public:
	KPerformanceCounter();

	virtual void StartCounter();

	/**
		returns delta time(milliseconds) between StartCounter and EndCounter calls.
	*/
	virtual double EndCounter();

	virtual ~KPerformanceCounter();

private:
	RFC_LEAK_DETECTOR(KPerformanceCounter)
};


// =========== KCursor.h ===========

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


/**
	Can be use to load cursor from file or resource.
*/
class KCursor
{
protected:
	HCURSOR hCursor;

public:
	KCursor();

	/**
		Loads cursor from resource
		@param resourceID resource ID of cursor file
		@returns false if cursor load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads cursor from file
		@param filePath path to cursor file
		@returns false if cursor load fails
	*/
	bool LoadFromFile(const KString& filePath);

	/**
		Returns cursor handle
	*/
	HCURSOR GetHandle();

	/**
		Returns cursor handle
	*/
	operator HCURSOR()const;

	virtual ~KCursor();

private:
	RFC_LEAK_DETECTOR(KCursor)
};


// =========== KGuid.h ===========

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


#pragma comment(lib, "Rpcrt4.lib")

// RPC_WSTR is not defined in mingw.
#ifdef __MINGW32__
	typedef unsigned short* RPC_WSTR;
#endif

class KGuid
{
public:

	KGuid(){}

	static bool GenerateGUID(GUID *pGUID)
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static KString GenerateGUID()
	{
		GUID guid;

		if (KGuid::GenerateGUID(&guid))
			return KGuid::GUIDToString(&guid);

		return KString();
	}

	static KString GUIDToString(GUID *pGUID)
	{
		wchar_t* strGuid = NULL;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		KString result(strGuid, KString::USE_COPY_OF_TEXT);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	virtual ~KGuid(){}

private:
	RFC_LEAK_DETECTOR(KGuid)
};


// =========== KDirectory.h ===========

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

#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class KDirectory
{
public:
	KDirectory();

	static bool IsDirExists(const KString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool CreateDir(const KString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool RemoveDir(const KString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static KString GetModuleDir(HMODULE hModule);

	/**
		returns the parent directory of given file.
	*/
	static KString GetParentDir(const KString& filePath);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static KString GetTempDir();

	/**
		returns the the Application Data directory. if isAllUsers is true this function will return dir shared across all users.
		returns empty string on error.
	*/
	static KString GetApplicationDataDir(bool isAllUsers = false);

	virtual ~KDirectory();

private:
	RFC_LEAK_DETECTOR(KDirectory)
};


// =========== KFile.h ===========

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


// macro to specify file format type in the first 4 bytes of file.
// use with KSettingsReader/Writer classes.
#define KFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

/**
	Can be use to read/write data from a file easily.
*/
class KFile
{
protected:
	KString fileName;
	HANDLE fileHandle;
	bool autoCloseHandle;
	DWORD desiredAccess;

public:
	KFile();

	/** 
		Used in file opening, to specify whether to open as read or write or both.
	*/
	enum FileAccessTypes
	{
		KREAD = GENERIC_READ,
		KWRITE = GENERIC_WRITE,
		KBOTH = GENERIC_READ | GENERIC_WRITE,
	};

	/** 
		If the file does not exist, it will be created.
	*/
	KFile(const KString& fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	/** 
		If the file does not exist, it will be created.
	*/
	virtual bool OpenFile(const KString& fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	virtual bool CloseFile();

	virtual HANDLE GetFileHandle();

	operator HANDLE()const;

	/** 
		fills given buffer and returns number of bytes read.
	*/
	virtual DWORD ReadFile(void* buffer, DWORD numberOfBytesToRead);

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use GetFileSize method. return value will be null on read error.
	*/
	virtual void* ReadAsData();

	virtual KString ReadAsString(bool isUnicode = true);

	/**
		returns number of bytes written.
	*/
	virtual DWORD WriteFile(void* buffer, DWORD numberOfBytesToWrite);

	virtual bool WriteString(const KString& text, bool isUnicode = true);

	virtual bool SetFilePointerToStart();

	/**
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	virtual bool SetFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN);

	virtual DWORD GetFilePointerPosition();

	virtual bool SetFilePointerToEnd();

	/**
		returns zero on error
	*/
	virtual DWORD GetFileSize();

	static bool DeleteFile(const KString& fileName);

	static bool IsFileExists(const KString& fileName);

	static bool CopyFile(const KString& sourceFileName, const KString& destFileName);

	virtual ~KFile();

private:
	RFC_LEAK_DETECTOR(KFile)
};



// =========== KRegistry.h ===========

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


class KRegistry
{

public:
	KRegistry();

	// returns true on success or if the key already exists.
	static bool CreateKey(HKEY hKeyRoot, const KString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool DeleteKey(HKEY hKeyRoot, const KString& subKey);

	static bool ReadString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString *result);

	static bool WriteString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value);

	static bool ReadDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD *result);

	static bool WriteDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void *buffer;
		DWORD bufferSize;
		if(KRegistry::ReadBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool ReadBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void **buffer, DWORD *buffSize);

	static bool WriteBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void *buffer, DWORD buffSize);

	virtual ~KRegistry();

};

// =========== KPointerList.h ===========

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
	T* list;

	CRITICAL_SECTION criticalSection;
	volatile bool isThreadSafe;

public:
	/**
		Constructs PointerList object.
		@param roomIncrement initial and reallocation size of internal memory block in DWORDS
		@param isThreadSafe make all the methods thread-safe
	*/
	KPointerList(const int roomIncrement = 1024, const bool isThreadSafe = false) // 1024*4=4096 = default alignment!
	{
		roomCount = roomIncrement;
		this->roomIncrement = roomIncrement;
		this->isThreadSafe = isThreadSafe;
		size = 0;
		list = (T*)::malloc(roomCount * RFC_PTR_SIZE);
		
		if(isThreadSafe)
			::InitializeCriticalSection(&criticalSection);
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool AddPointer(T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection); // thread safe!

		if(roomCount >= (size + 1) ) // no need reallocation. coz room count is enough!
		{
			list[size] = pointer;
			size++;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;
		}
		else // require reallocation!
		{
			roomCount += roomIncrement;
			void* retVal = ::realloc((void*)list, roomCount * RFC_PTR_SIZE);
			if(retVal)
			{
				list = (T*)retVal;
				list[size] = pointer;
				size++;

				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return true;
			}
			else // memory allocation failed!
			{
				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return false;
			}
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T GetPointer(const int id)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) ) // checks for valid range!
		{	
			T object = list[id];

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return object;
		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return NULL;
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
	bool SetPointer(const int id, T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) )
		{	
			list[id] = pointer;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;
		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return false;
		}
	}

	/**
		Remove pointer of given id
		@returns false if id is out of range!
	*/
	bool RemovePointer(const int id)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) )
		{	
			const int newRoomCount = (((size - 1) / roomIncrement) + 1) * roomIncrement;
			T* newList = (T*)::malloc(newRoomCount * RFC_PTR_SIZE);

			for(int i = 0, j = 0; i < size; i++)
			{
				if(i != id)
				{
					newList[j] = list[i];
					j++;
				}	
			}
			::free((void*)list); // free old list!
			list = newList;
			roomCount = newRoomCount;
			size--;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;

		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return false;
		}

	}

	/**
		Clears the list!
	*/
	void RemoveAll(bool reallocate = true)// remove all pointers from list!
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		::free((void*)list);
		roomCount = roomIncrement;
		list = reallocate ? (T*)::malloc(roomCount * RFC_PTR_SIZE) : NULL;
		size = 0;

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list.
	*/
	void DeleteAll(bool reallocate = true)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		for(int i = 0; i < size; i++)
		{
			T object = list[i];
			delete object;
		}

		::free((void*)list);

		roomCount = roomIncrement;
		list = reallocate ? (T*)::malloc(roomCount * RFC_PTR_SIZE) : NULL;
		size = 0;

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	/**
		Finds the id of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int GetID(T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		for(int i = 0; i < size; i++)
		{
			if (list[i] == pointer)
			{
				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return i;
			}
		}

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);

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
		if (list)
			::free((void*)list);

		if(isThreadSafe)
			::DeleteCriticalSection(&criticalSection);
	}

private:
	RFC_LEAK_DETECTOR(KPointerList)
};


// =========== KBitmap.h ===========

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


/**
	Can be use to load bitmap image from file or resource.
*/
class KBitmap
{
protected:
	HBITMAP hBitmap;

public:
	KBitmap();

	/**
		Loads bitmap image from resource
		@param resourceID resource ID of image
		@returns false if image load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads bitmap image from file
		@param filePath path to image
		@returns false if image load fails
	*/
	bool LoadFromFile(const KString& filePath);

	void DrawOnHDC(HDC hdc, int x, int y, int width, int height);

	/**
		Returns bitmap handle
	*/
	HBITMAP GetHandle();

	/**
		Returns bitmap handle
	*/
	operator HBITMAP()const;

	virtual ~KBitmap();

private:
	RFC_LEAK_DETECTOR(KBitmap)
};




// =========== KSettingsReader.h ===========

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


/**
	High performance configuration reading class.
*/
class KSettingsReader
{
protected:
	KFile settingsFile;

public:
	KSettingsReader();

	virtual bool OpenFile(const KString& fileName, int formatID);

	/**
		read struct, array or whatever...
	*/
	virtual void ReadData(DWORD size, void *buffer);

	virtual KString ReadString();

	virtual int ReadInt();

	virtual float ReadFloat();

	virtual double ReadDouble();

	virtual bool ReadBool();

	virtual ~KSettingsReader();

private:
	RFC_LEAK_DETECTOR(KSettingsReader)
};


// =========== KSettingsWriter.h ===========

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


/**
	High performance configuration writing class.
*/
class KSettingsWriter
{
protected:
	KFile settingsFile;

public:
	KSettingsWriter();

	virtual bool OpenFile(const KString& fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	virtual void WriteData(DWORD size, void *buffer);

	virtual void WriteString(const KString& text);

	virtual void WriteInt(int value);

	virtual void WriteFloat(float value);

	virtual void WriteDouble(double value);

	virtual void WriteBool(bool value);

	virtual ~KSettingsWriter();

private:
	RFC_LEAK_DETECTOR(KSettingsWriter)
};


// =========== UtilsModule.h ===========

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


// =========== KComponent.h ===========

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


/**
	Base class of all W32 gui objects.
*/
class KComponent
{
protected:
	KString compClassName;
	KString compText;
	HWND compHWND;
	HWND compParentHWND;
	DWORD compDwStyle;
	DWORD compDwExStyle;
	UINT compCtlID;
	int compX;
	int compY;
	int compWidth;
	int compHeight;
	int compDPI;
	bool compVisible;
	bool compEnabled;
	bool isRegistered;
	KFont *compFont;
	KCursor *cursor;

public:
	WNDCLASSEXW wc;

	/**
		Constructs a standard win32 component.
		@param generateWindowClassDetails	set to false if you are not registering window class and using standard class name like BUTTON, STATIC etc... wc member is invalid if generateWindowClassDetails is false.
	*/
	KComponent(bool generateWindowClassDetails);
	
	/**
		Returns HWND of this component
	*/
	operator HWND()const;

	/**
		Called after hotplugged into a given HWND.
	*/
	virtual void OnHotPlug();

	/**
		HotPlugs given HWND. this method does not update current compFont and cursor variables.
		Set fetchInfo to true if you want to acquire all the information about this HWND. (width, height, position etc...)
		Set fetchInfo to false if you just need to receive events. (button click etc...)
	*/
	virtual void HotPlugInto(HWND component, bool fetchInfo = true);

	/**
		Sets mouse cursor of this component.
	*/
	virtual void SetMouseCursor(KCursor *cursor);

	/**
		@returns autogenerated unique class name for this component
	*/
	virtual KString GetComponentClassName();

	/**
		Registers the class name and creates the component. 
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		@returns false if registration failed or component creation failed.
	*/
	virtual bool Create(bool requireInitialMessages = false);

	virtual void Destroy();

	/**
		Handles internal window messages. (subclassed window proc)
		Important: Pass unprocessed messages to parent if you override this method.
	*/
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
		Receives messages like WM_COMMAND, WM_NOTIFY, WM_DRAWITEM from the parent window. (if it belongs to this component)
		Pass unprocessed messages to parent if you override this method.
		@returns true if msg processed.
	*/
	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	/**
		Identifier of the child component which can be used with WM_MEASUREITEM like messages.
		@returns zero for top level windows
	*/
	virtual UINT GetControlID();

	/**
		Sets font of this component
	*/
	virtual void SetFont(KFont *compFont);

	/**
		Returns font of this component
	*/
	virtual KFont* GetFont();

	/**
		Returns caption of this component
	*/
	virtual KString GetText();

	/**
		Sets caption of this component
	*/
	virtual void SetText(const KString& compText);

	virtual void SetHWND(HWND compHWND);

	/**
		Returns HWND of this component
	*/
	virtual HWND GetHWND();

	/**
		Changes parent of this component
	*/
	virtual void SetParentHWND(HWND compParentHWND);

	/**
		Returns parent of this component
	*/
	virtual HWND GetParentHWND();

	/**
		Returns style of this component
	*/
	virtual DWORD GetStyle();

	/**
		Sets style of this component
	*/
	virtual void SetStyle(DWORD compStyle);

	/**
		Returns exstyle of this component
	*/
	virtual DWORD GetExStyle();

	/**
		Sets exstyle of this component
	*/
	virtual void SetExStyle(DWORD compExStyle);

	/**
		Returns x position of this component which is relative to parent component.
	*/
	virtual int GetX();

	/**
		Returns y position of this component which is relative to parent component.
	*/
	virtual int GetY();

	/**
		Returns width of the component.
	*/
	virtual int GetWidth();

	/**
		Returns height of the component.
	*/
	virtual int GetHeight();

	virtual int GetDPI();

	/**
		Sets width and height of the component.
	*/
	virtual void SetSize(int compWidth, int compHeight);

	/**
		Sets x and y position of the component. x and y are relative to parent component
	*/
	virtual void SetPosition(int compX, int compY);

	virtual void SetDPI(int newDPI);

	/**
		Sets visible state of the component
	*/
	virtual void SetVisible(bool state);

	/**
		Returns visible state of the component
	*/
	virtual bool IsVisible();

	/**
		Returns the component is ready for user input or not
	*/
	virtual bool IsEnabled();

	/**
		Sets component's user input reading state
	*/
	virtual void SetEnabled(bool state);

	/**
		Brings component to front
	*/
	virtual void BringToFront();

	/**
		Grabs keyboard focus into this component
	*/
	virtual void SetKeyboardFocus();

	/**
		Repaints the component
	*/
	virtual void Repaint();

	virtual ~KComponent();

private:
	RFC_LEAK_DETECTOR(KComponent)
};


// macros to handle window messages

#define BEGIN_KMSG_HANDLER \
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
	{\
	switch(msg)\
	{

#define ON_KMSG(_KMsg,_KMsgHandler) \
	case _KMsg: return _KMsgHandler(wParam,lParam);

#define END_KMSG_HANDLER(_KComponentParentClass) \
	default: return _KComponentParentClass::WindowProc(hwnd,msg,wParam,lParam); \
	}\
	}

// =========== ContainersModule.h ===========

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


// =========== KGridView.h ===========

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


class KGridViewListener;

class KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;
	KGridViewListener *listener;

public:
	KGridView(bool sortItems = false);

	virtual void SetListener(KGridViewListener *listener);

	virtual KGridViewListener* GetListener();

	virtual void InsertRecord(KString **columnsData);

	virtual void InsertRecordTo(int rowIndex, KString **columnsData);

	virtual KString GetRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int GetSelectedRow();

	virtual void RemoveRecordAt(int rowIndex);

	virtual void RemoveAll();

	virtual void UpdateRecordAt(int rowIndex, int columnIndex, const KString& text);

	virtual void SetColumnWidth(int columnIndex, int columnWidth);

	virtual int GetColumnWidth(int columnIndex);

	virtual void CreateColumn(const KString& text, int columnWidth = 100);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool Create(bool requireInitialMessages = false);

	virtual void OnItemSelect();

	virtual void OnItemRightClick();

	virtual void OnItemDoubleClick();

	virtual ~KGridView();
};


// =========== KProgressBar.h ===========

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


class KProgressBar : public KComponent
{
protected:
	int value;

public:
	KProgressBar(bool smooth=true, bool vertical=false);

	virtual int GetValue();

	virtual void SetValue(int value);

	virtual bool Create(bool requireInitialMessages = false);

	virtual ~KProgressBar();
};


// =========== KTextBox.h ===========

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


class KTextBox : public KComponent
{
public:
	KTextBox(bool readOnly = false);

	virtual KString GetText();

	virtual bool Create(bool requireInitialMessages = false);

	virtual ~KTextBox();
};


// =========== KLabel.h ===========

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


class KLabel : public KComponent
{
public:
	KLabel();

	virtual bool Create(bool requireInitialMessages = false);

	virtual ~KLabel();
};


// =========== KTrackBar.h ===========

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


class KTrackBarListener;

class KTrackBar : public KComponent
{
protected:
	int rangeMin,rangeMax,value;
	KTrackBarListener *listener;

public:
	KTrackBar(bool showTicks = false, bool vertical = false);

	/**
		Range between 0 to 100
	*/
	virtual void SetRange(int min, int max);

	virtual void SetValue(int value);

	virtual int GetValue();

	virtual void SetListener(KTrackBarListener *listener);

	virtual void OnChange();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool Create(bool requireInitialMessages = false);

	virtual ~KTrackBar();
};




// =========== KPropertyStorage.h ===========

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

// Readonly data storage model. Use Property Storage - Editor tool to write.
// Can be used when there is no order of storing data objects.

#pragma once

#include <windows.h>

#pragma comment(lib, "Rpcrt4.lib")

#define PS_V1_HEADER    { 'P','S','0','1'}

namespace KPSPropertyTypes
{
    enum KPSPropertyTypes
    {
        STRING = 0,
        INTEGER = 1,
        DWORD = 2,
        FLOAT = 3,
        INT_ARRAY = 4,
        GUID = 5,
        FILE = 6
    };
}

class KPSProperty
{
public:
    // ======== stuff written to file ============
    wchar_t* name;
    int nameLength;

    int type; // KPSPropertyTypes

    wchar_t* strValue;
    int strValueLength;

    int intValue;
    DWORD dwordValue;
    float floatValue;

    int* intArray;
    int intArraySize;

    GUID guidValue;

    wchar_t* fileName;
    int fileNameLength;
    unsigned char* fileData;
    DWORD fileDataSize;
    // ============================================

    KPSProperty()
    {
        name = NULL;
        strValue = NULL;
        fileName = NULL;
        fileData = NULL;
        intArray = NULL;
        
        nameLength = 0;
        strValueLength = 0;
        type = KPSPropertyTypes::INTEGER;
        intValue = 0;
        dwordValue = 0;
        floatValue = 0.0f;
        fileNameLength = 0;
        fileDataSize = 0;
        intArraySize = 0;
    }

    virtual ~KPSProperty()
    {
        if (name)
            ::free(name);
        if (strValue)
            ::free(strValue);
        if (intArray)
            ::free(intArray);
        if (fileName)
            ::free(fileName);
        if (fileData)
            ::free(fileData);
    }
};

class KPSObject
{
public:
    // ======== stuff written to file ============
    wchar_t* name;
    int nameLength;
    GUID objectID;
    // ============================================

    KPointerList<KPSProperty*> propertyList;

    KPSObject(int initialPropertyCount) : propertyList(initialPropertyCount)
    {
        name = NULL;
        nameLength = 0;     
    }

    bool Compare(const GUID& objectID)
    {
        return (::IsEqualGUID(objectID, this->objectID) == TRUE);
    }

    virtual ~KPSObject()
    {
        if (name)
            ::free(name);

        propertyList.DeleteAll(false);
    }
};

class KPSPropertyView : public KPSProperty {

public:
    wchar_t* intArrayStr;
    wchar_t* guidValueStr;

    KPSObject* parentObject;

    KPSPropertyView()
    {
        parentObject = NULL;
        guidValueStr = NULL;
        intArrayStr = NULL;
    }

    virtual ~KPSPropertyView()
    {
        if (intArrayStr)
            ::free(intArrayStr);
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);
    }

    void GenerateIntArrayString()
    {
        if (intArrayStr)
            ::free(intArrayStr);

        intArrayStr = NULL;

        if (intArraySize == 0)
            return;

        intArrayStr = (wchar_t*)::malloc(sizeof(wchar_t) * (16 + 1) * intArraySize);
        intArrayStr[0] = 0;

        for (int i = 0; i < intArraySize; i++)
        {
            wchar_t buffer[16];
            ::_itow_s(intArray[i], buffer, 16, 10);

            ::wcscat_s(intArrayStr, 16, buffer);

            if (i != (intArraySize - 1)) // not the last one
                ::wcscat_s(intArrayStr, 16, L",");
        }
    }

    void GenerateIntArrayByString(wchar_t* text)
    {
        if (intArray)
            ::free(intArray);

        intArray = NULL;
        intArraySize = 0;

        int textLength = (int)::wcslen(text);
        int commaCount = 0;

        for (int i = 0; i < textLength; i++)
        {
            if (text[i] == L',')
                commaCount++;
        }

        intArray = (int*)::malloc(sizeof(int) * (commaCount + 1));

        wchar_t* str = text;
        wchar_t* end = str;

        while (*end) {
            int n = ::wcstol(str, &end, 10);
            intArray[intArraySize] = n;
            intArraySize++;

            if (intArraySize == (commaCount + 1))
                break;

            // skip non numeric characters
            while ((*end != 0) && ((*end < L'0') || (*end > L'9'))) {
                end++;
            }

            str = end;
        }

        this->GenerateIntArrayString();
    }

    void GenerateGUIDValueString()
    {
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);

        guidValueStr = NULL;

        ::UuidToStringW(&guidValue, (RPC_WSTR*)&guidValueStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool GenerateGUIDValueByString(const wchar_t* text)
    {
        bool success = true;

        if (::UuidFromStringW((RPC_WSTR)text, &guidValue) != RPC_S_OK)
        {
            ::UuidFromStringW((RPC_WSTR)L"00000000-0000-0000-0000-000000000000", &guidValue); // default value
            success = false;
        }

        this->GenerateGUIDValueString();
        return success;
    }
};

class KPSObjectView : public KPSObject
{
public:
    wchar_t* objectIDStr; 

    KPSObjectView(int initialPropertyCount) : KPSObject(initialPropertyCount)
    {
        objectIDStr = NULL;
    }

    void GenerateObjectID()
    {
        ::CoCreateGuid(&objectID);
    }

    void GenerateObjectIDString()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);

        objectIDStr = nullptr;

        ::UuidToStringW(&objectID, (RPC_WSTR*)&objectIDStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool GenerateIDByString(const wchar_t* text)
    {
        if (::UuidFromStringW((RPC_WSTR)text, &objectID) != RPC_S_OK)
            return false;

        this->GenerateObjectIDString();
        return true;
    }

    virtual ~KPSObjectView()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);
    }
};

class KPSReader
{
protected:

public:
    KPointerList<KPSObject*> *psObjectList;

    KPSReader()
    {
        psObjectList = NULL;
    }

    // do not free returned object.
    KPSObject* GetPSObject(const GUID& objectID)
    {
        for (int i = 0; i < psObjectList->GetSize(); i++)
        {
            KPSObject* psObject = psObjectList->GetPointer(i);
            if (psObject->Compare(objectID))
                return psObject;
        }

        return NULL;
    }

    bool LoadFromFile(const wchar_t* path, bool readNames = true)
    {
        if (psObjectList)
        {
            psObjectList->DeleteAll(false);
            delete psObjectList;
            psObjectList = NULL;
        }

        HANDLE fileHandle = ::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fileHandle == INVALID_HANDLE_VALUE)
            return false;

        DWORD fileSize = ::GetFileSize(fileHandle, NULL);
        fileSize = (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;

        if (fileSize == 0)
        {
            ::CloseHandle(fileHandle);
            return false;
        }

        char fileHeader[4];
        DWORD bytesRead;
        ::ReadFile(fileHandle, fileHeader, 4, &bytesRead, NULL);

        char psFileHeader[4] = PS_V1_HEADER;

        for (int i = 0; i < 4; ++i)
        {
            if (psFileHeader[i] != fileHeader[i])
            {
                ::CloseHandle(fileHandle);
                return false;
            }
        }

        unsigned int objectCount = 0;
        ::ReadFile(fileHandle, &objectCount, sizeof(unsigned int), &bytesRead, NULL);

        if (objectCount == 0)
        {
            ::CloseHandle(fileHandle);
            return false;
        }

        psObjectList = new KPointerList<KPSObject*>(objectCount);

        for (unsigned int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
        {
            GUID objectID;
            ::ReadFile(fileHandle, &objectID, sizeof(GUID), &bytesRead, NULL);

            int nameLength;
            ::ReadFile(fileHandle, &nameLength, sizeof(int), &bytesRead, NULL);

            wchar_t* objectName = NULL;
            if (readNames)
            {
                objectName = (wchar_t*)::malloc(sizeof(wchar_t) * (nameLength + 1));
                ::ReadFile(fileHandle, objectName, sizeof(wchar_t) * nameLength, &bytesRead, NULL);
                objectName[nameLength] = 0;
            }
            else // ignore name
            {
                ::SetFilePointer(fileHandle, nameLength * sizeof(wchar_t), 0, FILE_CURRENT); 
            }

            unsigned int propertyCount;
            ::ReadFile(fileHandle, &propertyCount, sizeof(unsigned int), &bytesRead, NULL);


            if (propertyCount == 0) // ignore the objects which doesn't have properties.
            {
                if (objectName)
                    ::free(objectName);

                continue;
            }

            KPSObject* psObject = new KPSObject(propertyCount);
            psObject->objectID = objectID;
            psObject->nameLength = nameLength;
            psObject->name = objectName;

            for (unsigned int propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex)
            {
                KPSProperty* psProperty = new KPSProperty();

                ::ReadFile(fileHandle, &psProperty->nameLength, sizeof(int), &bytesRead, NULL);

                if (readNames)
                {
                    psProperty->name = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->nameLength + 1));
                    ::ReadFile(fileHandle, psProperty->name, sizeof(wchar_t) * psProperty->nameLength, &bytesRead, NULL);
                    psProperty->name[psProperty->nameLength] = 0;
                }
                else // ignore name
                {
                    ::SetFilePointer(fileHandle, psProperty->nameLength * sizeof(wchar_t), 0, FILE_CURRENT);
                }

                ::ReadFile(fileHandle, &psProperty->type, sizeof(int), &bytesRead, NULL);

                if (psProperty->type == KPSPropertyTypes::STRING) // string
                {
                    ::ReadFile(fileHandle, &psProperty->strValueLength, sizeof(int), &bytesRead, NULL);
                    psProperty->strValue = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->strValueLength + 1));
                    ::ReadFile(fileHandle, psProperty->strValue, sizeof(wchar_t) * psProperty->strValueLength, &bytesRead, NULL);
                    psProperty->strValue[psProperty->strValueLength] = 0;
                }
                else if (psProperty->type == KPSPropertyTypes::INTEGER) // int
                {
                    ::ReadFile(fileHandle, &psProperty->intValue, sizeof(int), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::DWORD) // DWORD
                {
                    ::ReadFile(fileHandle, &psProperty->dwordValue, sizeof(DWORD), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::FLOAT) // float
                {
                    ::ReadFile(fileHandle, &psProperty->floatValue, sizeof(float), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::INT_ARRAY) // int array
                {
                    ::ReadFile(fileHandle, &psProperty->intArraySize, sizeof(int), &bytesRead, NULL);
                    if (psProperty->intArraySize)
                    {
                        psProperty->intArray = (int*)::malloc(sizeof(int) * psProperty->intArraySize);
                        ::ReadFile(fileHandle, psProperty->intArray, sizeof(int) * psProperty->intArraySize, &bytesRead, NULL);
                    }
                }
                else if (psProperty->type == KPSPropertyTypes::GUID) // guid
                {
                    ::ReadFile(fileHandle, &psProperty->guidValue, sizeof(GUID), &bytesRead, NULL);
                }
                else // file
                {
                    ::ReadFile(fileHandle, &psProperty->fileNameLength, sizeof(int), &bytesRead, NULL);
                    psProperty->fileName = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->fileNameLength + 1));
                    ::ReadFile(fileHandle, psProperty->fileName, sizeof(wchar_t) * psProperty->fileNameLength, &bytesRead, NULL);
                    psProperty->fileName[psProperty->fileNameLength] = 0;
                    ::ReadFile(fileHandle, &psProperty->fileDataSize, sizeof(DWORD), &bytesRead, NULL);
                    if (psProperty->fileDataSize)
                    {
                        psProperty->fileData = (unsigned char*)::malloc(psProperty->fileDataSize);
                        ::ReadFile(fileHandle, psProperty->fileData, psProperty->fileDataSize, &bytesRead, NULL);
                    }
                }

                psObject->propertyList.AddPointer(psProperty);
            }

            psObjectList->AddPointer(psObject);
        }

        ::CloseHandle(fileHandle);
        return true;
    }

    virtual ~KPSReader() 
    {
        if (psObjectList)
        {
            psObjectList->DeleteAll(false);
            delete psObjectList;
        }
    }
};


// =========== KButton.h ===========

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


class KButtonListener;

class KButton : public KComponent
{
protected:
	KButtonListener *listener;

public:
	KButton();

	virtual void SetListener(KButtonListener *listener);

	virtual KButtonListener* GetListener();

	virtual void OnPress();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool Create(bool requireInitialMessages = false);

	virtual ~KButton();
};

// =========== KCheckBox.h ===========

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


class KCheckBox : public KButton
{
protected:
	bool checked;

public:
	KCheckBox();

	virtual bool Create(bool requireInitialMessages = false);

	virtual void OnPress();

	virtual bool IsChecked();

	virtual void SetCheckedState(bool state);

	virtual ~KCheckBox();
};


// =========== KComboBox.h ===========

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



class KComboBoxListener;

class KComboBox : public KComponent
{
protected:
	KPointerList<KString*> *stringList;
	int selectedItemIndex;
	KComboBoxListener *listener;

public:
	KComboBox(bool sort=false);

	virtual void AddItem(const KString& text);

	virtual void RemoveItem(int index);

	virtual void RemoveItem(const KString& text);

	virtual int GetItemIndex(const KString& text);

	virtual int GetItemCount();

	virtual int GetSelectedItemIndex();

	virtual KString GetSelectedItem();

	virtual void ClearList();

	virtual void SelectItem(int index);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool Create(bool requireInitialMessages = false);

	virtual void SetListener(KComboBoxListener *listener);

	virtual void OnItemSelect();

	virtual ~KComboBox();
};


// =========== KListBox.h ===========

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


class KListBoxListener;

class KListBox : public KComponent
{
protected:
	KPointerList<KString*> *stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

	KListBoxListener *listener;

public:
	KListBox(bool multipleSelection=false, bool sort=false, bool vscroll=true);

	virtual void SetListener(KListBoxListener *listener);

	virtual void AddItem(const KString& text);

	virtual void RemoveItem(int index);

	virtual void RemoveItem(const KString& text);

	virtual int GetItemIndex(const KString& text);

	virtual int GetItemCount();

	virtual int GetSelectedItemIndex();

	virtual KString GetSelectedItem();

	virtual int GetSelectedItems(int* itemArray, int itemCountInArray);

	virtual void ClearList();

	virtual void SelectItem(int index);

	virtual void SelectItems(int start, int end);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool Create(bool requireInitialMessages = false);

	virtual void OnItemSelect();

	virtual void OnItemDoubleClick();

	virtual ~KListBox();
};


// =========== KGUIProc.h ===========

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


// all the methods must be called only from the gui thread.
class KGUIProc
{
public:
	static ATOM AtomComponent;
	static ATOM AtomOldProc;

	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

	/**
		set requireInitialMessages to true to receive initial messages lke WM_CREATE... (installs a hook)
	*/
	static HWND CreateComponent(KComponent* component, bool requireInitialMessages);

	/**
		hwnd can be window, custom control, dialog or common control.
		hwnd will be subclassed if it is a common control or dialog.
	*/
	static void AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component);

	static int HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
	static HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
};


// =========== KLogger.h ===========

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



/**
	Super fast logging class for logging within a (audio)loop. (not thread safe)
	Writes logging data into ram & dumps data into file when needed.
	You can use this class instead of OutputDebugString API.(OutputDebugString is too slow & ETW is too complex?)
	Use Log Viewer tool to view generated log file.

	Log File Format:
		file header:				'R' 'L' 'O' 'G'
		event count:				int32					; (event count)
		event start packet format:	byte|short16|short16	; (event type|secs|mills)
		event param number format:	byte|data				; (param type|data)
		event param string format:	byte|byte|data			; (param type|data size[max 255]|data)
		event end packet format:	byte					; (EVT_END)
*/
class KLogger
{
protected:
	DWORD bufferSize;
	DWORD bufferIndex;
	unsigned int totalMills;
	unsigned int totalEvents;
	char *buffer;
	bool bufferFull, isFirstCall;
	KPerformanceCounter pCounter;

public:

	enum ByteSizes
	{
		SZ_MEGABYTE = 1024 * 1024,
		SZ_KILOBYTE = 1024,
	};

	enum EventTypes
	{
		EVT_END = 0,
		EVT_INFORMATION = 1,
		EVT_WARNING = 2,
		EVT_ERROR = 3,
	};

	enum ParamTypes
	{
		// skipped value zero. because parser will fail to recognize EVT_END.
		PARAM_STRING = 1,
		PARAM_INT32 = 2,
		PARAM_SHORT16 = 3,
		PARAM_FLOAT = 4,
		PARAM_DOUBLE = 5,
	};

	KLogger(DWORD bufferSize = (SZ_MEGABYTE * 10));

	virtual bool WriteNewEvent(unsigned char eventType = EVT_INFORMATION);

	virtual bool EndEvent();

	/**
		textLength is number of chars. max value is 255.
	*/
	virtual bool AddTextParam(const char *text, unsigned char textLength);

	virtual bool AddIntParam(int value);

	virtual bool AddShortParam(unsigned short value);

	virtual bool AddFloatParam(float value);
	
	virtual bool AddDoubleParam(double value);

	virtual bool IsBufferFull();

	virtual bool WriteToFile(const KString &filePath);

	virtual ~KLogger();

private:
	RFC_LEAK_DETECTOR(KLogger)
};


// =========== KWindow.h ===========

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


#define RFC_CUSTOM_MESSAGE WM_APP + 100

class KDPIChangeListener
{
public:
	virtual void OnDPIChange(HWND hwnd, int newDPI) = 0;
};

class KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;
	KDPIChangeListener* dpiChangeListener;
	bool enableDPIUnawareMode;
	KPointerList<KComponent*> componentList;

public:
	KWindow();

	virtual bool Create(bool requireInitialMessages = false);

	virtual void Flash();

	virtual void SetIcon(KIcon *icon);

	virtual void OnClose();

	virtual void OnDestroy();

	// Custom messages are used to send a signal/data from worker thread to gui thread.
	virtual void PostCustomMessage(WPARAM msgID, LPARAM param);

	virtual void OnCustomMessage(WPARAM msgID, LPARAM param);

	virtual void CenterScreen();

	/**
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool AddComponent(KComponent *component, bool requireInitialMessages = false);

	virtual void RemoveComponent(KComponent* component);

	virtual bool SetClientAreaSize(int width, int height);

	virtual void SetDPIChangeListener(KDPIChangeListener* dpiChangeListener);

	// Mixed-Mode DPI Scaling - window scaled by the system. can only call before create.
	// InitRFC must be called with KDPIAwareness::MIXEDMODE_ONLY
	// Only works with Win10 or higher
	virtual void SetEnableDPIUnawareMode(bool enable);

	static bool IsOffScreen(int posX, int posY);

	virtual bool GetClientAreaSize(int *width, int *height);

	virtual void OnMoved();

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if the window and controls are in different dpi scale. (use KDPIChangeListener)
	virtual void OnResized();

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual ~KWindow();
};


// =========== KTimer.h ===========

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



/**
	Encapsulates a timer.

	The timer can be started with the StartTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling SetTimerWindow method.
*/
class KTimer
{
protected:
	UINT timerID;
	int resolution;
	bool started;
	KWindow *window;
	KTimerListener *listener;

public:

	KTimer();

	/**
		@param resolution timer interval
	*/
	virtual void SetInterval(int resolution);

	virtual int GetInterval();

	/**
		Call this method before you start timer
	*/
	virtual void SetTimerWindow(KWindow *window);

	virtual void SetTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	virtual UINT GetTimerID();

	/**
		Starts timer
	*/
	virtual void StartTimer();

	/**
		Stops the timer. You can restart it by calling StartTimer() method.
	*/
	virtual void StopTimer();

	virtual void SetListener(KTimerListener *listener);

	virtual bool IsTimerRunning();

	virtual void OnTimer();

	virtual ~KTimer();

private:
	RFC_LEAK_DETECTOR(KTimer)
};


// =========== KIDGenerator.h ===========

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

#include <stdio.h>

/**
	Singleton class which can be used to generate class names, timer ids etc...
	Methods are not thread safe. So, only call from the gui thread.
	(this class is for internal use)
*/
class KIDGenerator
{
private:
	RFC_LEAK_DETECTOR(KIDGenerator)

	static KIDGenerator*_instance;
	KIDGenerator();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	KPointerList<KMenuItem*> *menuItemList;
	KPointerList<KTimer*> *timerList;

public:

	static KIDGenerator* GetInstance();

	UINT GenerateControlID();

	// KApplication:hInstance must be valid before calling this method
	KString GenerateClassName();

	UINT GenerateMenuItemID(KMenuItem *menuItem);
	KMenuItem* GetMenuItemByID(UINT id);

	UINT GenerateTimerID(KTimer *timer);
	KTimer* GetTimerByID(UINT id);

	~KIDGenerator();

};

// =========== FileModule.h ===========

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




// =========== KGroupBox.h ===========

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


class KGroupBox : public KButton
{
public:
	KGroupBox();

	virtual ~KGroupBox();
};


// =========== KRadioButton.h ===========

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


class KRadioButton : public KCheckBox
{
public:
	KRadioButton();

	virtual ~KRadioButton();
};


// =========== KNumericField.h ===========

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


class KNumericField : public KTextBox
{
public:
	KNumericField();

	virtual ~KNumericField();
};


// =========== KPasswordBox.h ===========

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


class KPasswordBox : public KTextBox
{
protected:
	wchar_t pwdChar;

public:
	KPasswordBox(bool readOnly=false);

	virtual void SetPasswordChar(const wchar_t pwdChar);

	virtual wchar_t GetPasswordChar();

	virtual bool Create(bool requireInitialMessages = false);

	virtual ~KPasswordBox();
};


// =========== KPushButton.h ===========

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


class KPushButton : public KCheckBox
{
public:
	KPushButton();

	virtual ~KPushButton();
};


// =========== KGlyphButton.h ===========

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


class KGlyphButton : public KButton
{
protected:
	KFont *glyphFont;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KGlyphButton();

	virtual ~KGlyphButton();

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual void SetDPI(int newDPI);
};


// =========== KTextArea.h ===========

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


class KTextArea : public KTextBox
{
public:
	KTextArea(bool autoScroll = false, bool readOnly = false);

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual ~KTextArea();
};


// =========== KWindowTypes.h ===========

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


class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual ~KHotPluggedDialog();
};

class KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow();

	virtual ~KOverlappedWindow();
};

class KFrame : public KWindow
{
public:
	KFrame();

	virtual ~KFrame();
};

class KDialog : public KWindow
{
public:
	KDialog();

	virtual ~KDialog();
};

class KToolWindow : public KWindow
{
public:
	KToolWindow();

	virtual ~KToolWindow();
};


// =========== KToolTip.h ===========

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


class KToolTip : public KComponent
{
protected:
	HWND attachedCompHWND;

public:
	KToolTip();

	virtual ~KToolTip();

	/**
		"parentWindow" must be created before you call this method.
		"attachedComponent" must be added to a window before you call this method.
		do not attach same tooltip into multiple components.
	*/
	virtual void AttachToComponent(KWindow *parentWindow, KComponent *attachedComponent);

	/**
		calling this method has no effect.
	*/
	virtual bool Create(bool requireInitialMessages = false);

	virtual void SetText(const KString& compText);
};




// =========== KMenu.h ===========

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


class KMenu
{
protected:
	HMENU hMenu;

public:
	KMenu();

	virtual void AddMenuItem(KMenuItem *menuItem);

	virtual void AddSubMenu(const KString& text, KMenu *menu);

	virtual void AddSeperator();

	virtual HMENU GetMenuHandle();

	virtual void PopUpMenu(KWindow *window);

	virtual ~KMenu();

private:
	RFC_LEAK_DETECTOR(KMenu)
};


// =========== KCommonDialogBox.h ===========

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


// generates filter text for KFILE_FILTER("Text Files", "txt") as follows: L"Text Files\0*.txt\0"
#define KFILE_FILTER(desc, ext) L##desc L"\0*." L##ext L"\0"

class KCommonDialogBox
{
public:
	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool ShowOpenFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName, bool saveLastLocation = false, const wchar_t* dialogGuid = 0);

	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool ShowSaveFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName, bool saveLastLocation = false, const wchar_t* dialogGuid = 0);
};


// =========== KMenuButton.h ===========

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


class KMenuButton : public KButton
{
protected:
	KFont *arrowFont;
	KFont *glyphFont;
	KMenu *buttonMenu;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KMenuButton();

	virtual ~KMenuButton();

	virtual void SetMenu(KMenu *buttonMenu);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual void SetDPI(int newDPI);

	virtual void OnPress();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);
};


// =========== KMenuBar.h ===========

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


class KMenuBar
{
protected:
	HMENU hMenu;

public:
	KMenuBar();

	virtual void AddMenu(const KString& text, KMenu *menu);

	virtual void AddToWindow(KWindow *window);

	virtual ~KMenuBar();

private:
	RFC_LEAK_DETECTOR(KMenuBar)
};


// =========== GUIModule.h ===========

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


#pragma comment(lib, "Comctl32.lib")

#endif

