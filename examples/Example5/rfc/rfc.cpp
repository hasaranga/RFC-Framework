
// ========== RFC Generator v1.0 ==========


#include "rfc.h"


// =========== KStaticAllocator.cpp ===========

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


// Static member definitions
char KStaticAllocator::memory_pool[KStaticAllocator::POOL_SIZE];
std::atomic<size_t> KStaticAllocator::current_offset{ 0 };

void* KStaticAllocator::allocate(size_t size, size_t alignment)
{
    size_t old_offset, new_offset;

    do {
        old_offset = current_offset.load(std::memory_order_acquire);

        // Align the offset
        size_t aligned_offset = (old_offset + alignment - 1) & ~(alignment - 1);

        if (aligned_offset + size > POOL_SIZE)
        {
            K_ASSERT(false, "KStaticAllocator KSTATIC_POOL_SIZE not enough!");
            return nullptr; // Pool exhausted
        }

        new_offset = aligned_offset + size;

    } while (!current_offset.compare_exchange_weak(
        old_offset, new_offset,
        std::memory_order_release,
        std::memory_order_acquire));

    // Calculate the actual aligned offset for the successful allocation
    size_t final_aligned_offset = (old_offset + alignment - 1) & ~(alignment - 1);
    return memory_pool + final_aligned_offset;
}

void KStaticAllocator::reset()
{
    current_offset.store(0, std::memory_order_release);
}

// =========== Core.cpp ===========

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


void InitRFCModules()
{
	// initialize modules
	RFCModuleInitFunc* initFuncList = KModuleManager::rfcModuleInitFuncList();
	for (int i = 0; i < MAX_RFC_MODULE_COUNT; ++i)
	{
		if (initFuncList[i])
		{
			if (!initFuncList[i]())
			{
				char strIndex[10];
				::_itoa_s(i, strIndex, 10);
				char msg[64];
				::strcpy_s(msg, "RFC Module Initialization error!\n\nModule index: ");
				::strcat_s(msg, strIndex);
				::MessageBoxA(0, msg, "RFC - Error", MB_ICONERROR);
				::ExitProcess(1);
			}
		}
	}
}

void DeInitRFCModules()
{
	// free modules
	RFCModuleFreeFunc* freeFuncList = KModuleManager::rfcModuleFreeFuncList();
	for (int i = (MAX_RFC_MODULE_COUNT-1); i >= 0; --i)
	{
		if (freeFuncList[i])
			freeFuncList[i]();
	}
}

void RFCDllInit()
{
	CoreModuleInitParams::hInstance = 0;
	CoreModuleInitParams::initCOMAsSTA = false;
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE;
	::InitRFCModules();
}

void RFCDllFree()
{
	::DeInitRFCModules();
}

// =========== CoreModule.cpp ===========

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


HINSTANCE CoreModuleInitParams::hInstance = 0;
bool CoreModuleInitParams::initCOMAsSTA = true;
KDPIAwareness CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE;

class RFC_CoreModule {
public:
	static bool rfcModuleInit()
	{
		if (!CoreModuleInitParams::hInstance)
		{
			//hInstance = ::GetModuleHandleW(NULL); // not work for dll
			::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
				GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
				(LPCWSTR)&InitRFCModules, &CoreModuleInitParams::hInstance);
		}

		KApplication::hInstance = CoreModuleInitParams::hInstance;
		KApplication::dpiAwareness = CoreModuleInitParams::dpiAwareness;

		if (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE)
		{
			KDPIUtility::initDPIFunctions();
			KDPIUtility::makeProcessDPIAware(KApplication::dpiAwareness);
		}

		if (CoreModuleInitParams::initCOMAsSTA)
			::CoInitialize(NULL); //Initializes COM as STA.

		return true;
	}

	static void rfcModuleFree()
	{
		if (CoreModuleInitParams::initCOMAsSTA)
			::CoUninitialize();
	}
};

REGISTER_RFC_MODULE(0, RFC_CoreModule)

// =========== KApplication.cpp ===========

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


HINSTANCE KApplication::hInstance = 0;
KDPIAwareness KApplication::dpiAwareness;
bool KApplication::dpiAwareAPICalled = false;

KApplication::KApplication()
{
}

void KApplication::modifyModuleInitParams()
{

}

int KApplication::main(wchar_t** argv, int argc)
{
	return 0;
}

bool KApplication::allowMultipleInstances()
{
	return true;
}

int KApplication::anotherInstanceIsRunning(wchar_t** argv, int argc)
{
	return 0;
}

const wchar_t* KApplication::getApplicationID()
{
	return L"RFC_APPLICATION";
}

void KApplication::messageLoop(bool handleTabKey)
{
	MSG msg;

	while (::GetMessageW(&msg, NULL, 0, 0))
	{
		if (handleTabKey)
		{
			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

KApplication::~KApplication()
{
}

// =========== KDPIUtility.cpp ===========

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


KGetDpiForMonitor KDPIUtility::pGetDpiForMonitor = nullptr;
KSetProcessDpiAwarenessContext KDPIUtility::pSetProcessDpiAwarenessContext = nullptr;
KSetProcessDpiAwareness KDPIUtility::pSetProcessDpiAwareness = nullptr;
KSetProcessDPIAware KDPIUtility::pSetProcessDPIAware = nullptr;
KSetThreadDpiAwarenessContext KDPIUtility::pSetThreadDpiAwarenessContext = nullptr;
KAdjustWindowRectExForDpi KDPIUtility::pAdjustWindowRectExForDpi = nullptr;

void KDPIUtility::initDPIFunctions()
{
	HMODULE hShcore = ::LoadLibraryW(L"Shcore.dll");
	if (hShcore)
	{
		KDPIUtility::pGetDpiForMonitor =
			reinterpret_cast<KGetDpiForMonitor>
			(::GetProcAddress(hShcore, "GetDpiForMonitor")); // win 8.1

		KDPIUtility::pSetProcessDpiAwareness =
			reinterpret_cast<KSetProcessDpiAwareness>
			(::GetProcAddress(hShcore, "SetProcessDpiAwareness")); // win 8.1
	}

	HMODULE hUser32 = ::LoadLibraryW(L"User32.dll");
	if (hUser32)
	{
		KDPIUtility::pSetThreadDpiAwarenessContext =
			reinterpret_cast<KSetThreadDpiAwarenessContext>
			(::GetProcAddress(hUser32, "SetThreadDpiAwarenessContext")); // win10

		KDPIUtility::pSetProcessDpiAwarenessContext =
			reinterpret_cast<KSetProcessDpiAwarenessContext>
			(::GetProcAddress(hUser32, "SetProcessDpiAwarenessContext")); // win10

		KDPIUtility::pAdjustWindowRectExForDpi =
			reinterpret_cast<KAdjustWindowRectExForDpi>
			(::GetProcAddress(hUser32, "AdjustWindowRectExForDpi")); // win10

		KDPIUtility::pSetProcessDPIAware =
			reinterpret_cast<KSetProcessDPIAware>
			(::GetProcAddress(hUser32, "SetProcessDPIAware")); // win7,8
	}
}

// https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-3/
WORD KDPIUtility::getWindowDPI(HWND hWnd)
{
	if (KDPIUtility::pGetDpiForMonitor != nullptr)
	{
		HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
		UINT uiDpiX, uiDpiY;
		HRESULT hr = KDPIUtility::pGetDpiForMonitor(hMonitor, 0, &uiDpiX, &uiDpiY);

		if (SUCCEEDED(hr))
			return static_cast<WORD>(uiDpiX);
	}

	// for win8 & win7
	HDC hScreenDC = ::GetDC(0);
	int iDpiX = ::GetDeviceCaps(hScreenDC, LOGPIXELSX);
	::ReleaseDC(0, hScreenDC);

	return static_cast<WORD>(iDpiX);
}

BOOL KDPIUtility::adjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi)
{
	if (KDPIUtility::pAdjustWindowRectExForDpi)
		return pAdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);

	return ::AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

void KDPIUtility::makeProcessDPIAware(KDPIAwareness dpiAwareness)
{
	if (dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY)
	{
		if (KDPIUtility::pSetProcessDpiAwarenessContext)
		{
			KDPIUtility::pSetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			KApplication::dpiAwareAPICalled = true;
		}
	}
	else if (dpiAwareness == KDPIAwareness::STANDARD_MODE)
	{
		if (KDPIUtility::pSetProcessDpiAwarenessContext)
		{
			KDPIUtility::pSetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			KApplication::dpiAwareAPICalled = true;
		}
		else if (KDPIUtility::pSetProcessDpiAwareness)
		{
			KDPIUtility::pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			KApplication::dpiAwareAPICalled = true;
		}
		else if (KDPIUtility::pSetProcessDPIAware)
		{
			KDPIUtility::pSetProcessDPIAware();
			KApplication::dpiAwareAPICalled = true;
		}
	}
}

// https://stackoverflow.com/questions/70976583/get-real-screen-resolution-using-win32-api
float KDPIUtility::getMonitorScalingRatio(HMONITOR monitor)
{
	MONITORINFOEXW info = {};
	info.cbSize = sizeof(MONITORINFOEXW);
	::GetMonitorInfoW(monitor, &info);
	DEVMODEW devmode = {};
	devmode.dmSize = sizeof(DEVMODEW);
	::EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
	return (info.rcMonitor.right - info.rcMonitor.left) / static_cast<float>(devmode.dmPelsWidth);
}

float KDPIUtility::getScaleForMonitor(HMONITOR monitor)
{
	return (float)(::GetDpiForSystem() / 96.0 / getMonitorScalingRatio(monitor));
}

int KDPIUtility::scaleToWindowDPI(int valueFor96DPI, HWND window)
{
	return KDPIUtility::scaleToNewDPI(valueFor96DPI, KDPIUtility::getWindowDPI(window));
}

int KDPIUtility::scaleToNewDPI(int valueFor96DPI, int newDPI)
{
	return ::MulDiv(valueFor96DPI, newDPI, USER_DEFAULT_SCREEN_DPI);
}

// =========== KModuleManager.cpp ===========

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



bool KModuleManager::registerRFCModule(int index, RFCModuleInitFunc initFunc, RFCModuleFreeFunc freeFunc)
{
	KModuleManager::rfcModuleInitFuncList()[index] = initFunc;
	KModuleManager::rfcModuleFreeFuncList()[index] = freeFunc;
	return true;
}

RFCModuleInitFunc* KModuleManager::rfcModuleInitFuncList()
{
	static RFCModuleInitFunc funcList[MAX_RFC_MODULE_COUNT] = {0};
	return funcList;
}

RFCModuleFreeFunc* KModuleManager::rfcModuleFreeFuncList()
{
	static RFCModuleFreeFunc funcList[MAX_RFC_MODULE_COUNT] = {0};
	return funcList;
}

// =========== KString.cpp ===========
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

#include <stdio.h>

const KString operator+ (const char* const string1, const KString& string2)
{
	KString s(string1);
	return s.append(string2);
}

const KString operator+ (const wchar_t* const string1, const KString& string2)
{
	KString s(string1, KStringBehaviour::DO_NOT_FREE, -1);
	return s.append(string2);
}

const KString operator+ (const KString& string1, const KString& string2)
{
	return string1.append(string2);
}

namespace kstring_literals {
	KString operator"" _st(const wchar_t* str, size_t len) {
		return KString(str, KStringBehaviour::DO_NOT_FREE, (int)len);
	}
}

void KString::markAsEmptyString()
{
	characterCount = 0;
	data.ssoBuffer[0] = 0;
	bufferType = KStringBufferType::SSOText;
}

KString::KString()
{
	markAsEmptyString();
}

void KString::initFromLiteral(const wchar_t* literal, size_t N)
{
	characterCount = (int)N-1;
	data.staticText = literal;
	bufferType = KStringBufferType::StaticText;
}

void KString::assignFromLiteral(const wchar_t* literal, size_t N)
{
	if (bufferType == KStringBufferType::HeapText)
		data.refCountedMem->releaseReference();

	initFromLiteral(literal, N);
}

void KString::copyFromOther(const KString& other)
{
	bufferType = other.bufferType;
	characterCount = other.characterCount;

	if (bufferType == KStringBufferType::StaticText)
	{
		data.staticText = other.data.staticText;
	}
	else if (bufferType == KStringBufferType::SSOText)
	{
		// Copy SSO buffer
		::memcpy(data.ssoBuffer, other.data.ssoBuffer, (characterCount + 1) * sizeof(wchar_t));
	}
	else if (bufferType == KStringBufferType::HeapText)
	{
		other.data.refCountedMem->addReference();
		data.refCountedMem = other.data.refCountedMem;
	}
	else
	{
		markAsEmptyString();
		K_ASSERT(false, "other string type is not sso, static or heap");
	}
}


KString::KString(const KString& other)
{
	copyFromOther(other);
}

KString::KString(KString&& other) noexcept
{
	copyFromOther(other);
	other.clear();
}

KString::KString(const char* const text, UINT codePage)
{
	if (text != nullptr)
	{
		int countWithNull = ::MultiByteToWideChar(codePage, 0, text, -1, 0, 0); // get char count with null character
		if (countWithNull > 1)
		{
			if (countWithNull <= SSO_BUFFER_SIZE)
			{
				if (::MultiByteToWideChar(codePage, 0, text, -1, data.ssoBuffer, countWithNull))
				{
					characterCount = countWithNull - 1; // ignore null character
					bufferType = KStringBufferType::SSOText;
					return;
				}
			}
			else
			{
				wchar_t* w_text = (wchar_t*)::malloc(countWithNull * sizeof(wchar_t));
				if (::MultiByteToWideChar(codePage, 0, text, -1, w_text, countWithNull))
				{
					data.refCountedMem = new KRefCountedMemory<wchar_t*>(w_text);
					characterCount = countWithNull - 1; // ignore null character
					bufferType = KStringBufferType::HeapText;
					return;
				}
				else
				{
					::free(w_text);
				}
			}
		}
	}

	markAsEmptyString();
}

KString::KString(const wchar_t* const text, KStringBehaviour behaviour, int length)
{
	if (text != nullptr)
	{
		int textLength = ((length == -1) ? (int)::wcslen(text) : length);
		if (textLength > 0)
		{
			characterCount = textLength;

			if (behaviour == KStringBehaviour::DO_NOT_FREE)
			{
				data.staticText = text;
				bufferType = KStringBufferType::StaticText;
				return;
			}
			else if (behaviour == KStringBehaviour::FREE_ON_DESTROY)
			{
				data.refCountedMem = new KRefCountedMemory<wchar_t*>((wchar_t*)text);
				bufferType = KStringBufferType::HeapText;
				return;
			}
			else if (behaviour == KStringBehaviour::MAKE_A_COPY)
			{
				if (characterCount < SSO_BUFFER_SIZE)
				{
					::memcpy(data.ssoBuffer, text, (characterCount + 1) * sizeof(wchar_t));
					bufferType = KStringBufferType::SSOText;
					return;
				}
				else
				{
					// since we already know the length, we can use malloc and memcpy instead of wcsdup.
					const int countWithNull = characterCount + 1;
					wchar_t* buffer = (wchar_t*)::malloc(countWithNull * sizeof(wchar_t));
					::memcpy(buffer, text, countWithNull * sizeof(wchar_t));
					data.refCountedMem = new KRefCountedMemory<wchar_t*>(buffer);
					bufferType = KStringBufferType::HeapText;
					return;
				}
			}
			else
			{
				K_ASSERT(false, "unknown KString behaviour");
			}
		}
	}

	markAsEmptyString();
}

KString::KString(const int value, const int radix)
{
	// Min Buffer Size required to convert radix 10 integer is 11 chars.

	if ((radix == 10) && (12 <= SSO_BUFFER_SIZE))
	{
		::_itow_s(value, data.ssoBuffer, 12, radix);
		characterCount = (int)::wcslen(data.ssoBuffer);
		bufferType = KStringBufferType::SSOText;
	}
	else
	{
		wchar_t* buffer = (wchar_t*)::malloc(34 * sizeof(wchar_t)); // max 33 digits
		::_itow_s(value, buffer, 34, radix);
		characterCount = (int)::wcslen(buffer);
		data.refCountedMem = new KRefCountedMemory<wchar_t*>(buffer);
		bufferType = KStringBufferType::HeapText;	
	}
}

KString::KString(const float value, const int numDecimals, bool compact)
{
	// round it to given digits
	char str_fmtp[32];
	char str_buf[64];

	sprintf_s(str_fmtp, 32, "%%.%df", numDecimals);
	sprintf_s(str_buf, 64, str_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(str_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (str_buf[len - i] == '0')
				str_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(str_buf) - 1;
		if (str_buf[len] == '.')
			str_buf[len] = 0; // kill it
	}

	const int countWithNull = ::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, 0, 0); // get char count with null character
	if (countWithNull > 1)
	{
		if (countWithNull <= SSO_BUFFER_SIZE)
		{
			if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, data.ssoBuffer, countWithNull))
			{
				characterCount = countWithNull - 1; // ignore null character
				bufferType = KStringBufferType::SSOText;
				return;
			}
		}
		else
		{
			wchar_t* w_text = (wchar_t*)::malloc(countWithNull * sizeof(wchar_t));
			if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, w_text, countWithNull))
			{
				characterCount = countWithNull - 1;
				data.refCountedMem = new KRefCountedMemory<wchar_t*>(w_text);
				bufferType = KStringBufferType::HeapText;
				return;
			}
			else
			{
				::free(w_text);
			}
		}
	}

	markAsEmptyString();
}

