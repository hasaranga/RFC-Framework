
// ========== RFC Generator v1.0 - 2025-06-10 18:04 PM ==========

#ifndef _RFC_H_
#define _RFC_H_ 

#define AMALGAMATED_VERSION


// =========== KAssert.h ===========

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

#include <crtdbg.h>

// spawns crt assertion error gui if condition is false.
#ifdef _DEBUG
#define K_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, nullptr, msg); \
            _CrtDbgBreak(); \
        } \
    } while (0)
#else
#define K_ASSERT(cond, msg) ((void)0)
#endif

// =========== KModuleManager.h ===========

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

typedef bool (*RFCModuleInitFunc)();
typedef void (*RFCModuleFreeFunc)();

#define MAX_RFC_MODULE_COUNT 3

class KModuleManager {
public:
	static bool registerRFCModule(int index, RFCModuleInitFunc initFunc, RFCModuleFreeFunc freeFunc);
	static RFCModuleInitFunc* rfcModuleInitFuncList();
	static RFCModuleFreeFunc* rfcModuleFreeFuncList();
};

#define REGISTER_RFC_MODULE(index, ModuleObjectType) \
static bool ModuleObjectType##_Registered = KModuleManager::registerRFCModule( index , ModuleObjectType::rfcModuleInit, ModuleObjectType::rfcModuleFree);

// =========== KLeakDetector.h ===========

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
		::InterlockedIncrement(&getCounter().numObjects);
	}

	KLeakDetector(const KLeakDetector&)
	{
		::InterlockedIncrement(&getCounter().numObjects);
	}

	~KLeakDetector()
	{
		::InterlockedDecrement(&getCounter().numObjects);
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
				::strcat_s(textBuffer, getLeakedClassName());

				::MessageBoxA(0, textBuffer, "Warning", MB_ICONWARNING);
			}
		}

		volatile long numObjects;
	};

	static const char* getLeakedClassName()
	{
		return T::rfc_GetLeakedClassName();
	}

	static LeakCounter& getCounter()
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

#include <windows.h>
#include <shellscalingapi.h>

typedef HRESULT(WINAPI* KGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
typedef BOOL(WINAPI* KSetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
typedef HRESULT(STDAPICALLTYPE* KSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
typedef BOOL (WINAPI* KSetProcessDPIAware)(VOID);
typedef DPI_AWARENESS_CONTEXT (WINAPI* KSetThreadDpiAwarenessContext) (DPI_AWARENESS_CONTEXT dpiContext);
typedef BOOL(WINAPI* KAdjustWindowRectExForDpi)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);


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
private: 
    static float getMonitorScalingRatio(HMONITOR monitor);
public:		
	static KGetDpiForMonitor pGetDpiForMonitor;
	static KSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext;
	static KSetProcessDpiAwareness pSetProcessDpiAwareness;
	static KSetProcessDPIAware pSetProcessDPIAware;
	static KSetThreadDpiAwarenessContext pSetThreadDpiAwarenessContext;
    static KAdjustWindowRectExForDpi pAdjustWindowRectExForDpi;

	static void initDPIFunctions();

    // returns dpi of monitor which our window is in. returns 96 if application is not dpi aware.
	static WORD getWindowDPI(HWND hWnd);

    // automatically fall back to AdjustWindowRectEx when lower than win10
    static BOOL adjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);

	static void makeProcessDPIAware(KDPIAwareness dpiAwareness);

    // gives real value regardless of the process dpi awareness state.
    // if the process is dpi unaware, os will always give 96dpi.
    // so, this method will return correct scale value.
    // it can be used with dpi unaware apps to get the scale of a monitor.
    // https://stackoverflow.com/questions/70976583/get-real-screen-resolution-using-win32-api
    /*
        Example:
        float monitorScale = 1.0f;
     	HMONITOR hmon = ::MonitorFromWindow(compHWND, MONITOR_DEFAULTTONEAREST);
		if (hmon != NULL)
			monitorScale = KDPIUtility::getScaleForMonitor(hmon);
    */
    static float getScaleForMonitor(HMONITOR monitor);

    // scale given 96dpi value according to window current dpi.
    static int scaleToWindowDPI(int valueFor96DPI, HWND window);

    // scale given 96dpi value according to new dpi.
    static int scaleToNewDPI(int valueFor96DPI, int newDPI);
};


// =========== Architecture.h ===========

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

#define RFC_PTR_SIZE sizeof(void*) 

#ifdef _WIN64
	#define RFC64
	#define RFC_NATIVE_INT __int64
#else
	#define RFC32
	#define RFC_NATIVE_INT int
#endif


// =========== KScopedStructPointer.h ===========

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

#include <malloc.h>
#include <Objbase.h>

template<class StructType>
class KReleaseUsingFree
{
public:
	static void release(StructType* structPtr)
	{
		::free(structPtr);
	}
};

