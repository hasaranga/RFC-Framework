
// ========== RFC Generator v1.0 - 2025-06-10 18:04 PM ==========

#ifndef _RFC_H_
#define _RFC_H_ 

#define AMALGAMATED_VERSION


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

#endif