const KString& KString::operator= (const KString& other)
{
	if (this != &other)
	{
		if (bufferType == KStringBufferType::HeapText)
			data.refCountedMem->releaseReference();
		
		copyFromOther(other);
	}
	return *this;
}

KString& KString::operator= (KString&& other)
{
	if (this != &other)
	{
		if (bufferType == KStringBufferType::HeapText)
			data.refCountedMem->releaseReference();

		copyFromOther(other);
		other.clear();
	}
	return *this;
}

const KString KString::operator+ (const KString& stringToAppend)
{
	return append(stringToAppend);
}

bool KString::operator== (const KString& other) const
{
	return compare(other);
}

KString::operator const wchar_t*()const
{
	return getStringPtr();
}

const wchar_t KString::operator[](const int index)const
{
	if (characterCount > 0)
	{
		if ((0 <= index) && (index < characterCount))
			return getStringPtr()[index];
	}
	return -1;
}

KString KString::append(const KString& otherString)const
{
	if (otherString.characterCount != 0)
	{
		if (characterCount != 0)
		{
			const int totalCharacterCount = characterCount + otherString.characterCount;
			const int sizeWithNull = totalCharacterCount + 1;

			if (totalCharacterCount < SSO_BUFFER_SIZE)
			{
				KString retStr;
				retStr.characterCount = totalCharacterCount;
				retStr.bufferType = KStringBufferType::SSOText;

				::memcpy(retStr.data.ssoBuffer, getStringPtr(), (characterCount + 1) * sizeof(wchar_t));
				::memcpy(&retStr.data.ssoBuffer[characterCount], otherString.getStringPtr(), 
					(otherString.characterCount + 1) * sizeof(wchar_t));

				return retStr;
			}
			else
			{		
				wchar_t* destText = (wchar_t*)::malloc(sizeWithNull * sizeof(wchar_t));
				::memcpy(destText, getStringPtr(), (characterCount + 1) * sizeof(wchar_t));
				::memcpy(&destText[characterCount], otherString.getStringPtr(),
					(otherString.characterCount + 1) * sizeof(wchar_t));

				return KString(destText, KStringBehaviour::FREE_ON_DESTROY, totalCharacterCount);
			}
		}
		else // this string is empty
		{
			return otherString;
		}
	}
	else // other string is empty
	{
		return *this;
	}
}

KString KString::appendStaticText(const wchar_t* const text, int length, bool appendToEnd)const
{
	if(length == 0)
		return *this;

	if (characterCount != 0)
	{
		const int totalCharacterCount = characterCount + length;
		const int sizeWithNull = totalCharacterCount + 1;

		if (totalCharacterCount < SSO_BUFFER_SIZE)
		{
			KString retStr;
			retStr.characterCount = totalCharacterCount;
			retStr.bufferType = KStringBufferType::SSOText;

			if (appendToEnd)
			{
				::memcpy(retStr.data.ssoBuffer, getStringPtr(), (characterCount + 1) * sizeof(wchar_t));
				::memcpy(&retStr.data.ssoBuffer[characterCount], text, (length + 1) * sizeof(wchar_t));
			}
			else
			{
				::memcpy(retStr.data.ssoBuffer, text, (length + 1) * sizeof(wchar_t));
				::memcpy(&retStr.data.ssoBuffer[length], getStringPtr(), (characterCount + 1) * sizeof(wchar_t));
			}

			return retStr;
		}
		else
		{
			wchar_t* destText = (wchar_t*)::malloc(sizeWithNull * sizeof(wchar_t));

			if (appendToEnd)
			{
				::memcpy(destText, getStringPtr(), (characterCount + 1) * sizeof(wchar_t));
				::memcpy(&destText[characterCount], text, (length + 1) * sizeof(wchar_t));
			}
			else
			{
				::memcpy(destText, text, (length + 1) * sizeof(wchar_t));
				::memcpy(&destText[length], getStringPtr(), (characterCount + 1) * sizeof(wchar_t));
			}

			return KString(destText, KStringBehaviour::FREE_ON_DESTROY, totalCharacterCount);
		}
	}
	else // this string is empty
	{
		return KString(text, KStringBehaviour::DO_NOT_FREE, length);
	}
}

void KString::assignStaticText(const wchar_t* const text, int length)
{
	if (bufferType == KStringBufferType::HeapText)
		data.refCountedMem->releaseReference();
	
	characterCount = length;
	bufferType = KStringBufferType::StaticText;
	data.staticText = text;
}

void KString::clear()
{
	if (bufferType == KStringBufferType::HeapText)
		data.refCountedMem->releaseReference();

	markAsEmptyString();
}

void KString::accessRawSSOBuffer(wchar_t** ssoBuffer, int** ppLength)
{
	clear();

	*ppLength = &characterCount;
	*ssoBuffer = data.ssoBuffer;
}

KString KString::subString(int start, int end)const
{
	const int lastIndex = characterCount -1;

	if ((0 <= start) && (start <= lastIndex))
	{
		if ((start < end) && (end <= lastIndex))
		{
			int size = (end - start) + 1;

			// todo: can be optimized with sso buffer

			wchar_t* buf = (wchar_t*)::malloc((size + 1) * sizeof(wchar_t));
			const wchar_t* src = getStringPtr();
			::wcsncpy_s(buf, (size + 1), &src[start], size);
			buf[size] = 0;

			return KString(buf, KStringBehaviour::FREE_ON_DESTROY, size);
		}
	}
	return KString();
}

bool KString::compareIgnoreCase(const KString& otherString)const
{
	if ((otherString.characterCount != 0) && (characterCount != 0))
		return (::_wcsicmp(getStringPtr(), otherString.getStringPtr()) == 0);

	return false;
}

bool KString::compare(const KString& otherString)const
{
	if ((otherString.characterCount != 0) && (characterCount != 0))
		return (::wcscmp(getStringPtr(), otherString.getStringPtr()) == 0);

	return false;
}

bool KString::compareWithStaticText(const wchar_t* const text)const
{
	if (characterCount != 0)
		return (::wcscmp(getStringPtr(), text) == 0);

	return false;
}

bool KString::startsWithChar(wchar_t character)const
{
	if (characterCount != 0)
		return (getStringPtr()[0] == character);

	return false;
}

bool KString::endsWithChar(wchar_t character)const
{
	if (characterCount != 0)
		return (getStringPtr()[characterCount - 1] == character);

	return false;
}

bool KString::isQuotedString()const
{
	if (characterCount > 1) // not empty + count greater than 1
		return (startsWithChar(L'\"') && endsWithChar(L'\"'));

	return false;
}

wchar_t KString::getCharAt(int index)const
{
	if ((0 <= index) && (index < characterCount))
		return getStringPtr()[index];

	return -1;
}

KStringBufferType KString::getBufferType()const
{
	return bufferType;
}

int KString::length()const
{
	return characterCount;
}

bool KString::isEmpty()const
{
	return (characterCount == 0);
}

bool KString::isNotEmpty()const
{
	return (characterCount != 0);
}

int KString::getIntValue()const
{
	if (characterCount == 0)
		return 0;

	return ::_wtoi(getStringPtr());
}

KString KString::toUpperCase()const
{
	if (characterCount == 0)
		return KString();

	KString result(getStringPtr(), KStringBehaviour::MAKE_A_COPY, characterCount);
	::CharUpperBuffW((wchar_t*)result.getStringPtr(), result.characterCount);

	return result;
}

KString KString::toLowerCase()const
{
	if (characterCount == 0)
		return KString();

	KString result(getStringPtr(), KStringBehaviour::MAKE_A_COPY, characterCount);
	::CharLowerBuffW((wchar_t*)result.getStringPtr(), result.characterCount);

	return result;
}

char* KString::toAnsiString(const wchar_t* text)
{
	if (text != nullptr)
	{
		const int length = ::WideCharToMultiByte(CP_UTF8, 0, text, -1, 0, 0, 0, 0);
		if (length)
		{
			char* retText = (char*)::malloc(length);
			if (::WideCharToMultiByte(CP_UTF8, 0, text, -1, retText, length, 0, 0))
				return retText;

			// conversion error
			retText[0] = 0;
			return retText;
		}
	}

	char* retText = (char*)::malloc(1);
	retText[0] = 0;
	return retText;
}

wchar_t* KString::toUnicodeString(const char* text)
{
	if (text != nullptr)
	{
		const int length = ::MultiByteToWideChar(CP_UTF8, 0, text, -1, 0, 0);
		if (length)
		{
			wchar_t* retText = (wchar_t*)::malloc(length * sizeof(wchar_t));
			if (::MultiByteToWideChar(CP_UTF8, 0, text, -1, retText, length))
				return retText;

			// conversion error
			retText[0] = 0;
			return retText;
		}
	}

	wchar_t* retText = (wchar_t*)::malloc(sizeof(wchar_t));
	retText[0] = 0;
	return retText;	
}

const wchar_t* KString::getStringPtr() const
{
	if (bufferType == KStringBufferType::StaticText)
		return data.staticText;
	else if (bufferType == KStringBufferType::SSOText)
		return data.ssoBuffer;
	else if (bufferType == KStringBufferType::HeapText)
		return data.refCountedMem->buffer;
	else
		return L"";
}

KString::~KString()
{
	if (bufferType == KStringBufferType::HeapText)
		data.refCountedMem->releaseReference();
}

// =========== KDirectory.cpp ===========

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



KDirectory::KDirectory(){}

KDirectory::~KDirectory(){}