template<class StructType>
class KReleaseUsingTaskMemFree
{
public:
	static void release(StructType* memory)
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
		structPointer = nullptr;
	}

	inline KScopedStructPointer(StructType* structPointer)
	{
		this->structPointer = structPointer;
	}

	KScopedStructPointer(KScopedStructPointer& structPointerToTransferFrom)
	{
		this->structPointer = structPointerToTransferFrom.structPointer;
		structPointerToTransferFrom.structPointer = nullptr;
	}

	bool isNull()
	{
		return (structPointer == nullptr);
	}

	/** 
		Removes the current struct pointer from this KScopedStructPointer without freeing it.
		This will return the current struct pointer, and set the KScopedStructPointer to a null pointer.
	*/
	StructType* detach()
	{ 
		StructType* m = structPointer;
		structPointer = nullptr;
		return m; 
	}

	~KScopedStructPointer()
	{
		if (structPointer)
			ReleaseMethod::release(structPointer);
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
				ReleaseMethod::release(oldStructPointer);
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


// =========== KScopedMemoryBlock.h ===========

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
	can use the detach() method.

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
		memoryBlock = nullptr;
	}

	inline KScopedMemoryBlock(T memoryBlock)
	{
		this->memoryBlock = memoryBlock;
	}

	KScopedMemoryBlock(KScopedMemoryBlock& memoryBlockToTransferFrom)
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = nullptr;
	}

	bool isNull()
	{
		return (memoryBlock == nullptr);
	}

	/** 
		Removes the current memory block from this KScopedMemoryBlock without freeing it.
		This will return the current memory block, and set the KScopedMemoryBlock to a null pointer.
	*/
	T detach()
	{ 
		T m = memoryBlock;
		memoryBlock = nullptr;
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


// =========== KScopedMallocPointer.h ===========

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

	bool isNull()
	{
		return (pointer == nullptr);
	}

	/** 
		Removes the current pointer from this KScopedMallocPointer without freeing it.
		This will return the current pointer, and set the KScopedMallocPointer to a null pointer.
	*/
	PointerType* detach()
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


// =========== KStaticAllocator.h ===========

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

#include <cstddef>
#include <new>
#include <atomic>

#ifndef KSTATIC_POOL_SIZE
    #define KSTATIC_POOL_SIZE 520
#endif

// thread-safe static allocation. (Lock-free)
class KStaticAllocator
{
private:
    static constexpr size_t POOL_SIZE = KSTATIC_POOL_SIZE; // 1MB pool
    static char memory_pool[POOL_SIZE];
    static std::atomic<size_t> current_offset;

public:
    // once allocated, returned buffer will stay until the application exit.
    // returns nullptr if KSTATIC_POOL_SIZE is not enough.
    static void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    
    static void reset();
};


// =========== KScopedGdiObject.h ===========

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

#include <windows.h>

/**
	This class holds a gdi object which is automatically freed when this object goes
	out of scope.

	If you need to get a gdi object out of a KScopedGdiObject without it being freed, you
	can use the detach() method.

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
	T detach()
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

#include <windows.h>

/**
	This class holds a pointer to CRITICAL_SECTION which is automatically released when this object goes
	out of scope.
*/
class KScopedCriticalSection
{
private:
	CRITICAL_SECTION* criticalSection;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedCriticalSection(CRITICAL_SECTION* criticalSection)
	{
		this->criticalSection = criticalSection;
		::EnterCriticalSection(criticalSection);
	}

	// does not call LeaveCriticalSection
	CRITICAL_SECTION* detach()
	{ 
		CRITICAL_SECTION* c = criticalSection;
		criticalSection = nullptr;
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
		object = nullptr;
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

	bool isNull()
	{
		return (object == nullptr);
	}

	/** 
		Removes the current COM object from this KScopedComPointer without releasing it.
		This will return the current object, and set the KScopedComPointer to a null pointer.
	*/
	T* detach()
	{ 
		T* o = object; 
		object = nullptr;
		return o; 
	}

	~KScopedComPointer()
	{
		if (object)
			object->Release();

		object = nullptr;
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
	can use the detach() method.

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
		object = nullptr;
	}

	inline KScopedClassPointer(T* object)
	{
		this->object = object;
	}

	KScopedClassPointer(KScopedClassPointer& objectToTransferFrom)
	{
		this->object = objectToTransferFrom.object;
		objectToTransferFrom.object = nullptr;
	}

	bool isNull()
	{
		return (object == nullptr);
	}

	/** 
		Removes the current object from this KScopedClassPointer without deleting it.
		This will return the current object, and set the KScopedClassPointer to a null pointer.
	*/
	T* detach()
	{ 
		T* o = object; 
		object = nullptr;
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

// =========== KScopedHandle.h ===========

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

	HANDLE detach()
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

	bool isNull()
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


// =========== KRefCountedMemory.h ===========

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

#include <windows.h>

/**
	This class holds reference counted heap memory which is allocated using malloc.
	when ref count reach zero, the memory will be released using ::free.
*/
template<class T>
class KRefCountedMemory
{
private:
	~KRefCountedMemory() {}

protected:
	volatile LONG refCount;

public:
	T buffer;

	KRefCountedMemory(T buffer) : refCount(1), buffer(buffer) {}
	
	/**
		Make sure to call this method if you construct new KRefCountedMemory or keep reference to another KRefCountedMemory object.
	*/
	void addReference()
	{
		::InterlockedIncrement(&refCount);
	}

	/**
		Make sure to call this method if you clear reference to KRefCountedMemory object. 
		it will release allocated memory for string if ref count is zero.
	*/
	void releaseReference()
	{
		const LONG res = ::InterlockedDecrement(&refCount);
		if (res == 0)
		{
			if (buffer)
				::free(buffer);

			delete this;
		}
	}

private:
	RFC_LEAK_DETECTOR(KRefCountedMemory)
};



// =========== KString.h ===========
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


#include <windows.h>
#include <string.h>
#include <malloc.h>

/*
	DO_NOT_FREE: supplied pointer is a static string literal(always available). it will not freed on destroy.
	FREE_ON_DESTROY: supplied pointer is a heap memory created using malloc. it will be freed on destroy.
	MAKE_A_COPY: make a copy of supplied string. if string length is larger than 11, heap memory will be allocated.
*/
enum class KStringBehaviour { DO_NOT_FREE,
	FREE_ON_DESTROY, 
	MAKE_A_COPY
};

// define RFC_NO_CHECK_ARRAY_AS_LITERAL if you want to disable the check for array passed as string literal at debug mode.
#ifndef RFC_NO_CHECK_ARRAY_AS_LITERAL
#define RFC_CHECK_ARRAY_AS_LITERAL(literal,N) K_ASSERT(wcslen(literal) == (N - 1), "array used as a string literal. please use the array with KString constructor that accepts a behavior parameter.");
#else
#define RFC_CHECK_ARRAY_AS_LITERAL(literal,N) 
#endif

enum class KStringBufferType : unsigned char { StaticText, SSOText, HeapText };

/**
	Using a reference-counted internal representation for long strings and Small String Optimization (SSO) for short strings.
	KString was optimized to use with unicode strings. So, use unicode strings instead of ansi.
	KString does not support for multiple zero terminated strings.

	SSO Implementation:
	- Strings with 11 characters or fewer are stored directly in the object (Small String Optimization)
	- Longer strings use the heap with KRefCountedMemory mechanism
	- Static text references just use pointer assignment

	Optimization tips:
	use unicode strings instead of ansi.
	try to use strings with length less than 12.
	L"hello"_st is same as CONST_TXT("hello") or KString(L"hello",KString::STATIC_TEXT_DO_NOT_FREE,5)
	use "CompareWithStaticText" method instead of "Compare" if you are comparing statically typed text.

*/
class KString
{
public:
	// SSO buffer size: can fit up to 11 wchar_t characters + null terminator in 24 bytes
	static const int SSO_BUFFER_SIZE = 12;

protected:
	// we try to make sizeof KString to be 32 bytes for better cache align.

	// you can use either data.ssoBuffer or data.staticText or data.refCountedMem. 
	// can use only one at a time. selected by the bufferType.
	union {
		KRefCountedMemory<wchar_t*>* refCountedMem;
		const wchar_t* staticText;
		wchar_t ssoBuffer[SSO_BUFFER_SIZE]; // for small strings
	} data; 

	int characterCount; // character count (empty string has zero characterCount)
	KStringBufferType bufferType;

	inline void markAsEmptyString();

	void initFromLiteral(const wchar_t* literal, size_t N);
	void assignFromLiteral(const wchar_t* literal, size_t N);
	void copyFromOther(const KString& other);
public:

	/**
		Constructs an empty string
	*/
	KString();

	/**
		Constructs copy of another string.
		Same performance as move. lightweight!
	*/
	KString(const KString& other);

	/**
		Move constructor. Same performance as copy. other string will be cleared.
	*/
	KString(KString&& other) noexcept;

	/**
		Constructs String object using ansi string
	*/
	KString(const char* const text, UINT codePage = CP_UTF8);

	/**
		Constructs String object using unicode string literal
	*/
	template<size_t N>
	KString(const wchar_t(&literal)[N])
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literal, N);
		initFromLiteral(literal, N);
	}

	/**
		Constructs String object using unicode string pointer
	*/
	KString(const wchar_t* const text, KStringBehaviour behaviour, int length = -1);

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

	// Move assignment. clears other string.
	KString& operator= (KString&& other);

	/**
		Replaces this string's contents with static unicode string literal.
	*/
	template<size_t N>
	const KString& operator= (const wchar_t(&literal)[N])
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literal, N);
		assignFromLiteral(literal, N);
		return *this;
	}

	// compare with other string
	bool operator==(const KString& other) const;

	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const KString& stringToAppend);

	/**
		Appends a unicode string literal at the end of this one.
		@returns     the concatenated string
	*/
	template<size_t N>
	const KString operator+ (const wchar_t(&literalToAppend)[N])
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literalToAppend, N);
		return appendStaticText(literalToAppend, (int)N - 1);
	}

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const wchar_t operator[](const int index)const;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	KString append(const KString& otherString)const;

	/**
		Appends a statically typed string to beginning or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to beginning if false
		@returns			the concatenated string
	*/
	KString appendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void assignStaticText(const wchar_t* const text, int length);

	// clears the content of the string.
	void clear();

	// the string automatically clears and converted to SSOText when you call accessRawSSOBuffer.
	// sso buffer size is KString::SSO_BUFFER_SIZE in wchars.
	void accessRawSSOBuffer(wchar_t** ssoBuffer, int** ppLength);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	KString subString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool compareIgnoreCase(const KString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool compare(const KString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool compareWithStaticText(const wchar_t* const text)const;

	/**
		Compare first character with given unicode character
	*/
	bool startsWithChar(wchar_t character)const;

	/**
		Compare last character with given unicode character
	*/
	bool endsWithChar(wchar_t character)const;

	/**
		Check if string is quoted or not
	*/
	bool isQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t getCharAt(int index)const;

	KStringBufferType getBufferType()const;

	/**
		Returns number of characters in string
	*/
	int length()const;

	/**
		Returns true if string is empty
	*/
	bool isEmpty()const;

	bool isNotEmpty()const;

	/**
		Returns value of string
	*/
	int getIntValue()const;

	/** 
		Returns an upper-case version of this string.
	*/
	KString toUpperCase()const;

	/** 
		Returns an lower-case version of this string. 
	*/
	KString toLowerCase()const;

	// free the returned buffer when done.
	static char* toAnsiString(const wchar_t* text);
	static wchar_t* toUnicodeString(const char* text);

	~KString();

private:
	/**
		Returns pointer to the actual string data regardless of storage type
	*/
	const wchar_t* getStringPtr() const;

	RFC_LEAK_DETECTOR(KString)
};

// static text literal operator
namespace kstring_literals {
	KString operator"" _st(const wchar_t* str, size_t len);
}

using namespace kstring_literals;

const KString operator+ (const char* const string1, const KString& string2);

const KString operator+ (const wchar_t* const string1, const KString& string2);

const KString operator+ (const KString& string1, const KString& string2);

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) KString(L##X, KStringBehaviour::DO_NOT_FREE, LEN_UNI_STR(L##X))

// do not make a copy + do not free + calculate length
#define STATIC_TXT(X) KString(L##X, KStringBehaviour::DO_NOT_FREE, -1)

// do not make a copy + free when done + calculate length
#define BUFFER_TXT(X) KString(X, KStringBehaviour::FREE_ON_DESTROY, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, KStringBehaviour::DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)

// =========== KApplication.h ===========

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

			int main(wchar_t** argv,int argc)
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
	virtual void modifyModuleInitParams();

	/** 
		Called when the application starts.
		Put your application code here and if you create a window, 
		then make sure to call messageLoop method before you return.

		@param argv array of command-line arguments! access them like this wchar_t* arg1=argv[0];
		@param argc number of arguments
	*/
	virtual int main(wchar_t** argv, int argc);

	/**
		Return false if your application is single instance only.
		Single instance applications must implement "getApplicationID" method.
	*/
	virtual bool allowMultipleInstances();

	/**
		This method will be called if the application is single instance only and another instance is already running.
		("main" method will not be called.)
	*/
	virtual int anotherInstanceIsRunning(wchar_t** argv, int argc);

	/**
		Unique id of your application which is limited to MAX_PATH characters.
		Single instance applications must implement this method.
	*/
	virtual const wchar_t* getApplicationID();

	static void messageLoop(bool handleTabKey = true);

	/** 
		Destructs an Application object.
	*/
	virtual ~KApplication();

private:
	RFC_LEAK_DETECTOR(KApplication)
};