bool KDirectory::isDirExists(const KString& dirName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::createDir(const KString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool KDirectory::removeDir(const KString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

void KDirectory::getModuleDir(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars)
{
	outBuffer[0] = 0;
	::GetModuleFileNameW(hModule, outBuffer, bufferSizeInWChars);

	wchar_t *p;
	for (p = outBuffer; *p; p++) {}	// find end
	for (; p > outBuffer && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it
}

void KDirectory::getModuleFilePath(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars)
{
	outBuffer[0] = 0;
	::GetModuleFileNameW(hModule, outBuffer, bufferSizeInWChars);
}

void KDirectory::getParentDir(const wchar_t* filePath, wchar_t* outBuffer, int bufferSizeInWChars)
{
	::wcscpy_s(outBuffer, bufferSizeInWChars, filePath);

	wchar_t *p;
	for (p = outBuffer; *p; p++) {}	// find end
	for (; p > outBuffer && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it
}

void KDirectory::getTempDir(wchar_t* outBuffer, int bufferSizeInWChars)
{
	outBuffer[0] = 0;
	::GetTempPathW(bufferSizeInWChars, outBuffer);
}

void KDirectory::getAllUserDataDir(wchar_t* outBuffer)
{
	outBuffer[0] = 0;
	::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, outBuffer);
}

void KDirectory::getLoggedInUserFolderPath(int csidl, wchar_t* outBuffer)
{
	DWORD dwProcessId;
	::GetWindowThreadProcessId(::GetShellWindow(), &dwProcessId);

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

	HANDLE tokenHandle = NULL;
	::OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_IMPERSONATE, &tokenHandle);
	::CloseHandle(hProcess);

	outBuffer[0] = 0;
	::SHGetFolderPathW(NULL, csidl, tokenHandle, 0, outBuffer);
	::CloseHandle(tokenHandle);
}

void KDirectory::getRoamingFolder(wchar_t* outBuffer)
{
	KDirectory::getLoggedInUserFolderPath(CSIDL_APPDATA, outBuffer);
}

void KDirectory::getNonRoamingFolder(wchar_t* outBuffer)
{
	KDirectory::getLoggedInUserFolderPath(CSIDL_LOCAL_APPDATA, outBuffer);
}

KPointerList<KString*,32, false>* KDirectory::scanFolderForExtension(const KString& folderPath, const KString& extension)
{
	KPointerList<KString*,32,false>* result = new KPointerList<KString*,32,false>();
	WIN32_FIND_DATAW findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	KString searchPath(folderPath + L"\\*." + extension);

	hFind = ::FindFirstFileW(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		return result;

	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			result->add(new KString(findData.cFileName,KStringBehaviour::MAKE_A_COPY));
		}
	} while (::FindNextFileW(hFind, &findData) != 0);

	::FindClose(hFind);

	return result;
}

// =========== KFile.cpp ===========

/*
	RFC - KFile.cpp
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


KFile::KFile()
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const wchar_t* fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::openFile(const wchar_t* fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool KFile::closeFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::getFileHandle()
{
	return fileHandle;
}

KFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD KFile::readFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::writeFile(const void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::setFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool KFile::setFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::getFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool KFile::setFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::getFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* KFile::readAsData()
{
	const DWORD fileSize = getFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool KFile::writeString(const KString& text, bool isUnicode)
{
	if (isUnicode)
	{
		void* buffer = (void*)(const wchar_t*)text;
		const DWORD numberOfBytesToWrite = text.length() * sizeof(wchar_t);
		const DWORD numberOfBytesWritten = writeFile(buffer, numberOfBytesToWrite);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
	else
	{
		void* buffer = (void*)KString::toAnsiString(text);
		const DWORD numberOfBytesToWrite = text.length() * sizeof(char);
		const DWORD numberOfBytesWritten = writeFile(buffer, numberOfBytesToWrite);
		::free(buffer);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
}

KString KFile::readAsString(bool isUnicode)
{
	DWORD fileSize = getFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (isUnicode)
			{
				return KString((const wchar_t*)buffer, KStringBehaviour::FREE_ON_DESTROY);
			}
			else
			{
				KString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return KString();
}

bool KFile::deleteFile(const wchar_t* fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool KFile::copyFile(const wchar_t* sourceFileName, const wchar_t* destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

KString KFile::getFileNameFromPath(const wchar_t* path)
{
	const wchar_t* fileNamePtr = ::PathFindFileNameW(path);

	if (path != fileNamePtr)
		return KString(fileNamePtr, KStringBehaviour::MAKE_A_COPY);

	return KString();
}

KString KFile::getFileExtension(const wchar_t* path)
{
	const wchar_t* extPtr = ::PathFindExtensionW(path);
	return KString(extPtr, KStringBehaviour::MAKE_A_COPY);
}

bool KFile::isFileExists(const wchar_t* fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

KFile::~KFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}

// =========== KLogger.cpp ===========

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


KLogger::KLogger(DWORD bufferSize)
{
	buffer = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;
	bufferIndex = 0;
	totalEvents = 0;
	totalMills = 0;
	bufferFull = false;
	isFirstCall = true;
}

bool KLogger::writeNewEvent(unsigned char eventType)
{
	if (!bufferFull)
	{
		if ((bufferIndex + 300) >= bufferSize) // assume each event data is not greater than 300 bytes
		{
			bufferFull = true;
			return false;
		}

		unsigned short secs = 0;
		unsigned short mills = 0;

		if (isFirstCall)
		{
			pCounter.startCounter();
			isFirstCall = false;
			totalMills = 0;
		}
		else{
			const double deltaMills = pCounter.endCounter();
			totalMills += (unsigned int)deltaMills;

			secs = (unsigned short)(totalMills/1000);
			mills = (unsigned short)(totalMills % 1000);

			pCounter.startCounter();
		}

		buffer[bufferIndex] = eventType; // write event type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = secs; // write secs
		bufferIndex += sizeof(unsigned short);

		*((unsigned short*)&buffer[bufferIndex]) = mills; // write mills
		bufferIndex += sizeof(unsigned short);

		totalEvents++;

		return true;
	}
	return false;
}

bool KLogger::endEvent()
{
	if (!bufferFull)
	{
		buffer[bufferIndex] = EVT_END; // write event end
		bufferIndex += sizeof(unsigned char);

		return true;
	}
	return false;
}

bool KLogger::addTextParam(const char *text, unsigned char textLength)
{
	if( (textLength < 255) && (!bufferFull) )
	{
		buffer[bufferIndex] = PARAM_STRING; // write param type
		bufferIndex += sizeof(unsigned char);

		buffer[bufferIndex] = textLength; // write data size
		bufferIndex += sizeof(unsigned char);

		for (int i = 0; i < textLength; i++) // write data
		{
			buffer[bufferIndex] = text[i];
			bufferIndex += sizeof(unsigned char);
		}

		return true;
	}
	return false;
}

bool KLogger::addIntParam(int value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_INT32; // write param type
		bufferIndex += sizeof(unsigned char);

		*((int*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(int);

		return true;
	}
	return false;
}

bool KLogger::addShortParam(unsigned short value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_SHORT16; // write param type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(unsigned short);

		return true;
	}
	return false;
}

bool KLogger::addFloatParam(float value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_FLOAT; // write param type
		bufferIndex += sizeof(unsigned char);

		*((float*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(float);

		return true;
	}
	return false;
}
	
bool KLogger::addDoubleParam(double value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_DOUBLE; // write param type
		bufferIndex += sizeof(unsigned char);

		*((double*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(double);

		return true;
	}
	return false;
}

bool KLogger::isBufferFull()
{
	return bufferFull;
}

bool KLogger::writeToFile(const KString &filePath)
{
	KFile file;

	if (KFile::isFileExists(filePath))
		KFile::deleteFile(filePath);

	if (file.openFile(filePath, KFile::KWRITE))
	{
		file.writeFile((void*)"RLOG", 4);
		file.writeFile(&totalEvents, 4);
		file.writeFile(buffer, bufferIndex);

		return true;
	}

	return false;
}

KLogger::~KLogger()
{
	free(buffer);
}

// =========== KSettingsReader.cpp ===========

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


KSettingsReader::KSettingsReader()
{

}

bool KSettingsReader::openFile(const wchar_t* fileName, int formatID)
{
	if (!KFile::isFileExists(fileName))
		return false;

	if (!settingsFile.openFile(fileName, KFile::KREAD))
		return false;

	settingsFile.setFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.readFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void KSettingsReader::readData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.readFile(buffer, size);
}

KString KSettingsReader::readString()
{
	int size = 0;
	settingsFile.readFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.readFile(buffer, size);

		return KString(buffer, KStringBehaviour::FREE_ON_DESTROY);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::readInt()
{
	int value = 0;
	settingsFile.readFile(&value, sizeof(int));

	return value;
}

float KSettingsReader::readFloat()
{
	float value = 0;
	settingsFile.readFile(&value, sizeof(float));

	return value;
}

double KSettingsReader::readDouble()
{
	double value = 0;
	settingsFile.readFile(&value, sizeof(double));

	return value;
}

bool KSettingsReader::readBool()
{
	bool value = 0;
	settingsFile.readFile(&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader()
{

}

// =========== KSettingsWriter.cpp ===========

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


KSettingsWriter::KSettingsWriter() {}

bool KSettingsWriter::openFile(const wchar_t* fileName, int formatID)
{
	if (KFile::isFileExists(fileName))
		KFile::deleteFile(fileName);

	if (!settingsFile.openFile(fileName, KFile::KWRITE))
		return false;

	settingsFile.setFilePointerToStart();
	settingsFile.writeFile(&formatID, sizeof(int));

	return true;
}

void KSettingsWriter::writeData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.writeFile(buffer, size);
}

void KSettingsWriter::writeString(const KString& text)
{
	int size = text.length();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.writeFile(&size, sizeof(int));

		settingsFile.writeFile((const wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.writeFile(&size, sizeof(int));
	}
}

void KSettingsWriter::writeInt(int value)
{
	settingsFile.writeFile(&value, sizeof(int));
}

void KSettingsWriter::writeFloat(float value)
{
	settingsFile.writeFile(&value, sizeof(float));
}

void KSettingsWriter::writeDouble(double value)
{
	settingsFile.writeFile(&value, sizeof(double));
}

void KSettingsWriter::writeBool(bool value)
{
	settingsFile.writeFile(&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter()
{

}

// =========== GUIModule.cpp ===========

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

#include <commctrl.h>

class RFC_GUIModule 
{
public:
	static bool rfcModuleInit()
	{		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);
		
		KGUIProc::atomComponent = ::GlobalAddAtomW(L"RFCComponent");
		KGUIProc::atomOldProc = ::GlobalAddAtomW(L"RFCOldProc");

		return true;
	}

	static void rfcModuleFree()
	{
		::GlobalDeleteAtom(KGUIProc::atomComponent);
		::GlobalDeleteAtom(KGUIProc::atomOldProc);
	}
};

REGISTER_RFC_MODULE(1, RFC_GUIModule)

// =========== KBitmap.cpp ===========

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


KBitmap::KBitmap()
{
	hBitmap = 0;
}

KBitmap::operator HBITMAP()const
{
	return hBitmap;
}

bool KBitmap::loadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool KBitmap::loadFromFile(const KString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(KApplication::hInstance, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

void KBitmap::drawOnHDC(HDC hdc, int x, int y, int width, int height)
{
	HDC memHDC = ::CreateCompatibleDC(hdc);

	::SelectObject(memHDC, hBitmap);
	::BitBlt(hdc, x, y, width, height, memHDC, 0, 0, SRCCOPY);

	::DeleteDC(memHDC);
}

HBITMAP KBitmap::getHandle()
{
	return hBitmap;
}

KBitmap::~KBitmap()
{
	if(hBitmap)
		::DeleteObject(hBitmap);
}

// =========== KButton.cpp ===========

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


KButton::KButton() : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("BUTTON"));
	compText.assignStaticText(TXT_WITH_LEN("Button"));

	compWidth = 100;
	compHeight = 30;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KButton::_onPress()
{
	if(onClick)
		onClick(this);
}

bool KButton::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
	{
		this->_onPress();

		*result = 0;
		return true;
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KButton::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KButton::~KButton()
{
}

// =========== KCheckBox.cpp ===========

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


KCheckBox::KCheckBox()
{
	checked = false;
	compText.assignStaticText(TXT_WITH_LEN("CheckBox"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_NOTIFY | WS_TABSTOP;
}

bool KCheckBox::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KCheckBox::_onPress()
{
	if(::SendMessageW(compHWND, BM_GETCHECK, 0, 0) == BST_CHECKED)
		checked = true;
	else
		checked = false;

	if(onClick)
		onClick(this);
}

bool KCheckBox::isChecked()
{
	return checked;
}

void KCheckBox::setCheckedState(bool state)
{
	checked = state;

	if(compHWND)
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
}

KCheckBox::~KCheckBox()
{
}

// =========== KComboBox.cpp ===========

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


KComboBox::KComboBox(bool sort) : KComponent(false)
{
	selectedItemIndex = -1;

	compClassName.assignStaticText(TXT_WITH_LEN("COMBOBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;

	if(sort)
		compDwStyle = compDwStyle | CBS_SORT;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

void KComboBox::addItem(const KString& text)
{
	stringList.add(text);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)text);
}

void KComboBox::removeItem(int index)
{
	stringList.remove(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::removeItem(const KString& text)
{
	const int itemIndex = getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

int KComboBox::getItemIndex(const KString& text)
{
	return stringList.getIndex(text);
}

int KComboBox::getItemCount()
{
	return stringList.size();
}

int KComboBox::getSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
	}
	return -1;		
}

KString KComboBox::getSelectedItem()
{
	const int itemIndex = getSelectedItemIndex();
	if(itemIndex > -1)
		return stringList.get(itemIndex);

	return KString();
}

void KComboBox::clearList()
{
	stringList.removeAll();
	if(compHWND)
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
}

void KComboBox::selectItem(int index)
{
	selectedItemIndex = index;
	if(compHWND)
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
}

bool KComboBox::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		_onItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KComboBox::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register COMBOBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList.size();
		if(listSize)
		{
			for (int i = 0; i < listSize; i++)
			{
				::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)stringList.get(i));
			}
		}

		if(selectedItemIndex > -1)
			::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KComboBox::_onItemSelect()
{
	if(onItemSelect)
		onItemSelect(this);
}

KComboBox::~KComboBox() {}



// =========== KCommonDialogBox.cpp ===========

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



#define RFC_OSD_REG_LOCATION L"Software\\CrownSoft\\RFC\\OSD"

bool KCommonDialogBox::showOpenFileDialog(KWindow* window, 
	const KString& title, 
	const wchar_t* filter, 
	KString* fileName, 
	bool saveLastLocation, 
	const KString& dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!	
	wchar_t *buff = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	buff[0] = 0;
 
	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	KString lastLocation;
	if (saveLastLocation)
	{
		KRegistry::readString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.length() > 0)
			ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
	}

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->getHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetOpenFileNameW(&ofn))
	{
		KString path(buff, KStringBehaviour::FREE_ON_DESTROY);
		*fileName = path;

		if (saveLastLocation)
		{
			wchar_t parentDir[RFC_MAX_PATH];
			KDirectory::getParentDir(path, parentDir, RFC_MAX_PATH);
			::wcscat_s(parentDir, RFC_MAX_PATH, L"\\");

			KRegistry::createKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::writeString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, 
				KString(parentDir, KStringBehaviour::DO_NOT_FREE));
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

bool KCommonDialogBox::showSaveFileDialog(KWindow* window, 
	const KString& title, 
	const wchar_t* filter, 
	KString* fileName,
	bool saveLastLocation,
	const KString& dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!
	wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	buff[0] = 0;

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	KString lastLocation;
	if (saveLastLocation)
	{		
		KRegistry::readString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.length() > 0)
			ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
	}

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->getHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetSaveFileNameW(&ofn))
	{
		KString path(buff, KStringBehaviour::FREE_ON_DESTROY);
		*fileName = path;

		if (saveLastLocation)
		{
			wchar_t parentDir[RFC_MAX_PATH];
			KDirectory::getParentDir(path, parentDir, RFC_MAX_PATH);
			::wcscat_s(parentDir, RFC_MAX_PATH, L"\\");

			KRegistry::createKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::writeString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, 
				KString(parentDir, KStringBehaviour::DO_NOT_FREE));
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

// =========== KComponent.cpp ===========

/*
	RFC - KComponent.cpp
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



KComponent::KComponent(bool generateWindowClassDetails)
{
	isRegistered = false;

	KIDGenerator *idGenerator = KIDGenerator::getInstance();
	compCtlID = idGenerator->generateControlID();

	compHWND = 0;
	compParentHWND = 0;
	compDwStyle = 0;
	compDwExStyle = 0;
	cursor = 0;
	compX = 0;
	compY = 0;
	compWidth = CW_USEDEFAULT;
	compHeight = CW_USEDEFAULT;
	compDPI = USER_DEFAULT_SCREEN_DPI;
	compVisible = true;
	compEnabled = true;

	if (generateWindowClassDetails)
	{
		idGenerator->generateClassName(compClassName);
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = 0;
		wc.lpszMenuName = 0;
		wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIconSm = 0;
		wc.style = 0;
		wc.hInstance = KApplication::hInstance;
		wc.lpszClassName = compClassName;

		wc.lpfnWndProc = KGUIProc::windowProc;
	}

	compFont = KFont::getDefaultFont();
}

KComponent::operator HWND()const
{
	return compHWND;
}

void KComponent::onHotPlug()
{

}

void KComponent::hotPlugInto(HWND component, bool fetchInfo)
{
	compHWND = component;

	if (fetchInfo)
	{
		wchar_t *clsName = (wchar_t*)::malloc(256 * sizeof(wchar_t));  // assume 256 is enough
		clsName[0] = 0;
		::GetClassNameW(compHWND, clsName, 256);
		compClassName = KString(clsName, KStringBehaviour::FREE_ON_DESTROY);

		::GetClassInfoExW(KApplication::hInstance, compClassName, &wc);

		compCtlID = (UINT)::GetWindowLongPtrW(compHWND, GWL_ID);

		RECT rect;
		::GetWindowRect(compHWND, &rect);
		compWidth = rect.right - rect.left;
		compHeight = rect.bottom - rect.top;
		compX = rect.left;
		compY = rect.top;

		compVisible = (::IsWindowVisible(compHWND) ? true : false);
		compEnabled = (::IsWindowEnabled(compHWND) ? true : false);

		compDwStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_STYLE);
		compDwExStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_EXSTYLE);

		compParentHWND = ::GetParent(compHWND);

		wchar_t *buff = (wchar_t*)::malloc(256 * sizeof(wchar_t)); // assume 256 is enough
		buff[0] = 0;
		::GetWindowTextW(compHWND, buff, 256);
		compText = KString(buff, KStringBehaviour::FREE_ON_DESTROY);
	}

	KGUIProc::attachRFCPropertiesToHWND(compHWND, (KComponent*)this);	

	this->onHotPlug();
}

UINT KComponent::getControlID()
{
	return compCtlID;
}

void KComponent::setMouseCursor(KCursor *cursor)
{
	this->cursor = cursor;
	if(compHWND)
		::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->getHandle());
}

KString KComponent::getComponentClassName()
{
	return compClassName;
}

bool KComponent::create(bool requireInitialMessages)
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered = true;

	KGUIProc::createComponent(this, requireInitialMessages);

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);
		::ShowWindow(compHWND, compVisible ? SW_SHOW : SW_HIDE);

		if(cursor)
			::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->getHandle());

		return true;
	}
	return false;
}

void KComponent::destroy()
{
	if (compHWND)
	{
		::DestroyWindow(compHWND);
		compHWND = 0;
	}
}

LRESULT KComponent::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::atomOldProc));
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)KGUIProc::windowProc) // it's a subclassed common control or hot-plugged dialog! RFCOldProc property of subclassed control|dialog is not KGUIProc::windowProc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam); // custom control or window
}

bool KComponent::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	return false;
}

void KComponent::setFont(KFont *compFont)
{
	this->compFont = compFont;
	if(compHWND)
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0));
}

void KComponent::setFont(KFont& compFont)
{
	setFont(&compFont);
}

KFont* KComponent::getFont()
{
	return compFont;
}

KString KComponent::getText()
{
	return compText;
}

void KComponent::setText(const KString& compText)
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, this->compText);
}

void KComponent::setHWND(HWND compHWND)
{
	this->compHWND = compHWND;
}

HWND KComponent::getHWND()
{
	return compHWND;
}

void KComponent::setParentHWND(HWND compParentHWND)
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::getParentHWND()
{
	return compParentHWND;
}

DWORD KComponent::getStyle()
{
	return compDwStyle;
}

void KComponent::setStyle(DWORD compStyle)
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::getExStyle()
{
	return compDwExStyle;
}

void KComponent::setExStyle(DWORD compDwExStyle)
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

int KComponent::getDPI()
{
	return compDPI;
}

int KComponent::getX()
{
	return compX; 
}

int KComponent::getY()
{
	return compY;
}

int KComponent::getWidth()
{
	return compWidth;
}

int KComponent::getHeight()
{
	return compHeight;
}

void KComponent::setDPI(int newDPI)
{
	if (newDPI == compDPI)
		return;

	const int oldDPI = compDPI;
	compDPI = newDPI;

	if (compDwStyle & WS_CHILD) // do not change position and font size of top level windows.
	{
		this->compX = ::MulDiv(compX, newDPI, oldDPI);
		this->compY = ::MulDiv(compY, newDPI, oldDPI);

		if (!compFont->isDefaultFont())
			compFont->setDPI(newDPI);
	}

	this->compWidth = ::MulDiv(compWidth, newDPI, oldDPI);
	this->compHeight = ::MulDiv(compHeight, newDPI, oldDPI);

	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compX, compY, compWidth, compHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
		if((!compFont->isDefaultFont()) && (compDwStyle & WS_CHILD))
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0));
	}
}

void KComponent::setSize(int compWidth, int compHeight)
{
	this->compWidth = compWidth;
	this->compHeight = compHeight;

	if(compHWND)
		::SetWindowPos(compHWND, 0, 0, 0, compWidth, compHeight, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::setPosition(int compX, int compY)
{
	this->compX = compX;
	this->compY = compY;

	if(compHWND)
		::SetWindowPos(compHWND, 0, compX, compY, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::setVisible(bool state)
{
	compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::isVisible()
{
	if (compHWND)
	{
		compVisible = (::IsWindowVisible(compHWND) == TRUE);
		return compVisible;
	}

	return false;
}

bool KComponent::isEnabled()
{
	if (compHWND)
		compEnabled = (::IsWindowEnabled(compHWND) == TRUE);

	return compEnabled;
}

void KComponent::setEnabled(bool state)
{
	compEnabled = state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::bringToFront()
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::setKeyboardFocus()
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::repaint()
{
	if (compHWND)
	{
		::InvalidateRect(compHWND, NULL, TRUE);
		::UpdateWindow(compHWND); // instant update
	}
}

KComponent::~KComponent()
{
	if(isRegistered)
		::UnregisterClassW(compClassName, KApplication::hInstance);
}

// =========== KCursor.cpp ===========

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


KCursor::KCursor()
{
	hCursor = 0;
}

bool KCursor::loadFromResource(WORD resourceID)
{
	hCursor = (HCURSOR)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

bool KCursor::loadFromFile(const KString& filePath)
{
	hCursor = (HCURSOR)::LoadImageW(KApplication::hInstance, filePath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

HCURSOR KCursor::getHandle()
{
	return hCursor;
}

KCursor::operator HCURSOR()const
{
	return hCursor;
}

KCursor::~KCursor()
{
	if(hCursor)
		::DestroyCursor(hCursor);
}

// =========== KFont.cpp ===========

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


KFont::KFont()
{
	hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	customFont = false;
	currentDPI = USER_DEFAULT_SCREEN_DPI;
}

KFont::KFont(const KString& face, int sizeFor96DPI, bool bold, bool italic, bool underline, bool antiAliased, int requiredDPI)
{
	this->fontFace = face;
	this->fontSizeFor96DPI = sizeFor96DPI;
	this->isBold = bold;
	this->isItalic = italic;
	this->isUnderline = underline;
	this->isAntiAliased = antiAliased;
	this->currentDPI = requiredDPI;

	hFont = ::CreateFontW(fontSizeFor96DPI * requiredDPI / USER_DEFAULT_SCREEN_DPI, 
		0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, underline ? TRUE : FALSE, 0, DEFAULT_CHARSET,
		0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, face);

	if(hFont)
		customFont = true;
}

bool KFont::load(const KString& face, int sizeFor96DPI, bool bold, bool italic, bool underline, bool antiAliased, int requiredDPI)
{
	if (customFont)
		::DeleteObject(hFont);

	this->fontFace = face;
	this->fontSizeFor96DPI = sizeFor96DPI;
	this->isBold = bold;
	this->isItalic = italic;
	this->isUnderline = underline;
	this->isAntiAliased = antiAliased;
	this->currentDPI = requiredDPI;

	hFont = ::CreateFontW(fontSizeFor96DPI * requiredDPI / USER_DEFAULT_SCREEN_DPI,
		0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, underline ? TRUE : FALSE, 0, DEFAULT_CHARSET,
		0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, face);

	if (hFont)
	{
		customFont = true;
		return true;
	}
	else
	{
		customFont = false;
		return false;
	}	
}

void KFont::setDPI(int newDPI)
{
	if( customFont && (currentDPI != newDPI) )
	{
		::DeleteObject(hFont);
		hFont = ::CreateFontW(fontSizeFor96DPI * newDPI / USER_DEFAULT_SCREEN_DPI, 0, 0, 0, isBold ? FW_BOLD : FW_NORMAL,
			isItalic ? TRUE : FALSE, isUnderline ? TRUE : FALSE, 0, DEFAULT_CHARSET, 0, 0,
			isAntiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, 
			fontFace);

		currentDPI = newDPI;
	}
}

KFont* KFont::getDefaultFont()
{
	static KFont defaultInstance;  // Created once, on first use (Meyer's Singleton)
	return &defaultInstance;
}

bool KFont::isDefaultFont()
{
	return !customFont;
}

bool KFont::loadPrivateFont(const KString& path)
{
	return (::AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
}

void KFont::removePrivateFont(const KString& path)
{
	::RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT KFont::getFontHandle()
{
	return hFont;
}

KFont::operator HFONT()const
{
	return hFont;
}

KFont::~KFont()
{
	if(customFont)
		::DeleteObject(hFont);
}

// =========== KGlyphButton.cpp ===========

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

#include <commctrl.h>

KGlyphButton::KGlyphButton()
{
	glyphFont = nullptr;
	glyphChar = nullptr;
	glyphLeft = 6;
}

KGlyphButton::~KGlyphButton()
{
}

void KGlyphButton::setGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->repaint();
}

void KGlyphButton::setDPI(int newDPI)
{
	if (glyphFont)
		glyphFont->setDPI(newDPI);

	KButton::setDPI(newDPI);
}

bool KGlyphButton::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (glyphFont)
	{
		if (msg == WM_NOTIFY)
		{
			if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
			{
				LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)lParam;

				*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

				if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
				{
					*result = CDRF_NOTIFYPOSTPAINT;
				}
				else if (CDDS_POSTPAINT == lpNMCD->dwDrawStage) //  postpaint stage
				{
					const RECT rc = lpNMCD->rc;
					const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED | CDIS_GRAYED)) != 0;

					HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, glyphFont->getFontHandle());
					const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);
					const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

					RECT rcIcon = { rc.left + ::MulDiv(glyphLeft, compDPI, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw glyph

					::SetBkMode(lpNMCD->hdc, oldBkMode);
					::SetTextColor(lpNMCD->hdc, oldTextColor);
					::SelectObject(lpNMCD->hdc, oldFont);

					*result = CDRF_DODEFAULT;
				}

				return true; // indicate that we processed this msg & result is valid.
			}
		}
	}

	return KButton::eventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KGraphics.cpp ===========

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


KGraphics::KGraphics(){}

KGraphics::~KGraphics(){}

void KGraphics::draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	KGraphics::draw3dRect(hdc, startX, startY, 2, 
		height, ::GetSysColor(COLOR_BTNSHADOW), 
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	KGraphics::draw3dRect(hdc, startX, startY, width, 
		2, ::GetSysColor(COLOR_BTNSHADOW), 
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::draw3dRect(hdc, lpRect->left, lpRect->top, 
		lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 
		clrTopLeft, clrBottomRight);
}

void KGraphics::draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::fillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	KGraphics::fillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	KGraphics::fillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	KGraphics::fillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void KGraphics::fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	KGraphics::fillSolidRect(hdc, &rect, color);
}

void KGraphics::fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	const COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

RECT KGraphics::calculateTextSize(const wchar_t* text, HFONT hFont)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = {0, 0, 0, 0};

	::DrawTextW(hDC, text, ::lstrlenW(text), &sz, DT_CALCRECT | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz;
}

int KGraphics::calculateTextHeight(const wchar_t* text, HFONT hFont, int width)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = { 0, 0, width, 0 };

	::DrawTextW(hDC, text, ::lstrlenW(text), &sz, 
		DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT);

	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz.bottom;
}

// https://devblogs.microsoft.com/oldnewthing/20210915-00/?p=105687
void KGraphics::makeBitmapOpaque(HDC hdc, int x, int y, int cx, int cy)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD bitmapBits = { 0x00, 0x00, 0x00, 0xFF };

	::StretchDIBits(hdc, x, y, cx, cy,
		0, 0, 1, 1, &bitmapBits, &bi,
		DIB_RGB_COLORS, SRCPAINT);
}

void KGraphics::setBitmapAlphaChannel(HDC hdc, int x, int y, int cx, int cy, BYTE alpha)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	if (alpha != 255) {
		RGBQUAD zeroAlpha = { 0xFF, 0xFF, 0xFF, 0x00 };
		::StretchDIBits(hdc, x, y, cx, cy,
			0, 0, 1, 1, &zeroAlpha, &bi,
			DIB_RGB_COLORS, SRCAND);
	}

	RGBQUAD alphaOnly = { 0x00, 0x00, 0x00, alpha };
	::StretchDIBits(hdc, x, y, cx, cy,
		0, 0, 1, 1, &alphaOnly, &bi,
		DIB_RGB_COLORS, SRCPAINT);
}

// =========== KGridView.cpp ===========

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

#include <commctrl.h>

KGridView::KGridView(bool sortItems) : KComponent(false)
{
	itemCount = 0;
	colCount = 0;

	compClassName.assignStaticText(TXT_WITH_LEN("SysListView32"));

	compWidth = 300;
	compHeight = 200;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_TABSTOP | WS_BORDER | 
		LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	compDwExStyle = WS_EX_WINDOWEDGE;

	if (sortItems)
		compDwStyle |= LVS_SORTASCENDING;
}

KGridView::~KGridView(){}

void KGridView::insertRecord(KString** columnsData)
{
	LVITEMW lvi = {};
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = itemCount;

	const int row = (int)::SendMessageW(compHWND, 
		LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem = {};
		lvItem.iSubItem = i;
		lvItem.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
			(WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

void KGridView::insertRecordTo(int rowIndex, KString **columnsData)
{
	LVITEMW lvi = {};
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = rowIndex;

	const int row = (int)::SendMessageW(compHWND, 
		LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem= {};
		lvItem.iSubItem = i;
		lvItem.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
			(WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

KString KGridView::getRecordAt(int rowIndex, int columnIndex)
{
	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = {};
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(compHWND, LVM_GETITEMTEXTW, 
		(WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return KString(buffer, KStringBehaviour::FREE_ON_DESTROY);
}

int KGridView::getSelectedRow()
{
	return ListView_GetNextItem(compHWND, -1, LVNI_SELECTED);
}

void KGridView::removeRecordAt(int rowIndex)
{
	if (ListView_DeleteItem(compHWND, rowIndex))
		--itemCount;
}

void KGridView::removeAll()
{
	ListView_DeleteAllItems(compHWND);
	itemCount = 0;
}

void KGridView::updateRecordAt(int rowIndex, int columnIndex, const KString& text)
{
	LV_ITEMW lvi = {};
	lvi.iSubItem = columnIndex;
	lvi.pszText = (wchar_t*)(const wchar_t*)text;

	::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
		(WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void KGridView::setColumnWidth(int columnIndex, int columnWidth)
{
	ListView_SetColumnWidth(compHWND, columnIndex, columnWidth);
}

int KGridView::getColumnWidth(int columnIndex)
{
	return ListView_GetColumnWidth(compHWND, columnIndex);
}

void KGridView::createColumn(const KString& text, int columnWidth)
{
	LVCOLUMNW lvc = {};

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = columnWidth;
	lvc.pszText = (wchar_t*)(const wchar_t*)text;
	lvc.iSubItem = colCount;

	::SendMessageW(compHWND, LVM_INSERTCOLUMNW, 
		(WPARAM)colCount, (LPARAM)&lvc);

	++colCount;
}

bool KGridView::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				_onItemSelect();
				*result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)lParam)->code == NM_RCLICK) // List view item right click
		{
			_onItemRightClick();
			*result = 0;
			return true;
		}
		else if (((LPNMHDR)lParam)->code == NM_DBLCLK) // List view item double click
		{
			_onItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KGridView::create(bool requireInitialMessages)
{
	if (!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register WC_LISTVIEWW class!

	if (compHWND)
	{
		ListView_SetExtendedListViewStyle(compHWND, 
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KGridView::_onItemSelect()
{
	if (onItemSelect)
		onItemSelect(this);
}

void KGridView::_onItemRightClick()
{
	if (onItemRightClick)
		onItemRightClick(this);
}

void KGridView::_onItemDoubleClick()
{
	if (onItemRightClick)
		onItemRightClick(this);
}


// =========== KGroupBox.cpp ===========

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


KGroupBox::KGroupBox()
{
	compText.assignStaticText(TXT_WITH_LEN("GroupBox"));
	compWidth = 100;
	compHeight = 100;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_GROUPBOX;
	compDwExStyle = WS_EX_WINDOWEDGE;
}


KGroupBox::~KGroupBox()
{
}

// =========== KGUIProc.cpp ===========

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


class RFCInternalVariables
{
public:
	static KComponent* currentComponent;
	static HHOOK wnd_hook;
};

KComponent* RFCInternalVariables::currentComponent = nullptr;
HHOOK RFCInternalVariables::wnd_hook = 0;

ATOM KGUIProc::atomComponent;
ATOM KGUIProc::atomOldProc;

LRESULT CALLBACK RFCCTL_CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);

	if (nCode == HCBT_CREATEWND) {
		HWND hwnd = (HWND)wParam;
		LPCBT_CREATEWNDW cbtCreateWnd = (LPCBT_CREATEWNDW)lParam;
		if (cbtCreateWnd)
		{
			if (cbtCreateWnd->lpcs)
			{
				if (cbtCreateWnd->lpcs->lpCreateParams == RFCInternalVariables::currentComponent) // only catch what we created. ignore unknown windows.
				{
					KGUIProc::attachRFCPropertiesToHWND(hwnd, RFCInternalVariables::currentComponent);

					// Call the next hook, if there is one
					const LRESULT result = ::CallNextHookEx(RFCInternalVariables::wnd_hook, 
						nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(RFCInternalVariables::wnd_hook); // unhooking at here will allow child creation at WM_CREATE. otherwise this will hook child also!

					return result;
				}
			}
		}
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);
}

void KGUIProc::attachRFCPropertiesToHWND(HWND hwnd, KComponent* component)
{
	::SetPropW(hwnd, MAKEINTATOM(KGUIProc::atomComponent), (HANDLE)component);

	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetPropW(hwnd, MAKEINTATOM(KGUIProc::atomOldProc), (HANDLE)lpfnOldWndProc);

	if (lpfnOldWndProc != (void*)KGUIProc::windowProc) // sublcass only if window proc is not KGUIProc::windowProc (common control or dialog)
		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)KGUIProc::windowProc); // subclassing...
}

LRESULT CALLBACK KGUIProc::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	KComponent* component = (KComponent*)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::atomComponent));

	if (!component) // for safe!
		return ::DefWindowProcW(hwnd, msg, wParam, lParam);

	if (!component->getHWND()) // window recieve msg for the first time!
		component->setHWND(hwnd);

	if (msg == WM_NCDESTROY) {
		::RemovePropW(hwnd, MAKEINTATOM(KGUIProc::atomComponent));

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::atomOldProc));
		::RemovePropW(hwnd, MAKEINTATOM(KGUIProc::atomOldProc));

		if (lpfnOldWndProc)
		{
			if (lpfnOldWndProc != (void*)KGUIProc::windowProc) // common control or dialog
			{
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
				return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
			}
		}
		// lpfnOldWndProc is not set or window or custom control
		return ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	return component->windowProc(hwnd, msg, wParam, lParam);
}

INT_PTR CALLBACK KGUIProc::dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if (comp)
			comp->hotPlugInto(hwndDlg, true);

		return FALSE;
	}
	return FALSE;
}

HWND KGUIProc::createComponent(KComponent* component, bool requireInitialMessages)
{
	if (requireInitialMessages)
	{
		RFCInternalVariables::currentComponent = component;

		// install hook to get called before WM_NCCREATE msg! (we can receive WM_NCCREATE msg even for a standard control using this technique)
		RFCInternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		// pass current component as lpParam. so CBT proc can ignore other unknown windows.
		HWND hwnd = ::CreateWindowExW(component->getExStyle(), component->getComponentClassName(), component->getText(),
			component->getStyle(), component->getX(), component->getY(), component->getWidth(), component->getHeight(),
			component->getParentHWND(), (HMENU)(UINT_PTR)component->getControlID(), KApplication::hInstance, (LPVOID)component);

		// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc.
		//::UnhookWindowsHookEx(RFCInternalVariables::wnd_hook);

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->getExStyle(), component->getComponentClassName(), component->getText(),
			component->getStyle(), component->getX(), component->getY(), component->getWidth(), component->getHeight(),
			component->getParentHWND(), (HMENU)(UINT_PTR)component->getControlID(), KApplication::hInstance, 0);

		KGUIProc::attachRFCPropertiesToHWND(hwnd, component);
		component->setHWND(hwnd);

		return hwnd;
	}
}

int KGUIProc::hotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, KGUIProc::dialogProc, (LPARAM)component);
}

HWND KGUIProc::hotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, KGUIProc::dialogProc, (LPARAM)component);
}

// =========== KHostPanel.cpp ===========

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



KHostPanel::KHostPanel() : KComponent(true)
{
	compDwStyle = WS_CHILD | WS_CLIPCHILDREN;
	compDwExStyle = compDwExStyle | WS_EX_CONTROLPARENT; // non-top-level windows that contain child controls/dialogs must have the WS_EX_CONTROLPARENT style.
	// otherwise you will get infinite loop when calling IsDialogMessage.

	compText.assignStaticText(TXT_WITH_LEN("KHostPanel"));
	enableDPIUnawareMode = false;
	componentList = nullptr;
}

void KHostPanel::setComponentList(KPointerList<KComponent*, 24, false>* componentList)
{
	this->componentList = componentList;
}

void KHostPanel::setEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

bool KHostPanel::addComponent(KComponent* component, bool requireInitialMessages)
{
	if (component)
	{
		if ((compHWND != 0) && (componentList != nullptr))
		{
			component->setParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
				component->setDPI(compDPI);

			componentList->add(component);

			return component->create(requireInitialMessages);
		}
	}
	return false;
}

void KHostPanel::removeComponent(KComponent* component)
{
	if (componentList == nullptr)
		return;

	int index = componentList->getIndex(component);
	if (index != -1)
	{
		componentList->remove(index);
		component->destroy();
	}
}

bool KHostPanel::addContainer(KHostPanel* container, bool requireInitialMessages)
{
	if (container)
	{
		container->setComponentList(componentList);
		container->setEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->addComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

LRESULT KHostPanel::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
		{
			if (wParam != 0) // ignore menus
			{
				KComponent* component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, 
					MAKEINTATOM(KGUIProc::atomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
		{
			KComponent* component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, 
				MAKEINTATOM(KGUIProc::atomComponent));

			if (component)
			{
				LRESULT result = 0; // just for safe
				if (component->eventProc(msg, wParam, lParam, &result))
					return result;
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
		{
			KComponent* component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::atomComponent));
			if (component)
			{
				LRESULT result = 0; // just for safe
				if (component->eventProc(msg, wParam, lParam, &result))
					return result;
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
		{
			if (wParam != 0) // ignore menus
			{
				KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd, 
					((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::atomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
		{
			KComponent* component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, 
				MAKEINTATOM(KGUIProc::atomComponent));

			if (component)
			{
				LRESULT result = 0; // just for safe
				if (component->eventProc(msg, wParam, lParam, &result))
					return result;
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
		{
			KTimer* timer = KIDGenerator::getInstance()->getTimerByID((UINT)wParam);
			if (timer)
			{
				timer->_onTimer();
				break;
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
		{
			if ((HIWORD(wParam) == 0) && (lParam == 0)) // its menu item! unfortunately windows does not send menu handle with clicked event!
			{
				KMenuItem* menuItem = KIDGenerator::getInstance()->getMenuItemByID(LOWORD(wParam));
				if (menuItem)
				{
					menuItem->_onPress();
					break;
				}
			}
			else if (lParam)// send to appropriate component
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)lParam, 
					MAKEINTATOM(KGUIProc::atomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			else if (LOWORD(wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
			{
				HWND currentComponent = ::GetFocus();

				// simulate enter key pressed event into current component. (might be a window)
				::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
				::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
				::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0);

				return 0;
			}
			else if (LOWORD(wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
			{
				HWND currentComponent = ::GetFocus();

				// simulate esc key pressed event into current component. (might be a window)
				::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
				::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
				::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0);

				return 0;
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		default:
			return KComponent::windowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

KHostPanel::~KHostPanel() {}

// =========== KIcon.cpp ===========

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

#include <commctrl.h>

KIcon::KIcon()
{
	resourceID = 0;
	hIcon = 0;
}

void KIcon::setResource(WORD resourceID)
{
	this->resourceID = resourceID;
}

HICON KIcon::getScaledIcon(int size)
{
	HICON icon = 0;
	::LoadIconWithScaleDown(KApplication::hInstance,
		MAKEINTRESOURCEW(resourceID), size, size,
		&icon);

	return icon;
}

bool KIcon::loadFromResource(WORD resourceID)
{
	this->resourceID = resourceID;

	hIcon = (HICON)::LoadImageW(KApplication::hInstance, 
		MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, 
		LR_DEFAULTSIZE | LR_DEFAULTCOLOR);

	if(hIcon)
		return true;	
	return false;
}

bool KIcon::loadFromFile(const KString& filePath)
{
	hIcon = (HICON)::LoadImageW(KApplication::hInstance, 
		filePath, IMAGE_ICON, 0, 0, 
		LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);

	if(hIcon)
		return true;	
	return false;
}

HICON KIcon::getHandle()
{
	return hIcon;
}

KIcon::operator HICON()const
{
	return hIcon;
}

KIcon::~KIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}

// =========== KIDGenerator.cpp ===========

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


KIDGenerator::KIDGenerator()
{
	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
}

KIDGenerator* KIDGenerator::getInstance()
{
	static KIDGenerator defaultInstance;  // Created once, on first use (Meyer's Singleton)
	return &defaultInstance;
}

UINT KIDGenerator::generateControlID()
{
	++controlCount;
	return controlCount + rfc_InitialControlID;
}

UINT KIDGenerator::generateMenuItemID(KMenuItem* menuItem)
{
	++menuItemCount;
	menuItemList.add(menuItem);

	return menuItemCount + rfc_InitialMenuItemID;
}

KMenuItem* KIDGenerator::getMenuItemByID(UINT id)
{
	return menuItemList.get(id - (rfc_InitialMenuItemID + 1));
}

void KIDGenerator::generateClassName(KString& stringToModify)
{ 
	// can generate up to 9999 class names.
	wchar_t* classNameBuffer;
	int* pLength;
	stringToModify.accessRawSSOBuffer(&classNameBuffer, &pLength);

	int hInstance32 = ::HandleToLong(KApplication::hInstance);
	if (hInstance32 < 0)
		hInstance32 *= -1;

	::_itow_s(hInstance32, classNameBuffer, KString::SSO_BUFFER_SIZE, 16);  // using hex code reduces char usage.

	int pos = (int)::wcslen(classNameBuffer);
	if (pos < 11) {  // Safety check
		::_itow_s(classCount, &classNameBuffer[pos], KString::SSO_BUFFER_SIZE - pos, 10);
		classCount++;

		*pLength = (int)::wcslen(classNameBuffer);
	}
	else
	{
		K_ASSERT(false, "class count overflow in KIDGenerator::generateClassName");
	}
}

UINT KIDGenerator::generateTimerID(KTimer* timer)
{
	++timerCount;
	timerList.add(timer);

	return timerCount + rfc_InitialTimerID;
}

KTimer* KIDGenerator::getTimerByID(UINT id)
{
	return timerList.get(id - (rfc_InitialTimerID + 1));
}

KIDGenerator::~KIDGenerator() {}

// =========== KLabel.cpp ===========

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


KLabel::KLabel() : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("STATIC"));
	compText.assignStaticText(TXT_WITH_LEN("Label"));

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	autoResize = false;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KLabel::resizeToTextSize()
{
	if (compText.isNotEmpty())
	{
		RECT rect = KGraphics::calculateTextSize(compText, compFont->getFontHandle());
		this->setSize(rect.right + AUTOSIZE_EXTRA_GAP, rect.bottom);
	}
	else // text is empty
	{
		this->setSize(20, 25);
	}
}

void KLabel::enableAutoResize(bool enable)
{
	autoResize = enable;

	if(autoResize)
		this->resizeToTextSize();
}

void KLabel::setText(const KString& compText)
{
	KComponent::setText(compText);

	if (autoResize)
		this->resizeToTextSize();
}

void KLabel::setFont(KFont* compFont)
{
	KComponent::setFont(compFont);

	if (autoResize)
		this->resizeToTextSize();
}

void KLabel::setFont(KFont& compFont)
{
	KComponent::setFont(&compFont);

	if (autoResize)
		this->resizeToTextSize();
}

void KLabel::setDPI(int newDPI)
{
	if (newDPI == compDPI)
		return;

	int oldDPI = compDPI;
	compDPI = newDPI;

	this->compX = ::MulDiv(compX, newDPI, oldDPI);
	this->compY = ::MulDiv(compY, newDPI, oldDPI);

	if (!compFont->isDefaultFont())
		compFont->setDPI(newDPI);
	
	if (compText.isNotEmpty() && autoResize)
	{
		RECT rect = KGraphics::calculateTextSize(compText, compFont->getFontHandle());
		this->compWidth = rect.right + AUTOSIZE_EXTRA_GAP;
		this->compHeight = rect.bottom;
	}
	else
	{
		this->compWidth = ::MulDiv(compWidth, newDPI, oldDPI);
		this->compHeight = ::MulDiv(compHeight, newDPI, oldDPI);
	}

	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compX, compY, compWidth, 
			compHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);

		if ((!compFont->isDefaultFont()) && (compDwStyle & WS_CHILD))
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0));
	}
}

bool KLabel::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register Label class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KLabel::~KLabel()
{
}

// =========== KListBox.cpp ===========

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

#include <windowsx.h>

KListBox::KListBox(bool multipleSelection, bool sort, bool vscroll) : KComponent(false)
{
	this->multipleSelection = multipleSelection;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	compClassName.assignStaticText(TXT_WITH_LEN("LISTBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = LBS_NOTIFY | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;
	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	if(multipleSelection)
		compDwStyle = compDwStyle | LBS_MULTIPLESEL;
	if(sort)
		compDwStyle = compDwStyle | LBS_SORT;
	if(vscroll)
		compDwStyle = compDwStyle | WS_VSCROLL;
}

void KListBox::addItem(const KString& text)
{
	stringList.add(text);

	if(compHWND)
		::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)text);
}

void KListBox::removeItem(int index)
{
	if(stringList.remove(index))
	{
		if (compHWND)
			::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
	}
}

void KListBox::removeItem(const KString& text)
{
	const int itemIndex = getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

void KListBox::updateItem(int index, const KString& text)
{
	if (stringList.set(index, text))
	{
		if (compHWND)
		{
			::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
			::SendMessageW(compHWND, LB_INSERTSTRING, index, (LPARAM)(const wchar_t*)text);
		}
	}
}

int KListBox::getItemIndex(const KString& text)
{
	return stringList.getIndex(text);
}

int KListBox::getItemCount()
{
	return stringList.size();
}

int KListBox::getSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

KString KListBox::getSelectedItem()
{
	const int itemIndex = getSelectedItemIndex();
	if(itemIndex > -1)
		return stringList.get(itemIndex);

	return KString();
}

int KListBox::getSelectedItems(int* itemArray, int itemCountInArray)
{
	if(compHWND)
	{	 
		const int items = (int)::SendMessageW(compHWND, 
			LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);

		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void KListBox::clearList()
{
	stringList.removeAll();

	if(compHWND)
		::SendMessageW(compHWND, LB_RESETCONTENT, 0, 0);
}

void KListBox::selectItem(int index)
{
	selectedItemIndex = index;

	if(compHWND)
		::SendMessageW(compHWND, LB_SETCURSEL, index, 0);
}

void KListBox::selectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(compHWND)
			::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool KListBox::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (msg == WM_COMMAND) 
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) // listbox sel change!
		{
			_onItemSelect();
			*result = 0;
			return true;
		}
		else if (HIWORD(wParam) == LBN_DBLCLK) // listbox double click
		{
			_onItemDoubleClick();
			*result = 0;
			return true;
		}
	}
	else if (msg == WM_CONTEXTMENU)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		// Convert screen coordinates to client coordinates for the ListBox
		::ScreenToClient(compHWND, &pt);

		// Determine which item is at the clicked position
		DWORD index = (DWORD)::SendMessageW(compHWND, LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));

		// HIWORD is 0 if the click is on a valid item
		if (HIWORD(index) == 0)
		{
			DWORD itemIndex = LOWORD(index);
			::SendMessageW(compHWND, LB_SETCURSEL, itemIndex, 0); // select it

			_onItemRightClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KListBox::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register LISTBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList.size();
		if(listSize)
		{
			for (int i = 0; i < listSize; i++)
			{
				::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)stringList.get(i));
			}
		}

		if(!multipleSelection) // single selction!
		{
			if(selectedItemIndex > -1)
				::SendMessageW(compHWND, LB_SETCURSEL, selectedItemIndex, 0);
		}else
		{
			if(selectedItemIndex>-1)
				::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(selectedItemIndex, selectedItemEnd));
		}

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KListBox::_onItemSelect()
{
	if(onItemSelect)
		onItemSelect(this);
}

void KListBox::_onItemDoubleClick()
{
	if(onItemDoubleClick)
		onItemDoubleClick(this);
}

void KListBox::_onItemRightClick()
{
	if (onItemRightClick)
		onItemRightClick(this);
}

KListBox::~KListBox() {}

// =========== KMenu.cpp ===========

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


KMenu::KMenu()
{
	hMenu = ::CreatePopupMenu();
}

void KMenu::addMenuItem(KMenuItem* menuItem)
{
	menuItem->addToMenu(hMenu);
}

void KMenu::addSubMenu(const KString& text, KMenu* menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, 
		(UINT_PTR)menu->getMenuHandle(), text);
}

void KMenu::addSeperator()
{
	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_SEPARATOR;

	::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
}

HMENU KMenu::getMenuHandle()
{
	return hMenu;
}

void KMenu::popUpMenu(HWND window, bool bringWindowToForeground)
{
	if (bringWindowToForeground)
		::SetForegroundWindow(window);

	POINT p;
	::GetCursorPos(&p);
	::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window, NULL);
}

KMenu::~KMenu()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuBar.cpp ===========

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


KMenuBar::KMenuBar()
{
	hMenu = ::CreateMenu();
}

void KMenuBar::addMenu(const KString& text, KMenu* menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, 
		(UINT_PTR)menu->getMenuHandle(), text);
}

void KMenuBar::addToWindow(KWindow* window)
{
	HWND hwnd = window->getHWND();
	if(hwnd)
		::SetMenu(hwnd, hMenu);
}

KMenuBar::~KMenuBar()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuButton.cpp ===========

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

#include <commctrl.h>

KMenuButton::KMenuButton() : arrowFont(L"Webdings", 18, false, false, false, false, USER_DEFAULT_SCREEN_DPI)
{
	buttonMenu = nullptr;
	glyphFont = nullptr;
	glyphChar = nullptr;
	glyphLeft = 6;
}	

KMenuButton::~KMenuButton() {}

void KMenuButton::setMenu(KMenu* buttonMenu)
{
	this->buttonMenu = buttonMenu;
}

void KMenuButton::setGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->repaint();
}

void KMenuButton::setDPI(int newDPI)
{
	if (glyphFont)
		glyphFont->setDPI(newDPI);

	arrowFont.setDPI(newDPI);

	KButton::setDPI(newDPI);
}

void KMenuButton::_onPress()
{
	if (buttonMenu)
	{
		POINT point = {compX, compY};
		::ClientToScreen(compParentHWND, &point); // get screen cordinates

		::TrackPopupMenu(buttonMenu->getMenuHandle(), 
			TPM_LEFTBUTTON, point.x, point.y + compHeight, 
			0, compParentHWND, NULL);
	}
}

bool KMenuButton::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW) lParam;

			*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				*result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				const RECT rc = lpNMCD->rc;
				KGraphics::draw3dVLine(lpNMCD->hdc, rc.right - ::MulDiv(22, compDPI, USER_DEFAULT_SCREEN_DPI),
					rc.top + ::MulDiv(6, compDPI, USER_DEFAULT_SCREEN_DPI), 
					rc.bottom - ::MulDiv(12, compDPI, USER_DEFAULT_SCREEN_DPI)); // draw line

				const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFont.getFontHandle());
				const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - ::MulDiv(18, compDPI, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphFont) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFont->getFontHandle());
					::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + ::MulDiv(glyphLeft, compDPI, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				*result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}

	return KButton::eventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KMenuItem.cpp ===========

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


KMenuItem::KMenuItem()
{
	hMenu = 0;
	param = nullptr;
	intParam = -1;
	enabled = true;
	checked = false;
	itemID = KIDGenerator::getInstance()->generateMenuItemID(this);
}

void KMenuItem::addToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
	mii.cch = itemText.length();
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);

}

void KMenuItem::setParam(void* param)
{
	this->param = param;
}

void KMenuItem::setIntParam(int intParam)
{
	this->intParam = intParam;
}

int KMenuItem::getIntParam()
{
	return intParam;
}

void* KMenuItem::getParam()
{
	return param;
}

bool KMenuItem::isChecked()
{
	return checked;
}

void KMenuItem::setCheckedState(bool state)
{
	checked = state;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool KMenuItem::isEnabled()
{
	return enabled; 
}

void KMenuItem::setEnabled(bool state)
{
	enabled = state;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void KMenuItem::setText(const KString& text)
{
	itemText = text;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

KString KMenuItem::getText()
{
	return itemText;
}

UINT KMenuItem::getItemID()
{
	return itemID;
}

HMENU KMenuItem::getMenuHandle()
{
	return hMenu;
}

void KMenuItem::_onPress()
{
	if(onPress)
		onPress(this);
}

KMenuItem::~KMenuItem()
{
}


// =========== KNumericField.cpp ===========

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


KNumericField::KNumericField() : KTextBox(false)
{
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL | ES_NUMBER;
}

KNumericField::~KNumericField(){}

// =========== KPasswordBox.cpp ===========

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


KPasswordBox::KPasswordBox(bool readOnly):KTextBox(readOnly)
{
	pwdChar = L'*';
	compDwStyle = compDwStyle | ES_PASSWORD;
}

void KPasswordBox::setPasswordChar(const wchar_t pwdChar)
{
	this->pwdChar = pwdChar;
	if(compHWND)
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		this->repaint();
	}
}

wchar_t KPasswordBox::getPasswordChar()
{
	return pwdChar;
}

bool KPasswordBox::create(bool requireInitialMessages)
{
	if(KTextBox::create(requireInitialMessages))
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		return true;
	}
	return false;
}

KPasswordBox::~KPasswordBox()
{
}

// =========== KProgressBar.cpp ===========

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

#include <commctrl.h>

KProgressBar::KProgressBar(bool smooth, bool vertical) : KComponent(false)
{
	value = 0;

	compClassName.assignStaticText(TXT_WITH_LEN("msctls_progress32"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS;
	compDwExStyle = WS_EX_WINDOWEDGE;

	if(smooth)
		compDwStyle = compDwStyle | PBS_SMOOTH;

	if(vertical)
		compDwStyle = compDwStyle | PBS_VERTICAL;
}

int KProgressBar::getValue()
{
	return value;
}

void KProgressBar::setValue(int value)
{
	this->value=value;

	if(compHWND)
		::SendMessageW(compHWND, PBM_SETPOS, value, 0);
}

bool KProgressBar::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register PROGRESS_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(compHWND, PBM_SETPOS, value, 0); // set current value!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;	
}

KProgressBar::~KProgressBar()
{
}

// =========== KPushButton.cpp ===========

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


KPushButton::KPushButton()
{
	compText.assignStaticText(TXT_WITH_LEN("Push Button"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | 
		BS_PUSHLIKE | BS_NOTIFY | WS_TABSTOP;
}

KPushButton::~KPushButton()
{
}

// =========== KRadioButton.cpp ===========

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


KRadioButton::KRadioButton()
{
	compText.assignStaticText(TXT_WITH_LEN("RadioButton"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | 
		BS_RADIOBUTTON | BS_NOTIFY | WS_TABSTOP;
}

KRadioButton::~KRadioButton()
{
}

// =========== KTextArea.cpp ===========

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


KTextArea::KTextArea(bool autoScroll, bool readOnly):KTextBox(readOnly)
{
	compWidth = 200;
	compHeight = 100;

	compDwStyle = compDwStyle | ES_MULTILINE | ES_WANTRETURN;

	if(autoScroll)
		compDwStyle = compDwStyle | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
	else
		compDwStyle = compDwStyle | WS_HSCROLL | WS_VSCROLL;
}

LRESULT KTextArea::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS; // to catch TAB key
	return KTextBox::windowProc(hwnd, msg, wParam, lParam);
}

KTextArea::~KTextArea()
{
}

// =========== KTextBox.cpp ===========

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


KTextBox::KTextBox(bool readOnly) : KComponent(false)
{
	compClassName.assignStaticText(TXT_WITH_LEN("EDIT"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL;

	if(readOnly)
		compDwStyle = compDwStyle | ES_READONLY;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

KString KTextBox::getText()
{
	if(compHWND)
	{
		const int length = ::GetWindowTextLengthW(compHWND);
		if(length)
		{
			const int size = (length + 1) * sizeof(wchar_t);
			wchar_t *text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(compHWND, text, size);
			compText = KString(text, KStringBehaviour::FREE_ON_DESTROY);
		}else
		{
			compText = KString();
		}
	}
	return compText;
}


bool KTextBox::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register EDIT class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KTextBox::~KTextBox()
{
}

// =========== KTimer.cpp ===========

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


KTimer::KTimer()
{
	resolution = 1000;
	started = false;
	timerID = KIDGenerator::getInstance()->generateTimerID(this);
}

void KTimer::setInterval(int resolution)
{
	this->resolution = resolution;
}

int KTimer::getInterval()
{
	return resolution;
}

void KTimer::setTimerWindow(KWindow* window)
{
	this->window = window;
}

void KTimer::setTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::getTimerID()
{
	return timerID;
}

void KTimer::startTimer()
{
	if(started)
		return;

	if(window)
	{
		HWND hwnd = window->getHWND();
		if(hwnd)
		{
			::SetTimer(hwnd, timerID, resolution, 0);
			started = true;
		}
	}
}

void KTimer::stopTimer()
{
	if(window)
	{
		HWND hwnd = window->getHWND();
		if(hwnd)
		{
			if(started)
				::KillTimer(hwnd, timerID);

			started = false;
		}
	}
}

bool KTimer::isTimerRunning()
{
	return started;
}

void KTimer::_onTimer()
{
	if(onTimer)
		onTimer(this);
}

KTimer::~KTimer()
{
	if(started)
		this->stopTimer();
}

// =========== KToolTip.cpp ===========

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

#include <commctrl.h>

KToolTip::KToolTip() : KComponent(false)
{
	attachedCompHWND = 0;
	compClassName.assignStaticText(TXT_WITH_LEN("tooltips_class32"));

	compDwStyle = WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX;
}

KToolTip::~KToolTip()
{
}

void KToolTip::attachToComponent(KWindow* parentWindow, KComponent* attachedComponent)
{
	compParentHWND = parentWindow->getHWND();
	attachedCompHWND = attachedComponent->getHWND();

	compHWND = ::CreateWindowExW(0, compClassName, NULL, 
		compDwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, compParentHWND, NULL, KApplication::hInstance, 0);

	if (compHWND)
	{
		::SetWindowPos(compHWND, HWND_TOPMOST, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		KGUIProc::attachRFCPropertiesToHWND(compHWND, (KComponent*)this);

		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = (wchar_t*)(const wchar_t*)compText;

		SendMessageW(compHWND, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}
}

bool KToolTip::create(bool requireInitialMessages)
{
	return false;
}

void KToolTip::setText(const KString& compText)
{
	this->compText = compText;
	if (compHWND)
	{
		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = (wchar_t*)(const wchar_t*)compText;
		toolInfo.hinst = KApplication::hInstance;

		SendMessageW(compHWND, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}


// =========== KTrackBar.cpp ===========

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


#include <commctrl.h>

KTrackBar::KTrackBar(bool showTicks, bool vertical) : KComponent(false)
{
	rangeMin = 0;
	rangeMax = 100;
	value = 0;

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	compDwStyle = (WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS) | 
		(showTicks ? TBS_AUTOTICKS : TBS_NOTICKS) | 
		(vertical ? TBS_VERT : TBS_HORZ);

	compDwExStyle = WS_EX_WINDOWEDGE;

	compClassName.assignStaticText(TXT_WITH_LEN("msctls_trackbar32"));
}

void KTrackBar::setRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));	
}

void KTrackBar::setValue(int value)
{
	this->value = value;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);
}

void KTrackBar::_onChange()
{
	value = (int)::SendMessageW(compHWND, TBM_GETPOS, 0, 0);
	if(onChange)
		onChange(this, value);
}

int KTrackBar::getValue()
{
	return value;
}

bool KTrackBar::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if( (msg == WM_HSCROLL) || (msg == WM_VSCROLL) )
	{
		const int nScrollCode = (int)LOWORD(wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || 
			(TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || 
			(TB_PAGEDOWN == nScrollCode) || (TB_THUMBPOSITION == nScrollCode)) // its trackbar!
		{
			this->_onChange();
			*result = 0;
			return true;
		}
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KTrackBar::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register TRACKBAR_CLASSW class!

	if(compHWND)
	{
		::EnableWindow(compHWND, compEnabled);
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!	
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(rangeMin, rangeMax));	
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

KTrackBar::~KTrackBar()
{
}


// =========== KTransparentBitmap.cpp ===========

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


KTransparentBitmap::KTransparentBitmap(void* data, int width, int height, int stride)
{
	if ((width * sizeof(unsigned int)) != stride) // this should not happen! no padding in 32bpp data.
	{
		hdcMem = 0;
		width = 0;
		height = 0;
		hbm = 0;
		hbmPrev = 0;
		pvBits = nullptr;
		return;
	}

	this->width = width;
	this->height = height;

	hdcMem = ::CreateCompatibleDC(NULL);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = ::CreateDIBSection(hdcMem, &bmi,
		DIB_RGB_COLORS, &pvBits,
		NULL, 0);

	::memcpy(pvBits, data, height * width * sizeof(unsigned int));

	// the color format for each pixel is 0xaarrggbb  
	/*
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			((unsigned int*)pvBits)[x + y * width] = ((unsigned int*)data)[x + y * width];
	*/

	hbmPrev = (HBITMAP)::SelectObject(hdcMem, hbm);

}

KTransparentBitmap::KTransparentBitmap(int width, int height)
{
	this->createEmptyBitmap(width, height);
}

void KTransparentBitmap::createEmptyBitmap(int width, int height)
{
	this->width = width;
	this->height = height;

	hdcMem = ::CreateCompatibleDC(NULL);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = ::CreateDIBSection(hdcMem, &bmi,
		DIB_RGB_COLORS, &pvBits,
		NULL, 0);

	::ZeroMemory(pvBits, height * width * sizeof(unsigned int));

	hbmPrev = (HBITMAP)::SelectObject(hdcMem, hbm);
}

void KTransparentBitmap::releaseResources()
{
	if (hdcMem == 0)
		return;

	::SelectObject(hdcMem, hbmPrev);
	::DeleteObject(hbm);
	::DeleteDC(hdcMem);
}

bool KTransparentBitmap::hitTest(int x, int y)
{
	if (pvBits == nullptr)
		return false;

	if ((x < 0) || (y < 0) || (x > (width - 1)) || (y > (height - 1)))
		return false;

	unsigned int pixelColor = ((unsigned int*)pvBits)[x + y * width];
	return ((pixelColor >> 24) == 0xff);
}

unsigned int KTransparentBitmap::getPixel(int x, int y)
{
	if (pvBits == nullptr)
		return 0;

	if ((x < 0) || (y < 0) || (x > (width - 1)) || (y > (height - 1)))
		return 0;

	return ((unsigned int*)pvBits)[x + y * width];
}

int KTransparentBitmap::getWidth()
{
	return width;
}

int KTransparentBitmap::getHeight()
{
	return height;
}

void KTransparentBitmap::resize(int width, int height)
{
	this->releaseResources();
	this->createEmptyBitmap(width, height);
}

HDC KTransparentBitmap::getDC()
{
	return hdcMem;
}

void KTransparentBitmap::draw(HDC destHdc, int destX, int destY, BYTE alpha)
{
	this->draw(destHdc, destX, destY, width, height, 0, 0, width, height, alpha);
}

void KTransparentBitmap::draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha)
{
	this->draw(destHdc, destX, destY, destWidth, destHeight, 0, 0, width, height, alpha);
}