// =========== Core.h ===========

/*
	Copyright (C) 2013-2025  CrownSoft

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

void InitRFCModules();
void DeInitRFCModules();

// use within a dll functions. do not use inside of DllMain.
void RFCDllInit();
void RFCDllFree();

#define RFC_MAX_PATH 512

#define START_RFC_CONSOLE_APP(AppClass) \
int main() \
{ \
	CoreModuleInitParams::hInstance = 0; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE; \
	int retVal = 0; \
	LPWSTR* args = nullptr; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	int argc = 0; \
	args = ::CommandLineToArgvW(::GetCommandLineW(), &argc); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(args, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(args, argc); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(args, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	} ::DeInitRFCModules(); \
	::LocalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_CONSOLE_APP_NO_CMD_ARGS(AppClass) \
int WINAPI main() \
{ \
	CoreModuleInitParams::hInstance = 0; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE; \
	int retVal = 0; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(0, 0); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	}::DeInitRFCModules(); \
	return retVal; \
}

#define START_RFC_APPLICATION(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	int retVal = 0; \
	LPWSTR* args = nullptr; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	int argc = 0; \
	args = ::CommandLineToArgvW(::GetCommandLineW(), &argc); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(args, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(args, argc); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(args, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	}\
	::DeInitRFCModules(); \
	::LocalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	int retVal = 0; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(0, 0); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	} ::DeInitRFCModules(); \
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


#ifdef _DEBUG
	#define DEBUG_PRINT(x) OutputDebugStringA(x);
#else 
	#define DEBUG_PRINT(x) 
#endif

// =========== CoreModule.h ===========

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

#include <windows.h>

// link default libs here so we don't need to link them from commandline(Clang).

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Comdlg32.lib")

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

// =========== KMenuItem.h ===========

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

#include <functional>

class KMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	KString itemText;
	bool enabled;
	bool checked;
	void* param;
	int intParam;

public:
	std::function<void(KMenuItem*)> onPress;

	KMenuItem();

	virtual void addToMenu(HMENU hMenu);

	virtual void setParam(void* param);

	virtual void setIntParam(int intParam);

	virtual int getIntParam();

	virtual void* getParam();

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual bool isEnabled();

	virtual void setEnabled(bool state);

	virtual void setText(const KString& text);

	virtual KString getText();

	virtual UINT getItemID();

	virtual HMENU getMenuHandle();

	virtual void _onPress();

	virtual ~KMenuItem();

private:
	RFC_LEAK_DETECTOR(KMenuItem)
};


// =========== KGraphics.h ===========

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

class KGraphics
{
public:
	KGraphics();

	virtual ~KGraphics();

	static void draw3dVLine(HDC hdc, int startX, int startY, int height);

	static void draw3dHLine(HDC hdc, int startX, int startY, int width);

	static void draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color);

	static void fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color);

	static RECT calculateTextSize(const wchar_t* text, HFONT hFont);

	static int calculateTextHeight(const wchar_t* text, HFONT hFont, int width);

	// This function sets the alpha channel to 255 without affecting any of the color channels.
	// hdc is a memory DC with a 32bpp bitmap selected into it.
	// can be use to fix 32bit bitmap alpha which is destroyed by the gdi operations.
	static void makeBitmapOpaque(HDC hdc, int x, int y, int cx, int cy);

	// hdc is a memory DC with a 32bpp bitmap selected into it.
	// This function sets the alpha channel without affecting any of the color channels.
	static void setBitmapAlphaChannel(HDC hdc, int x, int y, int cx, int cy, BYTE alpha);

private:
	RFC_LEAK_DETECTOR(KGraphics)
};


// =========== KIcon.h ===========

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


/**
	Can be use to load icon from file or resource.
*/
class KIcon
{
protected:
	HICON hIcon;
	WORD resourceID;

public:
	KIcon();

	// does not load. to be use with getScaledIcon method.
	void setResource(WORD resourceID);

	// this method scales down a larger image instead of scaling up a smaller image.
	// can be use for high-dpi requirements.
	// must destroy returned icon by calling DestroyIcon
	HICON getScaledIcon(int size);

	/**
		Loads icon from resource with default size given by the system
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads icon from file with default size given by the system
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool loadFromFile(const KString& filePath);

	/**
		Returns icon handle
	*/
	HICON getHandle();

	operator HICON()const;

	virtual ~KIcon();

private:
	RFC_LEAK_DETECTOR(KIcon)
};


// =========== KFont.h ===========

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


/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class KFont
{
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
	KFont(const KString& face, int sizeFor96DPI, bool bold = false,
		bool italic = false, bool underline = false, bool antiAliased = true, int requiredDPI = USER_DEFAULT_SCREEN_DPI);

	// destroys the existing font handle.
	virtual bool load(const KString& face, int sizeFor96DPI, bool bold = false,
		bool italic = false, bool underline = false, bool antiAliased = true, int requiredDPI = USER_DEFAULT_SCREEN_DPI);

	virtual void setDPI(int newDPI);

	/**
		If you want to use system default font, then use this static method. Do not delete the returned instance!
	*/
	static KFont* getDefaultFont();

	virtual bool isDefaultFont();

	/**
		Load a font from a file. loaded font only available to this application.
		make sure to call removePrivateFont when done.
	*/
	static bool loadPrivateFont(const KString& path);

	static void removePrivateFont(const KString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT getFontHandle();

	operator HFONT()const;

	virtual ~KFont();

	// Delete copy/move operations to prevent duplication
	KFont(const KFont&) = delete;
	KFont& operator=(const KFont&) = delete;
	KFont(KFont&&) = delete;
	KFont& operator=(KFont&&) = delete;

private:
	RFC_LEAK_DETECTOR(KFont)
};


// =========== KCursor.h ===========

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
	bool loadFromResource(WORD resourceID);

	/**
		Loads cursor from file
		@param filePath path to cursor file
		@returns false if cursor load fails
	*/
	bool loadFromFile(const KString& filePath);

	/**
		Returns cursor handle
	*/
	HCURSOR getHandle();

	/**
		Returns cursor handle
	*/
	operator HCURSOR()const;

	virtual ~KCursor();

private:
	RFC_LEAK_DETECTOR(KCursor)
};


// =========== KTransparentBitmap.h ===========

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

	void releaseResources();
	void createEmptyBitmap(int width, int height);

public:
	// data must be in 0xaarrggbb format with premultiplied alpha.
	// stride must be equal to width * 4
	KTransparentBitmap(void* data, int width, int height, int stride);

	// creates a transparent empty image
	KTransparentBitmap(int width, int height);

	// color format: 0xaarrggbb
	unsigned int getPixel(int x, int y);

	bool hitTest(int x, int y);

	int getWidth();

	int getHeight();

	// also clears the content
	void resize(int width, int height);

	// use AlphaBlend to draw
	// standard gdi drawing commands may not work with the returned hdc. (content has premultiplied alpha)
	// copy to secondary hdc using AlphaBlend or use gdi+ with PixelFormat32bppPARGB
	HDC getDC();

	void draw(HDC destHdc, int destX, int destY, BYTE alpha = 255);

	void draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha = 255);

	// can copy/scale specific part of the image
	void draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha = 255);

	virtual ~KTransparentBitmap();

private:
	RFC_LEAK_DETECTOR(KTransparentBitmap)
};