void KTransparentBitmap::draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha)
{
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
	::AlphaBlend(destHdc, destX, destY, destWidth, destHeight,
		hdcMem, srcX, srcY, srcWidth, srcHeight, bf);
}

KTransparentBitmap::~KTransparentBitmap()
{
	this->releaseResources();
}



// =========== KWindow.cpp ===========

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


KWindow::KWindow() : KComponent(true)
{
	compText.assignStaticText(TXT_WITH_LEN("KWindow"));

	compWidth = 400;
	compHeight = 200;

	compVisible = false;
	enableDPIUnawareMode = false;
	prevDPIContext = 0;
	dpiAwarenessContextChanged = false;
	compDwStyle = WS_POPUP;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
	dpiChangeListener = nullptr;
	windowIcon = nullptr;
	largeIconHandle = 0;
	smallIconHandle = 0;

	closeOperation = KCloseOperation::DestroyAndExit;
}

void KWindow::applyDPIUnawareModeToThread()
{
	if ((KApplication::dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY) && KApplication::dpiAwareAPICalled && enableDPIUnawareMode)
	{
		if (KDPIUtility::pSetThreadDpiAwarenessContext)
		{
			prevDPIContext = KDPIUtility::pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
			dpiAwarenessContextChanged = true;
		}
	}
}

void KWindow::restoreDPIModeOfThread()
{
	if (dpiAwarenessContextChanged)
		KDPIUtility::pSetThreadDpiAwarenessContext(prevDPIContext);
}

bool KWindow::create(bool requireInitialMessages)
{
	if (enableDPIUnawareMode)
		this->applyDPIUnawareModeToThread();

	bool retVal = KComponent::create(requireInitialMessages);

	if (enableDPIUnawareMode)
		this->restoreDPIModeOfThread();

	if(retVal && (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		int dpi = KDPIUtility::getWindowDPI(compHWND);
		if (dpi != USER_DEFAULT_SCREEN_DPI)
			this->setDPI(dpi);
	}

	return retVal;
}

void KWindow::flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::updateWindowIconForNewDPI()
{
	if (windowIcon == nullptr)
		return;

	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);

	// 32x32 for large icon
	// 16x16 for small icon

	if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		largeIconHandle = windowIcon->getScaledIcon(KDPIUtility::scaleToNewDPI(32, compDPI));
		smallIconHandle = windowIcon->getScaledIcon(KDPIUtility::scaleToNewDPI(16, compDPI));
	}
	else
	{
		largeIconHandle = windowIcon->getScaledIcon(32);
		smallIconHandle = windowIcon->getScaledIcon(16);
	}

	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)largeIconHandle);
	::SetClassLongPtrW(compHWND, GCLP_HICONSM, (LONG_PTR)smallIconHandle);
}