// =========== KTime.h ===========

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
	static _int64 delta100us(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		union timeunion {
			FILETIME fileTime;
			ULARGE_INTEGER ul;
		};

		timeunion ft1;
		timeunion ft2;

		::SystemTimeToFileTime(&time1, &ft1.fileTime);
		::SystemTimeToFileTime(&time2, &ft2.fileTime);

		return ft2.ul.QuadPart - ft1.ul.QuadPart;
	}

	// Returns time difference in seconds.
	static _int64 deltaSeconds(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (delta100us(time1, time2) / 10000000);
	}

	// Returns time difference in minutes.
	static _int64 deltaMinutes(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (deltaSeconds(time1, time2) / 60);
	}

	// Returns time difference in hours.
	static _int64 deltaHours(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (deltaMinutes(time1, time2) / 60);
	}

	static void getNow(SYSTEMTIME* time, const bool isLocalTime = true)
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


// =========== KStackInfo.h ===========

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

#include <stdio.h>

// shows current thread stack usage.
class KStackInfo
{
    static void _printUsage(size_t used, size_t total)
    {
        char buf[256];
        sprintf_s(buf, sizeof(buf),
            "Stack: %zu/%zu bytes (%.1f%% used, %.1f MB total)\n",
            used, total, (double)used / total * 100.0, total / (1024.0 * 1024.0));

        ::OutputDebugStringA(buf);
    }

public:
    static size_t getTotalStackSize()
    {
        ULONG_PTR low, high;
        ::GetCurrentThreadStackLimits(&low, &high);
        return high - low;
    }

    static size_t getCurrentStackUsage()
    {
        ULONG_PTR low, high;
        ::GetCurrentThreadStackLimits(&low, &high);

        volatile char dummy;
        void* currentSP = (void*)&dummy;

        return high - (ULONG_PTR)currentSP;
    }

    static void showStackInfo()
    {    
        size_t total = KStackInfo::getTotalStackSize();
        size_t used = KStackInfo::getCurrentStackUsage();

        KStackInfo::_printUsage(used, total);
    }
};

// =========== KPerformanceCounter.h ===========

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


class KPerformanceCounter
{
protected:
	double pcFreq;
	__int64 counterStart;

public:
	KPerformanceCounter();

	virtual void startCounter();

	/**
		returns delta time(milliseconds) between startCounter and endCounter calls.
	*/
	virtual double endCounter();

	virtual ~KPerformanceCounter();

private:
	RFC_LEAK_DETECTOR(KPerformanceCounter)
};


// =========== KRegistry.h ===========

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


class KRegistry
{

public:
	KRegistry();

	// returns true on success or if the key already exists.
	static bool createKey(HKEY hKeyRoot, const KString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool deleteKey(HKEY hKeyRoot, const KString& subKey);

	static bool readString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString* result);

	static bool writeString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value);

	static bool readDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD* result);

	static bool writeDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void *buffer;
		DWORD bufferSize;
		if(KRegistry::readBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool readBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void** buffer, DWORD* buffSize);

	static bool writeBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void* buffer, DWORD buffSize);

	virtual ~KRegistry();

};

// =========== KFile.h ===========

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

#include <shlwapi.h>

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
	KFile(const wchar_t* fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	/** 
		If the file does not exist, it will be created.
	*/
	virtual bool openFile(const wchar_t* fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	virtual bool closeFile();

	virtual HANDLE getFileHandle();

	operator HANDLE()const;

	/** 
		fills given buffer and returns number of bytes read.
	*/
	virtual DWORD readFile(void* buffer, DWORD numberOfBytesToRead);

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use getFileSize method. return value will be null on read error.
	*/
	virtual void* readAsData();

	virtual KString readAsString(bool isUnicode = true);

	/**
		returns number of bytes written.
	*/
	virtual DWORD writeFile(const void* buffer, DWORD numberOfBytesToWrite);

	virtual bool writeString(const KString& text, bool isUnicode = true);

	virtual bool setFilePointerToStart();

	/**
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	virtual bool setFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN);

	virtual DWORD getFilePointerPosition();

	virtual bool setFilePointerToEnd();

	/**
		returns zero on error
	*/
	virtual DWORD getFileSize();

	static bool deleteFile(const wchar_t* fileName);

	static bool isFileExists(const wchar_t* fileName);

	static bool copyFile(const wchar_t* sourceFileName, const wchar_t* destFileName);

	/**
		returns the file name part of the path.
	*/
	static KString getFileNameFromPath(const wchar_t* path);

	static KString getFileExtension(const wchar_t* path);

	virtual ~KFile();

private:
	RFC_LEAK_DETECTOR(KFile)
};



// =========== KGuid.h ===========

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


// RPC_WSTR is not defined in mingw.
#ifdef __MINGW32__
	typedef unsigned short* RPC_WSTR;
#endif

class KGuid
{
public:

	KGuid(){}

	static bool generateGUID(GUID* pGUID)
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static KString generateGUID()
	{
		GUID guid;

		if (KGuid::generateGUID(&guid))
			return KGuid::guidToString(&guid);

		return KString();
	}

	static KString guidToString(GUID* pGUID)
	{
		wchar_t* strGuid = nullptr;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		KString result(strGuid, KStringBehaviour::MAKE_A_COPY);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	virtual ~KGuid(){}

private:
	RFC_LEAK_DETECTOR(KGuid)
};


// =========== KBitmap.h ===========

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
	bool loadFromResource(WORD resourceID);

	/**
		Loads bitmap image from file
		@param filePath path to image
		@returns false if image load fails
	*/
	bool loadFromFile(const KString& filePath);

	// does not scale
	void drawOnHDC(HDC hdc, int x, int y, int width, int height);

	/**
		Returns bitmap handle
	*/
	HBITMAP getHandle();

	/**
		Returns bitmap handle
	*/
	operator HBITMAP()const;

	virtual ~KBitmap();

private:
	RFC_LEAK_DETECTOR(KBitmap)
};




// =========== KPointerList.h ===========

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

	KThreadSafetyBase()
	{
		::InitializeCriticalSection(&criticalSection);
	}

	~KThreadSafetyBase()
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

	void resetToSmallBuffer()
	{
		usingSmallBuffer = true;
		list = smallBuffer;
		roomCount = SmallBufferSize;
		itemCount = 0;
	}

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
	/**
		Constructs PointerList object.
		Thread safety is determined at compile time via template parameter.
	*/
	KPointerList()
	{
		resetToSmallBuffer();
		// Critical section initialization is handled by base class constructor
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool add(T pointer)
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
	T get(const int index)
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
	T operator[](const int index)
	{
		return get(index);
	}

	/**
		Replace pointer of given index with new pointer
		@returns false if index is out of range!
	*/
	bool set(const int index, T pointer)
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
	bool remove(const int index)
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

	bool remove(T pointer)
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
	void removeAll()
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
	void deleteAll()
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
	void forEach(std::function<void(T)> func)
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
	void forEachWithIndex(std::function<void(T, int)> func)
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
	bool forEachUntil(std::function<bool(T)> func)
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
		Finds the index of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int getIndex(T pointer)
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
	int size()
	{
		return itemCount;
	}

	/**
		@returns whether the list is currently using the small buffer optimization
	*/
	bool isUsingSmallBuffer() const
	{
		return usingSmallBuffer;
	}

	/**
		@returns the size of the small buffer
	*/
	static constexpr int getSmallBufferSize()
	{
		return SmallBufferSize;
	}

	/**
		@returns whether this instance is thread-safe (compile-time constant)
	*/
	static constexpr bool isThreadSafeInstance()
	{
		return IsThreadSafe;
	}

	/** Destructs PointerList object.*/
	~KPointerList()
	{
		if (!usingSmallBuffer)
			::free((void*)list);

		// Critical section cleanup is handled by base class destructor
	}

private:
	RFC_LEAK_DETECTOR(KPointerList)
};

// =========== KPointerQueue.h ===========

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

	virtual void push(T value)
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

	virtual T pop()
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
	virtual void deleteAllObjects()
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

// =========== KVector.h ===========

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

	void resetToSmallBuffer()
	{
		usingSmallBuffer = true;
		list = smallBuffer;
		roomCount = SmallBufferSize;
		itemCount = 0;
	}

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
	/**
		Constructs KVector object.
		Thread safety is determined at compile time via template parameter.
	*/
	KVector()
	{
		resetToSmallBuffer();
		// Critical section initialization is handled by base class constructor
	}

	/**
		Copy constructor
	*/
	KVector(const KVector& other)
	{
		resetToSmallBuffer();

		enterCriticalSectionIfNeeded();

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

		leaveCriticalSectionIfNeeded();
	}

	/**
		Assignment operator
	*/
	KVector& operator=(const KVector& other)
	{
		if (this == &other)
			return *this;

		enterCriticalSectionIfNeeded();

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
		return *this;
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool add(const T& item)
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

	T get(const int index)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount)) // checks for valid range!
		{
			T object(list[index]);
			leaveCriticalSectionIfNeeded();
			return object;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return T();
		}
	}

	// avoids extra copy
	bool get(const int index, T& outItem)
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

	T operator[](const int index)
	{
		return get(index);
	}

	/**
		@returns false if index is out of range!
	*/
	bool set(const int index, const T& item)
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
	bool remove(const int index)
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

	bool remove(const T& item)
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
	void removeAll()
	{
		enterCriticalSectionIfNeeded();

		if (!usingSmallBuffer)
			delete[] list;

		// we don't clear smallBuffer. 
		// remaining objects on smallBuffer will be destroyed at destructor or freed when adding new items.

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
		Finds the index of the first item which matches the item passed in.
		@returns -1 if not found!
	*/
	int getIndex(const T& item)
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
	int size() const
	{
		return itemCount;
	}

	/**
	 * Safely iterate through all items in the list with thread synchronization.
	 * The entire iteration is protected by critical section if thread safety is enabled.
	 * @param func Function/lambda to call for each item in the list
	*/
	void forEach(std::function<void(T&)> func)
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
	void forEachWithIndex(std::function<void(T&, int)> func)
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
	*/
	bool forEachUntil(std::function<bool(T&)> func)
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
		@returns whether the list is currently using the small buffer optimization
	*/
	bool isUsingSmallBuffer() const
	{
		return usingSmallBuffer;
	}

	/**
		@returns the size of the small buffer
	*/
	static constexpr int getSmallBufferSize()
	{
		return SmallBufferSize;
	}

	/**
		@returns whether this instance is thread-safe (compile-time constant)
	*/
	static constexpr bool isThreadSafeInstance()
	{
		return IsThreadSafe;
	}

	/** Destructs KVector object.*/
	~KVector()
	{
		if (!usingSmallBuffer)
			delete[] list;

		// Critical section cleanup is handled by base class destructor
	}

private:
	RFC_LEAK_DETECTOR(KVector)
};