void KWindow::setIcon(KIcon* icon)
{
	windowIcon = icon;

	if (compHWND)
		this->updateWindowIconForNewDPI();
}

void KWindow::setCloseOperation(KCloseOperation closeOperation)
{
	this->closeOperation = closeOperation;
}

void KWindow::setDPIChangeListener(KDPIChangeListener* dpiChangeListener)
{
	this->dpiChangeListener = dpiChangeListener;
}

void KWindow::setEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

void KWindow::onClose()
{
	if (closeOperation == KCloseOperation::DestroyAndExit)
		this->destroy();
	else if (closeOperation == KCloseOperation::Hide)
		this->setVisible(false);
}

void KWindow::onDestroy()
{
	if (closeOperation == KCloseOperation::DestroyAndExit)
		::PostQuitMessage(0);
}

void KWindow::postCustomMessage(WPARAM msgID, LPARAM param)
{
	::PostMessageW(compHWND, RFC_CUSTOM_MESSAGE, msgID, param);
}

void KWindow::onCustomMessage(WPARAM msgID, LPARAM param)
{

}

void KWindow::centerScreen()
{
	this->setPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

void KWindow::centerOnSameMonitor(HWND window)
{
	if (window)
	{
		HMONITOR hmon = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

		if (hmon != NULL)
		{
			MONITORINFO monitorInfo;
			::ZeroMemory(&monitorInfo, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if (::GetMonitorInfoW(hmon, &monitorInfo))
			{
				const int posX = monitorInfo.rcMonitor.left + (((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) - compWidth) / 2);
				const int posY = monitorInfo.rcMonitor.top + (((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) - compHeight) / 2);
				this->setPosition(posX, posY);

				return;
			}
		}
	}

	this->centerScreen();
}

void KWindow::getNormalSize(int* width, int* height)
{
	if (compHWND)
	{
		WINDOWPLACEMENT wndPlacement{ 0 };
		wndPlacement.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement(compHWND, &wndPlacement);

		*width = wndPlacement.rcNormalPosition.right - wndPlacement.rcNormalPosition.left;
		*height = wndPlacement.rcNormalPosition.bottom - wndPlacement.rcNormalPosition.top;
	}
	else
	{
		*width = compWidth;
		*height = compHeight;
	}
}

bool KWindow::addComponent(KComponent* component, bool requireInitialMessages)
{
	if(component)
	{
		if(compHWND)
		{		
			component->setParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled )
				component->setDPI(compDPI);

			componentList.add(component);

			if (enableDPIUnawareMode)
				this->applyDPIUnawareModeToThread();

			bool retVal = component->create(requireInitialMessages);

			if (enableDPIUnawareMode)
				this->restoreDPIModeOfThread();

			return retVal;
		}
	}
	return false;
}

bool KWindow::addComponent(KComponent& component, bool requireInitialMessages)
{
	return addComponent(&component, requireInitialMessages);
}

void KWindow::removeComponent(KComponent* component)
{
	int index = componentList.getIndex(component);
	if (index != -1)
	{
		componentList.remove(index);
		component->destroy();
	}
}

bool KWindow::addContainer(KHostPanel* container, bool requireInitialMessages)
{
	if (container)
	{
		container->setComponentList(&componentList);
		container->setEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->addComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

bool KWindow::setClientAreaSize(int width, int height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		wndRect.right = wndRect.left + width;
		wndRect.bottom = wndRect.top + height;

		KDPIUtility::adjustWindowRectExForDpi(&wndRect, compDwStyle,
			::GetMenu(compHWND) == NULL ? FALSE : TRUE, compDwExStyle, compDPI);

		this->setSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

		return true;
	}
	return false;
}

bool KWindow::isOffScreen(int posX, int posY)
{
	POINT point;
	point.x = posX;
	point.y = posY;
	return (::MonitorFromPoint(point, MONITOR_DEFAULTTONULL) == NULL);
}

bool KWindow::getClientAreaSize(int* width, int* height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		if (width)
			*width = wndRect.right - wndRect.left;

		if (height)
			*height = wndRect.bottom - wndRect.top;

		return true;
	}
	return false;
}

void KWindow::onMoved()
{

}

void KWindow::onResized()
{

}

LRESULT KWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::atomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->eventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				KComponent* component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, MAKEINTATOM(KGUIProc::atomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::atomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}				
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::atomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->eventProc(msg, wParam, lParam, &result))
							return result;
					}
				}			
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				KComponent* component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::atomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
			{
				KTimer* timer = KIDGenerator::getInstance()->getTimerByID((UINT)wParam);
				if(timer)
				{
					timer->_onTimer();
					break;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compWidth = rect.right - rect.left;
				this->compHeight = rect.bottom - rect.top;

				this->onResized();
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->onMoved();
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_DPICHANGED:
			{
				if ((KApplication::dpiAwareness == KDPIAwareness::UNAWARE_MODE) || enableDPIUnawareMode || (!KApplication::dpiAwareAPICalled))
					return KComponent::windowProc(hwnd, msg, wParam, lParam);

				this->compDPI = HIWORD(wParam);
				RECT* const prcNewWindow = (RECT*)lParam;

				this->compX = prcNewWindow->left;
				this->compY = prcNewWindow->top;
				this->compWidth = prcNewWindow->right - prcNewWindow->left;
				this->compHeight = prcNewWindow->bottom - prcNewWindow->top;

				::SetWindowPos(compHWND,
					NULL,
					this->compX,
					this->compY,
					this->compWidth,
					this->compHeight,
					SWP_NOZORDER | SWP_NOACTIVATE);

				this->updateWindowIconForNewDPI();

				::InvalidateRect(compHWND, NULL, TRUE);

				for (int i = 0; i < componentList.size(); i++)
				{
					componentList[i]->setDPI(compDPI);
				}

				if (dpiChangeListener)
					dpiChangeListener->onDPIChange(compHWND, compDPI);

				return 0;
			}

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == 0) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					KMenuItem* menuItem = KIDGenerator::getInstance()->getMenuItemByID(LOWORD(wParam));
					if(menuItem)
					{
						menuItem->_onPress();
						break;
					}
				}
				else if(lParam)// send to appropriate component
				{
					KComponent* component = (KComponent*)::GetPropW((HWND)lParam, 
						MAKEINTATOM(KGUIProc::atomComponent));

					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->eventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
				else if (LOWORD(wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate enter key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0); 

					return 0;
				}
				else if (LOWORD(wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate esc key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0); 

					return 0;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_CONTEXTMENU:
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)wParam, MAKEINTATOM(KGUIProc::atomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_ACTIVATE: // save last focused item when inactive
			if (wParam != WA_INACTIVE)
				return KComponent::windowProc(hwnd, msg, wParam, lParam);
			this->lastFocusedChild = ::GetFocus();
			break;

		case WM_SETFOCUS:
			if (this->lastFocusedChild) // set focus to last item
			{
				::SetFocus(this->lastFocusedChild);
			}
			else // set focus to first child
			{
				// if hCtl is NULL, GetNextDlgTabItem returns first control of the window.
				HWND hFirstControl = ::GetNextDlgTabItem(this->compHWND, NULL, FALSE);
				if (hFirstControl)
				{
					if (::GetWindowLongPtrW(hFirstControl, GWL_STYLE) & WS_TABSTOP)
						::SetFocus(hFirstControl);
				}
			}
			break;

		case WM_CLOSE:
			this->onClose();
			break;

		case WM_DESTROY:
			// os automatically destroy child controls after WM_DESTROY.
			this->onDestroy();
			break;

		case RFC_CUSTOM_MESSAGE:
			this->onCustomMessage(wParam, lParam);
			break;

		default:
			return KComponent::windowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow()
{
	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);
}

// =========== KWindowTypes.cpp ===========

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



KHotPluggedDialog::KHotPluggedDialog(){}


void KHotPluggedDialog::onClose()
{
	::EndDialog(compHWND, 0);
}

void KHotPluggedDialog::onDestroy(){}

KHotPluggedDialog::~KHotPluggedDialog(){}


KOverlappedWindow::KOverlappedWindow()
{
	compText.assignStaticText(TXT_WITH_LEN("KOverlapped Window"));
	compDwStyle = WS_OVERLAPPEDWINDOW;
}

KOverlappedWindow::~KOverlappedWindow(){}


KFrame::KFrame()
{
	compText.assignStaticText(TXT_WITH_LEN("KFrame"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
}

KFrame::~KFrame(){}



KDialog::KDialog()
{
	compText.assignStaticText(TXT_WITH_LEN("KDialog"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	compDwExStyle = WS_EX_DLGMODALFRAME;
}

KDialog::~KDialog(){}



KToolWindow::KToolWindow()
{
	compText.assignStaticText(TXT_WITH_LEN("KTool Window"));
	compDwStyle = WS_OVERLAPPED | WS_SYSMENU;
	compDwExStyle = WS_EX_TOOLWINDOW;
}

KToolWindow::~KToolWindow(){}


// =========== KPerformanceCounter.cpp ===========

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


KPerformanceCounter::KPerformanceCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);

	this->pcFreq = double(li.QuadPart) / 1000.0;
}

void KPerformanceCounter::startCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	this->counterStart = li.QuadPart;
}

double KPerformanceCounter::endCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - this->counterStart) / this->pcFreq;
}