// =========== KMenu.h ===========

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


class KMenu
{
protected:
	HMENU hMenu;

public:
	KMenu();

	virtual void addMenuItem(KMenuItem* menuItem);

	virtual void addSubMenu(const KString& text, KMenu* menu);

	virtual void addSeperator();

	virtual HMENU getMenuHandle();

	// set bringWindowToForeground when showing popup menu for notify icon(systray).
	// does not return until the menu close.
	virtual void popUpMenu(HWND window, bool bringWindowToForeground = false);

	virtual ~KMenu();

private:
	RFC_LEAK_DETECTOR(KMenu)
};


// =========== KSettingsReader.h ===========

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


/**
	High performance configuration reading class.
*/
class KSettingsReader
{
protected:
	KFile settingsFile;

public:
	KSettingsReader();

	virtual bool openFile(const wchar_t* fileName, int formatID);

	/**
		read struct, array or whatever...
	*/
	virtual void readData(DWORD size, void *buffer);

	virtual KString readString();

	virtual int readInt();

	virtual float readFloat();

	virtual double readDouble();

	virtual bool readBool();

	virtual ~KSettingsReader();

private:
	RFC_LEAK_DETECTOR(KSettingsReader)
};


// =========== KSettingsWriter.h ===========

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


/**
	High performance configuration writing class.
*/
class KSettingsWriter
{
protected:
	KFile settingsFile;

public:
	KSettingsWriter();

	virtual bool openFile(const wchar_t* fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	virtual void writeData(DWORD size, void *buffer);

	virtual void writeString(const KString& text);

	virtual void writeInt(int value);

	virtual void writeFloat(float value);

	virtual void writeDouble(double value);

	virtual void writeBool(bool value);

	virtual ~KSettingsWriter();

private:
	RFC_LEAK_DETECTOR(KSettingsWriter)
};


// =========== KComponent.h ===========

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
	virtual void onHotPlug();

	/**
		HotPlugs given HWND. this method does not update current compFont and cursor variables.
		Set fetchInfo to true if you want to acquire all the information about this HWND. (width, height, position etc...)
		Set fetchInfo to false if you just need to receive events. (button click etc...)
	*/
	virtual void hotPlugInto(HWND component, bool fetchInfo = true);

	/**
		Sets mouse cursor of this component.
	*/
	virtual void setMouseCursor(KCursor* cursor);

	/**
		@returns autogenerated unique class name for this component
	*/
	virtual KString getComponentClassName();

	/**
		Registers the class name and creates the component. 
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		@returns false if registration failed or component creation failed.
	*/
	virtual bool create(bool requireInitialMessages = false);

	virtual void destroy();

	/**
		Handles internal window messages. (subclassed window proc)
		Important: Pass unprocessed messages to parent if you override this method.
	*/
	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
		Receives messages like WM_COMMAND, WM_NOTIFY, WM_DRAWITEM from the parent window. (if it belongs to this component)
		Pass unprocessed messages to parent if you override this method.
		@returns true if msg processed.
	*/
	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result);

	/**
		Identifier of the child component which can be used with WM_MEASUREITEM like messages.
		@returns zero for top level windows
	*/
	virtual UINT getControlID();

	/**
		Sets font of this component. passed font object must live until this component destroy.
	*/
	virtual void setFont(KFont* compFont);

	/**
		Sets font of this component. passed font object must live until this component destroy.
	*/
	virtual void setFont(KFont& compFont);

	/**
		Returns font of this component
	*/
	virtual KFont* getFont();

	/**
		Returns caption of this component
	*/
	virtual KString getText();

	/**
		Sets caption of this component
	*/
	virtual void setText(const KString& compText);

	virtual void setHWND(HWND compHWND);

	/**
		Returns HWND of this component
	*/
	virtual HWND getHWND();

	/**
		Changes parent of this component
	*/
	virtual void setParentHWND(HWND compParentHWND);

	/**
		Returns parent of this component
	*/
	virtual HWND getParentHWND();

	/**
		Returns style of this component
	*/
	virtual DWORD getStyle();

	/**
		Sets style of this component
	*/
	virtual void setStyle(DWORD compStyle);

	/**
		Returns exstyle of this component
	*/
	virtual DWORD getExStyle();

	/**
		Sets exstyle of this component
	*/
	virtual void setExStyle(DWORD compExStyle);

	/**
		Returns x position of this component which is relative to parent component.
	*/
	virtual int getX();

	/**
		Returns y position of this component which is relative to parent component.
	*/
	virtual int getY();

	/**
		Returns width of the component.
	*/
	virtual int getWidth();

	/**
		Returns height of the component.
	*/
	virtual int getHeight();

	virtual int getDPI();

	/**
		Sets width and height of the component.
	*/
	virtual void setSize(int compWidth, int compHeight);

	/**
		Sets x and y position of the component. x and y are relative to parent component
	*/
	virtual void setPosition(int compX, int compY);

	virtual void setDPI(int newDPI);

	/**
		Sets visible state of the component
	*/
	virtual void setVisible(bool state);

	/**
		Returns visible state of the component
	*/
	virtual bool isVisible();

	/**
		Returns the component is ready for user input or not
	*/
	virtual bool isEnabled();

	/**
		Sets component's user input reading state
	*/
	virtual void setEnabled(bool state);

	/**
		Brings component to front
	*/
	virtual void bringToFront();

	/**
		Grabs keyboard focus into this component
	*/
	virtual void setKeyboardFocus();

	/**
		Repaints the component
	*/
	virtual void repaint();

	virtual ~KComponent();

private:
	RFC_LEAK_DETECTOR(KComponent)
};


// macros to handle window messages

#define BEGIN_KMSG_HANDLER \
	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override \
	{\
	switch(msg)\
	{

#define ON_KMSG(_KMsg,_KMsgHandler) \
	case _KMsg: return _KMsgHandler(wParam,lParam);

// msvc & clang supports __super keyword

#define END_KMSG_HANDLER \
	default: return __super::windowProc(hwnd,msg,wParam,lParam); \
	}\
	}


// =========== UtilsModule.h ===========

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


#pragma comment(lib, "Rpcrt4.lib")

// =========== ContainersModule.h ===========

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


// =========== KGridView.h ===========

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

#include <functional>

class KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;

public:
	std::function<void(KGridView*)> onItemSelect;
	std::function<void(KGridView*)> onItemRightClick;
	std::function<void(KGridView*)> onItemDoubleClick;

	KGridView(bool sortItems = false);

	virtual void insertRecord(KString** columnsData);

	virtual void insertRecordTo(int rowIndex, KString** columnsData);

	virtual KString getRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int getSelectedRow();

	virtual void removeRecordAt(int rowIndex);

	virtual void removeAll();

	virtual void updateRecordAt(int rowIndex, int columnIndex, const KString& text);

	virtual void setColumnWidth(int columnIndex, int columnWidth);

	virtual int getColumnWidth(int columnIndex);

	virtual void createColumn(const KString& text, int columnWidth = 100);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual void _onItemRightClick();

	virtual void _onItemDoubleClick();

	virtual ~KGridView();
};


// =========== KLabel.h ===========

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


class KLabel : public KComponent
{
protected:
	bool autoResize;

	virtual void resizeToTextSize();
	const int AUTOSIZE_EXTRA_GAP = 10;
public:
	KLabel();

	virtual bool create(bool requireInitialMessages = false) override;
	virtual void enableAutoResize(bool enable);
	virtual void setText(const KString& compText) override;
	virtual void setFont(KFont* compFont) override;
	virtual void setFont(KFont& compFont) override;
	virtual void setDPI(int newDPI) override;

	virtual ~KLabel();
};


// =========== KProgressBar.h ===========

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


class KProgressBar : public KComponent
{
protected:
	int value;

public:
	KProgressBar(bool smooth=true, bool vertical=false);

	virtual int getValue();

	virtual void setValue(int value);

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KProgressBar();
};


// =========== KTextBox.h ===========

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


class KTextBox : public KComponent
{
public:
	KTextBox(bool readOnly = false);

	virtual KString getText() override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KTextBox();
};


// =========== KTrackBar.h ===========

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

#include <functional>

class KTrackBar : public KComponent
{
protected:
	int rangeMin,rangeMax,value;

public:
	std::function<void(KTrackBar*,int)> onChange;

	KTrackBar(bool showTicks = false, bool vertical = false);

	/**
		Range between 0 to 100
	*/
	virtual void setRange(int min, int max);

	virtual void setValue(int value);

	virtual int getValue();

	virtual void _onChange();

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KTrackBar();
};




// =========== KPasswordBox.h ===========

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


class KPasswordBox : public KTextBox
{
protected:
	wchar_t pwdChar;

public:
	KPasswordBox(bool readOnly=false);

	virtual void setPasswordChar(const wchar_t pwdChar);

	virtual wchar_t getPasswordChar();

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KPasswordBox();
};


// =========== KPropertyStorage.h ===========

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

    KPointerList<KPSProperty*, 32, false> propertyList;

    KPSObject()
    {
        name = NULL;
        nameLength = 0;     
    }

    bool compare(const GUID& objectID)
    {
        return (::IsEqualGUID(objectID, this->objectID) == TRUE);
    }

    virtual ~KPSObject()
    {
        if (name)
            ::free(name);

        propertyList.deleteAll();
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

    void generateIntArrayString()
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

    void generateIntArrayByString(wchar_t* text)
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

        generateIntArrayString();
    }

    void generateGUIDValueString()
    {
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);

        guidValueStr = NULL;

        ::UuidToStringW(&guidValue, (RPC_WSTR*)&guidValueStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool generateGUIDValueByString(const wchar_t* text)
    {
        bool success = true;

        if (::UuidFromStringW((RPC_WSTR)text, &guidValue) != RPC_S_OK)
        {
            ::UuidFromStringW((RPC_WSTR)L"00000000-0000-0000-0000-000000000000", &guidValue); // default value
            success = false;
        }

        generateGUIDValueString();
        return success;
    }
};

class KPSObjectView : public KPSObject
{
public:
    wchar_t* objectIDStr; 

    KPSObjectView() : KPSObject()
    {
        objectIDStr = NULL;
    }

    void generateObjectID()
    {
        ::CoCreateGuid(&objectID);
    }

    void generateObjectIDString()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);

        objectIDStr = nullptr;

        ::UuidToStringW(&objectID, (RPC_WSTR*)&objectIDStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool generateIDByString(const wchar_t* text)
    {
        if (::UuidFromStringW((RPC_WSTR)text, &objectID) != RPC_S_OK)
            return false;

        generateObjectIDString();
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
    KPointerList<KPSObject*,16, false>* psObjectList;

    KPSReader()
    {
        psObjectList = NULL;
    }

    // do not free returned object.
    KPSObject* getPSObject(const GUID& objectID)
    {
        for (int i = 0; i < psObjectList->size(); i++)
        {
            KPSObject* psObject = psObjectList->get(i);
            if (psObject->compare(objectID))
                return psObject;
        }

        return NULL;
    }

    bool loadFromFile(const wchar_t* path, bool readNames = true)
    {
        if (psObjectList)
        {
            psObjectList->deleteAll();
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

        psObjectList = new KPointerList<KPSObject*, 16, false>();

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

            KPSObject* psObject = new KPSObject();
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

                psObject->propertyList.add(psProperty);
            }

            psObjectList->add(psObject);
        }

        ::CloseHandle(fileHandle);
        return true;
    }

    virtual ~KPSReader() 
    {
        if (psObjectList)
        {
            psObjectList->deleteAll();
            delete psObjectList;
        }
    }
};


// =========== KButton.h ===========

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

#include <functional>

class KButton : public KComponent
{
public:
	std::function<void(KButton*)> onClick;

	KButton();

	virtual void _onPress();

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KButton();
};

// =========== KDirectory.h ===========

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