KPerformanceCounter::~KPerformanceCounter()
{

}


// =========== KRegistry.cpp ===========

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


KRegistry::KRegistry()
{

}

bool KRegistry::createKey(HKEY hKeyRoot, const KString& subKey)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	::RegCloseKey(hkey);
	return true;
}

bool KRegistry::deleteKey(HKEY hKeyRoot, const KString& subKey)
{
	return ::RegDeleteKeyW(hKeyRoot, subKey) == ERROR_SUCCESS ? true : false;
}

bool KRegistry::readString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			if (requiredBytes == 0) // value might be empty
			{
				*result = KString();
			}
			else{
				void* buffer = ::malloc(requiredBytes + sizeof(wchar_t)); // + for strings which doesn't have ending null
				::ZeroMemory(buffer, requiredBytes + sizeof(wchar_t)); // zero the ending null

				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)buffer, &requiredBytes);
				*result = KString((wchar_t*)buffer, KStringBehaviour::FREE_ON_DESTROY);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::writeString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const int bCount = (value.length() + 1) * sizeof(wchar_t); // +1 for ending null
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_SZ, (LPBYTE)(const wchar_t*)value, bCount);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::readDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		const LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, &dwType, (LPBYTE)result, &dwSize);
		::RegCloseKey(hkey);

		if (ret == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool KRegistry::writeDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const DWORD dwSize = sizeof(DWORD);
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_DWORD, (LPBYTE)&value, dwSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::readBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void** buffer, DWORD* buffSize)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			*buffSize = requiredBytes;
			if (requiredBytes == 0) // value might be empty
			{
				*buffer = 0;
			}
			else{
				*buffer = ::malloc(requiredBytes);
				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)*buffer, &requiredBytes);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::writeBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void* buffer, DWORD buffSize)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_BINARY, (LPBYTE)buffer, buffSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

KRegistry::~KRegistry()
{

}