#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class KDirectory
{
public:
	KDirectory();

	static bool isDirExists(const KString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool createDir(const KString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool removeDir(const KString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static void getModuleDir(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars);

	static void getModuleFilePath(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars);

	/**
		returns the parent directory of given file.
	*/
	static void getParentDir(const wchar_t* filePath, wchar_t* outBuffer, int bufferSizeInWChars);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static void getTempDir(wchar_t* outBuffer, int bufferSizeInWChars);

	/**
		returns the all user data directory. Requires admin priviledges for writing to this dir.
		returns empty string on error.
		outBuffer size must be MAX_PATH
	*/
	static void getAllUserDataDir(wchar_t* outBuffer);

	/*
		known path for the logged in user of the pc. (not affected by right click -> run as admin)
		outBuffer size must be MAX_PATH
		CSIDL_ADMINTOOLS
		CSIDL_APPDATA
		CSIDL_COMMON_ADMINTOOLS
		CSIDL_COMMON_APPDATA
		CSIDL_COMMON_DOCUMENTS
		CSIDL_COOKIES
		CSIDL_FLAG_CREATE
		CSIDL_FLAG_DONT_VERIFY
		CSIDL_HISTORY
		CSIDL_INTERNET_CACHE
		CSIDL_LOCAL_APPDATA
		CSIDL_MYPICTURES
		CSIDL_PERSONAL
		CSIDL_PROGRAM_FILES
		CSIDL_PROGRAM_FILES_COMMON
		CSIDL_SYSTEM
		CSIDL_WINDOWS
	*/
	static void getLoggedInUserFolderPath(int csidl, wchar_t* outBuffer);

	// path for logged in user of pc (not affected by right click -> run as admin)
	// outBuffer size must be MAX_PATH
	static void getRoamingFolder(wchar_t* outBuffer);

	// path for logged in user of pc (not affected by right click -> run as admin)
	// outBuffer size must be MAX_PATH
	static void getNonRoamingFolder(wchar_t* outBuffer);

	// must delete returned strings and list.
	// extension without dot. ex: "mp3"
	// folderPath is without ending slash
	// returns only file names. not full path.
	// does not scan for child folders.
	static KPointerList<KString*, 32, false>* scanFolderForExtension(const KString& folderPath, const KString& extension);

	virtual ~KDirectory();

private:
	RFC_LEAK_DETECTOR(KDirectory)
};


// =========== KComboBox.h ===========

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

#include <functional>

class KComboBox : public KComponent
{
protected:
	KVector<KString, 10, false> stringList;
	int selectedItemIndex;

public:
	std::function<void(KComboBox*)> onItemSelect;

	KComboBox(bool sort=false);

	virtual void addItem(const KString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const KString& text);

	virtual int getItemIndex(const KString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual KString getSelectedItem();

	virtual void clearList();

	virtual void selectItem(int index);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual ~KComboBox();
};


// =========== KMenuButton.h ===========

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


class KMenuButton : public KButton
{
protected:
	KFont arrowFont;
	KFont* glyphFont;
	KMenu* buttonMenu;
	const wchar_t* glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KMenuButton();

	virtual ~KMenuButton();

	virtual void setMenu(KMenu* buttonMenu);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void setGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual void setDPI(int newDPI) override;

	virtual void _onPress() override;

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;
};


// =========== KListBox.h ===========

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

#include <functional>

class KListBox : public KComponent
{
protected:
	KVector<KString, 10, false> stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

public:
	std::function<void(KListBox*)> onItemSelect;
	std::function<void(KListBox*)> onItemRightClick;
	std::function<void(KListBox*)> onItemDoubleClick;

	KListBox(bool multipleSelection=false, bool sort=false, bool vscroll=true);

	virtual void addItem(const KString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const KString& text);

	virtual void updateItem(int index, const KString& text);

	virtual int getItemIndex(const KString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual KString getSelectedItem();

	virtual int getSelectedItems(int* itemArray, int itemCountInArray);

	virtual void clearList();

	virtual void selectItem(int index);

	virtual void selectItems(int start, int end);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual void _onItemDoubleClick();

	virtual void _onItemRightClick();

	virtual ~KListBox();
};


// =========== KLogger.h ===========

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

	virtual bool writeNewEvent(unsigned char eventType = EVT_INFORMATION);

	virtual bool endEvent();

	/**
		textLength is number of chars. max value is 255.
	*/
	virtual bool addTextParam(const char *text, unsigned char textLength);

	virtual bool addIntParam(int value);

	virtual bool addShortParam(unsigned short value);

	virtual bool addFloatParam(float value);
	
	virtual bool addDoubleParam(double value);

	virtual bool isBufferFull();

	virtual bool writeToFile(const KString &filePath);

	virtual ~KLogger();

private:
	RFC_LEAK_DETECTOR(KLogger)
};


// =========== KHostPanel.h ===========

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


class KHostPanel : public KComponent
{
protected:
	bool enableDPIUnawareMode;
	KPointerList<KComponent*, 24, false>* componentList;

public:
	KHostPanel();

	// called by the parent
	virtual void setComponentList(KPointerList<KComponent*, 24, false>* componentList);

	// called by the parent
	virtual void setEnableDPIUnawareMode(bool enable);

	/**
		add KHostPanel to window(call create) before adding items to it.
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool addComponent(KComponent* component, bool requireInitialMessages = false);

	// Can be also use to remove a container. Also destroys the hwnd.
	virtual void removeComponent(KComponent* component);

	/**
		add KHostPanel to window(call create) before adding items to it.
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool addContainer(KHostPanel* container, bool requireInitialMessages = false);

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KHostPanel();
};

// =========== KGUIProc.h ===========

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


// all the methods must be called only from the gui thread.
class KGUIProc
{
public:
	static ATOM atomComponent;
	static ATOM atomOldProc;

	static LRESULT CALLBACK windowProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK dialogProc(HWND, UINT, WPARAM, LPARAM);

	/**
		set requireInitialMessages to true to receive initial messages lke WM_CREATE... (installs a hook)
	*/
	static HWND createComponent(KComponent* component, bool requireInitialMessages);

	/**
		hwnd can be window, custom control, dialog or common control.
		hwnd will be subclassed if it is a common control or dialog.
	*/
	static void attachRFCPropertiesToHWND(HWND hwnd, KComponent* component);

	static int hotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
	static HWND hotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
};


// =========== KWindow.h ===========

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


#define RFC_CUSTOM_MESSAGE WM_APP + 100

class KDPIChangeListener
{
public:
	virtual void onDPIChange(HWND hwnd, int newDPI) = 0;
};

enum class KCloseOperation { DestroyAndExit, Hide, Nothing };

class KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;
	KDPIChangeListener* dpiChangeListener;
	bool enableDPIUnawareMode;
	KPointerList<KComponent*, 24, false> componentList; // KHostPanel is also using 24.
	KCloseOperation closeOperation;
	DPI_AWARENESS_CONTEXT prevDPIContext;
	bool dpiAwarenessContextChanged;
	KIcon* windowIcon;
	HICON largeIconHandle, smallIconHandle;

	void updateWindowIconForNewDPI();

public:
	KWindow();

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void flash();

	// can only call after create.
	virtual void setIcon(KIcon* icon);

	virtual void setCloseOperation(KCloseOperation closeOperation);

	virtual void onClose();

	virtual void onDestroy();

	// Custom messages are used to send a signal/data from worker thread to gui thread.
	virtual void postCustomMessage(WPARAM msgID, LPARAM param);

	virtual void onCustomMessage(WPARAM msgID, LPARAM param);

	virtual void centerScreen();

	// puts our window on same monitor as given window + centered
	virtual void centerOnSameMonitor(HWND window);

	/**
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		Adding a component does not mean that the window will own or delete the component - it's
		your responsibility to delete the component. you need to remove the component if you are
		deleting it before WM_DESTROY message arrived.
.
	*/
	virtual bool addComponent(KComponent* component, bool requireInitialMessages = false);

	virtual bool addComponent(KComponent& component, bool requireInitialMessages = false);

	// Can be also use to remove a container. Also destroys the hwnd.
	// you need to remove the component if you are deleting it before WM_DESTROY message arrived.
	virtual void removeComponent(KComponent* component);

	// use this method to add KHostPanel to the window.
	virtual bool addContainer(KHostPanel* container, bool requireInitialMessages = false);

	virtual bool setClientAreaSize(int width, int height);

	virtual void setDPIChangeListener(KDPIChangeListener* dpiChangeListener);

	// Mixed-Mode DPI Scaling - window scaled by the system. can only call before create.
	// InitRFC must be called with KDPIAwareness::MIXEDMODE_ONLY
	// Only works with Win10 or higher
	virtual void setEnableDPIUnawareMode(bool enable);

	// In mixed-mode dpi unaware window, before adding any child we need to set current thread dpi mode to unaware mode.
	// by default this method automatically called with AddComponent method.
	// if you add a child without calling AddComponent then you have to call ApplyDPIUnawareModeToThread method first.
	virtual void applyDPIUnawareModeToThread();

	// after adding the child, we need to restore the last dpi mode of the thread.
	// Mixed-Mode only
	virtual void restoreDPIModeOfThread();

	static bool isOffScreen(int posX, int posY);

	virtual bool getClientAreaSize(int* width, int* height);

	// can be use to get the window size even if it were minimized.
	virtual void getNormalSize(int* width, int* height);

	virtual void onMoved();

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if the window and controls are in different dpi scale. (use KDPIChangeListener)
	virtual void onResized();

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KWindow();
};


// =========== FileModule.h ===========

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


#pragma comment(lib,"Shlwapi.lib")



// =========== KGroupBox.h ===========

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


/*
* Win32 group box is designed to be placed around other controls. Do not use its hwnd
* as a parent/host for other controls. Position other controls over it and then call 
* BringToFront method.
*/
class KGroupBox : public KButton
{
public:
	KGroupBox();

	virtual ~KGroupBox();
};


// =========== KGlyphButton.h ===========

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


class KGlyphButton : public KButton
{
protected:
	KFont* glyphFont;
	const wchar_t* glyphChar;
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
	virtual void setGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual void setDPI(int newDPI) override;
};


// =========== KNumericField.h ===========

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


class KNumericField : public KTextBox
{
public:
	KNumericField();

	virtual ~KNumericField();
};


// =========== KCheckBox.h ===========

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


class KCheckBox : public KButton
{
protected:
	bool checked;

public:
	KCheckBox();

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onPress() override;

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual ~KCheckBox();
};


// =========== KPushButton.h ===========

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


class KPushButton : public KCheckBox
{
public:
	KPushButton();

	virtual ~KPushButton();
};


// =========== KRadioButton.h ===========

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


class KRadioButton : public KCheckBox
{
public:
	KRadioButton();

	virtual ~KRadioButton();
};


// =========== KTextArea.h ===========

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


class KTextArea : public KTextBox
{
public:
	KTextArea(bool autoScroll = false, bool readOnly = false);

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KTextArea();
};


// =========== KToolTip.h ===========

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
	virtual void attachToComponent(KWindow* parentWindow, KComponent* attachedComponent);

	/**
		calling this method has no effect.
	*/
	virtual bool create(bool requireInitialMessages = false) override;

	virtual void setText(const KString& compText) override;
};




// =========== KWindowTypes.h ===========

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

#include <Windowsx.h> // GET_X_LPARAM
#include <type_traits> // std::is_base_of

class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void onClose();

	virtual void onDestroy();

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

// enables client area dragging
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KDraggable : public T
{
protected:
	bool enableClientAreaDragging;
	bool windowDraging;
	short clientAreaDraggingX;
	short clientAreaDraggingY;

	virtual LRESULT onLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		if (enableClientAreaDragging)
		{
			clientAreaDraggingX = GET_X_LPARAM(lParam);
			clientAreaDraggingY = GET_Y_LPARAM(lParam);

			// disabled child will pass left click msg to parent window. we ignore that! 
			// we don't want to drag window by clicking on child.
			POINT point = { clientAreaDraggingX, clientAreaDraggingY };
			if (::RealChildWindowFromPoint(T::compHWND, point) != T::compHWND)
				return 0;

			windowDraging = true;
			::SetCapture(T::compHWND);
		}

		return 0;
	}

	virtual LRESULT onMouseMove(WPARAM wParam, LPARAM lParam)
	{
		if (windowDraging)
		{
			POINT pos;
			::GetCursorPos(&pos);

			this->setPosition(pos.x - clientAreaDraggingX, pos.y - clientAreaDraggingY);
		}

		return 0;
	}

	virtual LRESULT onLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		if (windowDraging)
		{
			::ReleaseCapture();
			windowDraging = false;
		}

		return 0;
	}

public:
	KDraggable()
	{
		enableClientAreaDragging = true;
	}

	virtual void setEnableClientAreaDrag(bool enable)
	{
		enableClientAreaDragging = enable;
	}

	virtual ~KDraggable() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_LBUTTONDOWN, onLButtonDown)
		ON_KMSG(WM_MOUSEMOVE, onMouseMove)
		ON_KMSG(WM_LBUTTONUP, onLButtonUp)
	END_KMSG_HANDLER
};

// provides flicker free double buffered drawing method.
// T must be derived from KComponent
template <class T,
	typename = typename std::enable_if<std::is_base_of<KComponent, T>::value>::type>
class KDrawable : public T
{
protected:

	// override this method in subclass and draw your stuff
	virtual void onPaint(HDC hDCMem, RECT* rect, const int width, const int height)
	{
		::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		::FrameRect(hDCMem, rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	}

	virtual LRESULT onWMPaint(WPARAM wParam, LPARAM lParam)
	{
		RECT rect;
		::GetClientRect(T::compHWND, &rect);

		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;

		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(T::compHWND, &ps);

		// double buffering
		HDC hDCMem = ::CreateCompatibleDC(hdc);
		HBITMAP memBMP = ::CreateCompatibleBitmap(hdc, width, height);;
		::SelectObject(hDCMem, memBMP);

		this->onPaint(hDCMem, &rect, width, height);

		::BitBlt(hdc, 0, 0, width, height, hDCMem, 0, 0, SRCCOPY);

		::DeleteDC(hDCMem);
		::DeleteObject(memBMP);

		::EndPaint(T::compHWND, &ps);

		return 0;
	}

	virtual LRESULT onEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return 1; // avoids flickering
	}

public:
	KDrawable(){}

	virtual ~KDrawable() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, onWMPaint)
		ON_KMSG(WM_ERASEBKGND, onEraseBackground)
	END_KMSG_HANDLER
};

class KWidget : public KDrawable<KDraggable<KWindow>>
{
public:
	KWidget()
	{
		compDwStyle = WS_POPUP;
		compDwExStyle = WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT;
		compWidth = 128;
		compHeight = 128;
	}

	virtual ~KWidget() {}
};

// =========== KTimer.h ===========

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

#include <functional>

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

public:
	std::function<void(KTimer*)> onTimer;

	KTimer();

	/**
		@param resolution timer interval
	*/
	virtual void setInterval(int resolution);

	virtual int getInterval();

	/**
		Call this method before you start timer
	*/
	virtual void setTimerWindow(KWindow* window);

	virtual void setTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	virtual UINT getTimerID();

	/**
		Starts timer
	*/
	virtual void startTimer();

	/**
		Stops the timer. You can restart it by calling startTimer() method.
	*/
	virtual void stopTimer();

	virtual bool isTimerRunning();

	virtual void _onTimer();

	virtual ~KTimer();

private:
	RFC_LEAK_DETECTOR(KTimer)
};


// =========== KNotifyIconHandler.h ===========

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

#include <shellapi.h>
#include <commctrl.h>
#include <type_traits> // std::is_base_of

#define RFC_NOTIFY_ICON_MESSAGE WM_APP + 101

// adds an icon into the systemtray and handles mouse messages.
// detects taskbar re-creation and adds the icon again.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KNotifyIconHandler : public T
{
protected:
	HICON notifyIconHandle;
	KString notifyIconToolTipText;
	UINT taskbarRestartMsg;

	virtual LRESULT onNotifyIconMessage(WPARAM wParam, LPARAM lParam)
	{
		if (lParam == WM_LBUTTONUP)
			this->onNotifyIconLeftClick();
		else if (lParam == WM_RBUTTONUP)
			this->onNotifyIconRightClick();

		return 0;
	}

	virtual void createNotifyIcon(HWND window, HICON icon, const KString& toolTipText)
	{
		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = window;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = RFC_NOTIFY_ICON_MESSAGE;
		nid.hIcon = icon;
		::wcscpy_s(nid.szTip, toolTipText);
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

		::Shell_NotifyIconW(NIM_ADD, &nid);
	}

	// on explorer crash
	virtual LRESULT onTaskBarReCreate(WPARAM wParam, LPARAM lParam)
	{
		if (notifyIconHandle)
			this->createNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);

		return 0;
	}

	// override this method in your subclass and show popup menu.
	virtual void onNotifyIconRightClick()
	{
		::SetForegroundWindow(this->compHWND);
		// show you popup menu here...
	}

	// override this method in your subclass.
	virtual void onNotifyIconLeftClick()
	{
		::SetForegroundWindow(this->compHWND);
	}

public:
	KNotifyIconHandler()
	{
		notifyIconHandle = 0;
		taskbarRestartMsg = ::RegisterWindowMessageW(L"TaskbarCreated");
	}

	virtual ~KNotifyIconHandler()
	{
		if (notifyIconHandle)
			::DestroyIcon(notifyIconHandle);
	}

	// window must be created.
	// maximum tooltip text size is 128
	virtual void addNotifyIcon(WORD iconResourceID, const KString& tooltipText)
	{
		// supports high dpi.
		// LoadIconMetric: only for system tray. cannot use for a window. because multiple window can have different dpi.
		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		notifyIconToolTipText = tooltipText;

		this->createNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);
	}

	virtual void updateNotifyIcon(WORD iconResourceID)
	{
		if (notifyIconHandle)
			::DestroyIcon(notifyIconHandle);

		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.hIcon = notifyIconHandle;
		nid.uFlags = NIF_ICON;

		::Shell_NotifyIconW(NIM_MODIFY, &nid);
	}

	// maximum tooltip text size is 128
	virtual void updateNotifyIconToolTip(const KString& tooltipText)
	{
		notifyIconToolTipText = tooltipText;

		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		::wcscpy_s(nid.szTip, tooltipText);
		nid.uFlags = NIF_TIP;

		::Shell_NotifyIconW(NIM_MODIFY, &nid);
	}

	virtual void destroyNotifyIcon()
	{
		NOTIFYICONDATAW nid = { 0 };
		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;

		::Shell_NotifyIconW(NIM_DELETE, &nid);
	}

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == RFC_NOTIFY_ICON_MESSAGE)
			return this->onNotifyIconMessage(wParam, lParam);
		else if (msg == taskbarRestartMsg)
			return this->onTaskBarReCreate(wParam, lParam);
		else
			return T::windowProc(hwnd, msg, wParam, lParam);
	}
};

// =========== KMenuBar.h ===========

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


class KMenuBar
{
protected:
	HMENU hMenu;

public:
	KMenuBar();

	virtual void addMenu(const KString& text, KMenu* menu);

	virtual void addToWindow(KWindow* window);

	virtual ~KMenuBar();

private:
	RFC_LEAK_DETECTOR(KMenuBar)
};


// =========== KCommonDialogBox.h ===========

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
	static bool showOpenFileDialog(KWindow* window, const KString& title, const wchar_t* filter, KString* fileName, bool saveLastLocation = false, const KString& dialogGuid = KString());

	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool showSaveFileDialog(KWindow* window, const KString& title, const wchar_t* filter, KString* fileName, bool saveLastLocation = false, const KString& dialogGuid = KString());
};


// =========== KIDGenerator.h ===========

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
	KIDGenerator();
	~KIDGenerator();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	static const int rfc_InitialMenuItemCount = 20;
	static const int rfc_InitialTimerCount = 10;

	static const int rfc_InitialControlID = 100;
	static const int rfc_InitialMenuItemID = 30000;
	static const int rfc_InitialTimerID = 1000;

	KPointerList<KMenuItem*, rfc_InitialMenuItemCount, false> menuItemList;
	KPointerList<KTimer*, rfc_InitialTimerCount, false> timerList;

public:
	// do not delete the returned instance.
	static KIDGenerator* getInstance();

	UINT generateControlID();

	// KApplication:hInstance must be valid before calling this method
	// can generate up to 9999 class names.
	void generateClassName(KString& stringToModify);

	UINT generateMenuItemID(KMenuItem* menuItem);
	KMenuItem* getMenuItemByID(UINT id);

	UINT generateTimerID(KTimer* timer);
	KTimer* getTimerByID(UINT id);
};

// =========== GUIModule.h ===========

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


#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Shell32.lib")

#endif

