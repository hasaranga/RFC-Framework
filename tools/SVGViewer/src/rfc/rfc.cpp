
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

void KComponent::onParentDestroy() {}

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

int KComponent::scaleToCurrentDPI(int valueFor96DPI)
{
	return MulDiv(valueFor96DPI, compDPI, USER_DEFAULT_SCREEN_DPI);
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

void KComponent::placeToRightOf(KComponent& target, int spacing)
{
	setPosition(target.getX() + target.getWidth() + spacing, target.getY());
}

void KComponent::placeToLeftOf(KComponent& target, int spacing)
{
	setPosition(target.getX() - getWidth() - spacing, target.getY());
}

void KComponent::placeBelow(KComponent& target, int spacing)
{
	setPosition(target.getX(), target.getY() + target.getHeight() + spacing);
}

void KComponent::placeAbove(KComponent& target, int spacing)
{
	setPosition(target.getX(), target.getY() - getHeight() - spacing);
}

void KComponent::alignTopWith(KComponent& target)
{
	setPosition(getX(), target.getY());
}

void KComponent::alignBottomWith(KComponent& target)
{
	setPosition(getX(), target.getY() + target.getHeight() - getHeight());
}

void KComponent::alignLeftWith(KComponent& target)
{
	setPosition(target.getX(), getY());
}

void KComponent::alignRightWith(KComponent& target)
{
	setPosition(target.getX() + target.getWidth() - getWidth(), getY());
}

void KComponent::alignCenterHorizontallyWith(KComponent& target)
{
	int x = target.getX() + (target.getWidth() - getWidth()) / 2;
	setPosition(x, getY());
}

void KComponent::alignCenterVerticallyWith(KComponent& target)
{
	int y = target.getY() + (target.getHeight() - getHeight()) / 2;
	setPosition(getX(), y);
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

	this->repaint(); // to fix bug when text contain one space character and os repaint only resized area.
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
	hwndWindow = NULL;
}

int KTimer::getInterval()
{
	return resolution;
}

void KTimer::setTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::getTimerID()
{
	return timerID;
}

void KTimer::start(int resolution, KWindow& window)
{
	if(started)
		return;

	this->resolution = resolution;

	hwndWindow = window.getHWND();
	K_ASSERT(hwndWindow != NULL, "KTimer start called without creating the window!");

	if(hwndWindow)
	{
		::SetTimer(hwndWindow, timerID, resolution, 0);
		started = true;
	}	
}

void KTimer::start(int resolution, KWindow& window, std::function<void(KTimer*)> onTimerCallback)
{
	onTimer = std::move(onTimerCallback);
	start(resolution, window);
}

void KTimer::stop()
{
	if (hwndWindow)
	{
		if(started)
			::KillTimer(hwndWindow, timerID);

		started = false;
	}
}

bool KTimer::isRunning()
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
		this->stop();
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
		this->createEmptyBitmap(width, height);
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
	compDwStyle = WS_POPUP | WS_CLIPCHILDREN;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
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

void KWindow::setEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

void KWindow::onClose()
{
	if (closeOperation == KCloseOperation::DestroyAndExit)
	{
		this->setVisible(false);
		this->destroy();
	}
	else if (closeOperation == KCloseOperation::Hide)
	{
		this->setVisible(false);
	}
}

void KWindow::onDestroy()
{
	for (int i = 0; i < componentList.size(); i++)
	{
		componentList[i]->onParentDestroy();
	}

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

				if (onDPIChange)
					onDPIChange(this);

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
	compDwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
}

KOverlappedWindow::~KOverlappedWindow(){}


KFrame::KFrame()
{
	compText.assignStaticText(TXT_WITH_LEN("KFrame"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
}

KFrame::~KFrame(){}



KDialog::KDialog()
{
	compText.assignStaticText(TXT_WITH_LEN("KDialog"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN;
	compDwExStyle = WS_EX_DLGMODALFRAME;
}

KDialog::~KDialog(){}



KToolWindow::KToolWindow()
{
	compText.assignStaticText(TXT_WITH_LEN("KTool Window"));
	compDwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN;
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

// =========== plutosvg.cpp ===========


/*
 * Copyright (c) 2020-2025 Samuel Ugochukwu <sammycageagle@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/


#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <intrin.h>
#include <setjmp.h>


namespace plutosvg {

#define COLOR_TABLE_SIZE 1024
    typedef struct {
        plutovg_matrix_t matrix;
        plutovg_spread_method_t spread;
        uint32_t colortable[COLOR_TABLE_SIZE];
        union {
            struct {
                float x1, y1;
                float x2, y2;
            } linear;
            struct {
                float cx, cy, cr;
                float fx, fy, fr;
            } radial;
        } values;
    } gradient_data_t;

    typedef struct {
        plutovg_matrix_t matrix;
        uint8_t* data;
        int width;
        int height;
        int stride;
        int const_alpha;
    } texture_data_t;

    typedef struct {
        float dx;
        float dy;
        float l;
        float off;
    } linear_gradient_values_t;

    typedef struct {
        float dx;
        float dy;
        float dr;
        float sqrfr;
        float a;
        bool extended;
    } radial_gradient_values_t;

    static inline uint32_t premultiply_color_with_opacity(const plutovg_color_t* color, float opacity)
    {
        uint32_t alpha = lroundf(color->a * opacity * 255);
        uint32_t pr = lroundf(color->r * alpha);
        uint32_t pg = lroundf(color->g * alpha);
        uint32_t pb = lroundf(color->b * alpha);
        return (alpha << 24) | (pr << 16) | (pg << 8) | (pb);
    }

    static inline uint32_t INTERPOLATE_PIXEL(uint32_t x, uint32_t a, uint32_t y, uint32_t b)
    {
        uint32_t t = (x & 0xff00ff) * a + (y & 0xff00ff) * b;
        t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
        t &= 0xff00ff;
        x = ((x >> 8) & 0xff00ff) * a + ((y >> 8) & 0xff00ff) * b;
        x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
        x &= 0xff00ff00;
        x |= t;
        return x;
    }

    static inline uint32_t BYTE_MUL(uint32_t x, uint32_t a)
    {
        uint32_t t = (x & 0xff00ff) * a;
        t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
        t &= 0xff00ff;
        x = ((x >> 8) & 0xff00ff) * a;
        x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
        x &= 0xff00ff00;
        x |= t;
        return x;
    }


    void plutovg_memfill32(unsigned int* dest, int length, unsigned int value)
    {
        while (length--) {
            *dest++ = value;
        }
    }


    static inline int gradient_clamp(const gradient_data_t* gradient, int ipos)
    {
        if (gradient->spread == PLUTOVG_SPREAD_METHOD_REPEAT) {
            ipos = ipos % COLOR_TABLE_SIZE;
            ipos = ipos < 0 ? COLOR_TABLE_SIZE + ipos : ipos;
        }
        else if (gradient->spread == PLUTOVG_SPREAD_METHOD_REFLECT) {
            const int limit = COLOR_TABLE_SIZE * 2;
            ipos = ipos % limit;
            ipos = ipos < 0 ? limit + ipos : ipos;
            ipos = ipos >= COLOR_TABLE_SIZE ? limit - 1 - ipos : ipos;
        }
        else {
            if (ipos < 0) {
                ipos = 0;
            }
            else if (ipos >= COLOR_TABLE_SIZE) {
                ipos = COLOR_TABLE_SIZE - 1;
            }
        }

        return ipos;
    }

#define FIXPT_BITS 8
#define FIXPT_SIZE (1 << FIXPT_BITS)
    static inline uint32_t gradient_pixel_fixed(const gradient_data_t* gradient, int fixed_pos)
    {
        int ipos = (fixed_pos + (FIXPT_SIZE / 2)) >> FIXPT_BITS;
        return gradient->colortable[gradient_clamp(gradient, ipos)];
    }

    static inline uint32_t gradient_pixel(const gradient_data_t* gradient, float pos)
    {
        int ipos = (int)(pos * (COLOR_TABLE_SIZE - 1) + 0.5f);
        return gradient->colortable[gradient_clamp(gradient, ipos)];
    }

    static void fetch_linear_gradient(uint32_t* buffer, const linear_gradient_values_t* v, const gradient_data_t* gradient, int y, int x, int length)
    {
        float t, inc;
        float rx = 0, ry = 0;

        if (v->l == 0.f) {
            t = inc = 0;
        }
        else {
            rx = gradient->matrix.c * (y + 0.5f) + gradient->matrix.a * (x + 0.5f) + gradient->matrix.e;
            ry = gradient->matrix.d * (y + 0.5f) + gradient->matrix.b * (x + 0.5f) + gradient->matrix.f;
            t = v->dx * rx + v->dy * ry + v->off;
            inc = v->dx * gradient->matrix.a + v->dy * gradient->matrix.b;
            t *= (COLOR_TABLE_SIZE - 1);
            inc *= (COLOR_TABLE_SIZE - 1);
        }

        const uint32_t* end = buffer + length;
        if (inc > -1e-5f && inc < 1e-5f) {
            plutovg_memfill32(buffer, length, gradient_pixel_fixed(gradient, (int)(t * FIXPT_SIZE)));
        }
        else {
            if (t + inc * length < (float)(INT_MAX >> (FIXPT_BITS + 1)) && t + inc * length >(float)(INT_MIN >> (FIXPT_BITS + 1))) {
                int t_fixed = (int)(t * FIXPT_SIZE);
                int inc_fixed = (int)(inc * FIXPT_SIZE);
                while (buffer < end) {
                    *buffer = gradient_pixel_fixed(gradient, t_fixed);
                    t_fixed += inc_fixed;
                    ++buffer;
                }
            }
            else {
                while (buffer < end) {
                    *buffer = gradient_pixel(gradient, t / COLOR_TABLE_SIZE);
                    t += inc;
                    ++buffer;
                }
            }
        }
    }

    static void fetch_radial_gradient(uint32_t* buffer, const radial_gradient_values_t* v, const gradient_data_t* gradient, int y, int x, int length)
    {
        if (v->a == 0.f) {
            plutovg_memfill32(buffer, length, 0);
            return;
        }

        float rx = gradient->matrix.c * (y + 0.5f) + gradient->matrix.e + gradient->matrix.a * (x + 0.5f);
        float ry = gradient->matrix.d * (y + 0.5f) + gradient->matrix.f + gradient->matrix.b * (x + 0.5f);

        rx -= gradient->values.radial.fx;
        ry -= gradient->values.radial.fy;

        float inv_a = 1.f / (2.f * v->a);
        float delta_rx = gradient->matrix.a;
        float delta_ry = gradient->matrix.b;

        float b = 2 * (v->dr * gradient->values.radial.fr + rx * v->dx + ry * v->dy);
        float delta_b = 2 * (delta_rx * v->dx + delta_ry * v->dy);
        float b_delta_b = 2 * b * delta_b;
        float delta_b_delta_b = 2 * delta_b * delta_b;

        float bb = b * b;
        float delta_bb = delta_b * delta_b;

        b *= inv_a;
        delta_b *= inv_a;

        float rxrxryry = rx * rx + ry * ry;
        float delta_rxrxryry = delta_rx * delta_rx + delta_ry * delta_ry;
        float rx_plus_ry = 2 * (rx * delta_rx + ry * delta_ry);
        float delta_rx_plus_ry = 2 * delta_rxrxryry;

        inv_a *= inv_a;

        float det = (bb - 4 * v->a * (v->sqrfr - rxrxryry)) * inv_a;
        float delta_det = (b_delta_b + delta_bb + 4 * v->a * (rx_plus_ry + delta_rxrxryry)) * inv_a;
        float delta_delta_det = (delta_b_delta_b + 4 * v->a * delta_rx_plus_ry) * inv_a;

        const uint32_t* end = buffer + length;
        if (v->extended) {
            while (buffer < end) {
                uint32_t result = 0;
                if (det >= 0) {
                    float w = sqrtf(det) - b;
                    if (gradient->values.radial.fr + v->dr * w >= 0) {
                        result = gradient_pixel(gradient, w);
                    }
                }

                *buffer = result;
                det += delta_det;
                delta_det += delta_delta_det;
                b += delta_b;
                ++buffer;
            }
        }
        else {
            while (buffer < end) {
                *buffer++ = gradient_pixel(gradient, sqrtf(det) - b);
                det += delta_det;
                delta_det += delta_delta_det;
                b += delta_b;
            }
        }
    }

    static void composition_solid_clear(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            plutovg_memfill32(dest, length, 0);
        }
        else {
            uint32_t ialpha = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(dest[i], ialpha);
            }
        }
    }

    static void composition_solid_source(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            plutovg_memfill32(dest, length, color);
        }
        else {
            uint32_t ialpha = 255 - const_alpha;
            color = BYTE_MUL(color, const_alpha);
            for (int i = 0; i < length; i++) {
                dest[i] = color + BYTE_MUL(dest[i], ialpha);
            }
        }
    }

    static void composition_solid_destination(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
    }

    static void composition_solid_source_over(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha != 255)
            color = BYTE_MUL(color, const_alpha);
        uint32_t ialpha = 255 - plutovg_alpha(color);
        for (int i = 0; i < length; i++) {
            dest[i] = color + BYTE_MUL(dest[i], ialpha);
        }
    }

    static void composition_solid_destination_over(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha != 255)
            color = BYTE_MUL(color, const_alpha);
        for (int i = 0; i < length; i++) {
            uint32_t d = dest[i];
            dest[i] = d + BYTE_MUL(color, plutovg_alpha(~d));
        }
    }

    static void composition_solid_source_in(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(color, plutovg_alpha(dest[i]));
            }
        }
        else {
            color = BYTE_MUL(color, const_alpha);
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                dest[i] = INTERPOLATE_PIXEL(color, plutovg_alpha(d), d, cia);
            }
        }
    }

    static void composition_solid_destination_in(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        uint32_t a = plutovg_alpha(color);
        if (const_alpha != 255)
            a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
        for (int i = 0; i < length; i++) {
            dest[i] = BYTE_MUL(dest[i], a);
        }
    }

    static void composition_solid_source_out(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(color, plutovg_alpha(~dest[i]));
            }
        }
        else {
            color = BYTE_MUL(color, const_alpha);
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                dest[i] = INTERPOLATE_PIXEL(color, plutovg_alpha(~d), d, cia);
            }
        }
    }

    static void composition_solid_destination_out(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        uint32_t a = plutovg_alpha(~color);
        if (const_alpha != 255)
            a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
        for (int i = 0; i < length; i++) {
            dest[i] = BYTE_MUL(dest[i], a);
        }
    }

    static void composition_solid_source_atop(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha != 255)
            color = BYTE_MUL(color, const_alpha);
        uint32_t sia = plutovg_alpha(~color);
        for (int i = 0; i < length; i++) {
            uint32_t d = dest[i];
            dest[i] = INTERPOLATE_PIXEL(color, plutovg_alpha(d), d, sia);
        }
    }

    static void composition_solid_destination_atop(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        uint32_t a = plutovg_alpha(color);
        if (const_alpha != 255) {
            color = BYTE_MUL(color, const_alpha);
            a = plutovg_alpha(color) + 255 - const_alpha;
        }

        for (int i = 0; i < length; i++) {
            uint32_t d = dest[i];
            dest[i] = INTERPOLATE_PIXEL(d, a, color, plutovg_alpha(~d));
        }
    }

    static void composition_solid_xor(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
    {
        if (const_alpha != 255)
            color = BYTE_MUL(color, const_alpha);
        uint32_t sia = plutovg_alpha(~color);
        for (int i = 0; i < length; i++) {
            uint32_t d = dest[i];
            dest[i] = INTERPOLATE_PIXEL(color, plutovg_alpha(~d), d, sia);
        }
    }

    typedef void(*composition_solid_function_t)(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha);

    static const composition_solid_function_t composition_solid_table[] = {
        composition_solid_clear,
        composition_solid_source,
        composition_solid_destination,
        composition_solid_source_over,
        composition_solid_destination_over,
        composition_solid_source_in,
        composition_solid_destination_in,
        composition_solid_source_out,
        composition_solid_destination_out,
        composition_solid_source_atop,
        composition_solid_destination_atop,
        composition_solid_xor
    };

    static void composition_clear(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            plutovg_memfill32(dest, length, 0);
        }
        else {
            uint32_t ialpha = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(dest[i], ialpha);
            }
        }
    }

    static void composition_source(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            memcpy(dest, src, length * sizeof(uint32_t));
        }
        else {
            uint32_t ialpha = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                dest[i] = INTERPOLATE_PIXEL(src[i], const_alpha, dest[i], ialpha);
            }
        }
    }

    static void composition_destination(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
    }

    static void composition_source_over(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                uint32_t s = src[i];
                if (s >= 0xff000000) {
                    dest[i] = s;
                }
                else if (s != 0) {
                    dest[i] = s + BYTE_MUL(dest[i], plutovg_alpha(~s));
                }
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                dest[i] = s + BYTE_MUL(dest[i], plutovg_alpha(~s));
            }
        }
    }

    static void composition_destination_over(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                dest[i] = d + BYTE_MUL(src[i], plutovg_alpha(~d));
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                dest[i] = d + BYTE_MUL(s, plutovg_alpha(~d));
            }
        }
    }

    static void composition_source_in(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(src[i], plutovg_alpha(dest[i]));
            }
        }
        else {
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                dest[i] = INTERPOLATE_PIXEL(s, plutovg_alpha(d), d, cia);
            }
        }
    }

    static void composition_destination_in(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(dest[i], plutovg_alpha(src[i]));
            }
        }
        else {
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t a = BYTE_MUL(plutovg_alpha(src[i]), const_alpha) + cia;
                dest[i] = BYTE_MUL(dest[i], a);
            }
        }
    }

    static void composition_source_out(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(src[i], plutovg_alpha(~dest[i]));
            }
        }
        else {
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                uint32_t d = dest[i];
                dest[i] = INTERPOLATE_PIXEL(s, plutovg_alpha(~d), d, cia);
            }
        }
    }

    static void composition_destination_out(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                dest[i] = BYTE_MUL(dest[i], plutovg_alpha(~src[i]));
            }
        }
        else {
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t sia = BYTE_MUL(plutovg_alpha(~src[i]), const_alpha) + cia;
                dest[i] = BYTE_MUL(dest[i], sia);
            }
        }
    }

    static void composition_source_atop(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                uint32_t s = src[i];
                uint32_t d = dest[i];
                dest[i] = INTERPOLATE_PIXEL(s, plutovg_alpha(d), d, plutovg_alpha(~s));
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                uint32_t d = dest[i];
                dest[i] = INTERPOLATE_PIXEL(s, plutovg_alpha(d), d, plutovg_alpha(~s));
            }
        }
    }

    static void composition_destination_atop(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                uint32_t s = src[i];
                uint32_t d = dest[i];
                dest[i] = INTERPOLATE_PIXEL(d, plutovg_alpha(s), s, plutovg_alpha(~d));
            }
        }
        else {
            uint32_t cia = 255 - const_alpha;
            for (int i = 0; i < length; i++) {
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                uint32_t d = dest[i];
                uint32_t a = plutovg_alpha(s) + cia;
                dest[i] = INTERPOLATE_PIXEL(d, a, s, plutovg_alpha(~d));
            }
        }
    }

    static void composition_xor(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
    {
        if (const_alpha == 255) {
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                uint32_t s = src[i];
                dest[i] = INTERPOLATE_PIXEL(s, plutovg_alpha(~d), d, plutovg_alpha(~s));
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                uint32_t d = dest[i];
                uint32_t s = BYTE_MUL(src[i], const_alpha);
                dest[i] = INTERPOLATE_PIXEL(s, plutovg_alpha(~d), d, plutovg_alpha(~s));
            }
        }
    }

    typedef void(*composition_function_t)(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha);

    static const composition_function_t composition_table[] = {
        composition_clear,
        composition_source,
        composition_destination,
        composition_source_over,
        composition_destination_over,
        composition_source_in,
        composition_destination_in,
        composition_source_out,
        composition_destination_out,
        composition_source_atop,
        composition_destination_atop,
        composition_xor
    };

    static void blend_solid(plutovg_surface_t* surface, plutovg_operator_t op, uint32_t solid, const plutovg_span_buffer_t* span_buffer)
    {
        composition_solid_function_t func = composition_solid_table[op];
        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
            func(target, spans->len, solid, spans->coverage);
            ++spans;
        }
    }

#define BUFFER_SIZE 1024
    static void blend_linear_gradient(plutovg_surface_t* surface, plutovg_operator_t op, const gradient_data_t* gradient, const plutovg_span_buffer_t* span_buffer)
    {
        composition_function_t func = composition_table[op];
        unsigned int buffer[BUFFER_SIZE];

        linear_gradient_values_t v;
        v.dx = gradient->values.linear.x2 - gradient->values.linear.x1;
        v.dy = gradient->values.linear.y2 - gradient->values.linear.y1;
        v.l = v.dx * v.dx + v.dy * v.dy;
        v.off = 0.f;
        if (v.l != 0.f) {
            v.dx /= v.l;
            v.dy /= v.l;
            v.off = -v.dx * gradient->values.linear.x1 - v.dy * gradient->values.linear.y1;
        }

        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            int length = spans->len;
            int x = spans->x;
            while (length) {
                int l = plutovg_min(length, BUFFER_SIZE);
                fetch_linear_gradient(buffer, &v, gradient, spans->y, x, l);
                uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
                func(target, l, buffer, spans->coverage);
                x += l;
                length -= l;
            }

            ++spans;
        }
    }

    static void blend_radial_gradient(plutovg_surface_t* surface, plutovg_operator_t op, const gradient_data_t* gradient, const plutovg_span_buffer_t* span_buffer)
    {
        composition_function_t func = composition_table[op];
        unsigned int buffer[BUFFER_SIZE];

        radial_gradient_values_t v;
        v.dx = gradient->values.radial.cx - gradient->values.radial.fx;
        v.dy = gradient->values.radial.cy - gradient->values.radial.fy;
        v.dr = gradient->values.radial.cr - gradient->values.radial.fr;
        v.sqrfr = gradient->values.radial.fr * gradient->values.radial.fr;
        v.a = v.dr * v.dr - v.dx * v.dx - v.dy * v.dy;
        v.extended = gradient->values.radial.fr != 0.f || v.a <= 0.f;

        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            int length = spans->len;
            int x = spans->x;
            while (length) {
                int l = plutovg_min(length, BUFFER_SIZE);
                fetch_radial_gradient(buffer, &v, gradient, spans->y, x, l);
                uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
                func(target, l, buffer, spans->coverage);
                x += l;
                length -= l;
            }

            ++spans;
        }
    }

    static void blend_untransformed_argb(plutovg_surface_t* surface, plutovg_operator_t op, const texture_data_t* texture, const plutovg_span_buffer_t* span_buffer)
    {
        composition_function_t func = composition_table[op];

        const int image_width = texture->width;
        const int image_height = texture->height;

        int xoff = (int)(texture->matrix.e);
        int yoff = (int)(texture->matrix.f);

        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            int x = spans->x;
            int length = spans->len;
            int sx = xoff + x;
            int sy = yoff + spans->y;
            if (sy >= 0 && sy < image_height && sx < image_width) {
                if (sx < 0) {
                    x -= sx;
                    length += sx;
                    sx = 0;
                }

                if (sx + length > image_width)
                    length = image_width - sx;
                if (length > 0) {
                    const int coverage = (spans->coverage * texture->const_alpha) >> 8;
                    const uint32_t* src = (const uint32_t*)(texture->data + sy * texture->stride) + sx;
                    uint32_t* dest = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
                    func(dest, length, src, coverage);
                }
            }

            ++spans;
        }
    }

#define FIXED_SCALE (1 << 16)
    static void blend_transformed_argb(plutovg_surface_t* surface, plutovg_operator_t op, const texture_data_t* texture, const plutovg_span_buffer_t* span_buffer)
    {
        composition_function_t func = composition_table[op];
        uint32_t buffer[BUFFER_SIZE];

        int image_width = texture->width;
        int image_height = texture->height;

        int fdx = (int)(texture->matrix.a * FIXED_SCALE);
        int fdy = (int)(texture->matrix.b * FIXED_SCALE);

        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;

            const float cx = spans->x + 0.5f;
            const float cy = spans->y + 0.5f;

            int x = (int)((texture->matrix.c * cy + texture->matrix.a * cx + texture->matrix.e) * FIXED_SCALE);
            int y = (int)((texture->matrix.d * cy + texture->matrix.b * cx + texture->matrix.f) * FIXED_SCALE);

            int length = spans->len;
            const int coverage = (spans->coverage * texture->const_alpha) >> 8;
            while (length) {
                int l = plutovg_min(length, BUFFER_SIZE);
                const uint32_t* end = buffer + l;
                uint32_t* b = buffer;
                while (b < end) {
                    int px = x >> 16;
                    int py = y >> 16;
                    if ((px < 0) || (px >= image_width) || (py < 0) || (py >= image_height)) {
                        *b = 0x00000000;
                    }
                    else {
                        *b = ((const uint32_t*)(texture->data + py * texture->stride))[px];
                    }

                    x += fdx;
                    y += fdy;
                    ++b;
                }

                func(target, l, buffer, coverage);
                target += l;
                length -= l;
            }

            ++spans;
        }
    }

    static void blend_untransformed_tiled_argb(plutovg_surface_t* surface, plutovg_operator_t op, const texture_data_t* texture, const plutovg_span_buffer_t* span_buffer)
    {
        composition_function_t func = composition_table[op];

        int image_width = texture->width;
        int image_height = texture->height;

        int xoff = (int)(texture->matrix.e) % image_width;
        int yoff = (int)(texture->matrix.f) % image_height;

        if (xoff < 0)
            xoff += image_width;
        if (yoff < 0) {
            yoff += image_height;
        }

        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            int x = spans->x;
            int length = spans->len;
            int sx = (xoff + spans->x) % image_width;
            int sy = (spans->y + yoff) % image_height;
            if (sx < 0)
                sx += image_width;
            if (sy < 0) {
                sy += image_height;
            }

            const int coverage = (spans->coverage * texture->const_alpha) >> 8;
            while (length) {
                int l = plutovg_min(image_width - sx, length);
                if (BUFFER_SIZE < l)
                    l = BUFFER_SIZE;
                const uint32_t* src = (const uint32_t*)(texture->data + sy * texture->stride) + sx;
                uint32_t* dest = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
                func(dest, l, src, coverage);
                x += l;
                sx += l;
                length -= l;
                if (sx >= image_width) {
                    sx = 0;
                }
            }

            ++spans;
        }
    }

    static void blend_transformed_tiled_argb(plutovg_surface_t* surface, plutovg_operator_t op, const texture_data_t* texture, const plutovg_span_buffer_t* span_buffer)
    {
        composition_function_t func = composition_table[op];
        uint32_t buffer[BUFFER_SIZE];

        int image_width = texture->width;
        int image_height = texture->height;
        const int scanline_offset = texture->stride / 4;

        int fdx = (int)(texture->matrix.a * FIXED_SCALE);
        int fdy = (int)(texture->matrix.b * FIXED_SCALE);

        int count = span_buffer->spans.size;
        const plutovg_span_t* spans = span_buffer->spans.data;
        while (count--) {
            uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
            const uint32_t* image_bits = (const uint32_t*)texture->data;

            const float cx = spans->x + 0.5f;
            const float cy = spans->y + 0.5f;

            int x = (int)((texture->matrix.c * cy + texture->matrix.a * cx + texture->matrix.e) * FIXED_SCALE);
            int y = (int)((texture->matrix.d * cy + texture->matrix.b * cx + texture->matrix.f) * FIXED_SCALE);

            const int coverage = (spans->coverage * texture->const_alpha) >> 8;
            int length = spans->len;
            while (length) {
                int l = plutovg_min(length, BUFFER_SIZE);
                const uint32_t* end = buffer + l;
                uint32_t* b = buffer;
                while (b < end) {
                    int px = x >> 16;
                    int py = y >> 16;
                    px %= image_width;
                    py %= image_height;
                    if (px < 0) px += image_width;
                    if (py < 0) py += image_height;
                    int y_offset = py * scanline_offset;

                    assert(px >= 0 && px < image_width);
                    assert(py >= 0 && py < image_height);

                    *b = image_bits[y_offset + px];
                    x += fdx;
                    y += fdy;
                    ++b;
                }

                func(target, l, buffer, coverage);
                target += l;
                length -= l;
            }

            ++spans;
        }
    }

    typedef struct plutovg_state {
        plutovg_paint_t* paint;
        plutovg_color_t color;
        plutovg_matrix_t matrix;
        plutovg_stroke_data_t stroke;
        plutovg_span_buffer_t clip_spans;
        plutovg_fill_rule_t winding;
        plutovg_operator_t op;
        float opacity;
        bool clipping;
        struct plutovg_state* next;
    } plutovg_state_t;

    struct plutovg_canvas {
        int ref_count;
        plutovg_surface_t* surface;
        plutovg_path_t* path;
        plutovg_state_t* state;
        plutovg_state_t* freed_state;
        plutovg_rect_t clip_rect;
        plutovg_span_buffer_t clip_spans;
        plutovg_span_buffer_t fill_spans;
    };

    static void plutovg_blend_color(plutovg_canvas_t* canvas, const plutovg_color_t* color, const plutovg_span_buffer_t* span_buffer)
    {
        plutovg_state_t* state = canvas->state;
        uint32_t solid = premultiply_color_with_opacity(color, state->opacity);
        uint32_t alpha = plutovg_alpha(solid);

        if (alpha == 255 && state->op == PLUTOVG_OPERATOR_SRC_OVER) {
            blend_solid(canvas->surface, PLUTOVG_OPERATOR_SRC, solid, span_buffer);
        }
        else {
            blend_solid(canvas->surface, state->op, solid, span_buffer);
        }
    }

    static void plutovg_blend_gradient(plutovg_canvas_t* canvas, const plutovg_gradient_paint_t* gradient, const plutovg_span_buffer_t* span_buffer)
    {
        if (gradient->nstops == 0)
            return;
        plutovg_state_t* state = canvas->state;
        gradient_data_t data;
        data.spread = gradient->spread;
        data.matrix = gradient->matrix;
        plutovg_matrix_multiply(&data.matrix, &data.matrix, &state->matrix);
        if (!plutovg_matrix_invert(&data.matrix, &data.matrix))
            return;
        int i, pos = 0, nstops = gradient->nstops;
        const plutovg_gradient_stop_t* curr, * next, * start, * last;
        uint32_t curr_color, next_color, last_color;
        uint32_t dist, idist;
        float delta, t, incr, fpos;
        float opacity = state->opacity;

        start = gradient->stops;
        curr = start;
        curr_color = premultiply_color_with_opacity(&curr->color, opacity);

        data.colortable[pos++] = curr_color;
        incr = 1.0f / COLOR_TABLE_SIZE;
        fpos = 1.5f * incr;

        while (fpos <= curr->offset) {
            data.colortable[pos] = data.colortable[pos - 1];
            ++pos;
            fpos += incr;
        }

        for (i = 0; i < nstops - 1; i++) {
            curr = (start + i);
            next = (start + i + 1);
            if (curr->offset == next->offset)
                continue;
            delta = 1.f / (next->offset - curr->offset);
            next_color = premultiply_color_with_opacity(&next->color, opacity);
            while (fpos < next->offset && pos < COLOR_TABLE_SIZE) {
                t = (fpos - curr->offset) * delta;
                dist = (uint32_t)(255 * t);
                idist = 255 - dist;
                data.colortable[pos] = INTERPOLATE_PIXEL(curr_color, idist, next_color, dist);
                ++pos;
                fpos += incr;
            }

            curr_color = next_color;
        }

        last = start + nstops - 1;
        last_color = premultiply_color_with_opacity(&last->color, opacity);
        for (; pos < COLOR_TABLE_SIZE; ++pos) {
            data.colortable[pos] = last_color;
        }

        if (gradient->type == PLUTOVG_GRADIENT_TYPE_LINEAR) {
            data.values.linear.x1 = gradient->values[0];
            data.values.linear.y1 = gradient->values[1];
            data.values.linear.x2 = gradient->values[2];
            data.values.linear.y2 = gradient->values[3];
            blend_linear_gradient(canvas->surface, state->op, &data, span_buffer);
        }
        else {
            data.values.radial.cx = gradient->values[0];
            data.values.radial.cy = gradient->values[1];
            data.values.radial.cr = gradient->values[2];
            data.values.radial.fx = gradient->values[3];
            data.values.radial.fy = gradient->values[4];
            data.values.radial.fr = gradient->values[5];
            blend_radial_gradient(canvas->surface, state->op, &data, span_buffer);
        }
    }

    static void plutovg_blend_texture(plutovg_canvas_t* canvas, const plutovg_texture_paint_t* texture, const plutovg_span_buffer_t* span_buffer)
    {
        if (texture->surface == NULL)
            return;
        plutovg_state_t* state = canvas->state;
        texture_data_t data;
        data.matrix = texture->matrix;
        data.data = texture->surface->data;
        data.width = texture->surface->width;
        data.height = texture->surface->height;
        data.stride = texture->surface->stride;
        data.const_alpha = lroundf(state->opacity * texture->opacity * 256);

        plutovg_matrix_multiply(&data.matrix, &data.matrix, &state->matrix);
        if (!plutovg_matrix_invert(&data.matrix, &data.matrix))
            return;
        const plutovg_matrix_t* matrix = &data.matrix;
        if (matrix->a == 1 && matrix->b == 0 && matrix->c == 0 && matrix->d == 1) {
            if (texture->type == PLUTOVG_TEXTURE_TYPE_PLAIN) {
                blend_untransformed_argb(canvas->surface, state->op, &data, span_buffer);
            }
            else {
                blend_untransformed_tiled_argb(canvas->surface, state->op, &data, span_buffer);
            }
        }
        else {
            if (texture->type == PLUTOVG_TEXTURE_TYPE_PLAIN) {
                blend_transformed_argb(canvas->surface, state->op, &data, span_buffer);
            }
            else {
                blend_transformed_tiled_argb(canvas->surface, state->op, &data, span_buffer);
            }
        }
    }

    void plutovg_blend(plutovg_canvas_t* canvas, const plutovg_span_buffer_t* span_buffer)
    {
        if (span_buffer->spans.size == 0)
            return;
        if (canvas->state->paint == NULL) {
            plutovg_blend_color(canvas, &canvas->state->color, span_buffer);
            return;
        }

        plutovg_paint_t* paint = canvas->state->paint;
        if (paint->type == PLUTOVG_PAINT_TYPE_COLOR) {
            plutovg_solid_paint_t* solid = (plutovg_solid_paint_t*)(paint);
            plutovg_blend_color(canvas, &solid->color, span_buffer);
        }
        else if (paint->type == PLUTOVG_PAINT_TYPE_GRADIENT) {
            plutovg_gradient_paint_t* gradient = (plutovg_gradient_paint_t*)(paint);
            plutovg_blend_gradient(canvas, gradient, span_buffer);
        }
        else {
            plutovg_texture_paint_t* texture = (plutovg_texture_paint_t*)(paint);
            plutovg_blend_texture(canvas, texture, span_buffer);
        }
    }


    void plutovg_matrix_init(plutovg_matrix_t* matrix, float a, float b, float c, float d, float e, float f)
    {
        matrix->a = a; matrix->b = b;
        matrix->c = c; matrix->d = d;
        matrix->e = e; matrix->f = f;
    }

    void plutovg_matrix_init_identity(plutovg_matrix_t* matrix)
    {
        plutovg_matrix_init(matrix, 1, 0, 0, 1, 0, 0);
    }

    void plutovg_matrix_init_translate(plutovg_matrix_t* matrix, float tx, float ty)
    {
        plutovg_matrix_init(matrix, 1, 0, 0, 1, tx, ty);
    }

    void plutovg_matrix_init_scale(plutovg_matrix_t* matrix, float sx, float sy)
    {
        plutovg_matrix_init(matrix, sx, 0, 0, sy, 0, 0);
    }

    void plutovg_matrix_init_rotate(plutovg_matrix_t* matrix, float angle)
    {
        float c = cosf(angle);
        float s = sinf(angle);
        plutovg_matrix_init(matrix, c, s, -s, c, 0, 0);
    }

    void plutovg_matrix_init_shear(plutovg_matrix_t* matrix, float shx, float shy)
    {
        plutovg_matrix_init(matrix, 1, tanf(shy), tanf(shx), 1, 0, 0);
    }

    void plutovg_matrix_translate(plutovg_matrix_t* matrix, float tx, float ty)
    {
        plutovg_matrix_t m;
        plutovg_matrix_init_translate(&m, tx, ty);
        plutovg_matrix_multiply(matrix, &m, matrix);
    }

    void plutovg_matrix_scale(plutovg_matrix_t* matrix, float sx, float sy)
    {
        plutovg_matrix_t m;
        plutovg_matrix_init_scale(&m, sx, sy);
        plutovg_matrix_multiply(matrix, &m, matrix);
    }

    void plutovg_matrix_rotate(plutovg_matrix_t* matrix, float angle)
    {
        plutovg_matrix_t m;
        plutovg_matrix_init_rotate(&m, angle);
        plutovg_matrix_multiply(matrix, &m, matrix);
    }

    void plutovg_matrix_shear(plutovg_matrix_t* matrix, float shx, float shy)
    {
        plutovg_matrix_t m;
        plutovg_matrix_init_shear(&m, shx, shy);
        plutovg_matrix_multiply(matrix, &m, matrix);
    }

    void plutovg_matrix_multiply(plutovg_matrix_t* matrix, const plutovg_matrix_t* left, const plutovg_matrix_t* right)
    {
        float a = left->a * right->a + left->b * right->c;
        float b = left->a * right->b + left->b * right->d;
        float c = left->c * right->a + left->d * right->c;
        float d = left->c * right->b + left->d * right->d;
        float e = left->e * right->a + left->f * right->c + right->e;
        float f = left->e * right->b + left->f * right->d + right->f;
        plutovg_matrix_init(matrix, a, b, c, d, e, f);
    }

    bool plutovg_matrix_invert(const plutovg_matrix_t* matrix, plutovg_matrix_t* inverse)
    {
        float det = (matrix->a * matrix->d - matrix->b * matrix->c);
        if (det == 0.f)
            return false;
        if (inverse) {
            float inv_det = 1.f / det;
            float a = matrix->a * inv_det;
            float b = matrix->b * inv_det;
            float c = matrix->c * inv_det;
            float d = matrix->d * inv_det;
            float e = (matrix->c * matrix->f - matrix->d * matrix->e) * inv_det;
            float f = (matrix->b * matrix->e - matrix->a * matrix->f) * inv_det;
            plutovg_matrix_init(inverse, d, -b, -c, a, e, f);
        }

        return true;
    }

    void plutovg_matrix_map(const plutovg_matrix_t* matrix, float x, float y, float* xx, float* yy)
    {
        *xx = x * matrix->a + y * matrix->c + matrix->e;
        *yy = x * matrix->b + y * matrix->d + matrix->f;
    }

    void plutovg_matrix_map_point(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst)
    {
        plutovg_matrix_map(matrix, src->x, src->y, &dst->x, &dst->y);
    }

    void plutovg_matrix_map_points(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst, int count)
    {
        for (int i = 0; i < count; ++i) {
            plutovg_matrix_map_point(matrix, &src[i], &dst[i]);
        }
    }

    void plutovg_matrix_map_rect(const plutovg_matrix_t* matrix, const plutovg_rect_t* src, plutovg_rect_t* dst)
    {
        plutovg_point_t p[4];
        p[0].x = src->x;
        p[0].y = src->y;
        p[1].x = src->x + src->w;
        p[1].y = src->y;
        p[2].x = src->x + src->w;
        p[2].y = src->y + src->h;
        p[3].x = src->x;
        p[3].y = src->y + src->h;
        plutovg_matrix_map_points(matrix, p, p, 4);

        float l = p[0].x;
        float t = p[0].y;
        float r = p[0].x;
        float b = p[0].y;

        for (int i = 1; i < 4; i++) {
            if (p[i].x < l) l = p[i].x;
            if (p[i].x > r) r = p[i].x;
            if (p[i].y < t) t = p[i].y;
            if (p[i].y > b) b = p[i].y;
        }

        dst->x = l;
        dst->y = t;
        dst->w = r - l;
        dst->h = b - t;
    }

    static int parse_matrix_parameters(const char** begin, const char* end, float values[6], int required, int optional)
    {
        if (!plutovg_skip_ws_and_delim(begin, end, '('))
            return 0;
        int count = 0;
        int max_count = required + optional;
        bool has_trailing_comma = false;
        for (; count < max_count; ++count) {
            if (!plutovg_parse_number(begin, end, values + count))
                break;
            plutovg_skip_ws_or_comma(begin, end, &has_trailing_comma);
        }

        if (!has_trailing_comma && (count == required || count == max_count)
            && plutovg_skip_delim(begin, end, ')')) {
            return count;
        }

        return 0;
    }

    bool plutovg_matrix_parse(plutovg_matrix_t* matrix, const char* data, int length)
    {
        float values[6];
        plutovg_matrix_init_identity(matrix);
        if (length == -1)
            length = (int)strlen(data);
        const char* it = data;
        const char* end = it + length;
        bool has_trailing_comma = false;
        plutovg_skip_ws(&it, end);
        while (it < end) {
            if (plutovg_skip_string(&it, end, "matrix")) {
                int count = parse_matrix_parameters(&it, end, values, 6, 0);
                if (count == 0)
                    return false;
                plutovg_matrix_t m = { values[0], values[1], values[2], values[3], values[4], values[5] };
                plutovg_matrix_multiply(matrix, &m, matrix);
            }
            else if (plutovg_skip_string(&it, end, "translate")) {
                int count = parse_matrix_parameters(&it, end, values, 1, 1);
                if (count == 0)
                    return false;
                if (count == 1) {
                    plutovg_matrix_translate(matrix, values[0], 0);
                }
                else {
                    plutovg_matrix_translate(matrix, values[0], values[1]);
                }
            }
            else if (plutovg_skip_string(&it, end, "scale")) {
                int count = parse_matrix_parameters(&it, end, values, 1, 1);
                if (count == 0)
                    return false;
                if (count == 1) {
                    plutovg_matrix_scale(matrix, values[0], values[0]);
                }
                else {
                    plutovg_matrix_scale(matrix, values[0], values[1]);
                }
            }
            else if (plutovg_skip_string(&it, end, "rotate")) {
                int count = parse_matrix_parameters(&it, end, values, 1, 2);
                if (count == 0)
                    return false;
                if (count == 3)
                    plutovg_matrix_translate(matrix, values[1], values[2]);
                plutovg_matrix_rotate(matrix, PLUTOVG_DEG2RAD(values[0]));
                if (count == 3) {
                    plutovg_matrix_translate(matrix, -values[1], -values[2]);
                }
            }
            else if (plutovg_skip_string(&it, end, "skewX")) {
                int count = parse_matrix_parameters(&it, end, values, 1, 0);
                if (count == 0)
                    return false;
                plutovg_matrix_shear(matrix, PLUTOVG_DEG2RAD(values[0]), 0);
            }
            else if (plutovg_skip_string(&it, end, "skewY")) {
                int count = parse_matrix_parameters(&it, end, values, 1, 0);
                if (count == 0)
                    return false;
                plutovg_matrix_shear(matrix, 0, PLUTOVG_DEG2RAD(values[0]));
            }
            else {
                return false;
            }

            plutovg_skip_ws_or_comma(&it, end, &has_trailing_comma);
        }

        return !has_trailing_comma;
    }

    void plutovg_path_iterator_init(plutovg_path_iterator_t* it, const plutovg_path_t* path)
    {
        it->elements = path->elements.data;
        it->size = path->elements.size;
        it->index = 0;
    }

    bool plutovg_path_iterator_has_next(const plutovg_path_iterator_t* it)
    {
        return it->index < it->size;
    }


    plutovg_path_command_t plutovg_path_iterator_next(plutovg_path_iterator_t* it, plutovg_point_t points[3])
    {
        const plutovg_path_element_t* elements = it->elements + it->index;
        switch (elements[0].header.command) {
        case PLUTOVG_PATH_COMMAND_MOVE_TO:
        case PLUTOVG_PATH_COMMAND_LINE_TO:
        case PLUTOVG_PATH_COMMAND_CLOSE:
            points[0] = elements[1].point;
            break;
        case PLUTOVG_PATH_COMMAND_CUBIC_TO:
            points[0] = elements[1].point;
            points[1] = elements[2].point;
            points[2] = elements[3].point;
            break;
        }

        it->index += elements[0].header.length;
        return elements[0].header.command;
    }

    plutovg_path_t* plutovg_path_create(void)
    {
        plutovg_path_t* path = (plutovg_path_t*)malloc(sizeof(plutovg_path_t));
        path->ref_count = 1;
        path->num_points = 0;
        path->num_contours = 0;
        path->num_curves = 0;
        path->start_point = PLUTOVG_EMPTY_POINT;
        plutovg_array_init(path->elements);
        return path;
    }

    plutovg_path_t* plutovg_path_reference(plutovg_path_t* path)
    {
        if (path == NULL)
            return NULL;
        ++path->ref_count;
        return path;
    }

    void plutovg_path_destroy(plutovg_path_t* path)
    {
        if (path == NULL)
            return;
        if (--path->ref_count == 0) {
            plutovg_array_destroy(path->elements);
            free(path);
        }
    }

    int plutovg_path_get_reference_count(const plutovg_path_t* path)
    {
        if (path)
            return path->ref_count;
        return 0;
    }

    int plutovg_path_get_elements(const plutovg_path_t* path, const plutovg_path_element_t** elements)
    {
        if (elements)
            *elements = path->elements.data;
        return path->elements.size;
    }

    static plutovg_path_element_t* plutovg_path_add_command(plutovg_path_t* path, plutovg_path_command_t command, int npoints)
    {
        const int length = npoints + 1;
        plutovg_array_ensure(path->elements, length);
        plutovg_path_element_t* elements = path->elements.data + path->elements.size;
        elements->header.command = command;
        elements->header.length = length;
        path->elements.size += length;
        path->num_points += npoints;
        return elements + 1;
    }

    void plutovg_path_move_to(plutovg_path_t* path, float x, float y)
    {
        plutovg_path_element_t* elements = plutovg_path_add_command(path, PLUTOVG_PATH_COMMAND_MOVE_TO, 1);
        elements[0].point = PLUTOVG_MAKE_POINT(x, y);
        path->start_point = PLUTOVG_MAKE_POINT(x, y);
        path->num_contours += 1;
    }

    void plutovg_path_line_to(plutovg_path_t* path, float x, float y)
    {
        if (path->elements.size == 0)
            plutovg_path_move_to(path, 0, 0);
        plutovg_path_element_t* elements = plutovg_path_add_command(path, PLUTOVG_PATH_COMMAND_LINE_TO, 1);
        elements[0].point = PLUTOVG_MAKE_POINT(x, y);
    }

    void plutovg_path_quad_to(plutovg_path_t* path, float x1, float y1, float x2, float y2)
    {
        float current_x, current_y;
        plutovg_path_get_current_point(path, &current_x, &current_y);
        float cp1x = 2.f / 3.f * x1 + 1.f / 3.f * current_x;
        float cp1y = 2.f / 3.f * y1 + 1.f / 3.f * current_y;
        float cp2x = 2.f / 3.f * x1 + 1.f / 3.f * x2;
        float cp2y = 2.f / 3.f * y1 + 1.f / 3.f * y2;
        plutovg_path_cubic_to(path, cp1x, cp1y, cp2x, cp2y, x2, y2);
    }

    void plutovg_path_cubic_to(plutovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3)
    {
        if (path->elements.size == 0)
            plutovg_path_move_to(path, 0, 0);
        plutovg_path_element_t* elements = plutovg_path_add_command(path, PLUTOVG_PATH_COMMAND_CUBIC_TO, 3);
        elements[0].point = PLUTOVG_MAKE_POINT(x1, y1);
        elements[1].point = PLUTOVG_MAKE_POINT(x2, y2);
        elements[2].point = PLUTOVG_MAKE_POINT(x3, y3);
        path->num_curves += 1;
    }

    void plutovg_path_arc_to(plutovg_path_t* path, float rx, float ry, float angle, bool large_arc_flag, bool sweep_flag, float x, float y)
    {
        float current_x, current_y;
        plutovg_path_get_current_point(path, &current_x, &current_y);
        if (rx == 0.f || ry == 0.f || (current_x == x && current_y == y)) {
            plutovg_path_line_to(path, x, y);
            return;
        }

        if (rx < 0.f) rx = -rx;
        if (ry < 0.f) ry = -ry;

        float dx = (current_x - x) * 0.5f;
        float dy = (current_y - y) * 0.5f;

        plutovg_matrix_t matrix;
        plutovg_matrix_init_rotate(&matrix, -angle);
        plutovg_matrix_map(&matrix, dx, dy, &dx, &dy);

        float rxrx = rx * rx;
        float ryry = ry * ry;
        float dxdx = dx * dx;
        float dydy = dy * dy;
        float radius = dxdx / rxrx + dydy / ryry;
        if (radius > 1.f) {
            rx *= sqrtf(radius);
            ry *= sqrtf(radius);
        }

        plutovg_matrix_init_scale(&matrix, 1.f / rx, 1.f / ry);
        plutovg_matrix_rotate(&matrix, -angle);

        float x1, y1;
        float x2, y2;
        plutovg_matrix_map(&matrix, current_x, current_y, &x1, &y1);
        plutovg_matrix_map(&matrix, x, y, &x2, &y2);

        float dx1 = x2 - x1;
        float dy1 = y2 - y1;
        float d = dx1 * dx1 + dy1 * dy1;
        float scale_sq = 1.f / d - 0.25f;
        if (scale_sq < 0.f) scale_sq = 0.f;
        float scale = sqrtf(scale_sq);
        if (sweep_flag == large_arc_flag)
            scale = -scale;
        dx1 *= scale;
        dy1 *= scale;

        float cx1 = 0.5f * (x1 + x2) - dy1;
        float cy1 = 0.5f * (y1 + y2) + dx1;

        float th1 = atan2f(y1 - cy1, x1 - cx1);
        float th2 = atan2f(y2 - cy1, x2 - cx1);
        float th_arc = th2 - th1;
        if (th_arc < 0.f && sweep_flag)
            th_arc += PLUTOVG_TWO_PI;
        else if (th_arc > 0.f && !sweep_flag)
            th_arc -= PLUTOVG_TWO_PI;
        plutovg_matrix_init_rotate(&matrix, angle);
        plutovg_matrix_scale(&matrix, rx, ry);
        int segments = (int)(ceilf(fabsf(th_arc / (PLUTOVG_HALF_PI + 0.001f))));
        for (int i = 0; i < segments; i++) {
            float th_start = th1 + i * th_arc / segments;
            float th_end = th1 + (i + 1) * th_arc / segments;
            float t = (8.f / 6.f) * tanf(0.25f * (th_end - th_start));

            float x3 = cosf(th_end) + cx1;
            float y3 = sinf(th_end) + cy1;

            float cp2x = x3 + t * sinf(th_end);
            float cp2y = y3 - t * cosf(th_end);

            float cp1x = cosf(th_start) - t * sinf(th_start);
            float cp1y = sinf(th_start) + t * cosf(th_start);

            cp1x += cx1;
            cp1y += cy1;

            plutovg_matrix_map(&matrix, cp1x, cp1y, &cp1x, &cp1y);
            plutovg_matrix_map(&matrix, cp2x, cp2y, &cp2x, &cp2y);
            plutovg_matrix_map(&matrix, x3, y3, &x3, &y3);

            plutovg_path_cubic_to(path, cp1x, cp1y, cp2x, cp2y, x3, y3);
        }
    }

    void plutovg_path_close(plutovg_path_t* path)
    {
        if (path->elements.size == 0)
            return;
        plutovg_path_element_t* elements = plutovg_path_add_command(path, PLUTOVG_PATH_COMMAND_CLOSE, 1);
        elements[0].point = path->start_point;
    }

    void plutovg_path_get_current_point(const plutovg_path_t* path, float* x, float* y)
    {
        float xx = 0.f;
        float yy = 0.f;
        if (path->num_points > 0) {
            xx = path->elements.data[path->elements.size - 1].point.x;
            yy = path->elements.data[path->elements.size - 1].point.y;
        }

        if (x) *x = xx;
        if (y) *y = yy;
    }

    void plutovg_path_reserve(plutovg_path_t* path, int count)
    {
        plutovg_array_ensure(path->elements, count);
    }

    void plutovg_path_reset(plutovg_path_t* path)
    {
        plutovg_array_clear(path->elements);
        path->start_point = PLUTOVG_EMPTY_POINT;
        path->num_points = 0;
        path->num_contours = 0;
        path->num_curves = 0;
    }

    void plutovg_path_add_rect(plutovg_path_t* path, float x, float y, float w, float h)
    {
        plutovg_path_reserve(path, 6 * 2);
        plutovg_path_move_to(path, x, y);
        plutovg_path_line_to(path, x + w, y);
        plutovg_path_line_to(path, x + w, y + h);
        plutovg_path_line_to(path, x, y + h);
        plutovg_path_line_to(path, x, y);
        plutovg_path_close(path);
    }

    void plutovg_path_add_round_rect(plutovg_path_t* path, float x, float y, float w, float h, float rx, float ry)
    {
        rx = plutovg_min(rx, w * 0.5f);
        ry = plutovg_min(ry, h * 0.5f);
        if (rx == 0.f && ry == 0.f) {
            plutovg_path_add_rect(path, x, y, w, h);
            return;
        }

        float right = x + w;
        float bottom = y + h;

        float cpx = rx * PLUTOVG_KAPPA;
        float cpy = ry * PLUTOVG_KAPPA;

        plutovg_path_reserve(path, 6 * 2 + 4 * 4);
        plutovg_path_move_to(path, x, y + ry);
        plutovg_path_cubic_to(path, x, y + ry - cpy, x + rx - cpx, y, x + rx, y);
        plutovg_path_line_to(path, right - rx, y);
        plutovg_path_cubic_to(path, right - rx + cpx, y, right, y + ry - cpy, right, y + ry);
        plutovg_path_line_to(path, right, bottom - ry);
        plutovg_path_cubic_to(path, right, bottom - ry + cpy, right - rx + cpx, bottom, right - rx, bottom);
        plutovg_path_line_to(path, x + rx, bottom);
        plutovg_path_cubic_to(path, x + rx - cpx, bottom, x, bottom - ry + cpy, x, bottom - ry);
        plutovg_path_line_to(path, x, y + ry);
        plutovg_path_close(path);
    }

    void plutovg_path_add_ellipse(plutovg_path_t* path, float cx, float cy, float rx, float ry)
    {
        float left = cx - rx;
        float top = cy - ry;
        float right = cx + rx;
        float bottom = cy + ry;

        float cpx = rx * PLUTOVG_KAPPA;
        float cpy = ry * PLUTOVG_KAPPA;

        plutovg_path_reserve(path, 2 * 2 + 4 * 4);
        plutovg_path_move_to(path, cx, top);
        plutovg_path_cubic_to(path, cx + cpx, top, right, cy - cpy, right, cy);
        plutovg_path_cubic_to(path, right, cy + cpy, cx + cpx, bottom, cx, bottom);
        plutovg_path_cubic_to(path, cx - cpx, bottom, left, cy + cpy, left, cy);
        plutovg_path_cubic_to(path, left, cy - cpy, cx - cpx, top, cx, top);
        plutovg_path_close(path);
    }

    void plutovg_path_add_circle(plutovg_path_t* path, float cx, float cy, float r)
    {
        plutovg_path_add_ellipse(path, cx, cy, r, r);
    }

    void plutovg_path_add_arc(plutovg_path_t* path, float cx, float cy, float r, float a0, float a1, bool ccw)
    {
        float da = a1 - a0;
        if (fabsf(da) > PLUTOVG_TWO_PI) {
            da = PLUTOVG_TWO_PI;
        }
        else if (da != 0.f && ccw != (da < 0.f)) {
            da += PLUTOVG_TWO_PI * (ccw ? -1 : 1);
        }

        int seg_n = (int)(ceilf(fabsf(da) / PLUTOVG_HALF_PI));
        if (seg_n == 0)
            return;
        float a = a0;
        float ax = cx + cosf(a) * r;
        float ay = cy + sinf(a) * r;

        float seg_a = da / seg_n;
        float d = (seg_a / PLUTOVG_HALF_PI) * PLUTOVG_KAPPA * r;
        float dx = -sinf(a) * d;
        float dy = cosf(a) * d;

        plutovg_path_reserve(path, 2 + 4 * seg_n);
        if (path->elements.size == 0) {
            plutovg_path_move_to(path, ax, ay);
        }
        else {
            plutovg_path_line_to(path, ax, ay);
        }

        for (int i = 0; i < seg_n; i++) {
            float cp1x = ax + dx;
            float cp1y = ay + dy;

            a += seg_a;
            ax = cx + cosf(a) * r;
            ay = cy + sinf(a) * r;

            dx = -sinf(a) * d;
            dy = cosf(a) * d;

            float cp2x = ax - dx;
            float cp2y = ay - dy;

            plutovg_path_cubic_to(path, cp1x, cp1y, cp2x, cp2y, ax, ay);
        }
    }

    void plutovg_path_transform(plutovg_path_t* path, const plutovg_matrix_t* matrix)
    {
        plutovg_path_element_t* elements = path->elements.data;
        for (int i = 0; i < path->elements.size; i += elements[i].header.length) {
            switch (elements[i].header.command) {
            case PLUTOVG_PATH_COMMAND_MOVE_TO:
            case PLUTOVG_PATH_COMMAND_LINE_TO:
            case PLUTOVG_PATH_COMMAND_CLOSE:
                plutovg_matrix_map_point(matrix, &elements[i + 1].point, &elements[i + 1].point);
                break;
            case PLUTOVG_PATH_COMMAND_CUBIC_TO:
                plutovg_matrix_map_point(matrix, &elements[i + 1].point, &elements[i + 1].point);
                plutovg_matrix_map_point(matrix, &elements[i + 2].point, &elements[i + 2].point);
                plutovg_matrix_map_point(matrix, &elements[i + 3].point, &elements[i + 3].point);
                break;
            }
        }
    }

    void plutovg_path_add_path(plutovg_path_t* path, const plutovg_path_t* source, const plutovg_matrix_t* matrix)
    {
        if (matrix == NULL) {
            plutovg_array_append(path->elements, source->elements);
            path->start_point = source->start_point;
            path->num_points += source->num_points;
            path->num_contours += source->num_contours;
            path->num_curves += source->num_curves;
            return;
        }

        plutovg_path_iterator_t it;
        plutovg_path_iterator_init(&it, source);

        plutovg_point_t points[3];
        plutovg_array_ensure(path->elements, source->elements.size);
        while (plutovg_path_iterator_has_next(&it)) {
            switch (plutovg_path_iterator_next(&it, points)) {
            case PLUTOVG_PATH_COMMAND_MOVE_TO:
                plutovg_matrix_map_points(matrix, points, points, 1);
                plutovg_path_move_to(path, points[0].x, points[0].y);
                break;
            case PLUTOVG_PATH_COMMAND_LINE_TO:
                plutovg_matrix_map_points(matrix, points, points, 1);
                plutovg_path_line_to(path, points[0].x, points[0].y);
                break;
            case PLUTOVG_PATH_COMMAND_CUBIC_TO:
                plutovg_matrix_map_points(matrix, points, points, 3);
                plutovg_path_cubic_to(path, points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);
                break;
            case PLUTOVG_PATH_COMMAND_CLOSE:
                plutovg_path_close(path);
                break;
            }
        }
    }

    void plutovg_path_traverse(const plutovg_path_t* path, plutovg_path_traverse_func_t traverse_func, void* closure)
    {
        plutovg_path_iterator_t it;
        plutovg_path_iterator_init(&it, path);

        plutovg_point_t points[3];
        while (plutovg_path_iterator_has_next(&it)) {
            switch (plutovg_path_iterator_next(&it, points)) {
            case PLUTOVG_PATH_COMMAND_MOVE_TO:
                traverse_func(closure, PLUTOVG_PATH_COMMAND_MOVE_TO, points, 1);
                break;
            case PLUTOVG_PATH_COMMAND_LINE_TO:
                traverse_func(closure, PLUTOVG_PATH_COMMAND_LINE_TO, points, 1);
                break;
            case PLUTOVG_PATH_COMMAND_CUBIC_TO:
                traverse_func(closure, PLUTOVG_PATH_COMMAND_CUBIC_TO, points, 3);
                break;
            case PLUTOVG_PATH_COMMAND_CLOSE:
                traverse_func(closure, PLUTOVG_PATH_COMMAND_CLOSE, points, 1);
                break;
            }
        }
    }

    typedef struct {
        float x1; float y1;
        float x2; float y2;
        float x3; float y3;
        float x4; float y4;
    } bezier_t;

    static inline void split_bezier(const bezier_t* b, bezier_t* first, bezier_t* second)
    {
        float c = (b->x2 + b->x3) * 0.5f;
        first->x2 = (b->x1 + b->x2) * 0.5f;
        second->x3 = (b->x3 + b->x4) * 0.5f;
        first->x1 = b->x1;
        second->x4 = b->x4;
        first->x3 = (first->x2 + c) * 0.5f;
        second->x2 = (second->x3 + c) * 0.5f;
        first->x4 = second->x1 = (first->x3 + second->x2) * 0.5f;

        c = (b->y2 + b->y3) * 0.5f;
        first->y2 = (b->y1 + b->y2) * 0.5f;
        second->y3 = (b->y3 + b->y4) * 0.5f;
        first->y1 = b->y1;
        second->y4 = b->y4;
        first->y3 = (first->y2 + c) * 0.5f;
        second->y2 = (second->y3 + c) * 0.5f;
        first->y4 = second->y1 = (first->y3 + second->y2) * 0.5f;
    }

    void plutovg_path_traverse_flatten(const plutovg_path_t* path, plutovg_path_traverse_func_t traverse_func, void* closure)
    {
        if (path->num_curves == 0) {
            plutovg_path_traverse(path, traverse_func, closure);
            return;
        }

        const float threshold = 0.25f;

        plutovg_path_iterator_t it;
        plutovg_path_iterator_init(&it, path);

        bezier_t beziers[32];
        plutovg_point_t points[3];
        plutovg_point_t current_point = { 0, 0 };
        while (plutovg_path_iterator_has_next(&it)) {
            plutovg_path_command_t command = plutovg_path_iterator_next(&it, points);
            switch (command) {
            case PLUTOVG_PATH_COMMAND_MOVE_TO:
            case PLUTOVG_PATH_COMMAND_LINE_TO:
            case PLUTOVG_PATH_COMMAND_CLOSE:
                traverse_func(closure, command, points, 1);
                current_point = points[0];
                break;
            case PLUTOVG_PATH_COMMAND_CUBIC_TO:
                beziers[0].x1 = current_point.x;
                beziers[0].y1 = current_point.y;
                beziers[0].x2 = points[0].x;
                beziers[0].y2 = points[0].y;
                beziers[0].x3 = points[1].x;
                beziers[0].y3 = points[1].y;
                beziers[0].x4 = points[2].x;
                beziers[0].y4 = points[2].y;
                bezier_t* b = beziers;
                while (b >= beziers) {
                    float y4y1 = b->y4 - b->y1;
                    float x4x1 = b->x4 - b->x1;
                    float l = fabsf(x4x1) + fabsf(y4y1);
                    float d;
                    if (l > 1.f) {
                        d = fabsf((x4x1) * (b->y1 - b->y2) - (y4y1) * (b->x1 - b->x2)) + fabsf((x4x1) * (b->y1 - b->y3) - (y4y1) * (b->x1 - b->x3));
                    }
                    else {
                        d = fabsf(b->x1 - b->x2) + fabsf(b->y1 - b->y2) + fabsf(b->x1 - b->x3) + fabsf(b->y1 - b->y3);
                        l = 1.f;
                    }

                    if (d < threshold * l || b == beziers + 31) {
                        plutovg_point_t p = { b->x4, b->y4 };
                        traverse_func(closure, PLUTOVG_PATH_COMMAND_LINE_TO, &p, 1);
                        --b;
                    }
                    else {
                        split_bezier(b, b + 1, b);
                        ++b;
                    }
                }

                current_point = points[2];
                break;
            }
        }
    }

    typedef struct {
        const float* dashes; int ndashes;
        float start_phase; float phase;
        int start_index; int index;
        bool start_toggle; bool toggle;
        plutovg_point_t current_point;
        plutovg_path_traverse_func_t traverse_func;
        void* closure;
    } dasher_t;

    static void dash_traverse_func(void* closure, plutovg_path_command_t command, const plutovg_point_t* points, int npoints)
    {
        dasher_t* dasher = (dasher_t*)(closure);
        if (command == PLUTOVG_PATH_COMMAND_MOVE_TO) {
            if (dasher->start_toggle)
                dasher->traverse_func(dasher->closure, PLUTOVG_PATH_COMMAND_MOVE_TO, points, npoints);
            dasher->current_point = points[0];
            dasher->phase = dasher->start_phase;
            dasher->index = dasher->start_index;
            dasher->toggle = dasher->start_toggle;
            return;
        }

        assert(command == PLUTOVG_PATH_COMMAND_LINE_TO || command == PLUTOVG_PATH_COMMAND_CLOSE);
        plutovg_point_t p0 = dasher->current_point;
        plutovg_point_t p1 = points[0];
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        float dist0 = sqrtf(dx * dx + dy * dy);
        float dist1 = 0.f;
        while (dist0 - dist1 > dasher->dashes[dasher->index % dasher->ndashes] - dasher->phase) {
            dist1 += dasher->dashes[dasher->index % dasher->ndashes] - dasher->phase;
            float a = dist1 / dist0;
            plutovg_point_t p = { p0.x + a * dx, p0.y + a * dy };
            if (dasher->toggle) {
                dasher->traverse_func(dasher->closure, PLUTOVG_PATH_COMMAND_LINE_TO, &p, 1);
            }
            else {
                dasher->traverse_func(dasher->closure, PLUTOVG_PATH_COMMAND_MOVE_TO, &p, 1);
            }

            dasher->phase = 0.f;
            dasher->toggle = !dasher->toggle;
            dasher->index++;
        }

        if (dasher->toggle) {
            dasher->traverse_func(dasher->closure, PLUTOVG_PATH_COMMAND_LINE_TO, &p1, 1);
        }

        dasher->phase += dist0 - dist1;
        dasher->current_point = p1;
    }

    void plutovg_path_traverse_dashed(const plutovg_path_t* path, float offset, const float* dashes, int ndashes, plutovg_path_traverse_func_t traverse_func, void* closure)
    {
        float dash_sum = 0.f;
        for (int i = 0; i < ndashes; ++i)
            dash_sum += dashes[i];
        if (ndashes % 2 == 1)
            dash_sum *= 2.f;
        if (dash_sum <= 0.f) {
            plutovg_path_traverse(path, traverse_func, closure);
            return;
        }

        dasher_t dasher;
        dasher.dashes = dashes;
        dasher.ndashes = ndashes;
        dasher.start_phase = fmodf(offset, dash_sum);
        if (dasher.start_phase < 0.f)
            dasher.start_phase += dash_sum;
        dasher.start_index = 0;
        dasher.start_toggle = true;
        while (dasher.start_phase > 0.f && dasher.start_phase >= dasher.dashes[dasher.start_index % dasher.ndashes]) {
            dasher.start_phase -= dashes[dasher.start_index % dasher.ndashes];
            dasher.start_toggle = !dasher.start_toggle;
            dasher.start_index++;
        }

        dasher.phase = dasher.start_phase;
        dasher.index = dasher.start_index;
        dasher.toggle = dasher.start_toggle;
        dasher.current_point = PLUTOVG_EMPTY_POINT;
        dasher.traverse_func = traverse_func;
        dasher.closure = closure;
        plutovg_path_traverse_flatten(path, dash_traverse_func, &dasher);
    }

    plutovg_path_t* plutovg_path_clone(const plutovg_path_t* path)
    {
        plutovg_path_t* clone = plutovg_path_create();
        plutovg_array_append(clone->elements, path->elements);
        clone->start_point = path->start_point;
        clone->num_points = path->num_points;
        clone->num_contours = path->num_contours;
        clone->num_curves = path->num_curves;
        return clone;
    }

    static void clone_traverse_func(void* closure, plutovg_path_command_t command, const plutovg_point_t* points, int npoints)
    {
        plutovg_path_t* path = (plutovg_path_t*)(closure);
        switch (command) {
        case PLUTOVG_PATH_COMMAND_MOVE_TO:
            plutovg_path_move_to(path, points[0].x, points[0].y);
            break;
        case PLUTOVG_PATH_COMMAND_LINE_TO:
            plutovg_path_line_to(path, points[0].x, points[0].y);
            break;
        case PLUTOVG_PATH_COMMAND_CUBIC_TO:
            plutovg_path_cubic_to(path, points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);
            break;
        case PLUTOVG_PATH_COMMAND_CLOSE:
            plutovg_path_close(path);
            break;
        }
    }

    plutovg_path_t* plutovg_path_clone_flatten(const plutovg_path_t* path)
    {
        plutovg_path_t* clone = plutovg_path_create();
        plutovg_path_reserve(clone, path->elements.size + path->num_curves * 32);
        plutovg_path_traverse_flatten(path, clone_traverse_func, clone);
        return clone;
    }

    plutovg_path_t* plutovg_path_clone_dashed(const plutovg_path_t* path, float offset, const float* dashes, int ndashes)
    {
        plutovg_path_t* clone = plutovg_path_create();
        plutovg_path_reserve(clone, path->elements.size + path->num_curves * 32);
        plutovg_path_traverse_dashed(path, offset, dashes, ndashes, clone_traverse_func, clone);
        return clone;
    }

    typedef struct {
        plutovg_point_t current_point;
        bool is_first_point;
        float length;
        float x1;
        float y1;
        float x2;
        float y2;
    } extents_calculator_t;

    static void extents_traverse_func(void* closure, plutovg_path_command_t command, const plutovg_point_t* points, int npoints)
    {
        extents_calculator_t* calculator = (extents_calculator_t*)(closure);
        if (calculator->is_first_point) {
            assert(command == PLUTOVG_PATH_COMMAND_MOVE_TO);
            calculator->is_first_point = false;
            calculator->current_point = points[0];
            calculator->x1 = points[0].x;
            calculator->y1 = points[0].y;
            calculator->x2 = points[0].x;
            calculator->y2 = points[0].y;
            calculator->length = 0;
            return;
        }

        for (int i = 0; i < npoints; ++i) {
            calculator->x1 = plutovg_min(calculator->x1, points[i].x);
            calculator->y1 = plutovg_min(calculator->y1, points[i].y);
            calculator->x2 = plutovg_max(calculator->x2, points[i].x);
            calculator->y2 = plutovg_max(calculator->y2, points[i].y);
            if (command != PLUTOVG_PATH_COMMAND_MOVE_TO)
                calculator->length += hypotf(points[i].x - calculator->current_point.x, points[i].y - calculator->current_point.y);
            calculator->current_point = points[i];
        }
    }

    float plutovg_path_extents(const plutovg_path_t* path, plutovg_rect_t* extents, bool tight)
    {
        extents_calculator_t calculator = { {0, 0}, true, 0, 0, 0, 0, 0 };
        if (tight) {
            plutovg_path_traverse_flatten(path, extents_traverse_func, &calculator);
        }
        else {
            plutovg_path_traverse(path, extents_traverse_func, &calculator);
        }

        if (extents) {
            extents->x = calculator.x1;
            extents->y = calculator.y1;
            extents->w = calculator.x2 - calculator.x1;
            extents->h = calculator.y2 - calculator.y1;
        }

        return calculator.length;
    }

    float plutovg_path_length(const plutovg_path_t* path)
    {
        return plutovg_path_extents(path, NULL, true);
    }

    static inline bool parse_arc_flag(const char** begin, const char* end, bool* flag)
    {
        if (plutovg_skip_delim(begin, end, '0'))
            *flag = 0;
        else if (plutovg_skip_delim(begin, end, '1'))
            *flag = 1;
        else
            return false;
        plutovg_skip_ws_or_comma(begin, end, NULL);
        return true;
    }

    static inline bool parse_path_coordinates(const char** begin, const char* end, float values[6], int offset, int count)
    {
        for (int i = 0; i < count; i++) {
            if (!plutovg_parse_number(begin, end, values + offset + i))
                return false;
            plutovg_skip_ws_or_comma(begin, end, NULL);
        }

        return true;
    }

    bool plutovg_path_parse(plutovg_path_t* path, const char* data, int length)
    {
        if (length == -1)
            length = (int)strlen(data);
        const char* it = data;
        const char* end = it + length;

        float values[6];
        bool flags[2];

        float start_x = 0;
        float start_y = 0;
        float current_x = 0;
        float current_y = 0;
        float last_control_x = 0;
        float last_control_y = 0;

        char command = 0;
        char last_command = 0;
        plutovg_skip_ws(&it, end);
        while (it < end) {
            if (PLUTOVG_IS_ALPHA(*it)) {
                command = *it++;
                plutovg_skip_ws(&it, end);
            }

            if (!last_command && !(command == 'M' || command == 'm'))
                return false;
            if (command == 'M' || command == 'm') {
                if (!parse_path_coordinates(&it, end, values, 0, 2))
                    return false;
                if (command == 'm') {
                    values[0] += current_x;
                    values[1] += current_y;
                }

                plutovg_path_move_to(path, values[0], values[1]);
                current_x = start_x = values[0];
                current_y = start_y = values[1];
                command = command == 'm' ? 'l' : 'L';
            }
            else if (command == 'L' || command == 'l') {
                if (!parse_path_coordinates(&it, end, values, 0, 2))
                    return false;
                if (command == 'l') {
                    values[0] += current_x;
                    values[1] += current_y;
                }

                plutovg_path_line_to(path, values[0], values[1]);
                current_x = values[0];
                current_y = values[1];
            }
            else if (command == 'H' || command == 'h') {
                if (!parse_path_coordinates(&it, end, values, 0, 1))
                    return false;
                if (command == 'h') {
                    values[0] += current_x;
                }

                plutovg_path_line_to(path, values[0], current_y);
                current_x = values[0];
            }
            else if (command == 'V' || command == 'v') {
                if (!parse_path_coordinates(&it, end, values, 1, 1))
                    return false;
                if (command == 'v') {
                    values[1] += current_y;
                }

                plutovg_path_line_to(path, current_x, values[1]);
                current_y = values[1];
            }
            else if (command == 'Q' || command == 'q') {
                if (!parse_path_coordinates(&it, end, values, 0, 4))
                    return false;
                if (command == 'q') {
                    values[0] += current_x;
                    values[1] += current_y;
                    values[2] += current_x;
                    values[3] += current_y;
                }

                plutovg_path_quad_to(path, values[0], values[1], values[2], values[3]);
                last_control_x = values[0];
                last_control_y = values[1];
                current_x = values[2];
                current_y = values[3];
            }
            else if (command == 'C' || command == 'c') {
                if (!parse_path_coordinates(&it, end, values, 0, 6))
                    return false;
                if (command == 'c') {
                    values[0] += current_x;
                    values[1] += current_y;
                    values[2] += current_x;
                    values[3] += current_y;
                    values[4] += current_x;
                    values[5] += current_y;
                }

                plutovg_path_cubic_to(path, values[0], values[1], values[2], values[3], values[4], values[5]);
                last_control_x = values[2];
                last_control_y = values[3];
                current_x = values[4];
                current_y = values[5];
            }
            else if (command == 'T' || command == 't') {
                if (last_command != 'Q' && last_command != 'q' && last_command != 'T' && last_command != 't') {
                    values[0] = current_x;
                    values[1] = current_y;
                }
                else {
                    values[0] = 2 * current_x - last_control_x;
                    values[1] = 2 * current_y - last_control_y;
                }

                if (!parse_path_coordinates(&it, end, values, 2, 2))
                    return false;
                if (command == 't') {
                    values[2] += current_x;
                    values[3] += current_y;
                }

                plutovg_path_quad_to(path, values[0], values[1], values[2], values[3]);
                last_control_x = values[0];
                last_control_y = values[1];
                current_x = values[2];
                current_y = values[3];
            }
            else if (command == 'S' || command == 's') {
                if (last_command != 'C' && last_command != 'c' && last_command != 'S' && last_command != 's') {
                    values[0] = current_x;
                    values[1] = current_y;
                }
                else {
                    values[0] = 2 * current_x - last_control_x;
                    values[1] = 2 * current_y - last_control_y;
                }

                if (!parse_path_coordinates(&it, end, values, 2, 4))
                    return false;
                if (command == 's') {
                    values[2] += current_x;
                    values[3] += current_y;
                    values[4] += current_x;
                    values[5] += current_y;
                }

                plutovg_path_cubic_to(path, values[0], values[1], values[2], values[3], values[4], values[5]);
                last_control_x = values[2];
                last_control_y = values[3];
                current_x = values[4];
                current_y = values[5];
            }
            else if (command == 'A' || command == 'a') {
                if (!parse_path_coordinates(&it, end, values, 0, 3)
                    || !parse_arc_flag(&it, end, &flags[0])
                    || !parse_arc_flag(&it, end, &flags[1])
                    || !parse_path_coordinates(&it, end, values, 3, 2)) {
                    return false;
                }

                if (command == 'a') {
                    values[3] += current_x;
                    values[4] += current_y;
                }

                plutovg_path_arc_to(path, values[0], values[1], PLUTOVG_DEG2RAD(values[2]), flags[0], flags[1], values[3], values[4]);
                current_x = values[3];
                current_y = values[4];
            }
            else if (command == 'Z' || command == 'z') {
                if (last_command == 'Z' || last_command == 'z')
                    return false;
                plutovg_path_close(path);
                current_x = start_x;
                current_y = start_y;
            }
            else {
                return false;
            }

            last_command = command;
        }

        return true;
    }

    static plutovg_surface_t* plutovg_surface_create_uninitialized(int width, int height)
    {
        static const int kMaxSize = 1 << 15;
        if (width <= 0 || height <= 0 || width >= kMaxSize || height >= kMaxSize)
            return NULL;
        const size_t size = width * height * 4;
        plutovg_surface_t* surface = (plutovg_surface_t*)malloc(size + sizeof(plutovg_surface_t));
        if (surface == NULL)
            return NULL;
        surface->ref_count = 1;
        surface->width = width;
        surface->height = height;
        surface->stride = width * 4;
        surface->data = (uint8_t*)(surface + 1);
        return surface;
    }

    plutovg_surface_t* plutovg_surface_create(int width, int height)
    {
        plutovg_surface_t* surface = plutovg_surface_create_uninitialized(width, height);
        if (surface)
            memset(surface->data, 0, surface->height * surface->stride);
        return surface;
    }

    plutovg_surface_t* plutovg_surface_create_for_data(unsigned char* data, int width, int height, int stride)
    {
        plutovg_surface_t* surface = (plutovg_surface_t*)malloc(sizeof(plutovg_surface_t));
        surface->ref_count = 1;
        surface->width = width;
        surface->height = height;
        surface->stride = stride;
        surface->data = data;
        return surface;
    }


    plutovg_surface_t* plutovg_surface_reference(plutovg_surface_t* surface)
    {
        if (surface == NULL)
            return NULL;
        ++surface->ref_count;
        return surface;
    }

    void plutovg_surface_destroy(plutovg_surface_t* surface)
    {
        if (surface == NULL)
            return;
        if (--surface->ref_count == 0) {
            free(surface);
        }
    }

    int plutovg_surface_get_reference_count(const plutovg_surface_t* surface)
    {
        if (surface)
            return surface->ref_count;
        return 0;
    }

    unsigned char* plutovg_surface_get_data(const plutovg_surface_t* surface)
    {
        return surface->data;
    }

    int plutovg_surface_get_width(const plutovg_surface_t* surface)
    {
        return surface->width;
    }

    int plutovg_surface_get_height(const plutovg_surface_t* surface)
    {
        return surface->height;
    }

    int plutovg_surface_get_stride(const plutovg_surface_t* surface)
    {
        return surface->stride;
    }

    void plutovg_surface_clear(plutovg_surface_t* surface, const plutovg_color_t* color)
    {
        uint32_t pixel = plutovg_premultiply_argb(plutovg_color_to_argb32(color));
        for (int y = 0; y < surface->height; y++) {
            uint32_t* pixels = (uint32_t*)(surface->data + surface->stride * y);
            plutovg_memfill32(pixels, surface->width, pixel);
        }
    }


    void plutovg_convert_argb_to_rgba(unsigned char* dst, const unsigned char* src, int width, int height, int stride)
    {
        for (int y = 0; y < height; y++) {
            const uint32_t* src_row = (const uint32_t*)(src + stride * y);
            unsigned char* dst_row = dst + stride * y;
            for (int x = 0; x < width; x++) {
                uint32_t pixel = src_row[x];
                uint32_t a = (pixel >> 24) & 0xFF;
                if (a == 0) {
                    *dst_row++ = 0;
                    *dst_row++ = 0;
                    *dst_row++ = 0;
                    *dst_row++ = 0;
                }
                else {
                    uint32_t r = (pixel >> 16) & 0xFF;
                    uint32_t g = (pixel >> 8) & 0xFF;
                    uint32_t b = (pixel >> 0) & 0xFF;
                    if (a != 255) {
                        r = (r * 255) / a;
                        g = (g * 255) / a;
                        b = (b * 255) / a;
                    }

                    *dst_row++ = r;
                    *dst_row++ = g;
                    *dst_row++ = b;
                    *dst_row++ = a;
                }
            }
        }
    }

    void plutovg_convert_rgba_to_argb(unsigned char* dst, const unsigned char* src, int width, int height, int stride)
    {
        for (int y = 0; y < height; y++) {
            const unsigned char* src_row = src + stride * y;
            uint32_t* dst_row = (uint32_t*)(dst + stride * y);
            for (int x = 0; x < width; x++) {
                uint32_t a = src_row[4 * x + 3];
                if (a == 0) {
                    dst_row[x] = 0x00000000;
                }
                else {
                    uint32_t r = src_row[4 * x + 0];
                    uint32_t g = src_row[4 * x + 1];
                    uint32_t b = src_row[4 * x + 2];
                    if (a != 255) {
                        r = (r * a) / 255;
                        g = (g * a) / 255;
                        b = (b * a) / 255;
                    }

                    dst_row[x] = (a << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }
    }


    void plutovg_color_init_rgb(plutovg_color_t* color, float r, float g, float b)
    {
        plutovg_color_init_rgba(color, r, g, b, 1.f);
    }

    void plutovg_color_init_rgba(plutovg_color_t* color, float r, float g, float b, float a)
    {
        color->r = plutovg_clamp(r, 0.f, 1.f);
        color->g = plutovg_clamp(g, 0.f, 1.f);
        color->b = plutovg_clamp(b, 0.f, 1.f);
        color->a = plutovg_clamp(a, 0.f, 1.f);
    }

    void plutovg_color_init_rgb8(plutovg_color_t* color, int r, int g, int b)
    {
        plutovg_color_init_rgba8(color, r, g, b, 255);
    }

    void plutovg_color_init_rgba8(plutovg_color_t* color, int r, int g, int b, int a)
    {
        plutovg_color_init_rgba(color, r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    void plutovg_color_init_rgba32(plutovg_color_t* color, unsigned int value)
    {
        uint8_t r = (value >> 24) & 0xFF;
        uint8_t g = (value >> 16) & 0xFF;
        uint8_t b = (value >> 8) & 0xFF;
        uint8_t a = (value >> 0) & 0xFF;
        plutovg_color_init_rgba8(color, r, g, b, a);
    }

    void plutovg_color_init_argb32(plutovg_color_t* color, unsigned int value)
    {
        uint8_t a = (value >> 24) & 0xFF;
        uint8_t r = (value >> 16) & 0xFF;
        uint8_t g = (value >> 8) & 0xFF;
        uint8_t b = (value >> 0) & 0xFF;
        plutovg_color_init_rgba8(color, r, g, b, a);
    }

    void plutovg_color_init_hsl(plutovg_color_t* color, float h, float s, float l)
    {
        plutovg_color_init_hsla(color, h, s, l, 1.f);
    }

    static inline float hsl_component(float h, float s, float l, float n)
    {
        const float k = fmodf(n + h / 30.f, 12.f);
        const float a = s * plutovg_min(l, 1.f - l);
        return l - a * plutovg_max(-1.f, plutovg_min(1.f, plutovg_min(k - 3.f, 9.f - k)));
    }

    void plutovg_color_init_hsla(plutovg_color_t* color, float h, float s, float l, float a)
    {
        h = fmodf(h, 360.f);
        if (h < 0.f) { h += 360.f; }

        float r = hsl_component(h, s, l, 0);
        float g = hsl_component(h, s, l, 8);
        float b = hsl_component(h, s, l, 4);
        plutovg_color_init_rgba(color, r, g, b, a);
    }

    unsigned int plutovg_color_to_rgba32(const plutovg_color_t* color)
    {
        uint32_t r = lroundf(color->r * 255);
        uint32_t g = lroundf(color->g * 255);
        uint32_t b = lroundf(color->b * 255);
        uint32_t a = lroundf(color->a * 255);
        return (r << 24) | (g << 16) | (b << 8) | (a);
    }

    unsigned int plutovg_color_to_argb32(const plutovg_color_t* color)
    {
        uint32_t a = lroundf(color->a * 255);
        uint32_t r = lroundf(color->r * 255);
        uint32_t g = lroundf(color->g * 255);
        uint32_t b = lroundf(color->b * 255);
        return (a << 24) | (r << 16) | (g << 8) | (b);
    }

    static inline uint8_t hex_digit(uint8_t c)
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'a' && c <= 'f')
            return 10 + c - 'a';
        return 10 + c - 'A';
    }

    static inline uint8_t hex_byte(uint8_t c1, uint8_t c2)
    {
        uint8_t h1 = hex_digit(c1);
        uint8_t h2 = hex_digit(c2);
        return (h1 << 4) | h2;
    }

    typedef struct {
        const char* name;
        uint32_t value;
    } color_entry_t;

    static int color_entry_compare(const void* a, const void* b)
    {
        const char* name = (const char*)a;
        const color_entry_t* entry = (const color_entry_t*)b;
        return strcmp(name, entry->name);
    }

    static bool parse_rgb_component(const char** begin, const char* end, float* component)
    {
        float value = 0;
        if (!plutovg_parse_number(begin, end, &value))
            return false;
        if (plutovg_skip_delim(begin, end, '%'))
            value *= 2.55f;
        *component = plutovg_clamp(value, 0.f, 255.f) / 255.f;
        return true;
    }

    static bool parse_alpha_component(const char** begin, const char* end, float* component)
    {
        float value = 0;
        if (!plutovg_parse_number(begin, end, &value))
            return false;
        if (plutovg_skip_delim(begin, end, '%'))
            value /= 100.f;
        *component = plutovg_clamp(value, 0.f, 1.f);
        return true;
    }

    int plutovg_color_parse(plutovg_color_t* color, const char* data, int length)
    {
        if (length == -1)
            length = (int)strlen(data);
        const char* it = data;
        const char* end = it + length;
        plutovg_skip_ws(&it, end);
        if (plutovg_skip_delim(&it, end, '#')) {
            int r, g, b, a = 255;
            const char* begin = it;
            while (it < end && isxdigit(*it))
                ++it;
            int count = (int)(it - begin);
            if (count == 3 || count == 4) {
                r = hex_byte(begin[0], begin[0]);
                g = hex_byte(begin[1], begin[1]);
                b = hex_byte(begin[2], begin[2]);
                if (count == 4) {
                    a = hex_byte(begin[3], begin[3]);
                }
            }
            else if (count == 6 || count == 8) {
                r = hex_byte(begin[0], begin[1]);
                g = hex_byte(begin[2], begin[3]);
                b = hex_byte(begin[4], begin[5]);
                if (count == 8) {
                    a = hex_byte(begin[6], begin[7]);
                }
            }
            else {
                return 0;
            }

            plutovg_color_init_rgba8(color, r, g, b, a);
        }
        else {
            const int MAX_NAME = 20;
            int name_length = 0;
            char name[MAX_NAME + 1];
            while (it < end && name_length < MAX_NAME && isalpha(*it))
                name[name_length++] = tolower(*it++);
            name[name_length] = '\0';

            if (strcmp(name, "transparent") == 0) {
                plutovg_color_init_rgba(color, 0, 0, 0, 0);
            }
            else if (strcmp(name, "rgb") == 0 || strcmp(name, "rgba") == 0) {
                if (!plutovg_skip_ws_and_delim(&it, end, '('))
                    return 0;
                float r, g, b, a = 1.f;
                if (!parse_rgb_component(&it, end, &r)
                    || !plutovg_skip_ws_and_comma(&it, end)
                    || !parse_rgb_component(&it, end, &g)
                    || !plutovg_skip_ws_and_comma(&it, end)
                    || !parse_rgb_component(&it, end, &b)) {
                    return 0;
                }

                if (plutovg_skip_ws_and_comma(&it, end)
                    && !parse_alpha_component(&it, end, &a)) {
                    return 0;
                }

                plutovg_skip_ws(&it, end);
                if (!plutovg_skip_delim(&it, end, ')'))
                    return 0;
                plutovg_color_init_rgba(color, r, g, b, a);
            }
            else if (strcmp(name, "hsl") == 0 || strcmp(name, "hsla") == 0) {
                if (!plutovg_skip_ws_and_delim(&it, end, '('))
                    return 0;
                float h, s, l, a = 1.f;
                if (!plutovg_parse_number(&it, end, &h)
                    || !plutovg_skip_ws_and_comma(&it, end)
                    || !parse_alpha_component(&it, end, &s)
                    || !plutovg_skip_ws_and_comma(&it, end)
                    || !parse_alpha_component(&it, end, &l)) {
                    return 0;
                }

                if (plutovg_skip_ws_and_comma(&it, end)
                    && !parse_alpha_component(&it, end, &a)) {
                    return 0;
                }

                plutovg_skip_ws(&it, end);
                if (!plutovg_skip_delim(&it, end, ')'))
                    return 0;
                plutovg_color_init_hsla(color, h, s, l, a);
            }
            else {
                static const color_entry_t colormap[] = {
                    {"aliceblue", 0xF0F8FF},
                    {"antiquewhite", 0xFAEBD7},
                    {"aqua", 0x00FFFF},
                    {"aquamarine", 0x7FFFD4},
                    {"azure", 0xF0FFFF},
                    {"beige", 0xF5F5DC},
                    {"bisque", 0xFFE4C4},
                    {"black", 0x000000},
                    {"blanchedalmond", 0xFFEBCD},
                    {"blue", 0x0000FF},
                    {"blueviolet", 0x8A2BE2},
                    {"brown", 0xA52A2A},
                    {"burlywood", 0xDEB887},
                    {"cadetblue", 0x5F9EA0},
                    {"chartreuse", 0x7FFF00},
                    {"chocolate", 0xD2691E},
                    {"coral", 0xFF7F50},
                    {"cornflowerblue", 0x6495ED},
                    {"cornsilk", 0xFFF8DC},
                    {"crimson", 0xDC143C},
                    {"cyan", 0x00FFFF},
                    {"darkblue", 0x00008B},
                    {"darkcyan", 0x008B8B},
                    {"darkgoldenrod", 0xB8860B},
                    {"darkgray", 0xA9A9A9},
                    {"darkgreen", 0x006400},
                    {"darkgrey", 0xA9A9A9},
                    {"darkkhaki", 0xBDB76B},
                    {"darkmagenta", 0x8B008B},
                    {"darkolivegreen", 0x556B2F},
                    {"darkorange", 0xFF8C00},
                    {"darkorchid", 0x9932CC},
                    {"darkred", 0x8B0000},
                    {"darksalmon", 0xE9967A},
                    {"darkseagreen", 0x8FBC8F},
                    {"darkslateblue", 0x483D8B},
                    {"darkslategray", 0x2F4F4F},
                    {"darkslategrey", 0x2F4F4F},
                    {"darkturquoise", 0x00CED1},
                    {"darkviolet", 0x9400D3},
                    {"deeppink", 0xFF1493},
                    {"deepskyblue", 0x00BFFF},
                    {"dimgray", 0x696969},
                    {"dimgrey", 0x696969},
                    {"dodgerblue", 0x1E90FF},
                    {"firebrick", 0xB22222},
                    {"floralwhite", 0xFFFAF0},
                    {"forestgreen", 0x228B22},
                    {"fuchsia", 0xFF00FF},
                    {"gainsboro", 0xDCDCDC},
                    {"ghostwhite", 0xF8F8FF},
                    {"gold", 0xFFD700},
                    {"goldenrod", 0xDAA520},
                    {"gray", 0x808080},
                    {"green", 0x008000},
                    {"greenyellow", 0xADFF2F},
                    {"grey", 0x808080},
                    {"honeydew", 0xF0FFF0},
                    {"hotpink", 0xFF69B4},
                    {"indianred", 0xCD5C5C},
                    {"indigo", 0x4B0082},
                    {"ivory", 0xFFFFF0},
                    {"khaki", 0xF0E68C},
                    {"lavender", 0xE6E6FA},
                    {"lavenderblush", 0xFFF0F5},
                    {"lawngreen", 0x7CFC00},
                    {"lemonchiffon", 0xFFFACD},
                    {"lightblue", 0xADD8E6},
                    {"lightcoral", 0xF08080},
                    {"lightcyan", 0xE0FFFF},
                    {"lightgoldenrodyellow", 0xFAFAD2},
                    {"lightgray", 0xD3D3D3},
                    {"lightgreen", 0x90EE90},
                    {"lightgrey", 0xD3D3D3},
                    {"lightpink", 0xFFB6C1},
                    {"lightsalmon", 0xFFA07A},
                    {"lightseagreen", 0x20B2AA},
                    {"lightskyblue", 0x87CEFA},
                    {"lightslategray", 0x778899},
                    {"lightslategrey", 0x778899},
                    {"lightsteelblue", 0xB0C4DE},
                    {"lightyellow", 0xFFFFE0},
                    {"lime", 0x00FF00},
                    {"limegreen", 0x32CD32},
                    {"linen", 0xFAF0E6},
                    {"magenta", 0xFF00FF},
                    {"maroon", 0x800000},
                    {"mediumaquamarine", 0x66CDAA},
                    {"mediumblue", 0x0000CD},
                    {"mediumorchid", 0xBA55D3},
                    {"mediumpurple", 0x9370DB},
                    {"mediumseagreen", 0x3CB371},
                    {"mediumslateblue", 0x7B68EE},
                    {"mediumspringgreen", 0x00FA9A},
                    {"mediumturquoise", 0x48D1CC},
                    {"mediumvioletred", 0xC71585},
                    {"midnightblue", 0x191970},
                    {"mintcream", 0xF5FFFA},
                    {"mistyrose", 0xFFE4E1},
                    {"moccasin", 0xFFE4B5},
                    {"navajowhite", 0xFFDEAD},
                    {"navy", 0x000080},
                    {"oldlace", 0xFDF5E6},
                    {"olive", 0x808000},
                    {"olivedrab", 0x6B8E23},
                    {"orange", 0xFFA500},
                    {"orangered", 0xFF4500},
                    {"orchid", 0xDA70D6},
                    {"palegoldenrod", 0xEEE8AA},
                    {"palegreen", 0x98FB98},
                    {"paleturquoise", 0xAFEEEE},
                    {"palevioletred", 0xDB7093},
                    {"papayawhip", 0xFFEFD5},
                    {"peachpuff", 0xFFDAB9},
                    {"peru", 0xCD853F},
                    {"pink", 0xFFC0CB},
                    {"plum", 0xDDA0DD},
                    {"powderblue", 0xB0E0E6},
                    {"purple", 0x800080},
                    {"rebeccapurple", 0x663399},
                    {"red", 0xFF0000},
                    {"rosybrown", 0xBC8F8F},
                    {"royalblue", 0x4169E1},
                    {"saddlebrown", 0x8B4513},
                    {"salmon", 0xFA8072},
                    {"sandybrown", 0xF4A460},
                    {"seagreen", 0x2E8B57},
                    {"seashell", 0xFFF5EE},
                    {"sienna", 0xA0522D},
                    {"silver", 0xC0C0C0},
                    {"skyblue", 0x87CEEB},
                    {"slateblue", 0x6A5ACD},
                    {"slategray", 0x708090},
                    {"slategrey", 0x708090},
                    {"snow", 0xFFFAFA},
                    {"springgreen", 0x00FF7F},
                    {"steelblue", 0x4682B4},
                    {"tan", 0xD2B48C},
                    {"teal", 0x008080},
                    {"thistle", 0xD8BFD8},
                    {"tomato", 0xFF6347},
                    {"turquoise", 0x40E0D0},
                    {"violet", 0xEE82EE},
                    {"wheat", 0xF5DEB3},
                    {"white", 0xFFFFFF},
                    {"whitesmoke", 0xF5F5F5},
                    {"yellow", 0xFFFF00},
                    {"yellowgreen", 0x9ACD32}
                };

                const color_entry_t* entry = (const color_entry_t*)bsearch(name, colormap, sizeof(colormap) / sizeof(color_entry_t), sizeof(color_entry_t), color_entry_compare);
                if (entry == NULL)
                    return 0;
                plutovg_color_init_argb32(color, 0xFF000000 | entry->value);
            }
        }

        plutovg_skip_ws(&it, end);
        return (int)(it - data);
    }

    static void* plutovg_paint_create(plutovg_paint_type_t type, size_t size)
    {
        plutovg_paint_t* paint = (plutovg_paint_t*)malloc(size);
        paint->ref_count = 1;
        paint->type = type;
        return paint;
    }

    plutovg_paint_t* plutovg_paint_create_rgb(float r, float g, float b)
    {
        return plutovg_paint_create_rgba(r, g, b, 1.f);
    }

    plutovg_paint_t* plutovg_paint_create_rgba(float r, float g, float b, float a)
    {
        plutovg_solid_paint_t* solid = (plutovg_solid_paint_t*)plutovg_paint_create(PLUTOVG_PAINT_TYPE_COLOR, sizeof(plutovg_solid_paint_t));
        solid->color.r = plutovg_clamp(r, 0.f, 1.f);
        solid->color.g = plutovg_clamp(g, 0.f, 1.f);
        solid->color.b = plutovg_clamp(b, 0.f, 1.f);
        solid->color.a = plutovg_clamp(a, 0.f, 1.f);
        return &solid->base;
    }

    plutovg_paint_t* plutovg_paint_create_color(const plutovg_color_t* color)
    {
        return plutovg_paint_create_rgba(color->r, color->g, color->b, color->a);
    }

    static plutovg_gradient_paint_t* plutovg_gradient_create(plutovg_gradient_type_t type, plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix)
    {
        plutovg_gradient_paint_t* gradient = (plutovg_gradient_paint_t*)plutovg_paint_create(PLUTOVG_PAINT_TYPE_GRADIENT, sizeof(plutovg_gradient_paint_t) + nstops * sizeof(plutovg_gradient_stop_t));
        gradient->type = type;
        gradient->spread = spread;
        gradient->matrix = matrix ? *matrix : PLUTOVG_IDENTITY_MATRIX;
        gradient->stops = (plutovg_gradient_stop_t*)(gradient + 1);
        gradient->nstops = nstops;

        float prev_offset = 0.f;
        for (int i = 0; i < nstops; ++i) {
            const plutovg_gradient_stop_t* stop = stops + i;
            gradient->stops[i].offset = plutovg_max(prev_offset, plutovg_clamp(stop->offset, 0.f, 1.f));
            gradient->stops[i].color.r = plutovg_clamp(stop->color.r, 0.f, 1.f);
            gradient->stops[i].color.g = plutovg_clamp(stop->color.g, 0.f, 1.f);
            gradient->stops[i].color.b = plutovg_clamp(stop->color.b, 0.f, 1.f);
            gradient->stops[i].color.a = plutovg_clamp(stop->color.a, 0.f, 1.f);
            prev_offset = gradient->stops[i].offset;
        }

        return gradient;
    }

    plutovg_paint_t* plutovg_paint_create_linear_gradient(float x1, float y1, float x2, float y2, plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix)
    {
        plutovg_gradient_paint_t* gradient = plutovg_gradient_create(PLUTOVG_GRADIENT_TYPE_LINEAR, spread, stops, nstops, matrix);
        gradient->values[0] = x1;
        gradient->values[1] = y1;
        gradient->values[2] = x2;
        gradient->values[3] = y2;
        return &gradient->base;
    }

    plutovg_paint_t* plutovg_paint_create_radial_gradient(float cx, float cy, float cr, float fx, float fy, float fr, plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix)
    {
        plutovg_gradient_paint_t* gradient = plutovg_gradient_create(PLUTOVG_GRADIENT_TYPE_RADIAL, spread, stops, nstops, matrix);
        gradient->values[0] = cx;
        gradient->values[1] = cy;
        gradient->values[2] = cr;
        gradient->values[3] = fx;
        gradient->values[4] = fy;
        gradient->values[5] = fr;
        return &gradient->base;
    }

    plutovg_paint_t* plutovg_paint_create_texture(plutovg_surface_t* surface, plutovg_texture_type_t type, float opacity, const plutovg_matrix_t* matrix)
    {
        plutovg_texture_paint_t* texture = (plutovg_texture_paint_t*)plutovg_paint_create(PLUTOVG_PAINT_TYPE_TEXTURE, sizeof(plutovg_texture_paint_t));
        texture->type = type;
        texture->opacity = plutovg_clamp(opacity, 0.f, 1.f);
        texture->matrix = matrix ? *matrix : PLUTOVG_IDENTITY_MATRIX;
        texture->surface = plutovg_surface_reference(surface);
        return &texture->base;
    }

    plutovg_paint_t* plutovg_paint_reference(plutovg_paint_t* paint)
    {
        if (paint == NULL)
            return NULL;
        ++paint->ref_count;
        return paint;
    }

    void plutovg_paint_destroy(plutovg_paint_t* paint)
    {
        if (paint == NULL)
            return;
        if (--paint->ref_count == 0) {
            if (paint->type == PLUTOVG_PAINT_TYPE_TEXTURE) {
                plutovg_texture_paint_t* texture = (plutovg_texture_paint_t*)(paint);
                plutovg_surface_destroy(texture->surface);
            }

            free(paint);
        }
    }

    int plutovg_paint_get_reference_count(const plutovg_paint_t* paint)
    {
        if (paint)
            return paint->ref_count;
        return 0;
    }

    int plutosvg_version(void)
    {
        return PLUTOSVG_VERSION;
    }

    const char* plutosvg_version_string(void)
    {
        return PLUTOSVG_VERSION_STRING;
    }

    enum {
        TAG_UNKNOWN = 0,
        TAG_CIRCLE,
        TAG_CLIP_PATH, // TODO
        TAG_DEFS,
        TAG_ELLIPSE,
        TAG_G,
        TAG_IMAGE,
        TAG_LINE,
        TAG_LINEAR_GRADIENT,
        TAG_PATH,
        TAG_POLYGON,
        TAG_POLYLINE,
        TAG_RADIAL_GRADIENT,
        TAG_RECT,
        TAG_STOP,
        TAG_SVG,
        TAG_SYMBOL,
        TAG_USE
    };

    enum {
        ATTR_UNKNOWN = 0,
        ATTR_CLIP_PATH,
        ATTR_CLIP_PATH_UNITS,
        ATTR_CLIP_RULE,
        ATTR_COLOR,
        ATTR_CX,
        ATTR_CY,
        ATTR_D,
        ATTR_DISPLAY,
        ATTR_FILL,
        ATTR_FILL_OPACITY,
        ATTR_FILL_RULE,
        ATTR_FX,
        ATTR_FY,
        ATTR_GRADIENT_TRANSFORM,
        ATTR_GRADIENT_UNITS,
        ATTR_HEIGHT,
        ATTR_HREF,
        ATTR_ID,
        ATTR_OFFSET,
        ATTR_OPACITY,
        ATTR_POINTS,
        ATTR_PRESERVE_ASPECT_RATIO,
        ATTR_R,
        ATTR_RX,
        ATTR_RY,
        ATTR_SPREAD_METHOD,
        ATTR_STOP_COLOR,
        ATTR_STOP_OPACITY,
        ATTR_STROKE,
        ATTR_STROKE_DASHARRAY,
        ATTR_STROKE_DASHOFFSET,
        ATTR_STROKE_LINECAP,
        ATTR_STROKE_LINEJOIN,
        ATTR_STROKE_MITERLIMIT,
        ATTR_STROKE_OPACITY,
        ATTR_STROKE_WIDTH,
        ATTR_STYLE,
        ATTR_TRANSFORM,
        ATTR_VIEW_BOX,
        ATTR_VISIBILITY,
        ATTR_WIDTH,
        ATTR_X,
        ATTR_X1,
        ATTR_X2,
        ATTR_Y,
        ATTR_Y1,
        ATTR_Y2
    };

    typedef struct {
        const char* name;
        int id;
    } name_entry_t;

    static int name_entry_compare(const void* a, const void* b)
    {
        const char* name = (const char*)a;
        const name_entry_t* entry = (const name_entry_t*)b;
        return strcmp(name, entry->name);
    }


    static int lookupid(const char* data, size_t length, const name_entry_t* table, size_t count)
    {
        const int MAX_NAME = 19;
        if (length > MAX_NAME)
            return 0;
        char name[MAX_NAME + 1];
        for (int i = 0; i < length; i++)
            name[i] = data[i];
        name[length] = '\0';

        name_entry_t* entry = (name_entry_t*)bsearch(name, table, count / sizeof(name_entry_t), sizeof(name_entry_t), name_entry_compare);
        if (entry == NULL)
            return 0;
        return entry->id;
    }

    static int elementid(const char* data, size_t length)
    {
        static const name_entry_t table[] = {
            {"circle", TAG_CIRCLE},
            {"clipPath", TAG_CLIP_PATH},
            {"defs", TAG_DEFS},
            {"ellipse", TAG_ELLIPSE},
            {"g", TAG_G},
            {"image", TAG_IMAGE},
            {"line", TAG_LINE},
            {"linearGradient", TAG_LINEAR_GRADIENT},
            {"path", TAG_PATH},
            {"polygon", TAG_POLYGON},
            {"polyline", TAG_POLYLINE},
            {"radialGradient", TAG_RADIAL_GRADIENT},
            {"rect", TAG_RECT},
            {"stop", TAG_STOP},
            {"svg", TAG_SVG},
            {"symbol", TAG_SYMBOL},
            {"use", TAG_USE}
        };

        return lookupid(data, length, table, sizeof(table));
    }

    static int attributeid(const char* data, size_t length)
    {
        static const name_entry_t table[] = {
            {"clip-path", ATTR_CLIP_PATH},
            {"clip-rule", ATTR_CLIP_RULE},
            {"clipPathUnits", ATTR_CLIP_PATH_UNITS},
            {"color", ATTR_COLOR},
            {"cx", ATTR_CX},
            {"cy", ATTR_CY},
            {"d", ATTR_D},
            {"display", ATTR_DISPLAY},
            {"fill", ATTR_FILL},
            {"fill-opacity", ATTR_FILL_OPACITY},
            {"fill-rule", ATTR_FILL_RULE},
            {"fx", ATTR_FX},
            {"fy", ATTR_FY},
            {"gradientTransform", ATTR_GRADIENT_TRANSFORM},
            {"gradientUnits", ATTR_GRADIENT_UNITS},
            {"height", ATTR_HEIGHT},
            {"href", ATTR_HREF},
            {"id", ATTR_ID},
            {"offset", ATTR_OFFSET},
            {"opacity", ATTR_OPACITY},
            {"points", ATTR_POINTS},
            {"preserveAspectRatio", ATTR_PRESERVE_ASPECT_RATIO},
            {"r", ATTR_R},
            {"rx", ATTR_RX},
            {"ry", ATTR_RY},
            {"spreadMethod", ATTR_SPREAD_METHOD},
            {"stop-color", ATTR_STOP_COLOR},
            {"stop-opacity", ATTR_STOP_OPACITY},
            {"stroke", ATTR_STROKE},
            {"stroke-dasharray", ATTR_STROKE_DASHARRAY},
            {"stroke-dashoffset", ATTR_STROKE_DASHOFFSET},
            {"stroke-linecap", ATTR_STROKE_LINECAP},
            {"stroke-linejoin", ATTR_STROKE_LINEJOIN},
            {"stroke-miterlimit", ATTR_STROKE_MITERLIMIT},
            {"stroke-opacity", ATTR_STROKE_OPACITY},
            {"stroke-width", ATTR_STROKE_WIDTH},
            {"style", ATTR_STYLE},
            {"transform", ATTR_TRANSFORM},
            {"viewBox", ATTR_VIEW_BOX},
            {"visibility", ATTR_VISIBILITY},
            {"width", ATTR_WIDTH},
            {"x", ATTR_X},
            {"x1", ATTR_X1},
            {"x2", ATTR_X2},
            {"xlink:href", ATTR_HREF},
            {"y", ATTR_Y},
            {"y1", ATTR_Y1},
            {"y2", ATTR_Y2}
        };

        return lookupid(data, length, table, sizeof(table));
    }

    static int cssattributeid(const char* data, size_t length)
    {
        static const name_entry_t table[] = {
            {"clip-path", ATTR_CLIP_PATH},
            {"clip-rule", ATTR_CLIP_RULE},
            {"color", ATTR_COLOR},
            {"display", ATTR_DISPLAY},
            {"fill", ATTR_FILL},
            {"fill-opacity", ATTR_FILL_OPACITY},
            {"fill-rule", ATTR_FILL_RULE},
            {"opacity", ATTR_OPACITY},
            {"stop-color", ATTR_STOP_COLOR},
            {"stop-opacity", ATTR_STOP_OPACITY},
            {"stroke", ATTR_STROKE},
            {"stroke-dasharray", ATTR_STROKE_DASHARRAY},
            {"stroke-dashoffset", ATTR_STROKE_DASHOFFSET},
            {"stroke-linecap", ATTR_STROKE_LINECAP},
            {"stroke-linejoin", ATTR_STROKE_LINEJOIN},
            {"stroke-miterlimit", ATTR_STROKE_MITERLIMIT},
            {"stroke-opacity", ATTR_STROKE_OPACITY},
            {"stroke-width", ATTR_STROKE_WIDTH},
            {"visibility", ATTR_VISIBILITY}
        };

        return lookupid(data, length, table, sizeof(table));
    }

    typedef struct {
        const char* data;
        size_t length;
    } string_t;

    typedef struct attribute {
        int id;
        string_t value;
        struct attribute* next;
    } attribute_t;

    typedef struct element {
        int id;
        struct element* parent;
        struct element* last_child;
        struct element* first_child;
        struct element* next_sibling;
        struct attribute* attributes;
    } element_t;

    typedef struct heap_chunk {
        struct heap_chunk* next;
    } heap_chunk_t;

    typedef struct {
        heap_chunk_t* chunk;
        size_t size;
    } heap_t;

    static heap_t* heap_create(void)
    {
        heap_t* heap = (heap_t*)malloc(sizeof(heap_t));
        heap->chunk = NULL;
        heap->size = 0;
        return heap;
    }

#define CHUNK_SIZE 4096
#define ALIGN_SIZE(size) (((size) + 7ul) & ~7ul)
    static void* heap_alloc(heap_t* heap, size_t size)
    {
        size = ALIGN_SIZE(size);
        if (heap->chunk == NULL || heap->size + size > CHUNK_SIZE) {
            heap_chunk_t* chunk = (heap_chunk_t*)malloc(CHUNK_SIZE + sizeof(heap_chunk_t));
            chunk->next = heap->chunk;
            heap->chunk = chunk;
            heap->size = 0;
        }

        void* data = (char*)(heap->chunk) + sizeof(heap_chunk_t) + heap->size;
        heap->size += size;
        return data;
    }

    static void heap_destroy(heap_t* heap)
    {
        while (heap->chunk) {
            heap_chunk_t* chunk = heap->chunk;
            heap->chunk = chunk->next;
            free(chunk);
        }

        free(heap);
    }

    typedef struct hashmap_entry {
        size_t hash;
        string_t name;
        void* value;
        struct hashmap_entry* next;
    } hashmap_entry_t;

    typedef struct {
        hashmap_entry_t** buckets;
        size_t size;
        size_t capacity;
    } hashmap_t;

    static hashmap_t* hashmap_create(void)
    {
        hashmap_t* map = (hashmap_t*)malloc(sizeof(hashmap_t));
        map->buckets = (hashmap_entry_t**)calloc(16, sizeof(hashmap_entry_t*));
        map->size = 0;
        map->capacity = 16;
        return map;
    }

    static size_t hashmap_hash(const char* data, size_t length)
    {
        size_t h = length;
        for (size_t i = 0; i < length; i++) {
            h = h * 31 + *data;
            ++data;
        }

        return h;
    }

    static bool hashmap_eq(const hashmap_entry_t* entry, const char* data, size_t length)
    {
        const string_t* name = &entry->name;
        if (name->length != length)
            return false;
        for (size_t i = 0; i < length; i++) {
            if (data[i] != name->data[i]) {
                return false;
            }
        }

        return true;
    }

    static void hashmap_expand(hashmap_t* map)
    {
        if (map->size > (map->capacity * 3 / 4)) {
            size_t newcapacity = map->capacity << 1;
            hashmap_entry_t** newbuckets = (hashmap_entry_t**)calloc(newcapacity, sizeof(hashmap_entry_t*));
            for (size_t i = 0; i < map->capacity; i++) {
                hashmap_entry_t* entry = map->buckets[i];
                while (entry) {
                    hashmap_entry_t* next = entry->next;
                    size_t index = entry->hash & (newcapacity - 1);
                    entry->next = newbuckets[index];
                    newbuckets[index] = entry;
                    entry = next;
                }
            }

            free(map->buckets);
            map->buckets = newbuckets;
            map->capacity = newcapacity;
        }
    }

    static void hashmap_put(hashmap_t* map, heap_t* heap, const char* data, size_t length, void* value)
    {
        size_t hash = hashmap_hash(data, length);
        size_t index = hash & (map->capacity - 1);

        hashmap_entry_t** p = &map->buckets[index];
        while (true) {
            hashmap_entry_t* current = *p;
            if (current == NULL) {
                hashmap_entry_t* entry = (hashmap_entry_t*)heap_alloc(heap, sizeof(hashmap_entry_t));
                entry->name.data = data;
                entry->name.length = length;
                entry->hash = hash;
                entry->value = value;
                entry->next = NULL;
                *p = entry;
                map->size += 1;
                hashmap_expand(map);
                break;
            }

            if (current->hash == hash && hashmap_eq(current, data, length)) {
                current->value = value;
                break;
            }

            p = &current->next;
        }
    }

    static void* hashmap_get(const hashmap_t* map, const char* data, size_t length)
    {
        size_t hash = hashmap_hash(data, length);
        size_t index = hash & (map->capacity - 1);

        hashmap_entry_t* entry = map->buckets[index];
        while (entry) {
            if (entry->hash == hash && hashmap_eq(entry, data, length))
                return entry->value;
            entry = entry->next;
        }

        return NULL;
    }

    static void hashmap_destroy(hashmap_t* map)
    {
        if (map == NULL)
            return;
        free(map->buckets);
        free(map);
    }

    static inline const string_t* find_attribute(const element_t* element, int id, bool inherit)
    {
        do {
            const attribute_t* attribute = element->attributes;
            while (attribute != NULL) {
                if (attribute->id == id) {
                    const string_t* value = &attribute->value;
                    if (inherit && value->length == 7 && strncmp(value->data, "inherit", 7) == 0)
                        break;
                    return value;
                }

                attribute = attribute->next;
            }

            element = element->parent;
        } while (inherit && element);
        return NULL;
    }

    static inline bool has_attribute(const element_t* element, int id)
    {
        const attribute_t* attribute = element->attributes;
        while (attribute != NULL) {
            if (attribute->id == id)
                return true;
            attribute = attribute->next;
        }

        return false;
    }

#define IS_NUM(c) ((c) >= '0' && (c) <= '9')
    static inline bool parse_float(const char** begin, const char* end, float* number)
    {
        const char* it = *begin;
        float integer = 0;
        float fraction = 0;
        float exponent = 0;
        int sign = 1;
        int expsign = 1;

        if (it < end && *it == '+')
            ++it;
        else if (it < end && *it == '-') {
            ++it;
            sign = -1;
        }

        if (it >= end || (*it != '.' && !IS_NUM(*it)))
            return false;
        if (IS_NUM(*it)) {
            do {
                integer = 10.f * integer + (*it++ - '0');
            } while (it < end && IS_NUM(*it));
        }

        if (it < end && *it == '.') {
            ++it;
            if (it >= end || !IS_NUM(*it))
                return false;
            float divisor = 1.f;
            do {
                fraction = 10.f * fraction + (*it++ - '0');
                divisor *= 10.f;
            } while (it < end && IS_NUM(*it));
            fraction /= divisor;
        }

        if (it + 1 < end && (it[0] == 'e' || it[0] == 'E') && (it[1] != 'x' && it[1] != 'm')) {
            ++it;
            if (it < end && *it == '+')
                ++it;
            else if (it < end && *it == '-') {
                ++it;
                expsign = -1;
            }

            if (it >= end || !IS_NUM(*it))
                return false;
            do {
                exponent = 10 * exponent + (*it++ - '0');
            } while (it < end && IS_NUM(*it));
        }

        *begin = it;
        *number = sign * (integer + fraction);
        if (exponent)
            *number *= powf(10.f, expsign * exponent);
        return *number >= -FLT_MAX && *number <= FLT_MAX;
    }

    static inline bool skip_string(const char** begin, const char* end, const char* data)
    {
        const char* it = *begin;
        while (it < end && *data && *it == *data) {
            ++data;
            ++it;
        }

        if (*data == '\0') {
            *begin = it;
            return true;
        }

        return false;
    }

    static inline const char* string_find(const char* it, const char* end, const char* data)
    {
        while (it < end) {
            const char* begin = it;
            if (skip_string(&it, end, data))
                return begin;
            ++it;
        }

        return NULL;
    }

    static inline bool skip_delim(const char** begin, const char* end, const char delim)
    {
        const char* it = *begin;
        if (it < end && *it == delim) {
            *begin = it + 1;
            return true;
        }

        return false;
    }

#define IS_WS(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
    static inline bool skip_ws(const char** begin, const char* end)
    {
        const char* it = *begin;
        while (it < end && IS_WS(*it))
            ++it;
        *begin = it;
        return it < end;
    }

    static inline bool skip_ws_delim(const char** begin, const char* end, char delim)
    {
        const char* it = *begin;
        if (it < end && !IS_WS(*it) && *it != delim)
            return false;
        if (skip_ws(&it, end)) {
            if (skip_delim(&it, end, delim)) {
                skip_ws(&it, end);
            }
        }

        *begin = it;
        return it < end;
    }

    static inline bool skip_ws_comma(const char** begin, const char* end)
    {
        return skip_ws_delim(begin, end, ',');
    }

    static inline const char* rtrim(const char* begin, const char* end)
    {
        while (end > begin && IS_WS(end[-1]))
            --end;
        return end;
    }

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (hi) < (v) ? (hi) : (v))
    static bool parse_number(const element_t* element, int id, float* number, bool percent, bool inherit)
    {
        const string_t* value = find_attribute(element, id, inherit);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (!parse_float(&it, end, number))
            return false;
        if (percent) {
            if (skip_delim(&it, end, '%'))
                *number /= 100.f;
            *number = CLAMP(*number, 0.f, 1.f);
        }

        return true;
    }

    typedef enum {
        length_type_unknown,
        length_type_fixed,
        length_type_percent
    } length_type_t;

    typedef struct {
        float value;
        length_type_t type;
    } length_t;

#define is_length_zero(length) ((length).value == 0)
#define is_length_valid(length) ((length).type != length_type_unknown)
    static bool parse_length_value(const char** begin, const char* end, length_t* length, bool negative)
    {
        float value = 0;
        const char* it = *begin;
        if (!parse_float(&it, end, &value))
            return false;
        if (!negative && value < 0.f) {
            return false;
        }

        char units[2] = { 0, 0 };
        if (it + 0 < end)
            units[0] = it[0];
        if (it + 1 < end) {
            units[1] = it[1];
        }

        static const float dpi = 96.f;
        switch (units[0]) {
        case '%':
            length->value = value;
            length->type = length_type_percent;
            it += 1;
            break;
        case 'p':
            if (units[1] == 'x')
                length->value = value;
            else if (units[1] == 'c')
                length->value = value * dpi / 6.f;
            else if (units[1] == 't')
                length->value = value * dpi / 72.f;
            else
                return false;
            length->type = length_type_fixed;
            it += 2;
            break;
        case 'i':
            if (units[1] == 'n')
                length->value = value * dpi;
            else
                return false;
            length->type = length_type_fixed;
            it += 2;
            break;
        case 'c':
            if (units[1] == 'm')
                length->value = value * dpi / 2.54f;
            else
                return false;
            length->type = length_type_fixed;
            it += 2;
            break;
        case 'm':
            if (units[1] == 'm')
                length->value = value * dpi / 25.4f;
            else
                return false;
            length->type = length_type_fixed;
            it += 2;
            break;
        default:
            length->value = value;
            length->type = length_type_fixed;
            break;
        }

        *begin = it;
        return true;
    }

    static bool parse_length(const element_t* element, int id, length_t* length, bool negative, bool inherit)
    {
        const string_t* value = find_attribute(element, id, inherit);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (parse_length_value(&it, end, length, negative))
            return it == end;
        return false;
    }

    static inline float convert_length(const length_t* length, float maximum)
    {
        if (length->type == length_type_percent)
            return length->value * maximum / 100.f;
        return length->value;
    }

    typedef enum {
        color_type_fixed,
        color_type_current
    } color_type_t;

    typedef struct {
        color_type_t type;
        uint32_t value;
    } color_t;

    typedef enum {
        paint_type_none,
        paint_type_color,
        paint_type_url,
        paint_type_var
    } paint_type_t;

    typedef struct {
        paint_type_t type;
        color_t color;
        string_t id;
    } paint_t;

    static bool parse_color_value(const char** begin, const char* end, color_t* color)
    {
        const char* it = *begin;
        if (skip_string(&it, end, "currentColor")) {
            color->type = color_type_current;
            color->value = 0xFF000000;
        }
        else {
            plutovg_color_t value;
            int length = plutovg_color_parse(&value, it, (int)(end - it));
            if (length == 0)
                return false;
            color->type = color_type_fixed;
            color->value = plutovg_color_to_argb32(&value);
            it += length;
        }

        *begin = it;
        skip_ws(begin, end);
        return true;
    }

    static bool parse_color(const element_t* element, int id, color_t* color, bool inherit)
    {
        const string_t* value = find_attribute(element, id, inherit);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (parse_color_value(&it, end, color))
            return it == end;
        return false;
    }

    static bool parse_url_value(const char** begin, const char* end, string_t* id)
    {
        const char* it = *begin;
        if (!skip_string(&it, end, "url")
            || !skip_ws(&it, end)
            || !skip_delim(&it, end, '(')
            || !skip_ws(&it, end)) {
            return false;
        }

        if (!skip_delim(&it, end, '#'))
            return false;
        id->data = it;
        id->length = 0;
        while (it < end && *it != ')') {
            ++id->length;
            ++it;
        }

        if (!skip_delim(&it, end, ')'))
            return false;
        *begin = it;
        skip_ws(begin, end);
        return true;
    }

#define IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_STARTNAMECHAR(c) (IS_ALPHA(c) ||  (c) == '_' || (c) == ':')
#define IS_NAMECHAR(c) (IS_STARTNAMECHAR(c) || IS_NUM(c) || (c) == '-' || (c) == '.')
    static bool parse_paint(const element_t* element, int id, paint_t* paint)
    {
        const string_t* value = find_attribute(element, id, true);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "none")) {
            paint->type = paint_type_none;
            return !skip_ws(&it, end);
        }

        if (parse_url_value(&it, end, &paint->id)) {
            paint->type = paint_type_url;
            paint->color.value = 0x00000000;
            if (skip_ws(&it, end)) {
                if (!parse_color_value(&it, end, &paint->color)) {
                    return false;
                }
            }

            return it == end;
        }

        if (skip_string(&it, end, "var")) {
            if (!skip_ws(&it, end)
                || !skip_delim(&it, end, '(')
                || !skip_ws(&it, end)) {
                return false;
            }

            if (!skip_string(&it, end, "--"))
                return false;
            const char* begin = it;
            while (it < end && IS_NAMECHAR(*it))
                ++it;
            paint->type = paint_type_var;
            paint->id.data = begin;
            paint->id.length = it - begin;
            paint->color.value = 0x00000000;
            skip_ws(&it, end);
            if (skip_delim(&it, end, ',')) {
                skip_ws(&it, end);
                if (!parse_color_value(&it, end, &paint->color)) {
                    return false;
                }
            }

            return skip_delim(&it, end, ')') && !skip_ws(&it, end);
        }

        if (parse_color_value(&it, end, &paint->color)) {
            paint->type = paint_type_color;
            return it == end;
        }

        return false;
    }

    static bool parse_view_box(const element_t* element, int id, plutovg_rect_t* view_box)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;

        float x, y, w, h;
        if (!parse_float(&it, end, &x)
            || !skip_ws_comma(&it, end)
            || !parse_float(&it, end, &y)
            || !skip_ws_comma(&it, end)
            || !parse_float(&it, end, &w)
            || !skip_ws_comma(&it, end)
            || !parse_float(&it, end, &h)
            || skip_ws(&it, end)) {
            return false;
        }

        if (w <= 0.f || h <= 0.f)
            return false;
        view_box->x = x;
        view_box->y = y;
        view_box->w = w;
        view_box->h = h;
        return true;
    }

    typedef enum {
        view_align_none,
        view_align_x_min_y_min,
        view_align_x_mid_y_min,
        view_align_x_max_y_min,
        view_align_x_min_y_mid,
        view_align_x_mid_y_mid,
        view_align_x_max_y_mid,
        view_align_x_min_y_max,
        view_align_x_mid_y_max,
        view_align_x_max_y_max
    } view_align_t;

    typedef enum {
        view_scale_meet,
        view_scale_slice
    } view_scale_t;

    typedef struct {
        view_align_t align;
        view_scale_t scale;
    } view_position_t;


    static bool parse_view_position(const element_t* element, int id, view_position_t* position)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "none"))
            position->align = view_align_none;
        else if (skip_string(&it, end, "xMinYMin"))
            position->align = view_align_x_min_y_min;
        else if (skip_string(&it, end, "xMidYMin"))
            position->align = view_align_x_mid_y_min;
        else if (skip_string(&it, end, "xMaxYMin"))
            position->align = view_align_x_max_y_min;
        else if (skip_string(&it, end, "xMinYMid"))
            position->align = view_align_x_min_y_mid;
        else if (skip_string(&it, end, "xMidYMid"))
            position->align = view_align_x_mid_y_mid;
        else if (skip_string(&it, end, "xMaxYMid"))
            position->align = view_align_x_max_y_mid;
        else if (skip_string(&it, end, "xMinYMax"))
            position->align = view_align_x_min_y_max;
        else if (skip_string(&it, end, "xMidYMax"))
            position->align = view_align_x_mid_y_max;
        else if (skip_string(&it, end, "xMaxYMax"))
            position->align = view_align_x_max_y_max;
        else
            return false;
        position->scale = view_scale_meet;
        if (position->align != view_align_none) {
            skip_ws(&it, end);
            if (skip_string(&it, end, "meet"))
                position->scale = view_scale_meet;
            else if (skip_string(&it, end, "slice")) {
                position->scale = view_scale_slice;
            }
        }

        return !skip_ws(&it, end);
    }

    static bool parse_transform(const element_t* element, int id, plutovg_matrix_t* matrix)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        return plutovg_matrix_parse(matrix, value->data, (int)value->length);
    }

    static bool parse_points(const element_t* element, int id, plutovg_path_t* path)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;

        bool requires_move = true;
        while (it < end) {
            float x, y;
            if (!parse_float(&it, end, &x)
                || !skip_ws_comma(&it, end)
                || !parse_float(&it, end, &y)) {
                return false;
            }

            skip_ws_comma(&it, end);
            if (requires_move)
                plutovg_path_move_to(path, x, y);
            else
                plutovg_path_line_to(path, x, y);
            requires_move = false;
        }

        if (element->id == TAG_POLYGON)
            plutovg_path_close(path);
        return true;
    }

    static bool parse_path(const element_t* element, int id, plutovg_path_t* path)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        return plutovg_path_parse(path, value->data, (int)value->length);
    }

#define MAX_DASHES 128
    typedef struct {
        length_t data[MAX_DASHES];
        size_t size;
    } stroke_dash_array_t;

    static bool parse_dash_array(const element_t* element, int id, stroke_dash_array_t* dash_array)
    {
        const string_t* value = find_attribute(element, id, true);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        while (it < end && dash_array->size < MAX_DASHES) {
            if (!parse_length_value(&it, end, dash_array->data + dash_array->size, false))
                return false;
            skip_ws_comma(&it, end);
            dash_array->size += 1;
        }

        return true;
    }

    static bool parse_line_cap(const element_t* element, int id, plutovg_line_cap_t* line_cap)
    {
        const string_t* value = find_attribute(element, id, true);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "butt"))
            *line_cap = PLUTOVG_LINE_CAP_BUTT;
        else if (skip_string(&it, end, "round"))
            *line_cap = PLUTOVG_LINE_CAP_ROUND;
        else if (skip_string(&it, end, "square"))
            *line_cap = PLUTOVG_LINE_CAP_SQUARE;
        return !skip_ws(&it, end);
    }

    static bool parse_line_join(const element_t* element, int id, plutovg_line_join_t* line_join)
    {
        const string_t* value = find_attribute(element, id, true);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "miter"))
            *line_join = PLUTOVG_LINE_JOIN_MITER;
        else if (skip_string(&it, end, "round"))
            *line_join = PLUTOVG_LINE_JOIN_ROUND;
        else if (skip_string(&it, end, "bevel"))
            *line_join = PLUTOVG_LINE_JOIN_BEVEL;
        return !skip_ws(&it, end);
    }

    static bool parse_fill_rule(const element_t* element, int id, plutovg_fill_rule_t* fill_rule)
    {
        const string_t* value = find_attribute(element, id, true);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "nonzero"))
            *fill_rule = PLUTOVG_FILL_RULE_NON_ZERO;
        else if (skip_string(&it, end, "evenodd"))
            *fill_rule = PLUTOVG_FILL_RULE_EVEN_ODD;
        return !skip_ws(&it, end);
    }

    static bool parse_spread_method(const element_t* element, int id, plutovg_spread_method_t* spread_method)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "pad"))
            *spread_method = PLUTOVG_SPREAD_METHOD_PAD;
        else if (skip_string(&it, end, "reflect"))
            *spread_method = PLUTOVG_SPREAD_METHOD_REFLECT;
        else if (skip_string(&it, end, "repeat"))
            *spread_method = PLUTOVG_SPREAD_METHOD_REPEAT;
        return !skip_ws(&it, end);
    }

    typedef enum {
        display_inline,
        display_none
    } display_t;

    static bool parse_display(const element_t* element, int id, display_t* display)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "inline"))
            *display = display_inline;
        else if (skip_string(&it, end, "none"))
            *display = display_none;
        return !skip_ws(&it, end);
    }

    typedef enum {
        visibility_visible,
        visibility_hidden,
        visibility_collapse
    } visibility_t;

    static bool parse_visibility(const element_t* element, int id, visibility_t* visibility)
    {
        const string_t* value = find_attribute(element, id, true);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "visible"))
            *visibility = visibility_visible;
        else if (skip_string(&it, end, "hidden"))
            *visibility = visibility_hidden;
        else if (skip_string(&it, end, "collapse"))
            *visibility = visibility_collapse;
        return !skip_ws(&it, end);
    }

    typedef enum {
        units_type_object_bounding_box,
        units_type_user_space_on_use
    } units_type_t;

    static bool parse_units_type(const element_t* element, int id, units_type_t* units_type)
    {
        const string_t* value = find_attribute(element, id, false);
        if (value == NULL)
            return false;
        const char* it = value->data;
        const char* end = it + value->length;
        if (skip_string(&it, end, "objectBoundingBox"))
            *units_type = units_type_object_bounding_box;
        else if (skip_string(&it, end, "userSpaceOnUse"))
            *units_type = units_type_user_space_on_use;
        return !skip_ws(&it, end);
    }

    struct plutosvg_document {
        heap_t* heap;
        plutovg_path_t* path;
        hashmap_t* id_cache;
        element_t* root_element;
        plutovg_destroy_func_t destroy_func;
        void* closure;
        float width;
        float height;
    };

    static plutosvg_document_t* plutosvg_document_create(float width, float height, plutovg_destroy_func_t destroy_func, void* closure)
    {
        plutosvg_document_t* document = (plutosvg_document_t*)malloc(sizeof(plutosvg_document_t));
        document->heap = heap_create();
        document->path = plutovg_path_create();
        document->id_cache = NULL;
        document->root_element = NULL;
        document->destroy_func = destroy_func;
        document->closure = closure;
        document->width = width;
        document->height = height;
        return document;
    }

    void plutosvg_document_destroy(plutosvg_document_t* document)
    {
        if (document == NULL)
            return;
        plutovg_path_destroy(document->path);
        hashmap_destroy(document->id_cache);
        heap_destroy(document->heap);
        if (document->destroy_func)
            document->destroy_func(document->closure);
        free(document);
    }

    static void add_attribute(element_t* element, plutosvg_document_t* document, int id, const char* data, size_t length)
    {
        attribute_t* attribute = (attribute_t*)heap_alloc(document->heap, sizeof(attribute_t));
        attribute->id = id;
        attribute->value.data = data;
        attribute->value.length = length;
        attribute->next = element->attributes;
        element->attributes = attribute;
    }

#define IS_CSS_STARTNAMECHAR(c) (IS_ALPHA(c) || c == '_')
#define IS_CSS_NAMECHAR(c) (IS_CSS_STARTNAMECHAR(c) || IS_NUM(c) || c == '-')
    static void parse_style(const char* data, int length, element_t* element, plutosvg_document_t* document)
    {
        const char* it = data;
        const char* end = it + length;
        while (it < end && IS_CSS_STARTNAMECHAR(*it)) {
            data = it++;
            while (it < end && IS_CSS_NAMECHAR(*it))
                ++it;
            int id = cssattributeid(data, it - data);
            skip_ws(&it, end);
            if (it >= end || *it != ':')
                return;
            ++it;
            skip_ws(&it, end);
            data = it;
            while (it < end && *it != ';')
                ++it;
            length = (int)(rtrim(data, it) - data);
            if (id && element)
                add_attribute(element, document, id, data, length);
            skip_ws_delim(&it, end, ';');
        }
    }

    static bool parse_attributes(const char** begin, const char* end, element_t* element, plutosvg_document_t* document)
    {
        const char* it = *begin;
        while (it < end && IS_STARTNAMECHAR(*it)) {
            const char* data = it++;
            while (it < end && IS_NAMECHAR(*it))
                ++it;
            int id = attributeid(data, it - data);
            skip_ws(&it, end);
            if (it >= end || *it != '=')
                return false;
            ++it;
            skip_ws(&it, end);
            if (it >= end || (*it != '"' && *it != '\''))
                return false;
            const char quote = *it++;
            skip_ws(&it, end);
            data = it;
            while (it < end && *it != quote)
                ++it;
            if (it >= end || *it != quote)
                return false;
            int length = (int)(rtrim(data, it) - data);
            if (id && element) {
                if (id == ATTR_ID) {
                    if (document->id_cache == NULL)
                        document->id_cache = hashmap_create();
                    hashmap_put(document->id_cache, document->heap, data, length, element);
                }
                else if (id == ATTR_STYLE) {
                    parse_style(data, length, element, document);
                }
                else {
                    add_attribute(element, document, id, data, length);
                }
            }

            ++it;
            skip_ws(&it, end);
        }

        *begin = it;
        return true;
    }

    plutosvg_document_t* plutosvg_document_load_from_data(const char* data, int length, float width, float height, plutovg_destroy_func_t destroy_func, void* closure)
    {
        if (length == -1)
            length = (int)strlen(data);
        const char* it = data;
        const char* end = it + length;

        plutosvg_document_t* document = plutosvg_document_create(width, height, destroy_func, closure);
        element_t* current = NULL;
        int ignoring = 0;
        while (it < end) {
            if (current == NULL) {
                while (it < end && IS_WS(*it))
                    ++it;
                if (it >= end) {
                    break;
                }
            }
            else {
                while (it < end && *it != '<') {
                    ++it;
                }
            }

            if (it >= end || *it != '<')
                goto error;
            ++it;
            if (it < end && *it == '?') {
                ++it;
                if (!skip_string(&it, end, "xml"))
                    goto error;
                skip_ws(&it, end);
                if (!parse_attributes(&it, end, NULL, NULL))
                    goto error;
                if (!skip_string(&it, end, "?>"))
                    goto error;
                skip_ws(&it, end);
                continue;
            }

            if (it < end && *it == '!') {
                ++it;
                if (skip_string(&it, end, "--")) {
                    const char* begin = string_find(it, end, "-->");
                    if (begin == NULL)
                        goto error;
                    it = begin + 3;
                    skip_ws(&it, end);
                    continue;
                }

                if (skip_string(&it, end, "[CDATA[")) {
                    const char* begin = string_find(it, end, "]]>");
                    if (begin == NULL)
                        goto error;
                    it = begin + 3;
                    skip_ws(&it, end);
                    continue;
                }

                if (skip_string(&it, end, "DOCTYPE")) {
                    while (it < end && *it != '>') {
                        if (*it == '[') {
                            ++it;
                            int depth = 1;
                            while (it < end && depth > 0) {
                                if (*it == '[') ++depth;
                                else if (*it == ']') --depth;
                                ++it;
                            }
                        }
                        else {
                            ++it;
                        }
                    }

                    if (!skip_delim(&it, end, '>'))
                        goto error;
                    skip_ws(&it, end);
                    continue;
                }

                goto error;
            }

            if (it < end && *it == '/') {
                if (current == NULL && ignoring == 0)
                    goto error;
                ++it;
                if (it >= end || !IS_STARTNAMECHAR(*it))
                    goto error;
                const char* begin = it++;
                while (it < end && IS_NAMECHAR(*it))
                    ++it;
                skip_ws(&it, end);
                if (it >= end || *it != '>')
                    goto error;
                if (ignoring == 0) {
                    int id = elementid(begin, it - begin);
                    if (id != current->id)
                        goto error;
                    current = current->parent;
                }
                else {
                    --ignoring;
                }

                ++it;
                continue;
            }

            if (it >= end || !IS_STARTNAMECHAR(*it))
                goto error;
            const char* begin = it++;
            while (it < end && IS_NAMECHAR(*it))
                ++it;
            element_t* element = NULL;
            if (ignoring > 0) {
                ++ignoring;
            }
            else {
                int id = elementid(begin, it - begin);
                if (id == TAG_UNKNOWN) {
                    ignoring = 1;
                }
                else {
                    if (document->root_element && current == NULL)
                        goto error;
                    element = (element_t*)heap_alloc(document->heap, sizeof(element_t));
                    element->id = id;
                    element->parent = NULL;
                    element->next_sibling = NULL;
                    element->first_child = NULL;
                    element->last_child = NULL;
                    element->attributes = NULL;
                    if (document->root_element == NULL) {
                        if (element->id != TAG_SVG)
                            goto error;
                        document->root_element = element;
                    }
                    else {
                        element->parent = current;
                        if (current->last_child) {
                            current->last_child->next_sibling = element;
                            current->last_child = element;
                        }
                        else {
                            current->last_child = element;
                            current->first_child = element;
                        }
                    }
                }
            }

            skip_ws(&it, end);
            if (!parse_attributes(&it, end, element, document))
                goto error;
            if (it < end && *it == '>') {
                if (element)
                    current = element;
                ++it;
                continue;
            }

            if (it < end && *it == '/') {
                ++it;
                if (it >= end || *it != '>')
                    goto error;
                if (ignoring > 0)
                    --ignoring;
                ++it;
                continue;
            }

            goto error;
        }

        if (it == end && ignoring == 0 && current == NULL && document->root_element) {
            length_t w = { 100, length_type_percent };
            length_t h = { 100, length_type_percent };

            parse_length(document->root_element, ATTR_WIDTH, &w, false, false);
            parse_length(document->root_element, ATTR_HEIGHT, &h, false, false);

            float intrinsic_width = convert_length(&w, width);
            float intrinsic_height = convert_length(&h, height);
            if (intrinsic_width <= 0.f || intrinsic_height <= 0.f) {
                plutovg_rect_t view_box = { 0, 0, 0, 0 };
                if (parse_view_box(document->root_element, ATTR_VIEW_BOX, &view_box)) {
                    float intrinsic_ratio = view_box.w / view_box.h;
                    if (intrinsic_width <= 0.f && intrinsic_height > 0.f) {
                        intrinsic_width = intrinsic_height * intrinsic_ratio;
                    }
                    else if (intrinsic_width > 0.f && intrinsic_height <= 0.f) {
                        intrinsic_height = intrinsic_width / intrinsic_ratio;
                    }
                    else {
                        intrinsic_width = view_box.w;
                        intrinsic_height = view_box.h;
                    }
                }
                else {
                    if (intrinsic_width == -1)
                        intrinsic_width = 300;
                    if (intrinsic_height == -1) {
                        intrinsic_height = 150;
                    }
                }
            }

            if (intrinsic_width <= 0.f || intrinsic_height <= 0.f)
                goto error;
            document->width = intrinsic_width;
            document->height = intrinsic_height;
            return document;
        }

    error:
        plutosvg_document_destroy(document);
        return NULL;
    }

    plutosvg_document_t* plutosvg_document_load_from_file(const char* filename, float width, float height)
    {
        FILE* fp = 0;
        fopen_s(&fp, filename, "rb");
        if (fp == NULL) {
            return NULL;
        }

        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        if (length == -1L) {
            fclose(fp);
            return NULL;
        }

        void* data = malloc(length);
        if (data == NULL) {
            fclose(fp);
            return NULL;
        }

        fseek(fp, 0, SEEK_SET);
        size_t nread = fread(data, 1, length, fp);
        fclose(fp);

        if (nread != length) {
            free(data);
            return NULL;
        }

        return plutosvg_document_load_from_data((const char*)data, length, width, height, free, data);
    }

    typedef enum render_mode {
        render_mode_painting,
        render_mode_clipping,
        render_mode_bounding
    } render_mode_t;

    typedef struct render_state {
        struct render_state* parent;
        const element_t* element;
        render_mode_t mode;
        float opacity;

        float view_width;
        float view_height;

        plutovg_matrix_t matrix;
        plutovg_rect_t extents;
    } render_state_t;

#define INVALID_RECT PLUTOVG_MAKE_RECT(0, 0, -1, -1)
#define EMPTY_RECT PLUTOVG_MAKE_RECT(0, 0, 0, 0)

#define IS_INVALID_RECT(rect) ((rect).w < 0 || (rect).h < 0)
#define IS_EMPTY_RECT(rect) ((rect).w <= 0 || (rect).h <= 0)
    static void render_state_begin(const element_t* element, render_state_t* state, render_state_t* parent)
    {
        state->parent = parent;
        state->element = element;
        state->mode = parent->mode;
        state->opacity = parent->opacity;
        state->matrix = parent->matrix;
        state->extents = INVALID_RECT;

        state->view_width = parent->view_width;
        state->view_height = parent->view_height;

        if (element->parent && parse_transform(element, ATTR_TRANSFORM, &state->matrix))
            plutovg_matrix_multiply(&state->matrix, &state->matrix, &parent->matrix);
        if (state->mode == render_mode_painting) {
            if (parse_number(element, ATTR_OPACITY, &state->opacity, true, false)) {
                state->opacity *= parent->opacity;
            }
        }
    }


    static void render_state_end(render_state_t* state)
    {
        if (state->mode == render_mode_painting)
            return;
        if (IS_INVALID_RECT(state->extents)) {
            return;
        }

        plutovg_matrix_t matrix;
        plutovg_matrix_invert(&state->parent->matrix, &matrix);
        plutovg_matrix_multiply(&matrix, &state->matrix, &matrix);

        plutovg_rect_t extents;
        plutovg_matrix_map_rect(&matrix, &state->extents, &extents);
        if (IS_INVALID_RECT(state->parent->extents)) {
            state->parent->extents = extents;
            return;
        }

        float l = MIN(state->parent->extents.x, extents.x);
        float t = MIN(state->parent->extents.y, extents.y);
        float r = MAX(state->parent->extents.x + state->parent->extents.w, extents.x + extents.w);
        float b = MAX(state->parent->extents.y + state->parent->extents.h, extents.y + extents.h);

        state->parent->extents.x = l;
        state->parent->extents.y = t;
        state->parent->extents.w = r - l;
        state->parent->extents.h = b - t;
    }

    static bool has_cycle_reference(const render_state_t* state, const element_t* element)
    {
        do {
            if (element == state->element)
                return true;
            state = state->parent;
        } while (state);
        return false;
    }

    int plutovg_version(void)
    {
        return PLUTOVG_VERSION;
    }

    const char* plutovg_version_string(void)
    {
        return PLUTOVG_VERSION_STRING;
    }

#define PLUTOVG_DEFAULT_STROKE_STYLE plutovg_stroke_style_t{1.f, PLUTOVG_LINE_CAP_BUTT, PLUTOVG_LINE_JOIN_MITER, 10.f}

    static plutovg_state_t* plutovg_state_create(void)
    {
        plutovg_state_t* state = (plutovg_state_t*)malloc(sizeof(plutovg_state_t));
        state->paint = NULL;
        state->color = PLUTOVG_BLACK_COLOR;
        state->matrix = PLUTOVG_IDENTITY_MATRIX;
        state->stroke.style = PLUTOVG_DEFAULT_STROKE_STYLE;
        state->stroke.dash.offset = 0.f;
        plutovg_array_init(state->stroke.dash.array);
        plutovg_span_buffer_init(&state->clip_spans);
        state->winding = PLUTOVG_FILL_RULE_NON_ZERO;
        state->op = PLUTOVG_OPERATOR_SRC_OVER;
        state->opacity = 1.f;
        state->clipping = false;
        state->next = NULL;
        return state;
    }

    static void plutovg_state_reset(plutovg_state_t* state)
    {
        plutovg_paint_destroy(state->paint);
        state->paint = NULL;
        state->color = PLUTOVG_BLACK_COLOR;
        state->matrix = PLUTOVG_IDENTITY_MATRIX;
        state->stroke.style = PLUTOVG_DEFAULT_STROKE_STYLE;
        state->stroke.dash.offset = 0.f;
        plutovg_array_clear(state->stroke.dash.array);
        plutovg_span_buffer_reset(&state->clip_spans);
        state->winding = PLUTOVG_FILL_RULE_NON_ZERO;
        state->op = PLUTOVG_OPERATOR_SRC_OVER;
        state->opacity = 1.f;
        state->clipping = false;
    }


    static void plutovg_state_copy(plutovg_state_t* state, const plutovg_state_t* source)
    {
        state->paint = plutovg_paint_reference(source->paint);
        state->color = source->color;
        state->matrix = source->matrix;
        state->stroke.style = source->stroke.style;
        state->stroke.dash.offset = source->stroke.dash.offset;
        plutovg_array_clear(state->stroke.dash.array);
        plutovg_array_append(state->stroke.dash.array, source->stroke.dash.array);
        plutovg_span_buffer_copy(&state->clip_spans, &source->clip_spans);
        state->winding = source->winding;
        state->op = source->op;
        state->opacity = source->opacity;
        state->clipping = source->clipping;
    }

    static void plutovg_state_destroy(plutovg_state_t* state)
    {
        plutovg_paint_destroy(state->paint);
        plutovg_array_destroy(state->stroke.dash.array);
        plutovg_span_buffer_destroy(&state->clip_spans);
        free(state);
    }

    plutovg_canvas_t* plutovg_canvas_create(plutovg_surface_t* surface)
    {
        plutovg_canvas_t* canvas = (plutovg_canvas_t*)malloc(sizeof(plutovg_canvas_t));
        canvas->ref_count = 1;
        canvas->surface = plutovg_surface_reference(surface);
        canvas->path = plutovg_path_create();
        canvas->state = plutovg_state_create();
        canvas->freed_state = NULL;
        canvas->clip_rect = PLUTOVG_MAKE_RECT(0, 0, (float)surface->width, (float)surface->height);
        plutovg_span_buffer_init(&canvas->clip_spans);
        plutovg_span_buffer_init(&canvas->fill_spans);
        return canvas;
    }



    plutovg_canvas_t* plutovg_canvas_reference(plutovg_canvas_t* canvas)
    {
        if (canvas == NULL)
            return NULL;
        ++canvas->ref_count;
        return canvas;
    }

    void plutovg_canvas_destroy(plutovg_canvas_t* canvas)
    {
        if (canvas == NULL)
            return;
        if (--canvas->ref_count == 0) {
            while (canvas->state) {
                plutovg_state_t* state = canvas->state;
                canvas->state = state->next;
                plutovg_state_destroy(state);
            }

            while (canvas->freed_state) {
                plutovg_state_t* state = canvas->freed_state;
                canvas->freed_state = state->next;
                plutovg_state_destroy(state);
            }

            plutovg_span_buffer_destroy(&canvas->fill_spans);
            plutovg_span_buffer_destroy(&canvas->clip_spans);
            plutovg_surface_destroy(canvas->surface);
            plutovg_path_destroy(canvas->path);
            free(canvas);
        }
    }

    int plutovg_canvas_get_reference_count(const plutovg_canvas_t* canvas)
    {
        if (canvas == NULL)
            return 0;
        return canvas->ref_count;
    }

    plutovg_surface_t* plutovg_canvas_get_surface(const plutovg_canvas_t* canvas)
    {
        return canvas->surface;
    }

    void plutovg_canvas_save(plutovg_canvas_t* canvas)
    {
        plutovg_state_t* new_state = canvas->freed_state;
        if (new_state == NULL)
            new_state = plutovg_state_create();
        else
            canvas->freed_state = new_state->next;
        plutovg_state_copy(new_state, canvas->state);
        new_state->next = canvas->state;
        canvas->state = new_state;
    }

    void plutovg_canvas_restore(plutovg_canvas_t* canvas)
    {
        if (canvas->state->next == NULL)
            return;
        plutovg_state_t* old_state = canvas->state;
        canvas->state = old_state->next;
        plutovg_state_reset(old_state);
        old_state->next = canvas->freed_state;
        canvas->freed_state = old_state;
    }

    void plutovg_canvas_set_rgb(plutovg_canvas_t* canvas, float r, float g, float b)
    {
        plutovg_canvas_set_rgba(canvas, r, g, b, 1.f);
    }

    void plutovg_canvas_set_rgba(plutovg_canvas_t* canvas, float r, float g, float b, float a)
    {
        plutovg_color_init_rgba(&canvas->state->color, r, g, b, a);
        plutovg_canvas_set_paint(canvas, NULL);
    }

    void plutovg_canvas_set_color(plutovg_canvas_t* canvas, const plutovg_color_t* color)
    {
        plutovg_canvas_set_rgba(canvas, color->r, color->g, color->b, color->a);
    }

    void plutovg_canvas_set_linear_gradient(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2, plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix)
    {
        plutovg_paint_t* paint = plutovg_paint_create_linear_gradient(x1, y1, x2, y2, spread, stops, nstops, matrix);
        plutovg_canvas_set_paint(canvas, paint);
        plutovg_paint_destroy(paint);
    }

    void plutovg_canvas_set_radial_gradient(plutovg_canvas_t* canvas, float cx, float cy, float cr, float fx, float fy, float fr, plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix)
    {
        plutovg_paint_t* paint = plutovg_paint_create_radial_gradient(cx, cy, cr, fx, fy, fr, spread, stops, nstops, matrix);
        plutovg_canvas_set_paint(canvas, paint);
        plutovg_paint_destroy(paint);
    }

    void plutovg_canvas_set_texture(plutovg_canvas_t* canvas, plutovg_surface_t* surface, plutovg_texture_type_t type, float opacity, const plutovg_matrix_t* matrix)
    {
        plutovg_paint_t* paint = plutovg_paint_create_texture(surface, type, opacity, matrix);
        plutovg_canvas_set_paint(canvas, paint);
        plutovg_paint_destroy(paint);
    }

    void plutovg_canvas_set_paint(plutovg_canvas_t* canvas, plutovg_paint_t* paint)
    {
        paint = plutovg_paint_reference(paint);
        plutovg_paint_destroy(canvas->state->paint);
        canvas->state->paint = paint;
    }

    plutovg_paint_t* plutovg_canvas_get_paint(const plutovg_canvas_t* canvas, plutovg_color_t* color)
    {
        if (color)
            *color = canvas->state->color;
        return canvas->state->paint;
    }


    void plutovg_canvas_set_fill_rule(plutovg_canvas_t* canvas, plutovg_fill_rule_t winding)
    {
        canvas->state->winding = winding;
    }

    plutovg_fill_rule_t plutovg_canvas_get_fill_rule(const plutovg_canvas_t* canvas)
    {
        return canvas->state->winding;
    }

    void plutovg_canvas_set_operator(plutovg_canvas_t* canvas, plutovg_operator_t op)
    {
        canvas->state->op = op;
    }

    plutovg_operator_t plutovg_canvas_get_operator(const plutovg_canvas_t* canvas)
    {
        return canvas->state->op;
    }

    void plutovg_canvas_set_opacity(plutovg_canvas_t* canvas, float opacity)
    {
        canvas->state->opacity = plutovg_clamp(opacity, 0.f, 1.f);
    }

    float plutovg_canvas_get_opacity(const plutovg_canvas_t* canvas)
    {
        return canvas->state->opacity;
    }

    void plutovg_canvas_set_line_width(plutovg_canvas_t* canvas, float line_width)
    {
        canvas->state->stroke.style.width = line_width;
    }

    float plutovg_canvas_get_line_width(const plutovg_canvas_t* canvas)
    {
        return canvas->state->stroke.style.width;
    }

    void plutovg_canvas_set_line_cap(plutovg_canvas_t* canvas, plutovg_line_cap_t line_cap)
    {
        canvas->state->stroke.style.cap = line_cap;
    }

    plutovg_line_cap_t plutovg_canvas_get_line_cap(const plutovg_canvas_t* canvas)
    {
        return canvas->state->stroke.style.cap;
    }

    void plutovg_canvas_set_line_join(plutovg_canvas_t* canvas, plutovg_line_join_t line_join)
    {
        canvas->state->stroke.style.join = line_join;
    }

    plutovg_line_join_t plutovg_canvas_get_line_join(const plutovg_canvas_t* canvas)
    {
        return canvas->state->stroke.style.join;
    }

    void plutovg_canvas_set_miter_limit(plutovg_canvas_t* canvas, float miter_limit)
    {
        canvas->state->stroke.style.miter_limit = miter_limit;
    }

    float plutovg_canvas_get_miter_limit(const plutovg_canvas_t* canvas)
    {
        return canvas->state->stroke.style.miter_limit;
    }

    void plutovg_canvas_set_dash(plutovg_canvas_t* canvas, float offset, const float* dashes, int ndashes)
    {
        plutovg_canvas_set_dash_offset(canvas, offset);
        plutovg_canvas_set_dash_array(canvas, dashes, ndashes);
    }

    void plutovg_canvas_set_dash_offset(plutovg_canvas_t* canvas, float offset)
    {
        canvas->state->stroke.dash.offset = offset;
    }

    float plutovg_canvas_get_dash_offset(const plutovg_canvas_t* canvas)
    {
        return canvas->state->stroke.dash.offset;
    }

    void plutovg_canvas_set_dash_array(plutovg_canvas_t* canvas, const float* dashes, int ndashes)
    {
        plutovg_array_clear(canvas->state->stroke.dash.array);
        plutovg_array_append_data(canvas->state->stroke.dash.array, dashes, ndashes);
    }

    int plutovg_canvas_get_dash_array(const plutovg_canvas_t* canvas, const float** dashes)
    {
        if (dashes)
            *dashes = canvas->state->stroke.dash.array.data;
        return canvas->state->stroke.dash.array.size;
    }

    void plutovg_canvas_translate(plutovg_canvas_t* canvas, float tx, float ty)
    {
        plutovg_matrix_translate(&canvas->state->matrix, tx, ty);
    }

    void plutovg_canvas_scale(plutovg_canvas_t* canvas, float sx, float sy)
    {
        plutovg_matrix_scale(&canvas->state->matrix, sx, sy);
    }

    void plutovg_canvas_shear(plutovg_canvas_t* canvas, float shx, float shy)
    {
        plutovg_matrix_shear(&canvas->state->matrix, shx, shy);
    }

    void plutovg_canvas_rotate(plutovg_canvas_t* canvas, float angle)
    {
        plutovg_matrix_rotate(&canvas->state->matrix, angle);
    }

    void plutovg_canvas_transform(plutovg_canvas_t* canvas, const plutovg_matrix_t* matrix)
    {
        plutovg_matrix_multiply(&canvas->state->matrix, matrix, &canvas->state->matrix);
    }

    void plutovg_canvas_reset_matrix(plutovg_canvas_t* canvas)
    {
        plutovg_matrix_init_identity(&canvas->state->matrix);
    }

    void plutovg_canvas_set_matrix(plutovg_canvas_t* canvas, const plutovg_matrix_t* matrix)
    {
        canvas->state->matrix = matrix ? *matrix : PLUTOVG_IDENTITY_MATRIX;
    }

    void plutovg_canvas_get_matrix(const plutovg_canvas_t* canvas, plutovg_matrix_t* matrix)
    {
        *matrix = canvas->state->matrix;
    }

    void plutovg_canvas_map(const plutovg_canvas_t* canvas, float x, float y, float* xx, float* yy)
    {
        plutovg_matrix_map(&canvas->state->matrix, x, y, xx, yy);
    }

    void plutovg_canvas_map_point(const plutovg_canvas_t* canvas, const plutovg_point_t* src, plutovg_point_t* dst)
    {
        plutovg_matrix_map_point(&canvas->state->matrix, src, dst);
    }

    void plutovg_canvas_map_rect(const plutovg_canvas_t* canvas, const plutovg_rect_t* src, plutovg_rect_t* dst)
    {
        plutovg_matrix_map_rect(&canvas->state->matrix, src, dst);
    }

    void plutovg_canvas_move_to(plutovg_canvas_t* canvas, float x, float y)
    {
        plutovg_path_move_to(canvas->path, x, y);
    }

    void plutovg_canvas_line_to(plutovg_canvas_t* canvas, float x, float y)
    {
        plutovg_path_line_to(canvas->path, x, y);
    }

    void plutovg_canvas_quad_to(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2)
    {
        plutovg_path_quad_to(canvas->path, x1, y1, x2, y2);
    }

    void plutovg_canvas_cubic_to(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2, float x3, float y3)
    {
        plutovg_path_cubic_to(canvas->path, x1, y1, x2, y2, x3, y3);
    }

    void plutovg_canvas_arc_to(plutovg_canvas_t* canvas, float rx, float ry, float angle, bool large_arc_flag, bool sweep_flag, float x, float y)
    {
        plutovg_path_arc_to(canvas->path, rx, ry, angle, large_arc_flag, sweep_flag, x, y);
    }

    void plutovg_canvas_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h)
    {
        plutovg_path_add_rect(canvas->path, x, y, w, h);
    }

    void plutovg_canvas_round_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h, float rx, float ry)
    {
        plutovg_path_add_round_rect(canvas->path, x, y, w, h, rx, ry);
    }

    void plutovg_canvas_ellipse(plutovg_canvas_t* canvas, float cx, float cy, float rx, float ry)
    {
        plutovg_path_add_ellipse(canvas->path, cx, cy, rx, ry);
    }

    void plutovg_canvas_circle(plutovg_canvas_t* canvas, float cx, float cy, float r)
    {
        plutovg_path_add_circle(canvas->path, cx, cy, r);
    }

    void plutovg_canvas_arc(plutovg_canvas_t* canvas, float cx, float cy, float r, float a0, float a1, bool ccw)
    {
        plutovg_path_add_arc(canvas->path, cx, cy, r, a0, a1, ccw);
    }

    void plutovg_canvas_add_path(plutovg_canvas_t* canvas, const plutovg_path_t* path)
    {
        plutovg_path_add_path(canvas->path, path, NULL);
    }

    void plutovg_canvas_new_path(plutovg_canvas_t* canvas)
    {
        plutovg_path_reset(canvas->path);
    }

    void plutovg_canvas_close_path(plutovg_canvas_t* canvas)
    {
        plutovg_path_close(canvas->path);
    }

    void plutovg_canvas_get_current_point(const plutovg_canvas_t* canvas, float* x, float* y)
    {
        plutovg_path_get_current_point(canvas->path, x, y);
    }

    plutovg_path_t* plutovg_canvas_get_path(const plutovg_canvas_t* canvas)
    {
        return canvas->path;
    }

    void plutovg_canvas_fill_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents)
    {
        plutovg_path_extents(canvas->path, extents, true);
        plutovg_canvas_map_rect(canvas, extents, extents);
    }

    void plutovg_canvas_stroke_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents)
    {
        plutovg_stroke_data_t* stroke = &canvas->state->stroke;
        float cap_limit = stroke->style.width / 2.f;
        if (stroke->style.cap == PLUTOVG_LINE_CAP_SQUARE)
            cap_limit *= PLUTOVG_SQRT2;
        float join_limit = stroke->style.width / 2.f;
        if (stroke->style.join == PLUTOVG_LINE_JOIN_MITER) {
            join_limit *= stroke->style.miter_limit;
        }

        float delta = plutovg_max(cap_limit, join_limit);
        plutovg_path_extents(canvas->path, extents, true);
        extents->x -= delta;
        extents->y -= delta;
        extents->w += delta * 2.f;
        extents->h += delta * 2.f;
        plutovg_canvas_map_rect(canvas, extents, extents);
    }

    void plutovg_canvas_clip_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents)
    {
        if (canvas->state->clipping) {
            plutovg_span_buffer_extents(&canvas->state->clip_spans, extents);
        }
        else {
            extents->x = canvas->clip_rect.x;
            extents->y = canvas->clip_rect.y;
            extents->w = canvas->clip_rect.w;
            extents->h = canvas->clip_rect.h;
        }
    }

    void plutovg_canvas_fill(plutovg_canvas_t* canvas)
    {
        plutovg_canvas_fill_preserve(canvas);
        plutovg_canvas_new_path(canvas);
    }

    void plutovg_canvas_stroke(plutovg_canvas_t* canvas)
    {
        plutovg_canvas_stroke_preserve(canvas);
        plutovg_canvas_new_path(canvas);
    }

    void plutovg_canvas_clip(plutovg_canvas_t* canvas)
    {
        plutovg_canvas_clip_preserve(canvas);
        plutovg_canvas_new_path(canvas);
    }

    void plutovg_canvas_paint(plutovg_canvas_t* canvas)
    {
        if (canvas->state->clipping) {
            plutovg_blend(canvas, &canvas->state->clip_spans);
        }
        else {
            plutovg_span_buffer_init_rect(&canvas->clip_spans, 0, 0, canvas->surface->width, canvas->surface->height);
            plutovg_blend(canvas, &canvas->clip_spans);
        }
    }

    
    void plutovg_canvas_fill_preserve(plutovg_canvas_t* canvas)
    {
        plutovg_rasterize(&canvas->fill_spans, canvas->path, &canvas->state->matrix, &canvas->clip_rect, NULL, canvas->state->winding);
        if (canvas->state->clipping) {
            plutovg_span_buffer_intersect(&canvas->clip_spans, &canvas->fill_spans, &canvas->state->clip_spans);
            plutovg_blend(canvas, &canvas->clip_spans);
        }
        else {
            plutovg_blend(canvas, &canvas->fill_spans);
        }
    }

    void plutovg_canvas_stroke_preserve(plutovg_canvas_t* canvas)
    {
        plutovg_rasterize(&canvas->fill_spans, canvas->path, &canvas->state->matrix, &canvas->clip_rect, &canvas->state->stroke, PLUTOVG_FILL_RULE_NON_ZERO);
        if (canvas->state->clipping) {
            plutovg_span_buffer_intersect(&canvas->clip_spans, &canvas->fill_spans, &canvas->state->clip_spans);
            plutovg_blend(canvas, &canvas->clip_spans);
        }
        else {
            plutovg_blend(canvas, &canvas->fill_spans);
        }
    }

    void plutovg_canvas_clip_preserve(plutovg_canvas_t* canvas)
    {
        if (canvas->state->clipping) {
            plutovg_rasterize(&canvas->fill_spans, canvas->path, &canvas->state->matrix, &canvas->clip_rect, NULL, canvas->state->winding);
            plutovg_span_buffer_intersect(&canvas->clip_spans, &canvas->fill_spans, &canvas->state->clip_spans);
            plutovg_span_buffer_copy(&canvas->state->clip_spans, &canvas->clip_spans);
        }
        else {
            plutovg_rasterize(&canvas->state->clip_spans, canvas->path, &canvas->state->matrix, &canvas->clip_rect, NULL, canvas->state->winding);
            canvas->state->clipping = true;
        }
    }

    void plutovg_canvas_fill_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h)
    {
        plutovg_canvas_new_path(canvas);
        plutovg_canvas_rect(canvas, x, y, w, h);
        plutovg_canvas_fill(canvas);
    }

    void plutovg_canvas_fill_path(plutovg_canvas_t* canvas, const plutovg_path_t* path)
    {
        plutovg_canvas_new_path(canvas);
        plutovg_canvas_add_path(canvas, path);
        plutovg_canvas_fill(canvas);
    }

    void plutovg_canvas_stroke_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h)
    {
        plutovg_canvas_new_path(canvas);
        plutovg_canvas_rect(canvas, x, y, w, h);
        plutovg_canvas_stroke(canvas);
    }

    void plutovg_canvas_stroke_path(plutovg_canvas_t* canvas, const plutovg_path_t* path)
    {
        plutovg_canvas_new_path(canvas);
        plutovg_canvas_add_path(canvas, path);
        plutovg_canvas_stroke(canvas);
    }

    void plutovg_canvas_clip_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h)
    {
        plutovg_canvas_new_path(canvas);
        plutovg_canvas_rect(canvas, x, y, w, h);
        plutovg_canvas_clip(canvas);
    }

    void plutovg_canvas_clip_path(plutovg_canvas_t* canvas, const plutovg_path_t* path)
    {
        plutovg_canvas_new_path(canvas);
        plutovg_canvas_add_path(canvas, path);
        plutovg_canvas_clip(canvas);
    }

    typedef struct {
        const plutosvg_document_t* document;
        plutovg_canvas_t* canvas;
        const plutovg_color_t* current_color;
        plutosvg_palette_func_t palette_func;
        void* closure;
    } render_context_t;

    static float resolve_length(const render_state_t* state, const length_t* length, char mode)
    {
        float maximum = 0.f;
        if (length->type == length_type_percent) {
            if (mode == 'x') {
                maximum = state->view_width;
            }
            else if (mode == 'y') {
                maximum = state->view_height;
            }
            else if (mode == 'o') {
                maximum = hypotf(state->view_width, state->view_height) / PLUTOVG_SQRT2;
            }
        }

        return convert_length(length, maximum);
    }



    static element_t* find_element(const plutosvg_document_t* document, const string_t* id)
    {
        if (document->id_cache && id->length > 0)
            return (element_t*)hashmap_get(document->id_cache, id->data, id->length);
        return NULL;
    }

    static element_t* resolve_href(const plutosvg_document_t* document, const element_t* element)
    {
        const string_t* value = find_attribute(element, ATTR_HREF, false);
        if (value && value->length > 1 && value->data[0] == '#') {
            string_t id = { value->data + 1, value->length - 1 };
            return find_element(document, &id);
        }

        return NULL;
    }

    static plutovg_color_t convert_color(const color_t* color)
    {
        plutovg_color_t value;
        plutovg_color_init_argb32(&value, color->value);
        return value;
    }

    static plutovg_color_t resolve_current_color(const render_context_t* context, const element_t* element)
    {
        color_t color = { color_type_current };
        parse_color(element, ATTR_COLOR, &color, true);
        if (color.type == color_type_fixed)
            return convert_color(&color);
        if (element->parent == NULL) {
            if (context->current_color)
                return *context->current_color;
            return PLUTOVG_BLACK_COLOR;
        }

        return resolve_current_color(context, element->parent);
    }

    static plutovg_color_t resolve_color(const render_context_t* context, const element_t* element, const color_t* color)
    {
        if (color->type == color_type_fixed)
            return convert_color(color);
        return resolve_current_color(context, element);
    }

#define MAX_STOPS 64
    typedef struct {
        plutovg_gradient_stop_t data[MAX_STOPS];
        size_t size;
    } gradient_stop_array_t;

    static void resolve_gradient_stops(const render_context_t* context, const element_t* element, gradient_stop_array_t* stops)
    {
        const element_t* child = element->first_child;
        while (child && stops->size < MAX_STOPS) {
            if (child->id == TAG_STOP) {
                float offset = 0.f;
                float stop_opacity = 1.f;
                color_t stop_color = { color_type_fixed, 0xFF000000 };

                parse_number(child, ATTR_OFFSET, &offset, true, false);
                parse_number(child, ATTR_STOP_OPACITY, &stop_opacity, true, false);
                parse_color(child, ATTR_STOP_COLOR, &stop_color, false);

                stops->data[stops->size].offset = offset;
                stops->data[stops->size].color = resolve_color(context, child, &stop_color);
                stops->data[stops->size].color.a *= stop_opacity;
                stops->size += 1;
            }

            child = child->next_sibling;
        }
    }




    static float resolve_gradient_length(const render_state_t* state, const length_t* length, units_type_t units, char mode)
    {
        if (units == units_type_user_space_on_use)
            return resolve_length(state, length, mode);
        return convert_length(length, 1.f);
    }

    typedef struct {
        const element_t* units;
        const element_t* spread;
        const element_t* transform;
        const element_t* stops;
    } gradient_attributes_t;

    static void collect_gradient_attributes(const element_t* element, gradient_attributes_t* attributes)
    {
        if (attributes->units == NULL && has_attribute(element, ATTR_GRADIENT_UNITS))
            attributes->units = element;
        if (attributes->spread == NULL && has_attribute(element, ATTR_SPREAD_METHOD))
            attributes->spread = element;
        if (attributes->transform == NULL && has_attribute(element, ATTR_GRADIENT_TRANSFORM))
            attributes->transform = element;
        if (attributes->stops == NULL) {
            for (const element_t* child = element->first_child; child; child = child->next_sibling) {
                if (child->id == TAG_STOP) {
                    attributes->stops = element;
                    break;
                }
            }
        }
    }

    static void fill_gradient_attributes(const element_t* element, gradient_attributes_t* attributes)
    {
        if (attributes->units == NULL) attributes->units = element;
        if (attributes->spread == NULL) attributes->spread = element;
        if (attributes->transform == NULL) attributes->transform = element;
        if (attributes->stops == NULL) {
            attributes->stops = element;
        }
    }

    static void resolve_gradient_attributes(const render_context_t* context, const render_state_t* state, const gradient_attributes_t* attributes, units_type_t* units, plutovg_spread_method_t* spread, plutovg_matrix_t* transform, gradient_stop_array_t* stops)
    {
        parse_units_type(attributes->units, ATTR_GRADIENT_UNITS, units);
        parse_spread_method(attributes->spread, ATTR_SPREAD_METHOD, spread);
        parse_transform(attributes->transform, ATTR_GRADIENT_TRANSFORM, transform);
        resolve_gradient_stops(context, attributes->stops, stops);
        if (*units == units_type_object_bounding_box) {
            plutovg_matrix_t matrix;
            plutovg_matrix_init_translate(&matrix, state->extents.x, state->extents.y);
            plutovg_matrix_scale(&matrix, state->extents.w, state->extents.h);
            plutovg_matrix_multiply(transform, transform, &matrix);
        }
    }

    typedef struct {
        gradient_attributes_t base;
        const element_t* x1;
        const element_t* y1;
        const element_t* x2;
        const element_t* y2;
    } linear_gradient_attributes_t;

#define MAX_GRADIENT_DEPTH 128
    static bool apply_linear_gradient(render_state_t* state, const render_context_t* context, const element_t* element)
    {
        linear_gradient_attributes_t attributes = { 0 };
        const element_t* current = element;
        for (int i = 0; i < MAX_GRADIENT_DEPTH; ++i) {
            collect_gradient_attributes(current, &attributes.base);
            if (current->id == TAG_LINEAR_GRADIENT) {
                if (attributes.x1 == NULL && has_attribute(current, ATTR_X1))
                    attributes.x1 = current;
                if (attributes.y1 == NULL && has_attribute(current, ATTR_Y1))
                    attributes.y1 = current;
                if (attributes.x2 == NULL && has_attribute(current, ATTR_X2))
                    attributes.x2 = current;
                if (attributes.y2 == NULL && has_attribute(current, ATTR_Y2)) {
                    attributes.y2 = current;
                }
            }

            const element_t* ref = resolve_href(context->document, current);
            if (ref == NULL || !(ref->id == TAG_LINEAR_GRADIENT || ref->id == TAG_RADIAL_GRADIENT))
                break;
            current = ref;
        }

        if (attributes.base.stops == NULL)
            return false;
        fill_gradient_attributes(element, &attributes.base);
        if (attributes.x1 == NULL) attributes.x1 = element;
        if (attributes.y1 == NULL) attributes.y1 = element;
        if (attributes.x2 == NULL) attributes.x2 = element;
        if (attributes.y2 == NULL) attributes.y2 = element;

        units_type_t units = units_type_object_bounding_box;
        plutovg_spread_method_t spread = PLUTOVG_SPREAD_METHOD_PAD;
        plutovg_matrix_t transform = { 1, 0, 0, 1, 0, 0 };
        gradient_stop_array_t stops = { 0 };
        resolve_gradient_attributes(context, state, &attributes.base, &units, &spread, &transform, &stops);

        length_t x1 = { 0, length_type_fixed };
        length_t y1 = { 0, length_type_fixed };
        length_t x2 = { 100, length_type_percent };
        length_t y2 = { 0, length_type_fixed };

        parse_length(attributes.x1, ATTR_X1, &x1, true, false);
        parse_length(attributes.y1, ATTR_Y1, &y1, true, false);
        parse_length(attributes.x2, ATTR_X2, &x2, true, false);
        parse_length(attributes.y2, ATTR_Y2, &y2, true, false);

        float _x1 = resolve_gradient_length(state, &x1, units, 'x');
        float _y1 = resolve_gradient_length(state, &y1, units, 'y');
        float _x2 = resolve_gradient_length(state, &x2, units, 'x');
        float _y2 = resolve_gradient_length(state, &y2, units, 'y');

        plutovg_canvas_set_linear_gradient(context->canvas, _x1, _y1, _x2, _y2, spread, stops.data, (int)stops.size, &transform);
        return true;
    }

    typedef struct {
        gradient_attributes_t base;
        const element_t* cx;
        const element_t* cy;
        const element_t* r;
        const element_t* fx;
        const element_t* fy;
    } radial_gradient_attributes_t;

    static bool apply_radial_gradient(render_state_t* state, const render_context_t* context, const element_t* element)
    {
        radial_gradient_attributes_t attributes = { 0 };
        const element_t* current = element;
        for (int i = 0; i < MAX_GRADIENT_DEPTH; ++i) {
            collect_gradient_attributes(current, &attributes.base);
            if (current->id == TAG_RADIAL_GRADIENT) {
                if (attributes.cx == NULL && has_attribute(current, ATTR_CX))
                    attributes.cx = current;
                if (attributes.cy == NULL && has_attribute(current, ATTR_CY))
                    attributes.cy = current;
                if (attributes.r == NULL && has_attribute(current, ATTR_R))
                    attributes.r = current;
                if (attributes.fx == NULL && has_attribute(current, ATTR_FX))
                    attributes.fx = current;
                if (attributes.fy == NULL && has_attribute(current, ATTR_FY)) {
                    attributes.fy = current;
                }
            }

            const element_t* ref = resolve_href(context->document, current);
            if (ref == NULL || !(ref->id == TAG_LINEAR_GRADIENT || ref->id == TAG_RADIAL_GRADIENT))
                break;
            current = ref;
        }

        if (attributes.base.stops == NULL)
            return false;
        fill_gradient_attributes(element, &attributes.base);
        if (attributes.cx == NULL) attributes.cx = element;
        if (attributes.cy == NULL) attributes.cy = element;
        if (attributes.r == NULL) attributes.r = element;

        units_type_t units = units_type_object_bounding_box;
        plutovg_spread_method_t spread = PLUTOVG_SPREAD_METHOD_PAD;
        plutovg_matrix_t transform = { 1, 0, 0, 1, 0, 0 };
        gradient_stop_array_t stops = { 0 };
        resolve_gradient_attributes(context, state, &attributes.base, &units, &spread, &transform, &stops);

        length_t cx = { 50, length_type_percent };
        length_t cy = { 50, length_type_percent };
        length_t r = { 50, length_type_percent };
        length_t fx = { 50, length_type_percent };
        length_t fy = { 50, length_type_percent };

        parse_length(attributes.cx, ATTR_CX, &cx, true, false);
        parse_length(attributes.cy, ATTR_CY, &cy, true, false);
        parse_length(attributes.r, ATTR_R, &r, false, false);

        if (attributes.fx) {
            parse_length(attributes.fx, ATTR_FX, &fx, true, false);
        }
        else {
            parse_length(attributes.cx, ATTR_CX, &fx, true, false);
        }

        if (attributes.fy) {
            parse_length(attributes.fy, ATTR_FY, &fy, true, false);
        }
        else {
            parse_length(attributes.cy, ATTR_CY, &fy, true, false);
        }

        float _cx = resolve_gradient_length(state, &cx, units, 'x');
        float _cy = resolve_gradient_length(state, &cy, units, 'y');
        float _r = resolve_gradient_length(state, &r, units, 'o');
        float _fx = resolve_gradient_length(state, &fx, units, 'x');
        float _fy = resolve_gradient_length(state, &fy, units, 'y');

        plutovg_canvas_set_radial_gradient(context->canvas, _cx, _cy, _r, _fx, _fy, 0.f, spread, stops.data, (int)stops.size, &transform);
        return true;
    }

    static bool apply_paint(render_state_t* state, const render_context_t* context, const paint_t* paint)
    {
        if (paint->type == paint_type_none)
            return false;
        if (paint->type == paint_type_color) {
            plutovg_color_t color = resolve_color(context, state->element, &paint->color);
            plutovg_canvas_set_color(context->canvas, &color);
            return true;
        }

        if (paint->type == paint_type_var) {
            plutovg_color_t color;
            if (!context->palette_func || !context->palette_func(context->closure, paint->id.data, (int)paint->id.length, &color))
                color = resolve_color(context, state->element, &paint->color);
            plutovg_canvas_set_color(context->canvas, &color);
            return true;
        }

        const element_t* ref = find_element(context->document, &paint->id);
        if (ref == NULL) {
            plutovg_color_t color = resolve_color(context, state->element, &paint->color);
            plutovg_canvas_set_color(context->canvas, &color);
            return true;
        }

        if (ref->id == TAG_LINEAR_GRADIENT)
            return apply_linear_gradient(state, context, ref);
        if (ref->id == TAG_RADIAL_GRADIENT)
            return apply_radial_gradient(state, context, ref);
        return false;
    }

    static void draw_shape(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        paint_t stroke = { paint_type_none };
        parse_paint(element, ATTR_STROKE, &stroke);

        length_t stroke_width = { 1.f, length_type_fixed };
        plutovg_line_cap_t line_cap = PLUTOVG_LINE_CAP_BUTT;
        plutovg_line_join_t line_join = PLUTOVG_LINE_JOIN_MITER;
        float miter_limit = 4.f;

        if (stroke.type > paint_type_none) {
            parse_length(element, ATTR_STROKE_WIDTH, &stroke_width, false, true);
            parse_line_cap(element, ATTR_STROKE_LINECAP, &line_cap);
            parse_line_join(element, ATTR_STROKE_LINEJOIN, &line_join);
            parse_number(element, ATTR_STROKE_MITERLIMIT, &miter_limit, false, true);
        }

        if (state->mode == render_mode_bounding) {
            if (stroke.type == paint_type_none)
                return;
            float line_width = resolve_length(state, &stroke_width, 'o');
            float cap_limit = line_width / 2.f;
            if (line_cap == PLUTOVG_LINE_CAP_SQUARE)
                cap_limit *= PLUTOVG_SQRT2;
            float join_limit = line_width / 2.f;
            if (line_join == PLUTOVG_LINE_JOIN_MITER) {
                join_limit *= miter_limit;
            }

            float delta = MAX(cap_limit, join_limit);
            state->extents.x -= delta;
            state->extents.y -= delta;
            state->extents.w += delta * 2.f;
            state->extents.h += delta * 2.f;
            return;
        }

        paint_t fill = { paint_type_color, {color_type_fixed, 0xFF000000} };
        parse_paint(element, ATTR_FILL, &fill);

        if (apply_paint(state, context, &fill)) {
            float fill_opacity = 1.f;
            parse_number(element, ATTR_FILL_OPACITY, &fill_opacity, true, true);

            plutovg_fill_rule_t fill_rule = PLUTOVG_FILL_RULE_NON_ZERO;
            parse_fill_rule(element, ATTR_FILL_RULE, &fill_rule);

            plutovg_canvas_set_fill_rule(context->canvas, fill_rule);
            plutovg_canvas_set_opacity(context->canvas, fill_opacity * state->opacity);
            plutovg_canvas_set_matrix(context->canvas, &state->matrix);
            plutovg_canvas_fill_path(context->canvas, context->document->path);
        }

        if (apply_paint(state, context, &stroke)) {
            float stroke_opacity = 1.f;
            parse_number(element, ATTR_STROKE_OPACITY, &stroke_opacity, true, true);

            length_t dash_offset = { 0.f, length_type_fixed };
            parse_length(element, ATTR_STROKE_DASHOFFSET, &dash_offset, false, true);

            stroke_dash_array_t dash_array = { 0 };
            parse_dash_array(element, ATTR_STROKE_DASHARRAY, &dash_array);

            float dashes[MAX_DASHES];
            for (int i = 0; i < dash_array.size; ++i) {
                dashes[i] = resolve_length(state, dash_array.data + i, 'o');
            }

            plutovg_canvas_set_dash_offset(context->canvas, resolve_length(state, &dash_offset, 'o'));
            plutovg_canvas_set_dash_array(context->canvas, dashes, (int)dash_array.size);

            plutovg_canvas_set_line_width(context->canvas, resolve_length(state, &stroke_width, 'o'));
            plutovg_canvas_set_line_cap(context->canvas, line_cap);
            plutovg_canvas_set_line_join(context->canvas, line_join);
            plutovg_canvas_set_miter_limit(context->canvas, miter_limit);
            plutovg_canvas_set_opacity(context->canvas, stroke_opacity * state->opacity);
            plutovg_canvas_set_matrix(context->canvas, &state->matrix);
            plutovg_canvas_stroke_path(context->canvas, context->document->path);
        }
    }

    static bool is_display_none(const element_t* element)
    {
        display_t display = display_inline;
        parse_display(element, ATTR_DISPLAY, &display);
        return display == display_none;
    }

    static bool is_visibility_hidden(const element_t* element)
    {
        visibility_t visibility = visibility_visible;
        parse_visibility(element, ATTR_VISIBILITY, &visibility);
        return visibility != visibility_visible;
    }

    static void render_element(const element_t* element, const render_context_t* context, render_state_t* state);
    static void render_children(const element_t* element, const render_context_t* context, render_state_t* state);

    static void apply_view_transform(render_state_t* state, float width, float height)
    {
        plutovg_rect_t view_box = { 0, 0, 0, 0 };
        if (!parse_view_box(state->element, ATTR_VIEW_BOX, &view_box))
            return;
        view_position_t position = { view_align_x_mid_y_mid, view_scale_meet };
        parse_view_position(state->element, ATTR_PRESERVE_ASPECT_RATIO, &position);
        float scale_x = width / view_box.w;
        float scale_y = height / view_box.h;
        if (position.align == view_align_none) {
            plutovg_matrix_scale(&state->matrix, scale_x, scale_y);
            plutovg_matrix_translate(&state->matrix, -view_box.x, -view_box.y);
        }
        else {
            float scale = (position.scale == view_scale_meet) ? MIN(scale_x, scale_y) : MAX(scale_x, scale_y);
            float offset_x = -view_box.x * scale;
            float offset_y = -view_box.y * scale;
            float view_width = view_box.w * scale;
            float view_height = view_box.h * scale;
            switch (position.align) {
            case view_align_x_mid_y_min:
            case view_align_x_mid_y_mid:
            case view_align_x_mid_y_max:
                offset_x += (width - view_width) * 0.5f;
                break;
            case view_align_x_max_y_min:
            case view_align_x_max_y_mid:
            case view_align_x_max_y_max:
                offset_x += (width - view_width);
                break;
            default:
                break;
            }

            switch (position.align) {
            case view_align_x_min_y_mid:
            case view_align_x_mid_y_mid:
            case view_align_x_max_y_mid:
                offset_y += (height - view_height) * 0.5f;
                break;
            case view_align_x_min_y_max:
            case view_align_x_mid_y_max:
            case view_align_x_max_y_max:
                offset_y += (height - view_height);
                break;
            default:
                break;
            }

            plutovg_matrix_translate(&state->matrix, offset_x, offset_y);
            plutovg_matrix_scale(&state->matrix, scale, scale);
        }

        state->view_width = view_box.w;
        state->view_height = view_box.h;
    }

    static void render_symbol(const element_t* element, const render_context_t* context, render_state_t* state, float x, float y, float width, float height)
    {
        if (width <= 0.f || height <= 0.f || is_display_none(element))
            return;
        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        new_state.view_width = width;
        new_state.view_height = height;
        plutovg_matrix_translate(&new_state.matrix, x, y);

        apply_view_transform(&new_state, width, height);
        render_children(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void render_svg(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (element->parent == NULL) {
            render_symbol(element, context, state, 0.f, 0.f, context->document->width, context->document->height);
            return;
        }

        length_t x = { 0, length_type_fixed };
        length_t y = { 0, length_type_fixed };

        length_t w = { 100, length_type_percent };
        length_t h = { 100, length_type_percent };

        parse_length(element, ATTR_X, &x, true, false);
        parse_length(element, ATTR_Y, &y, true, false);
        parse_length(element, ATTR_WIDTH, &w, false, false);
        parse_length(element, ATTR_HEIGHT, &h, false, false);

        float _x = resolve_length(state, &x, 'x');
        float _y = resolve_length(state, &y, 'y');
        float _w = resolve_length(state, &w, 'x');
        float _h = resolve_length(state, &h, 'y');
        render_symbol(element, context, state, _x, _y, _w, _h);
    }

    static void render_use(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || has_cycle_reference(state, element))
            return;
        element_t* ref = resolve_href(context->document, element);
        if (ref == NULL)
            return;
        length_t x = { 0, length_type_fixed };
        length_t y = { 0, length_type_fixed };

        parse_length(element, ATTR_X, &x, true, false);
        parse_length(element, ATTR_Y, &y, true, false);

        float _x = resolve_length(state, &x, 'x');
        float _y = resolve_length(state, &y, 'y');

        render_state_t new_state;
        render_state_begin(element, &new_state, state);
        plutovg_matrix_translate(&new_state.matrix, _x, _y);

        const element_t* parent = ref->parent;
        ref->parent = (element_t*)(element);
        if (ref->id == TAG_SVG || ref->id == TAG_SYMBOL) {
            render_svg(ref, context, &new_state);
        }
        else {
            render_element(ref, context, &new_state);
        }

        ref->parent = (element_t*)(parent);
        render_state_end(&new_state);
    }

    static void render_g(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element))
            return;
        render_state_t new_state;
        render_state_begin(element, &new_state, state);
        render_children(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void render_line(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || is_visibility_hidden(element))
            return;
        length_t x1 = { 0, length_type_fixed };
        length_t y1 = { 0, length_type_fixed };
        length_t x2 = { 0, length_type_fixed };
        length_t y2 = { 0, length_type_fixed };

        parse_length(element, ATTR_X1, &x1, true, false);
        parse_length(element, ATTR_Y1, &y1, true, false);
        parse_length(element, ATTR_X2, &x2, true, false);
        parse_length(element, ATTR_Y2, &y2, true, false);

        float _x1 = resolve_length(state, &x1, 'x');
        float _y1 = resolve_length(state, &y1, 'y');
        float _x2 = resolve_length(state, &x2, 'x');
        float _y2 = resolve_length(state, &y2, 'y');

        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        new_state.extents.x = MIN(_x1, _x2);
        new_state.extents.y = MIN(_y1, _y2);
        new_state.extents.w = fabsf(_x2 - _x1);
        new_state.extents.h = fabsf(_y2 - _y1);

        plutovg_path_reset(context->document->path);
        plutovg_path_move_to(context->document->path, _x1, _y1);
        plutovg_path_line_to(context->document->path, _x2, _y2);
        draw_shape(element, context, &new_state);
        render_state_end(&new_state);
    }


    static void render_ellipse(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || is_visibility_hidden(element))
            return;
        length_t rx = { 0, length_type_fixed };
        length_t ry = { 0, length_type_fixed };

        parse_length(element, ATTR_RX, &rx, false, false);
        parse_length(element, ATTR_RY, &ry, false, false);
        if (is_length_zero(rx) || is_length_zero(ry))
            return;
        length_t cx = { 0, length_type_fixed };
        length_t cy = { 0, length_type_fixed };

        parse_length(element, ATTR_CX, &cx, true, false);
        parse_length(element, ATTR_CY, &cy, true, false);

        float _cx = resolve_length(state, &cx, 'x');
        float _cy = resolve_length(state, &cy, 'y');
        float _rx = resolve_length(state, &rx, 'x');
        float _ry = resolve_length(state, &ry, 'y');

        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        new_state.extents.x = _cx - _rx;
        new_state.extents.y = _cy - _ry;
        new_state.extents.w = _rx + _rx;
        new_state.extents.h = _ry + _ry;

        plutovg_path_reset(context->document->path);
        plutovg_path_add_ellipse(context->document->path, _cx, _cy, _rx, _ry);
        draw_shape(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void render_circle(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || is_visibility_hidden(element))
            return;
        length_t r = { 0, length_type_fixed };
        parse_length(element, ATTR_R, &r, false, false);
        if (is_length_zero(r))
            return;
        length_t cx = { 0, length_type_fixed };
        length_t cy = { 0, length_type_fixed };

        parse_length(element, ATTR_CX, &cx, true, false);
        parse_length(element, ATTR_CY, &cy, true, false);

        float _cx = resolve_length(state, &cx, 'x');
        float _cy = resolve_length(state, &cy, 'y');
        float _r = resolve_length(state, &r, 'o');

        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        new_state.extents.x = _cx - _r;
        new_state.extents.y = _cy - _r;
        new_state.extents.w = _r + _r;
        new_state.extents.h = _r + _r;

        plutovg_path_reset(context->document->path);
        plutovg_path_add_circle(context->document->path, _cx, _cy, _r);
        draw_shape(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void render_rect(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || is_visibility_hidden(element))
            return;
        length_t w = { 0, length_type_fixed };
        length_t h = { 0, length_type_fixed };

        parse_length(element, ATTR_WIDTH, &w, false, false);
        parse_length(element, ATTR_HEIGHT, &h, false, false);
        if (is_length_zero(w) || is_length_zero(h))
            return;
        length_t x = { 0, length_type_fixed };
        length_t y = { 0, length_type_fixed };

        parse_length(element, ATTR_X, &x, true, false);
        parse_length(element, ATTR_Y, &y, true, false);

        float _x = resolve_length(state, &x, 'x');
        float _y = resolve_length(state, &y, 'y');
        float _w = resolve_length(state, &w, 'x');
        float _h = resolve_length(state, &h, 'y');

        length_t rx = { 0, length_type_unknown };
        length_t ry = { 0, length_type_unknown };

        parse_length(element, ATTR_RX, &rx, false, false);
        parse_length(element, ATTR_RY, &ry, false, false);

        float _rx = resolve_length(state, &rx, 'x');
        float _ry = resolve_length(state, &ry, 'y');

        if (!is_length_valid(rx)) _rx = _ry;
        if (!is_length_valid(ry)) _ry = _rx;

        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        new_state.extents.x = _x;
        new_state.extents.y = _y;
        new_state.extents.w = _w;
        new_state.extents.h = _h;

        plutovg_path_reset(context->document->path);
        plutovg_path_add_round_rect(context->document->path, _x, _y, _w, _h, _rx, _ry);
        draw_shape(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void render_poly(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || is_visibility_hidden(element))
            return;
        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        plutovg_path_reset(context->document->path);
        parse_points(element, ATTR_POINTS, context->document->path);
        plutovg_path_extents(context->document->path, &new_state.extents, false);
        draw_shape(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void render_path(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        if (is_display_none(element) || is_visibility_hidden(element))
            return;
        render_state_t new_state;
        render_state_begin(element, &new_state, state);

        plutovg_path_reset(context->document->path);
        parse_path(element, ATTR_D, context->document->path);
        plutovg_path_extents(context->document->path, &new_state.extents, false);
        draw_shape(element, context, &new_state);
        render_state_end(&new_state);
    }

    static void transform_view_rect(const view_position_t* position, plutovg_rect_t* dst_rect, plutovg_rect_t* src_rect)
    {
        if (position->align == view_align_none)
            return;
        float view_width = dst_rect->w;
        float view_height = dst_rect->h;
        float image_width = src_rect->w;
        float image_height = src_rect->h;
        if (position->scale == view_scale_meet) {
            float scale = image_height / image_width;
            if (view_height > view_width * scale) {
                dst_rect->h = view_width * scale;
                switch (position->align) {
                case view_align_x_min_y_mid:
                case view_align_x_mid_y_mid:
                case view_align_x_max_y_mid:
                    dst_rect->y += (view_height - dst_rect->h) * 0.5f;
                    break;
                case view_align_x_min_y_max:
                case view_align_x_mid_y_max:
                case view_align_x_max_y_max:
                    dst_rect->y += view_height - dst_rect->h;
                    break;
                default:
                    break;
                }
            }

            if (view_width > view_height / scale) {
                dst_rect->w = view_height / scale;
                switch (position->align) {
                case view_align_x_mid_y_min:
                case view_align_x_mid_y_mid:
                case view_align_x_mid_y_max:
                    dst_rect->x += (view_width - dst_rect->w) * 0.5f;
                    break;
                case view_align_x_max_y_min:
                case view_align_x_max_y_mid:
                case view_align_x_max_y_max:
                    dst_rect->x += view_width - dst_rect->w;
                    break;
                default:
                    break;
                }
            }
        }
        else if (position->scale == view_scale_slice) {
            float scale = image_height / image_width;
            if (view_height < view_width * scale) {
                src_rect->h = view_height * (image_width / view_width);
                switch (position->align) {
                case view_align_x_min_y_mid:
                case view_align_x_mid_y_mid:
                case view_align_x_max_y_mid:
                    src_rect->y += (image_height - src_rect->h) * 0.5f;
                    break;
                case view_align_x_min_y_max:
                case view_align_x_mid_y_max:
                case view_align_x_max_y_max:
                    src_rect->y += image_height - src_rect->h;
                    break;
                default:
                    break;
                }
            }

            if (view_width < view_height / scale) {
                src_rect->w = view_width * (image_height / view_height);
                switch (position->align) {
                case view_align_x_mid_y_min:
                case view_align_x_mid_y_mid:
                case view_align_x_mid_y_max:
                    src_rect->x += (image_width - src_rect->w) * 0.5f;
                    break;
                case view_align_x_max_y_min:
                case view_align_x_max_y_mid:
                case view_align_x_max_y_max:
                    src_rect->x += image_width - src_rect->w;
                    break;
                default:
                    break;
                }
            }
        }
    }

    static void render_element(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        switch (element->id) {
        case TAG_SVG:
            render_svg(element, context, state);
            break;
        case TAG_USE:
            render_use(element, context, state);
            break;
        case TAG_G:
            render_g(element, context, state);
            break;
        case TAG_LINE:
            render_line(element, context, state);
            break;
        case TAG_ELLIPSE:
            render_ellipse(element, context, state);
            break;
        case TAG_CIRCLE:
            render_circle(element, context, state);
            break;
        case TAG_RECT:
            render_rect(element, context, state);
            break;
        case TAG_POLYLINE:
        case TAG_POLYGON:
            render_poly(element, context, state);
            break;
        case TAG_PATH:
            render_path(element, context, state);
            break;
        }
    }

    static void render_children(const element_t* element, const render_context_t* context, render_state_t* state)
    {
        const element_t* child = element->first_child;
        while (child) {
            render_element(child, context, state);
            child = child->next_sibling;
        }
    }

    bool plutosvg_document_render(const plutosvg_document_t* document, const char* id, plutovg_canvas_t* canvas, const plutovg_color_t* current_color, plutosvg_palette_func_t palette_func, void* closure)
    {
        render_state_t state;
        state.parent = NULL;
        state.mode = render_mode_painting;
        state.opacity = 1.f;
        state.extents = INVALID_RECT;
        state.view_width = document->width;
        state.view_height = document->height;
        plutovg_canvas_get_matrix(canvas, &state.matrix);
        if (id == NULL) {
            state.element = document->root_element;
        }
        else {
            const string_t name = { id, strlen(id) };
            const element_t* element = find_element(document, &name);
            if (element == NULL)
                return false;
            state.element = element;
        }

        render_context_t context = { document, canvas, current_color, palette_func, closure };
        render_element(state.element, &context, &state);
        return true;
    }

    plutovg_surface_t* plutosvg_document_render_to_surface(const plutosvg_document_t* document, const char* id, int width, int height, const plutovg_color_t* current_color, plutosvg_palette_func_t palette_func, void* closure)
    {
        plutovg_rect_t extents = { 0, 0, document->width, document->height };
        if (id && !plutosvg_document_extents(document, id, &extents))
            return NULL;
        if (extents.w <= 0.f || extents.h <= 0.f)
            return NULL;
        if (width <= 0 && height <= 0) {
            width = (int)(ceilf(extents.w));
            height = (int)(ceilf(extents.h));
        }
        else if (width > 0 && height <= 0) {
            height = (int)(ceilf(width * extents.h / extents.w));
        }
        else if (height > 0 && width <= 0) {
            width = (int)(ceilf(height * extents.w / extents.h));
        }

        plutovg_surface_t* surface = plutovg_surface_create(width, height);
        if (surface == NULL)
            return NULL;
        plutovg_canvas_t* canvas = plutovg_canvas_create(surface);
        plutovg_canvas_scale(canvas, width / extents.w, height / extents.h);
        plutovg_canvas_translate(canvas, -extents.x, -extents.y);
        if (!plutosvg_document_render(document, id, canvas, current_color, palette_func, closure)) {
            plutovg_canvas_destroy(canvas);
            plutovg_surface_destroy(surface);
            return NULL;
        }

        plutovg_canvas_destroy(canvas);
        return surface;
    }


    float plutosvg_document_get_width(const plutosvg_document_t* document)
    {
        return document->width;
    }

    float plutosvg_document_get_height(const plutosvg_document_t* document)
    {
        return document->height;
    }

    bool plutosvg_document_extents(const plutosvg_document_t* document, const char* id, plutovg_rect_t* extents)
    {
        render_state_t state;
        state.parent = NULL;
        state.mode = render_mode_bounding;
        state.opacity = 1.f;
        state.extents = INVALID_RECT;
        state.view_width = document->width;
        state.view_height = document->height;
        plutovg_matrix_init_identity(&state.matrix);
        if (id == NULL) {
            state.element = document->root_element;
        }
        else {
            const string_t name = { id, strlen(id) };
            const element_t* element = find_element(document, &name);
            if (element == NULL) {
                *extents = EMPTY_RECT;
                return false;
            }

            state.element = element;
        }

        render_context_t context = { document, NULL, NULL, NULL, NULL };
        render_element(state.element, &context, &state);
        if (IS_INVALID_RECT(state.extents)) {
            *extents = EMPTY_RECT;
        }
        else {
            *extents = state.extents;
        }

        return true;
    }

    void plutovg_span_buffer_reset(plutovg_span_buffer_t* span_buffer)
    {
        plutovg_array_clear(span_buffer->spans);
        span_buffer->x = 0;
        span_buffer->y = 0;
        span_buffer->w = -1;
        span_buffer->h = -1;
    }

    void plutovg_span_buffer_init(plutovg_span_buffer_t* span_buffer)
    {
        plutovg_array_init(span_buffer->spans);
        plutovg_span_buffer_reset(span_buffer);
    }

    void plutovg_span_buffer_destroy(plutovg_span_buffer_t* span_buffer)
    {
        plutovg_array_destroy(span_buffer->spans);
    }

    void plutovg_span_buffer_copy(plutovg_span_buffer_t* span_buffer, const plutovg_span_buffer_t* source)
    {
        plutovg_array_clear(span_buffer->spans);
        plutovg_array_append(span_buffer->spans, source->spans);
        span_buffer->x = source->x;
        span_buffer->y = source->y;
        span_buffer->w = source->w;
        span_buffer->h = source->h;
    }

    void plutovg_span_buffer_intersect(plutovg_span_buffer_t* span_buffer, const plutovg_span_buffer_t* a, const plutovg_span_buffer_t* b)
    {
        plutovg_span_buffer_reset(span_buffer);
        plutovg_array_ensure(span_buffer->spans, plutovg_max(a->spans.size, b->spans.size));

        plutovg_span_t* a_spans = a->spans.data;
        plutovg_span_t* a_end = a_spans + a->spans.size;

        plutovg_span_t* b_spans = b->spans.data;
        plutovg_span_t* b_end = b_spans + b->spans.size;
        while (a_spans < a_end && b_spans < b_end) {
            if (b_spans->y > a_spans->y) {
                ++a_spans;
                continue;
            }

            if (a_spans->y != b_spans->y) {
                ++b_spans;
                continue;
            }

            int ax1 = a_spans->x;
            int ax2 = ax1 + a_spans->len;
            int bx1 = b_spans->x;
            int bx2 = bx1 + b_spans->len;
            if (bx1 < ax1 && bx2 < ax1) {
                ++b_spans;
                continue;
            }

            if (ax1 < bx1 && ax2 < bx1) {
                ++a_spans;
                continue;
            }

            int x = plutovg_max(ax1, bx1);
            int len = plutovg_min(ax2, bx2) - x;
            if (len) {
                plutovg_array_ensure(span_buffer->spans, 1);
                plutovg_span_t* span = span_buffer->spans.data + span_buffer->spans.size;
                span->x = x;
                span->len = len;
                span->y = a_spans->y;
                span->coverage = (a_spans->coverage * b_spans->coverage) / 255;
                span_buffer->spans.size += 1;
            }

            if (ax2 < bx2) {
                ++a_spans;
            }
            else {
                ++b_spans;
            }
        }
    }

    static void plutovg_span_buffer_update_extents(plutovg_span_buffer_t* span_buffer)
    {
        if (span_buffer->w != -1 && span_buffer->h != -1)
            return;
        if (span_buffer->spans.size == 0) {
            span_buffer->x = 0;
            span_buffer->y = 0;
            span_buffer->w = 0;
            span_buffer->h = 0;
            return;
        }

        plutovg_span_t* spans = span_buffer->spans.data;
        int x1 = INT_MAX;
        int y1 = spans[0].y;
        int x2 = 0;
        int y2 = spans[span_buffer->spans.size - 1].y;
        for (int i = 0; i < span_buffer->spans.size; i++) {
            if (spans[i].x < x1) x1 = spans[i].x;
            if (spans[i].x + spans[i].len > x2) x2 = spans[i].x + spans[i].len;
        }

        span_buffer->x = x1;
        span_buffer->y = y1;
        span_buffer->w = x2 - x1;
        span_buffer->h = y2 - y1 + 1;
    }

    void plutovg_span_buffer_extents(plutovg_span_buffer_t* span_buffer, plutovg_rect_t* extents)
    {
        plutovg_span_buffer_update_extents(span_buffer);
        extents->x = (float)span_buffer->x;
        extents->y = (float)span_buffer->y;
        extents->w = (float)span_buffer->w;
        extents->h = (float)span_buffer->h;
    }

    void plutovg_span_buffer_init_rect(plutovg_span_buffer_t* span_buffer, int x, int y, int width, int height)
    {
        plutovg_array_clear(span_buffer->spans);
        plutovg_array_ensure(span_buffer->spans, height);
        plutovg_span_t* spans = span_buffer->spans.data;
        for (int i = 0; i < height; i++) {
            spans[i].x = x;
            spans[i].y = y + i;
            spans[i].len = width;
            spans[i].coverage = 255;
        }

        span_buffer->x = x;
        span_buffer->y = y;
        span_buffer->w = width;
        span_buffer->h = height;
        span_buffer->spans.size = height;
    }

    typedef signed long  PVG_FT_Fixed;

    typedef signed int  PVG_FT_Int;

    typedef unsigned int  PVG_FT_UInt;

    typedef signed long  PVG_FT_Long;

    typedef unsigned long PVG_FT_ULong;

    typedef signed short  PVG_FT_Short;

    typedef unsigned char  PVG_FT_Byte;

    typedef unsigned char  PVG_FT_Bool;

    typedef int  PVG_FT_Error;

    typedef signed long  PVG_FT_Pos;

    typedef struct  PVG_FT_Vector_
    {
        PVG_FT_Pos  x;
        PVG_FT_Pos  y;

    } PVG_FT_Vector;


    typedef long long int           PVG_FT_Int64;
    typedef unsigned long long int  PVG_FT_UInt64;

    typedef signed int              PVG_FT_Int32;
    typedef unsigned int            PVG_FT_UInt32;

#define PVG_FT_BOOL( x )  ( (PVG_FT_Bool)( x ) )

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

    typedef struct  PVG_FT_Outline_
    {
        int       n_contours;      /* number of contours in glyph        */
        int       n_points;        /* number of points in the glyph      */

        PVG_FT_Vector* points;          /* the outline's points               */
        char* tags;            /* the points flags                   */
        int* contours;        /* the contour end points             */
        char* contours_flag;   /* the contour open flags             */

        int         flags;           /* outline masks                      */

    } PVG_FT_Outline;

#define PVG_FT_OUTLINE_NONE             0x0
#define PVG_FT_OUTLINE_OWNER            0x1
#define PVG_FT_OUTLINE_EVEN_ODD_FILL    0x2
#define PVG_FT_OUTLINE_REVERSE_FILL     0x4

    /* */

#define PVG_FT_CURVE_TAG( flag )  ( flag & 3 )

#define PVG_FT_CURVE_TAG_ON            1
#define PVG_FT_CURVE_TAG_CONIC         0
#define PVG_FT_CURVE_TAG_CUBIC         2


#define PVG_FT_Curve_Tag_On       PVG_FT_CURVE_TAG_ON
#define PVG_FT_Curve_Tag_Conic    PVG_FT_CURVE_TAG_CONIC
#define PVG_FT_Curve_Tag_Cubic    PVG_FT_CURVE_TAG_CUBIC

#define PVG_FT_RASTER_FLAG_DEFAULT  0x0
#define PVG_FT_RASTER_FLAG_AA       0x1
#define PVG_FT_RASTER_FLAG_DIRECT   0x2
#define PVG_FT_RASTER_FLAG_CLIP     0x4

    typedef struct  PVG_FT_Span_
    {
        int x;
        int len;
        int y;
        unsigned char coverage;

    } PVG_FT_Span;

    typedef struct  PVG_FT_BBox_
    {
        PVG_FT_Pos  xMin, yMin;
        PVG_FT_Pos  xMax, yMax;

    } PVG_FT_BBox;

    typedef void
    (*PVG_FT_SpanFunc)(int             count,
        const PVG_FT_Span* spans,
        void* user);

#define PVG_FT_Raster_Span_Func  PVG_FT_SpanFunc

    typedef struct  PVG_FT_Raster_Params_
    {
        const void* source;
        int                     flags;
        PVG_FT_SpanFunc          gray_spans;
        void* user;
        PVG_FT_BBox              clip_box;

    } PVG_FT_Raster_Params;


#define PVG_FT_MIN( a, b )  ( (a) < (b) ? (a) : (b) )
#define PVG_FT_MAX( a, b )  ( (a) > (b) ? (a) : (b) )

#define PVG_FT_ABS( a )     ( (a) < 0 ? -(a) : (a) )

#define PVG_FT_HYPOT( x, y )                 \
          ( x = PVG_FT_ABS( x ),             \
            y = PVG_FT_ABS( y ),             \
            x > y ? x + ( 3 * y >> 3 )   \
                  : y + ( 3 * x >> 3 ) )

    typedef PVG_FT_Fixed  PVG_FT_Angle;
#define PVG_FT_ANGLE_PI  ( 180L << 16 )
#define PVG_FT_ANGLE_2PI  ( PVG_FT_ANGLE_PI * 2 )
#define PVG_FT_ANGLE_PI2  ( PVG_FT_ANGLE_PI / 2 )
#define PVG_FT_ANGLE_PI4  ( PVG_FT_ANGLE_PI / 4 )

#if defined(_MSC_VER)

    static inline int clz(unsigned int x) {
        unsigned long r = 0;
        if (_BitScanReverse(&r, x))
            return 31 - r;
        return 32;
    }
#define PVG_FT_MSB(x)  (31 - clz(x))
#elif defined(__GNUC__)
#define PVG_FT_MSB(x)  (31 - __builtin_clz(x))
#else
    static inline int clz(unsigned int x) {
        int n = 0;
        if (x == 0) return 32;
        if (x <= 0x0000FFFFU) { n += 16; x <<= 16; }
        if (x <= 0x00FFFFFFU) { n += 8; x <<= 8; }
        if (x <= 0x0FFFFFFFU) { n += 4; x <<= 4; }
        if (x <= 0x3FFFFFFFU) { n += 2; x <<= 2; }
        if (x <= 0x7FFFFFFFU) { n += 1; }
        return n;
    }
#define PVG_FT_MSB(x)  (31 - clz(x))
#endif


#define PVG_FT_PAD_FLOOR(x, n) ((x) & ~((n)-1))
#define PVG_FT_PAD_ROUND(x, n) PVG_FT_PAD_FLOOR((x) + ((n) / 2), n)
#define PVG_FT_PAD_CEIL(x, n) PVG_FT_PAD_FLOOR((x) + ((n)-1), n)

#define PVG_FT_BEGIN_STMNT do {
#define PVG_FT_END_STMNT } while (0)

    /* transfer sign leaving a positive number */
#define PVG_FT_MOVE_SIGN(x, s) \
    PVG_FT_BEGIN_STMNT         \
    if (x < 0) {              \
        x = -x;               \
        s = -s;               \
    }                         \
    PVG_FT_END_STMNT

    PVG_FT_Long PVG_FT_MulFix(PVG_FT_Long a, PVG_FT_Long b)
    {
        PVG_FT_Int  s = 1;
        PVG_FT_Long c;

        PVG_FT_MOVE_SIGN(a, s);
        PVG_FT_MOVE_SIGN(b, s);

        c = (PVG_FT_Long)(((PVG_FT_Int64)a * b + 0x8000L) >> 16);

        return (s > 0) ? c : -c;
    }

    PVG_FT_Long PVG_FT_MulDiv(PVG_FT_Long a, PVG_FT_Long b, PVG_FT_Long c)
    {
        PVG_FT_Int  s = 1;
        PVG_FT_Long d;

        PVG_FT_MOVE_SIGN(a, s);
        PVG_FT_MOVE_SIGN(b, s);
        PVG_FT_MOVE_SIGN(c, s);

        d = (PVG_FT_Long)(c > 0 ? ((PVG_FT_Int64)a * b + (c >> 1)) / c : 0x7FFFFFFFL);

        return (s > 0) ? d : -d;
    }

    PVG_FT_Long PVG_FT_DivFix(PVG_FT_Long a, PVG_FT_Long b)
    {
        PVG_FT_Int  s = 1;
        PVG_FT_Long q;

        PVG_FT_MOVE_SIGN(a, s);
        PVG_FT_MOVE_SIGN(b, s);

        q = (PVG_FT_Long)(b > 0 ? (((PVG_FT_UInt64)a << 16) + (b >> 1)) / b
            : 0x7FFFFFFFL);

        return (s < 0 ? -q : q);
    }

    /* the Cordic shrink factor 0.858785336480436 * 2^32 */
#define PVG_FT_TRIG_SCALE 0xDBD95B16UL

/* the highest bit in overflow-safe vector components, */
/* MSB of 0.858785336480436 * sqrt(0.5) * 2^30         */
#define PVG_FT_TRIG_SAFE_MSB 29

/* this table was generated for PVG_FT_PI = 180L << 16, i.e. degrees */
#define PVG_FT_TRIG_MAX_ITERS 23

    static const PVG_FT_Fixed ft_trig_arctan_table[] = {
        1740967L, 919879L, 466945L, 234379L, 117304L, 58666L, 29335L, 14668L,
        7334L,    3667L,   1833L,   917L,    458L,    229L,   115L,   57L,
        29L,      14L,     7L,      4L,      2L,      1L };

    /* multiply a given value by the CORDIC shrink factor */
    static PVG_FT_Fixed ft_trig_downscale(PVG_FT_Fixed val)
    {
        PVG_FT_Fixed s;
        PVG_FT_Int64 v;

        s = val;
        val = PVG_FT_ABS(val);

        v = (val * (PVG_FT_Int64)PVG_FT_TRIG_SCALE) + 0x100000000UL;
        val = (PVG_FT_Fixed)(v >> 32);

        return (s >= 0) ? val : -val;
    }

    /* undefined and never called for zero vector */
    static PVG_FT_Int ft_trig_prenorm(PVG_FT_Vector* vec)
    {
        PVG_FT_Pos x, y;
        PVG_FT_Int shift;

        x = vec->x;
        y = vec->y;

        shift = PVG_FT_MSB(PVG_FT_ABS(x) | PVG_FT_ABS(y));

        if (shift <= PVG_FT_TRIG_SAFE_MSB) {
            shift = PVG_FT_TRIG_SAFE_MSB - shift;
            vec->x = (PVG_FT_Pos)((PVG_FT_ULong)x << shift);
            vec->y = (PVG_FT_Pos)((PVG_FT_ULong)y << shift);
        }
        else {
            shift -= PVG_FT_TRIG_SAFE_MSB;
            vec->x = x >> shift;
            vec->y = y >> shift;
            shift = -shift;
        }

        return shift;
    }

    static void ft_trig_pseudo_rotate(PVG_FT_Vector* vec, PVG_FT_Angle theta)
    {
        PVG_FT_Int          i;
        PVG_FT_Fixed        x, y, xtemp, b;
        const PVG_FT_Fixed* arctanptr;

        x = vec->x;
        y = vec->y;

        /* Rotate inside [-PI/4,PI/4] sector */
        while (theta < -PVG_FT_ANGLE_PI4) {
            xtemp = y;
            y = -x;
            x = xtemp;
            theta += PVG_FT_ANGLE_PI2;
        }

        while (theta > PVG_FT_ANGLE_PI4) {
            xtemp = -y;
            y = x;
            x = xtemp;
            theta -= PVG_FT_ANGLE_PI2;
        }

        arctanptr = ft_trig_arctan_table;

        /* Pseudorotations, with right shifts */
        for (i = 1, b = 1; i < PVG_FT_TRIG_MAX_ITERS; b <<= 1, i++) {
            PVG_FT_Fixed v1 = ((y + b) >> i);
            PVG_FT_Fixed v2 = ((x + b) >> i);
            if (theta < 0) {
                xtemp = x + v1;
                y = y - v2;
                x = xtemp;
                theta += *arctanptr++;
            }
            else {
                xtemp = x - v1;
                y = y + v2;
                x = xtemp;
                theta -= *arctanptr++;
            }
        }

        vec->x = x;
        vec->y = y;
    }

    static void ft_trig_pseudo_polarize(PVG_FT_Vector* vec)
    {
        PVG_FT_Angle        theta;
        PVG_FT_Int          i;
        PVG_FT_Fixed        x, y, xtemp, b;
        const PVG_FT_Fixed* arctanptr;

        x = vec->x;
        y = vec->y;

        /* Get the vector into [-PI/4,PI/4] sector */
        if (y > x) {
            if (y > -x) {
                theta = PVG_FT_ANGLE_PI2;
                xtemp = y;
                y = -x;
                x = xtemp;
            }
            else {
                theta = y > 0 ? PVG_FT_ANGLE_PI : -PVG_FT_ANGLE_PI;
                x = -x;
                y = -y;
            }
        }
        else {
            if (y < -x) {
                theta = -PVG_FT_ANGLE_PI2;
                xtemp = -y;
                y = x;
                x = xtemp;
            }
            else {
                theta = 0;
            }
        }

        arctanptr = ft_trig_arctan_table;

        /* Pseudorotations, with right shifts */
        for (i = 1, b = 1; i < PVG_FT_TRIG_MAX_ITERS; b <<= 1, i++) {
            PVG_FT_Fixed v1 = ((y + b) >> i);
            PVG_FT_Fixed v2 = ((x + b) >> i);
            if (y > 0) {
                xtemp = x + v1;
                y = y - v2;
                x = xtemp;
                theta += *arctanptr++;
            }
            else {
                xtemp = x - v1;
                y = y + v2;
                x = xtemp;
                theta -= *arctanptr++;
            }
        }

        /* round theta */
        if (theta >= 0)
            theta = PVG_FT_PAD_ROUND(theta, 32);
        else
            theta = -PVG_FT_PAD_ROUND(-theta, 32);

        vec->x = x;
        vec->y = theta;
    }

    /* documentation is in fttrigon.h */

    PVG_FT_Fixed PVG_FT_Cos(PVG_FT_Angle angle)
    {
        PVG_FT_Vector v;

        v.x = PVG_FT_TRIG_SCALE >> 8;
        v.y = 0;
        ft_trig_pseudo_rotate(&v, angle);

        return (v.x + 0x80L) >> 8;
    }

    /* documentation is in fttrigon.h */

    PVG_FT_Fixed PVG_FT_Sin(PVG_FT_Angle angle)
    {
        return PVG_FT_Cos(PVG_FT_ANGLE_PI2 - angle);
    }

    /* documentation is in fttrigon.h */

    PVG_FT_Fixed PVG_FT_Tan(PVG_FT_Angle angle)
    {
        PVG_FT_Vector v;

        v.x = PVG_FT_TRIG_SCALE >> 8;
        v.y = 0;
        ft_trig_pseudo_rotate(&v, angle);

        return PVG_FT_DivFix(v.y, v.x);
    }

    /* documentation is in fttrigon.h */

    PVG_FT_Angle PVG_FT_Atan2(PVG_FT_Fixed dx, PVG_FT_Fixed dy)
    {
        PVG_FT_Vector v;

        if (dx == 0 && dy == 0) return 0;

        v.x = dx;
        v.y = dy;
        ft_trig_prenorm(&v);
        ft_trig_pseudo_polarize(&v);

        return v.y;
    }

    /* documentation is in fttrigon.h */

    void PVG_FT_Vector_Unit(PVG_FT_Vector* vec, PVG_FT_Angle angle)
    {
        vec->x = PVG_FT_TRIG_SCALE >> 8;
        vec->y = 0;
        ft_trig_pseudo_rotate(vec, angle);
        vec->x = (vec->x + 0x80L) >> 8;
        vec->y = (vec->y + 0x80L) >> 8;
    }

    void PVG_FT_Vector_Rotate(PVG_FT_Vector* vec, PVG_FT_Angle angle)
    {
        PVG_FT_Int     shift;
        PVG_FT_Vector  v = *vec;

        if (v.x == 0 && v.y == 0)
            return;

        shift = ft_trig_prenorm(&v);
        ft_trig_pseudo_rotate(&v, angle);
        v.x = ft_trig_downscale(v.x);
        v.y = ft_trig_downscale(v.y);

        if (shift > 0)
        {
            PVG_FT_Int32  half = (PVG_FT_Int32)1L << (shift - 1);


            vec->x = (v.x + half - (v.x < 0)) >> shift;
            vec->y = (v.y + half - (v.y < 0)) >> shift;
        }
        else
        {
            shift = -shift;
            vec->x = (PVG_FT_Pos)((PVG_FT_ULong)v.x << shift);
            vec->y = (PVG_FT_Pos)((PVG_FT_ULong)v.y << shift);
        }
    }

    /* documentation is in fttrigon.h */

    PVG_FT_Fixed PVG_FT_Vector_Length(PVG_FT_Vector* vec)
    {
        PVG_FT_Int    shift;
        PVG_FT_Vector v;

        v = *vec;

        /* handle trivial cases */
        if (v.x == 0) {
            return PVG_FT_ABS(v.y);
        }
        else if (v.y == 0) {
            return PVG_FT_ABS(v.x);
        }

        /* general case */
        shift = ft_trig_prenorm(&v);
        ft_trig_pseudo_polarize(&v);

        v.x = ft_trig_downscale(v.x);

        if (shift > 0) return (v.x + (1 << (shift - 1))) >> shift;

        return (PVG_FT_Fixed)((PVG_FT_UInt32)v.x << -shift);
    }

    /* documentation is in fttrigon.h */

    void PVG_FT_Vector_Polarize(PVG_FT_Vector* vec, PVG_FT_Fixed* length,
        PVG_FT_Angle* angle)
    {
        PVG_FT_Int    shift;
        PVG_FT_Vector v;

        v = *vec;

        if (v.x == 0 && v.y == 0) return;

        shift = ft_trig_prenorm(&v);
        ft_trig_pseudo_polarize(&v);

        v.x = ft_trig_downscale(v.x);

        *length = (shift >= 0) ? (v.x >> shift)
            : (PVG_FT_Fixed)((PVG_FT_UInt32)v.x << -shift);
        *angle = v.y;
    }

    /* documentation is in fttrigon.h */

    void PVG_FT_Vector_From_Polar(PVG_FT_Vector* vec, PVG_FT_Fixed length,
        PVG_FT_Angle angle)
    {
        vec->x = length;
        vec->y = 0;

        PVG_FT_Vector_Rotate(vec, angle);
    }

    /* documentation is in fttrigon.h */

    PVG_FT_Angle PVG_FT_Angle_Diff(PVG_FT_Angle  angle1, PVG_FT_Angle  angle2)
    {
        PVG_FT_Angle  delta = angle2 - angle1;

        while (delta <= -PVG_FT_ANGLE_PI)
            delta += PVG_FT_ANGLE_2PI;

        while (delta > PVG_FT_ANGLE_PI)
            delta -= PVG_FT_ANGLE_2PI;

        return delta;
    }


#define PVG_FT_BEGIN_STMNT  do {

#define pvg_ft_setjmp   setjmp
#define pvg_ft_longjmp  longjmp
#define pvg_ft_jmp_buf  jmp_buf

    typedef ptrdiff_t  PVG_FT_PtrDist;

#define ErrRaster_Invalid_Mode      -2
#define ErrRaster_Invalid_Outline   -1
#define ErrRaster_Invalid_Argument  -3
#define ErrRaster_Memory_Overflow   -4
#define ErrRaster_OutOfMemory       -6


#define PVG_FT_MINIMUM_POOL_SIZE 8192

#define RAS_ARG   PWorker  worker
#define RAS_ARG_  PWorker  worker,

#define RAS_VAR   worker
#define RAS_VAR_  worker,

#define ras       (*worker)

    /* must be at least 6 bits! */
#define PIXEL_BITS  8

#define ONE_PIXEL       ( 1L << PIXEL_BITS )
#define TRUNC( x )      (TCoord)( (x) >> PIXEL_BITS )
#define FRACT( x )      (TCoord)( (x) & ( ONE_PIXEL - 1 ) )

#if PIXEL_BITS >= 6
#define UPSCALE( x )    ( (x) * ( ONE_PIXEL >> 6 ) )
#define DOWNSCALE( x )  ( (x) >> ( PIXEL_BITS - 6 ) )
#else
#define UPSCALE( x )    ( (x) >> ( 6 - PIXEL_BITS ) )
#define DOWNSCALE( x )  ( (x) * ( 64 >> PIXEL_BITS ) )
#endif

/* Compute `dividend / divisor' and return both its quotient and     */
/* remainder, cast to a specific type.  This macro also ensures that */
/* the remainder is always positive.                                 */
#define PVG_FT_DIV_MOD( type, dividend, divisor, quotient, remainder ) \
PVG_FT_BEGIN_STMNT                                                   \
  (quotient)  = (type)( (dividend) / (divisor) );                \
  (remainder) = (type)( (dividend) % (divisor) );                \
  if ( (remainder) < 0 )                                         \
  {                                                              \
    (quotient)--;                                                \
    (remainder) += (type)(divisor);                              \
  }                                                              \
PVG_FT_END_STMNT

  /* These macros speed up repetitive divisions by replacing them */
  /* with multiplications and right shifts.                       */
#define PVG_FT_UDIVPREP( b )                                       \
  long  b ## _r = (long)( ULONG_MAX >> PIXEL_BITS ) / ( b )
#define PVG_FT_UDIV( a, b )                                        \
  ( ( (unsigned long)( a ) * (unsigned long)( b ## _r ) ) >>   \
    ( sizeof( long ) * CHAR_BIT - PIXEL_BITS ) )


    typedef long   TCoord;   /* integer scanline/pixel coordinate */
    typedef long   TPos;     /* sub-pixel coordinate              */
    typedef long   TArea;   /* cell areas, coordinate products   */

    /* maximal number of gray spans in a call to the span callback */
#define PVG_FT_MAX_GRAY_SPANS  256


    typedef struct TCell_* PCell;

    typedef struct  TCell_
    {
        int    x;
        int    cover;
        TArea  area;
        PCell  next;

    } TCell;


    typedef struct  TWorker_
    {
        TCoord  ex, ey;
        TPos    min_ex, max_ex;
        TPos    min_ey, max_ey;
        TPos    count_ex, count_ey;

        TArea   area;
        int     cover;
        int     invalid;

        PCell   cells;
        PVG_FT_PtrDist     max_cells;
        PVG_FT_PtrDist     num_cells;

        TPos    x, y;

        PVG_FT_Outline  outline;
        PVG_FT_BBox     clip_box;

        int clip_flags;
        int clipping;

        PVG_FT_Span     gray_spans[PVG_FT_MAX_GRAY_SPANS];
        int         num_gray_spans;
        int         skip_spans;

        PVG_FT_Raster_Span_Func  render_span;
        void* render_span_data;

        int  band_size;
        int  band_shoot;

        pvg_ft_jmp_buf  jump_buffer;

        void* buffer;
        long        buffer_size;

        PCell* ycells;
        TPos       ycount;
    } TWorker, * PWorker;


    /*************************************************************************/
    /*                                                                       */
    /* Initialize the cells table.                                           */
    /*                                                                       */
    static void
        gray_init_cells(RAS_ARG_ void* buffer,
            long   byte_size)
    {
        ras.buffer = buffer;
        ras.buffer_size = byte_size;

        ras.ycells = (PCell*)buffer;
        ras.cells = NULL;
        ras.max_cells = 0;
        ras.num_cells = 0;
        ras.area = 0;
        ras.cover = 0;
        ras.invalid = 1;
    }


    /*************************************************************************/
    /*                                                                       */
    /* Compute the outline bounding box.                                     */
    /*                                                                       */
    static void
        gray_compute_cbox(RAS_ARG)
    {
        PVG_FT_Outline* outline = &ras.outline;
        PVG_FT_Vector* vec = outline->points;
        PVG_FT_Vector* limit = vec + outline->n_points;


        if (outline->n_points <= 0)
        {
            ras.min_ex = ras.max_ex = 0;
            ras.min_ey = ras.max_ey = 0;
            return;
        }

        ras.min_ex = ras.max_ex = vec->x;
        ras.min_ey = ras.max_ey = vec->y;

        vec++;

        for (; vec < limit; vec++)
        {
            TPos  x = vec->x;
            TPos  y = vec->y;


            if (x < ras.min_ex) ras.min_ex = x;
            if (x > ras.max_ex) ras.max_ex = x;
            if (y < ras.min_ey) ras.min_ey = y;
            if (y > ras.max_ey) ras.max_ey = y;
        }

        /* truncate the bounding box to integer pixels */
        ras.min_ex = ras.min_ex >> 6;
        ras.min_ey = ras.min_ey >> 6;
        ras.max_ex = (ras.max_ex + 63) >> 6;
        ras.max_ey = (ras.max_ey + 63) >> 6;
    }


    /*************************************************************************/
    /*                                                                       */
    /* Record the current cell in the table.                                 */
    /*                                                                       */
    static PCell
        gray_find_cell(RAS_ARG)
    {
        PCell* pcell, cell;
        TPos    x = ras.ex;


        if (x > ras.count_ex)
            x = ras.count_ex;

        pcell = &ras.ycells[ras.ey];
        for (;;)
        {
            cell = *pcell;
            if (cell == NULL || cell->x > x)
                break;

            if (cell->x == x)
                goto Exit;

            pcell = &cell->next;
        }

        if (ras.num_cells >= ras.max_cells)
            pvg_ft_longjmp(ras.jump_buffer, 1);

        cell = ras.cells + ras.num_cells++;
        cell->x = x;
        cell->area = 0;
        cell->cover = 0;

        cell->next = *pcell;
        *pcell = cell;

    Exit:
        return cell;
    }


    static void
        gray_record_cell(RAS_ARG)
    {
        if (ras.area | ras.cover)
        {
            PCell  cell = gray_find_cell(RAS_VAR);


            cell->area += ras.area;
            cell->cover += ras.cover;
        }
    }


    /*************************************************************************/
    /*                                                                       */
    /* Set the current cell to a new position.                               */
    /*                                                                       */
    static void
        gray_set_cell(RAS_ARG_ TCoord  ex,
            TCoord  ey)
    {
        /* Move the cell pointer to a new position.  We set the `invalid'      */
        /* flag to indicate that the cell isn't part of those we're interested */
        /* in during the render phase.  This means that:                       */
        /*                                                                     */
        /* . the new vertical position must be within min_ey..max_ey-1.        */
        /* . the new horizontal position must be strictly less than max_ex     */
        /*                                                                     */
        /* Note that if a cell is to the left of the clipping region, it is    */
        /* actually set to the (min_ex-1) horizontal position.                 */

        /* All cells that are on the left of the clipping region go to the */
        /* min_ex - 1 horizontal position.                                 */
        ey -= ras.min_ey;

        if (ex > ras.max_ex)
            ex = ras.max_ex;

        ex -= ras.min_ex;
        if (ex < 0)
            ex = -1;

        /* are we moving to a different cell ? */
        if (ex != ras.ex || ey != ras.ey)
        {
            /* record the current one if it is valid */
            if (!ras.invalid)
                gray_record_cell(RAS_VAR);

            ras.area = 0;
            ras.cover = 0;
            ras.ex = ex;
            ras.ey = ey;
        }

        ras.invalid = ((unsigned int)ey >= (unsigned int)ras.count_ey ||
            ex >= ras.count_ex);
    }


    /*************************************************************************/
    /*                                                                       */
    /* Start a new contour at a given cell.                                  */
    /*                                                                       */
    static void
        gray_start_cell(RAS_ARG_ TCoord  ex,
            TCoord  ey)
    {
        if (ex > ras.max_ex)
            ex = (TCoord)(ras.max_ex);

        if (ex < ras.min_ex)
            ex = (TCoord)(ras.min_ex - 1);

        ras.area = 0;
        ras.cover = 0;
        ras.ex = ex - ras.min_ex;
        ras.ey = ey - ras.min_ey;
        ras.invalid = 0;

        gray_set_cell(RAS_VAR_ ex, ey);
    }

    // The new render-line implementation is not yet used
#if 1

  /*************************************************************************/
  /*                                                                       */
  /* Render a scanline as one or more cells.                               */
  /*                                                                       */
    static void
        gray_render_scanline(RAS_ARG_ TCoord  ey,
            TPos    x1,
            TCoord  y1,
            TPos    x2,
            TCoord  y2)
    {
        TCoord  ex1, ex2, fx1, fx2, first, dy, delta, mod;
        TPos    p, dx;
        int     incr;


        ex1 = TRUNC(x1);
        ex2 = TRUNC(x2);

        /* trivial case.  Happens often */
        if (y1 == y2)
        {
            gray_set_cell(RAS_VAR_ ex2, ey);
            return;
        }

        fx1 = FRACT(x1);
        fx2 = FRACT(x2);

        /* everything is located in a single cell.  That is easy! */
        /*                                                        */
        if (ex1 == ex2)
            goto End;

        /* ok, we'll have to render a run of adjacent cells on the same */
        /* scanline...                                                  */
        /*                                                              */
        dx = x2 - x1;
        dy = y2 - y1;

        if (dx > 0)
        {
            p = (ONE_PIXEL - fx1) * dy;
            first = ONE_PIXEL;
            incr = 1;
        }
        else {
            p = fx1 * dy;
            first = 0;
            incr = -1;
            dx = -dx;
        }

        PVG_FT_DIV_MOD(TCoord, p, dx, delta, mod);

        ras.area += (TArea)(fx1 + first) * delta;
        ras.cover += delta;
        y1 += delta;
        ex1 += incr;
        gray_set_cell(RAS_VAR_ ex1, ey);

        if (ex1 != ex2)
        {
            TCoord  lift, rem;


            p = ONE_PIXEL * dy;
            PVG_FT_DIV_MOD(TCoord, p, dx, lift, rem);

            do
            {
                delta = lift;
                mod += rem;
                if (mod >= (TCoord)dx)
                {
                    mod -= (TCoord)dx;
                    delta++;
                }

                ras.area += (TArea)(ONE_PIXEL * delta);
                ras.cover += delta;
                y1 += delta;
                ex1 += incr;
                gray_set_cell(RAS_VAR_ ex1, ey);
            } while (ex1 != ex2);
        }
        fx1 = ONE_PIXEL - first;

    End:
        dy = y2 - y1;

        ras.area += (TArea)((fx1 + fx2) * dy);
        ras.cover += dy;
    }


    /*************************************************************************/
    /*                                                                       */
    /* Render a given line as a series of scanlines.                         */
    /*                                                                       */
    static void
        gray_render_line(RAS_ARG_ TPos from_x, TPos from_y, TPos  to_x, TPos  to_y)
    {
        TCoord  ey1, ey2, fy1, fy2, first, delta, mod;
        TPos    p, dx, dy, x, x2;
        int     incr;

        ey1 = TRUNC(from_y);
        ey2 = TRUNC(to_y);     /* if (ey2 >= ras.max_ey) ey2 = ras.max_ey-1; */

        /* perform vertical clipping */
        if ((ey1 >= ras.max_ey && ey2 >= ras.max_ey) ||
            (ey1 < ras.min_ey && ey2 < ras.min_ey))
            return;

        fy1 = FRACT(from_y);
        fy2 = FRACT(to_y);

        /* everything is on a single scanline */
        if (ey1 == ey2)
        {
            gray_render_scanline(RAS_VAR_ ey1, from_x, fy1, to_x, fy2);
            return;
        }

        dx = to_x - from_x;
        dy = to_y - from_y;

        /* vertical line - avoid calling gray_render_scanline */
        if (dx == 0)
        {
            TCoord  ex = TRUNC(from_x);
            TCoord  two_fx = FRACT(from_x) << 1;
            TPos    area, max_ey1;


            if (dy > 0)
            {
                first = ONE_PIXEL;
            }
            else
            {
                first = 0;
            }

            delta = first - fy1;
            ras.area += (TArea)two_fx * delta;
            ras.cover += delta;

            delta = first + first - ONE_PIXEL;
            area = (TArea)two_fx * delta;
            max_ey1 = ras.count_ey + ras.min_ey;
            if (dy < 0) {
                if (ey1 > max_ey1) {
                    ey1 = (max_ey1 > ey2) ? max_ey1 : ey2;
                    gray_set_cell(&ras, ex, ey1);
                }
                else {
                    ey1--;
                    gray_set_cell(&ras, ex, ey1);
                }
                while (ey1 > ey2 && ey1 >= ras.min_ey)
                {
                    ras.area += area;
                    ras.cover += delta;
                    ey1--;

                    gray_set_cell(&ras, ex, ey1);
                }
                if (ey1 != ey2) {
                    ey1 = ey2;
                    gray_set_cell(&ras, ex, ey1);
                }
            }
            else {
                if (ey1 < ras.min_ey) {
                    ey1 = (ras.min_ey < ey2) ? ras.min_ey : ey2;
                    gray_set_cell(&ras, ex, ey1);
                }
                else {
                    ey1++;
                    gray_set_cell(&ras, ex, ey1);
                }
                while (ey1 < ey2 && ey1 < max_ey1)
                {
                    ras.area += area;
                    ras.cover += delta;
                    ey1++;

                    gray_set_cell(&ras, ex, ey1);
                }
                if (ey1 != ey2) {
                    ey1 = ey2;
                    gray_set_cell(&ras, ex, ey1);
                }
            }

            delta = (int)(fy2 - ONE_PIXEL + first);
            ras.area += (TArea)two_fx * delta;
            ras.cover += delta;

            return;
        }

        /* ok, we have to render several scanlines */
        if (dy > 0)
        {
            p = (ONE_PIXEL - fy1) * dx;
            first = ONE_PIXEL;
            incr = 1;
        }
        else
        {
            p = fy1 * dx;
            first = 0;
            incr = -1;
            dy = -dy;
        }

        /* the fractional part of x-delta is mod/dy. It is essential to */
        /* keep track of its accumulation for accurate rendering.       */
        PVG_FT_DIV_MOD(TCoord, p, dy, delta, mod);

        x = from_x + delta;
        gray_render_scanline(RAS_VAR_ ey1, from_x, fy1, x, (TCoord)first);

        ey1 += incr;
        gray_set_cell(RAS_VAR_ TRUNC(x), ey1);

        if (ey1 != ey2)
        {
            TCoord  lift, rem;


            p = ONE_PIXEL * dx;
            PVG_FT_DIV_MOD(TCoord, p, dy, lift, rem);

            do
            {
                delta = lift;
                mod += rem;
                if (mod >= (TCoord)dy)
                {
                    mod -= (TCoord)dy;
                    delta++;
                }

                x2 = x + delta;
                gray_render_scanline(RAS_VAR_ ey1,
                    x, ONE_PIXEL - first,
                    x2, first);
                x = x2;

                ey1 += incr;
                gray_set_cell(RAS_VAR_ TRUNC(x), ey1);
            } while (ey1 != ey2);
        }

        gray_render_scanline(RAS_VAR_ ey1,
            x, ONE_PIXEL - first,
            to_x, fy2);
    }


#else

  /*************************************************************************/
  /*                                                                       */
  /* Render a straight line across multiple cells in any direction.        */
  /*                                                                       */
    static void
        gray_render_line(RAS_ARG_ TPos from_x, TPos from_y, TPos  to_x, TPos  to_y)
    {
        TPos    dx, dy, fx1, fy1, fx2, fy2;
        TCoord  ex1, ex2, ey1, ey2;


        ex1 = TRUNC(from_x);
        ex2 = TRUNC(to_x);
        ey1 = TRUNC(from_y);
        ey2 = TRUNC(to_y);

        /* perform vertical clipping */
        if ((ey1 >= ras.max_ey && ey2 >= ras.max_ey) ||
            (ey1 < ras.min_ey && ey2 < ras.min_ey))
            return;

        dx = to_x - from_x;
        dy = to_y - from_y;

        fx1 = FRACT(from_x);
        fy1 = FRACT(from_y);

        if (ex1 == ex2 && ey1 == ey2)       /* inside one cell */
            ;
        else if (dy == 0) /* ex1 != ex2 */  /* any horizontal line */
        {
            ex1 = ex2;
            gray_set_cell(RAS_VAR_ ex1, ey1);
        }
        else if (dx == 0)
        {
            if (dy > 0)                       /* vertical line up */
                do
                {
                    fy2 = ONE_PIXEL;
                    ras.cover += (fy2 - fy1);
                    ras.area += (fy2 - fy1) * fx1 * 2;
                    fy1 = 0;
                    ey1++;
                    gray_set_cell(RAS_VAR_ ex1, ey1);
                } while (ey1 != ey2);
            else                                /* vertical line down */
                do
                {
                    fy2 = 0;
                    ras.cover += (fy2 - fy1);
                    ras.area += (fy2 - fy1) * fx1 * 2;
                    fy1 = ONE_PIXEL;
                    ey1--;
                    gray_set_cell(RAS_VAR_ ex1, ey1);
                } while (ey1 != ey2);
        }
        else                                  /* any other line */
        {
            TArea  prod = dx * fy1 - dy * fx1;
            PVG_FT_UDIVPREP(dx);
            PVG_FT_UDIVPREP(dy);


            /* The fundamental value `prod' determines which side and the  */
            /* exact coordinate where the line exits current cell.  It is  */
            /* also easily updated when moving from one cell to the next.  */
            do
            {
                if (prod <= 0 &&
                    prod - dx * ONE_PIXEL > 0) /* left */
                {
                    fx2 = 0;
                    fy2 = (TPos)PVG_FT_UDIV(-prod, -dx);
                    prod -= dy * ONE_PIXEL;
                    ras.cover += (fy2 - fy1);
                    ras.area += (fy2 - fy1) * (fx1 + fx2);
                    fx1 = ONE_PIXEL;
                    fy1 = fy2;
                    ex1--;
                }
                else if (prod - dx * ONE_PIXEL <= 0 &&
                    prod - dx * ONE_PIXEL + dy * ONE_PIXEL > 0) /* up */
                {
                    prod -= dx * ONE_PIXEL;
                    fx2 = (TPos)PVG_FT_UDIV(-prod, dy);
                    fy2 = ONE_PIXEL;
                    ras.cover += (fy2 - fy1);
                    ras.area += (fy2 - fy1) * (fx1 + fx2);
                    fx1 = fx2;
                    fy1 = 0;
                    ey1++;
                }
                else if (prod - dx * ONE_PIXEL + dy * ONE_PIXEL <= 0 &&
                    prod + dy * ONE_PIXEL >= 0) /* right */
                {
                    prod += dy * ONE_PIXEL;
                    fx2 = ONE_PIXEL;
                    fy2 = (TPos)PVG_FT_UDIV(prod, dx);
                    ras.cover += (fy2 - fy1);
                    ras.area += (fy2 - fy1) * (fx1 + fx2);
                    fx1 = 0;
                    fy1 = fy2;
                    ex1++;
                }
                else /* ( prod                  + dy * ONE_PIXEL <  0 &&
                          prod                                   >  0 )    down */
                {
                    fx2 = (TPos)PVG_FT_UDIV(prod, -dy);
                    fy2 = 0;
                    prod += dx * ONE_PIXEL;
                    ras.cover += (fy2 - fy1);
                    ras.area += (fy2 - fy1) * (fx1 + fx2);
                    fx1 = fx2;
                    fy1 = ONE_PIXEL;
                    ey1--;
                }

                gray_set_cell(RAS_VAR_ ex1, ey1);
            } while (ex1 != ex2 || ey1 != ey2);
        }

        fx2 = FRACT(to_x);
        fy2 = FRACT(to_y);

        ras.cover += (fy2 - fy1);
        ras.area += (fy2 - fy1) * (fx1 + fx2);
    }

#endif

    static int
        gray_clip_flags(RAS_ARG_ TPos x, TPos y)
    {
        return ((x > ras.clip_box.xMax) << 0) | ((y > ras.clip_box.yMax) << 1) |
            ((x < ras.clip_box.xMin) << 2) | ((y < ras.clip_box.yMin) << 3);
    }

    static int
        gray_clip_vflags(RAS_ARG_ TPos y)
    {
        return ((y > ras.clip_box.yMax) << 1) | ((y < ras.clip_box.yMin) << 3);
    }

    static void
        gray_vline(RAS_ARG_ TPos x1, TPos y1, TPos x2, TPos y2, int f1, int f2)
    {
        f1 &= 10;
        f2 &= 10;
        if ((f1 | f2) == 0) /* Fully visible */
        {
            gray_render_line(RAS_VAR_ x1, y1, x2, y2);
        }
        else if (f1 == f2) /* Invisible by Y */
        {
            return;
        }
        else
        {
            TPos tx1, ty1, tx2, ty2;
            TPos clip_y1, clip_y2;

            tx1 = x1;
            ty1 = y1;
            tx2 = x2;
            ty2 = y2;

            clip_y1 = ras.clip_box.yMin;
            clip_y2 = ras.clip_box.yMax;

            if (f1 & 8) /* y1 < clip_y1 */
            {
                tx1 = x1 + PVG_FT_MulDiv(clip_y1 - y1, x2 - x1, y2 - y1);
                ty1 = clip_y1;
            }

            if (f1 & 2) /* y1 > clip_y2 */
            {
                tx1 = x1 + PVG_FT_MulDiv(clip_y2 - y1, x2 - x1, y2 - y1);
                ty1 = clip_y2;
            }

            if (f2 & 8) /* y2 < clip_y1 */
            {
                tx2 = x1 + PVG_FT_MulDiv(clip_y1 - y1, x2 - x1, y2 - y1);
                ty2 = clip_y1;
            }

            if (f2 & 2) /* y2 > clip_y2 */
            {
                tx2 = x1 + PVG_FT_MulDiv(clip_y2 - y1, x2 - x1, y2 - y1);
                ty2 = clip_y2;
            }

            gray_render_line(RAS_VAR_ tx1, ty1, tx2, ty2);
        }
    }

    static void
        gray_line_to(RAS_ARG_ TPos x2, TPos y2)
    {
        if (!ras.clipping)
        {
            gray_render_line(RAS_VAR_ ras.x, ras.y, x2, y2);
        }
        else
        {
            TPos x1, y1, y3, y4;
            TPos clip_x1, clip_x2;
            int f1, f2, f3, f4;

            f1 = ras.clip_flags;
            f2 = gray_clip_flags(RAS_VAR_ x2, y2);

            if ((f1 & 10) == (f2 & 10) && (f1 & 10) != 0) /* Invisible by Y */
            {
                ras.clip_flags = f2;
                goto End;
            }

            x1 = ras.x;
            y1 = ras.y;

            clip_x1 = ras.clip_box.xMin;
            clip_x2 = ras.clip_box.xMax;

            switch (((f1 & 5) << 1) | (f2 & 5))
            {
            case 0: /* Visible by X */
                gray_vline(RAS_VAR_ x1, y1, x2, y2, f1, f2);
                break;

            case 1: /* x2 > clip_x2 */
                y3 = y1 + PVG_FT_MulDiv(clip_x2 - x1, y2 - y1, x2 - x1);
                f3 = gray_clip_vflags(RAS_VAR_ y3);
                gray_vline(RAS_VAR_ x1, y1, clip_x2, y3, f1, f3);
                gray_vline(RAS_VAR_ clip_x2, y3, clip_x2, y2, f3, f2);
                break;

            case 2: /* x1 > clip_x2 */
                y3 = y1 + PVG_FT_MulDiv(clip_x2 - x1, y2 - y1, x2 - x1);
                f3 = gray_clip_vflags(RAS_VAR_ y3);
                gray_vline(RAS_VAR_ clip_x2, y1, clip_x2, y3, f1, f3);
                gray_vline(RAS_VAR_ clip_x2, y3, x2, y2, f3, f2);
                break;

            case 3: /* x1 > clip_x2 && x2 > clip_x2 */
                gray_vline(RAS_VAR_ clip_x2, y1, clip_x2, y2, f1, f2);
                break;

            case 4: /* x2 < clip_x1 */
                y3 = y1 + PVG_FT_MulDiv(clip_x1 - x1, y2 - y1, x2 - x1);
                f3 = gray_clip_vflags(RAS_VAR_ y3);
                gray_vline(RAS_VAR_ x1, y1, clip_x1, y3, f1, f3);
                gray_vline(RAS_VAR_ clip_x1, y3, clip_x1, y2, f3, f2);
                break;

            case 6: /* x1 > clip_x2 && x2 < clip_x1 */
                y3 = y1 + PVG_FT_MulDiv(clip_x2 - x1, y2 - y1, x2 - x1);
                y4 = y1 + PVG_FT_MulDiv(clip_x1 - x1, y2 - y1, x2 - x1);
                f3 = gray_clip_vflags(RAS_VAR_ y3);
                f4 = gray_clip_vflags(RAS_VAR_ y4);
                gray_vline(RAS_VAR_ clip_x2, y1, clip_x2, y3, f1, f3);
                gray_vline(RAS_VAR_ clip_x2, y3, clip_x1, y4, f3, f4);
                gray_vline(RAS_VAR_ clip_x1, y4, clip_x1, y2, f4, f2);
                break;

            case 8: /* x1 < clip_x1 */
                y3 = y1 + PVG_FT_MulDiv(clip_x1 - x1, y2 - y1, x2 - x1);
                f3 = gray_clip_vflags(RAS_VAR_ y3);
                gray_vline(RAS_VAR_ clip_x1, y1, clip_x1, y3, f1, f3);
                gray_vline(RAS_VAR_ clip_x1, y3, x2, y2, f3, f2);
                break;

            case 9:  /* x1 < clip_x1 && x2 > clip_x2 */
                y3 = y1 + PVG_FT_MulDiv(clip_x1 - x1, y2 - y1, x2 - x1);
                y4 = y1 + PVG_FT_MulDiv(clip_x2 - x1, y2 - y1, x2 - x1);
                f3 = gray_clip_vflags(RAS_VAR_ y3);
                f4 = gray_clip_vflags(RAS_VAR_ y4);
                gray_vline(RAS_VAR_ clip_x1, y1, clip_x1, y3, f1, f3);
                gray_vline(RAS_VAR_ clip_x1, y3, clip_x2, y4, f3, f4);
                gray_vline(RAS_VAR_ clip_x2, y4, clip_x2, y2, f4, f2);
                break;

            case 12: /* x1 < clip_x1 && x2 < clip_x1 */
                gray_vline(RAS_VAR_ clip_x1, y1, clip_x1, y2, f1, f2);
                break;
            }

            ras.clip_flags = f2;
        }

    End:
        ras.x = x2;
        ras.y = y2;
    }

    static void
        gray_split_conic(PVG_FT_Vector* base)
    {
        TPos  a, b;


        base[4].x = base[2].x;
        b = base[1].x;
        a = base[3].x = (base[2].x + b) / 2;
        b = base[1].x = (base[0].x + b) / 2;
        base[2].x = (a + b) / 2;

        base[4].y = base[2].y;
        b = base[1].y;
        a = base[3].y = (base[2].y + b) / 2;
        b = base[1].y = (base[0].y + b) / 2;
        base[2].y = (a + b) / 2;
    }


    static void
        gray_render_conic(RAS_ARG_ const PVG_FT_Vector* control,
            const PVG_FT_Vector* to)
    {
        PVG_FT_Vector   bez_stack[16 * 2 + 1];  /* enough to accommodate bisections */
        PVG_FT_Vector* arc = bez_stack;
        TPos        dx, dy;
        int         draw, split;


        arc[0].x = UPSCALE(to->x);
        arc[0].y = UPSCALE(to->y);
        arc[1].x = UPSCALE(control->x);
        arc[1].y = UPSCALE(control->y);
        arc[2].x = ras.x;
        arc[2].y = ras.y;

        /* short-cut the arc that crosses the current band */
        if ((TRUNC(arc[0].y) >= ras.max_ey &&
            TRUNC(arc[1].y) >= ras.max_ey &&
            TRUNC(arc[2].y) >= ras.max_ey) ||
            (TRUNC(arc[0].y) < ras.min_ey &&
                TRUNC(arc[1].y) < ras.min_ey &&
                TRUNC(arc[2].y) < ras.min_ey))
        {
            if (ras.clipping)
                ras.clip_flags = gray_clip_flags(RAS_VAR_ arc[0].x, arc[0].y);
            ras.x = arc[0].x;
            ras.y = arc[0].y;
            return;
        }

        dx = PVG_FT_ABS(arc[2].x + arc[0].x - 2 * arc[1].x);
        dy = PVG_FT_ABS(arc[2].y + arc[0].y - 2 * arc[1].y);
        if (dx < dy)
            dx = dy;

        /* We can calculate the number of necessary bisections because  */
        /* each bisection predictably reduces deviation exactly 4-fold. */
        /* Even 32-bit deviation would vanish after 16 bisections.      */
        draw = 1;
        while (dx > ONE_PIXEL / 4)
        {
            dx >>= 2;
            draw <<= 1;
        }

        /* We use decrement counter to count the total number of segments */
        /* to draw starting from 2^level. Before each draw we split as    */
        /* many times as there are trailing zeros in the counter.         */
        do
        {
            split = 1;
            while ((draw & split) == 0)
            {
                gray_split_conic(arc);
                arc += 2;
                split <<= 1;
            }

            gray_line_to(RAS_VAR_ arc[0].x, arc[0].y);
            arc -= 2;

        } while (--draw);
    }


    static void
        gray_split_cubic(PVG_FT_Vector* base)
    {
        TPos  a, b, c, d;


        base[6].x = base[3].x;
        c = base[1].x;
        d = base[2].x;
        base[1].x = a = (base[0].x + c) / 2;
        base[5].x = b = (base[3].x + d) / 2;
        c = (c + d) / 2;
        base[2].x = a = (a + c) / 2;
        base[4].x = b = (b + c) / 2;
        base[3].x = (a + b) / 2;

        base[6].y = base[3].y;
        c = base[1].y;
        d = base[2].y;
        base[1].y = a = (base[0].y + c) / 2;
        base[5].y = b = (base[3].y + d) / 2;
        c = (c + d) / 2;
        base[2].y = a = (a + c) / 2;
        base[4].y = b = (b + c) / 2;
        base[3].y = (a + b) / 2;
    }


    static void
        gray_render_cubic(RAS_ARG_ const PVG_FT_Vector* control1,
            const PVG_FT_Vector* control2,
            const PVG_FT_Vector* to)
    {
        PVG_FT_Vector   bez_stack[16 * 3 + 1];  /* enough to accommodate bisections */
        PVG_FT_Vector* arc = bez_stack;
        PVG_FT_Vector* limit = bez_stack + 45;
        TPos        dx, dy, dx_, dy_;
        TPos        dx1, dy1, dx2, dy2;
        TPos        L, s, s_limit;


        arc[0].x = UPSCALE(to->x);
        arc[0].y = UPSCALE(to->y);
        arc[1].x = UPSCALE(control2->x);
        arc[1].y = UPSCALE(control2->y);
        arc[2].x = UPSCALE(control1->x);
        arc[2].y = UPSCALE(control1->y);
        arc[3].x = ras.x;
        arc[3].y = ras.y;

        /* short-cut the arc that crosses the current band */
        if ((TRUNC(arc[0].y) >= ras.max_ey &&
            TRUNC(arc[1].y) >= ras.max_ey &&
            TRUNC(arc[2].y) >= ras.max_ey &&
            TRUNC(arc[3].y) >= ras.max_ey) ||
            (TRUNC(arc[0].y) < ras.min_ey &&
                TRUNC(arc[1].y) < ras.min_ey &&
                TRUNC(arc[2].y) < ras.min_ey &&
                TRUNC(arc[3].y) < ras.min_ey))
        {
            if (ras.clipping)
                ras.clip_flags = gray_clip_flags(RAS_VAR_ arc[0].x, arc[0].y);
            ras.x = arc[0].x;
            ras.y = arc[0].y;
            return;
        }

        for (;;)
        {
            /* Decide whether to split or draw. See `Rapid Termination          */
            /* Evaluation for Recursive Subdivision of Bezier Curves' by Thomas */
            /* F. Hain, at                                                      */
            /* http://www.cis.southalabama.edu/~hain/general/Publications/Bezier/Camera-ready%20CISST02%202.pdf */


            /* dx and dy are x and y components of the P0-P3 chord vector. */
            dx = dx_ = arc[3].x - arc[0].x;
            dy = dy_ = arc[3].y - arc[0].y;

            L = PVG_FT_HYPOT(dx_, dy_);

            /* Avoid possible arithmetic overflow below by splitting. */
            if (L >= (1 << 23))
                goto Split;

            /* Max deviation may be as much as (s/L) * 3/4 (if Hain's v = 1). */
            s_limit = L * (TPos)(ONE_PIXEL / 6);

            /* s is L * the perpendicular distance from P1 to the line P0-P3. */
            dx1 = arc[1].x - arc[0].x;
            dy1 = arc[1].y - arc[0].y;
            s = PVG_FT_ABS(dy * dx1 - dx * dy1);

            if (s > s_limit)
                goto Split;

            /* s is L * the perpendicular distance from P2 to the line P0-P3. */
            dx2 = arc[2].x - arc[0].x;
            dy2 = arc[2].y - arc[0].y;
            s = PVG_FT_ABS(dy * dx2 - dx * dy2);

            if (s > s_limit)
                goto Split;

            /* Split super curvy segments where the off points are so far
               from the chord that the angles P0-P1-P3 or P0-P2-P3 become
               acute as detected by appropriate dot products. */
            if (dx1 * (dx1 - dx) + dy1 * (dy1 - dy) > 0 ||
                dx2 * (dx2 - dx) + dy2 * (dy2 - dy) > 0)
                goto Split;

            gray_line_to(RAS_VAR_ arc[0].x, arc[0].y);

            if (arc == bez_stack)
                return;

            arc -= 3;
            continue;

        Split:
            if (arc == limit)
                return;
            gray_split_cubic(arc);
            arc += 3;
        }
    }



    static int
        gray_move_to(const PVG_FT_Vector* to,
            PWorker           worker)
    {
        TPos  x, y;


        /* record current cell, if any */
        if (!ras.invalid)
            gray_record_cell(worker);

        /* start to a new position */
        x = UPSCALE(to->x);
        y = UPSCALE(to->y);

        gray_start_cell(worker, TRUNC(x), TRUNC(y));

        if (ras.clipping)
            ras.clip_flags = gray_clip_flags(worker, x, y);
        ras.x = x;
        ras.y = y;
        return 0;
    }


    static void
        gray_hline(RAS_ARG_ TCoord  x,
            TCoord  y,
            TPos    area,
            int     acount)
    {
        int coverage;


        /* compute the coverage line's coverage, depending on the    */
        /* outline fill rule                                         */
        /*                                                           */
        /* the coverage percentage is area/(PIXEL_BITS*PIXEL_BITS*2) */
        /*                                                           */
        coverage = (int)(area >> (PIXEL_BITS * 2 + 1 - 8));
        /* use range 0..256 */
        if (coverage < 0)
            coverage = -coverage;

        if (ras.outline.flags & PVG_FT_OUTLINE_EVEN_ODD_FILL)
        {
            coverage &= 511;

            if (coverage > 256)
                coverage = 512 - coverage;
            else if (coverage == 256)
                coverage = 255;
        }
        else
        {
            /* normal non-zero winding rule */
            if (coverage >= 256)
                coverage = 255;
        }

        y += (TCoord)ras.min_ey;
        x += (TCoord)ras.min_ex;

        /* PVG_FT_Span.x is an int, so limit our coordinates appropriately */
        if (x >= (1 << 23))
            x = (1 << 23) - 1;

        /* PVG_FT_Span.y is an int, so limit our coordinates appropriately */
        if (y >= (1 << 23))
            y = (1 << 23) - 1;

        if (coverage)
        {
            PVG_FT_Span* span;
            int       count;
            int       skip;

            /* see whether we can add this span to the current list */
            count = ras.num_gray_spans;
            span = ras.gray_spans + count - 1;
            if (count > 0 &&
                span->y == y &&
                span->x + span->len == x &&
                span->coverage == coverage)
            {
                span->len = span->len + acount;
                return;
            }

            if (count >= PVG_FT_MAX_GRAY_SPANS)
            {
                if (ras.render_span && count > ras.skip_spans)
                {
                    skip = ras.skip_spans > 0 ? ras.skip_spans : 0;
                    ras.render_span(ras.num_gray_spans - skip,
                        ras.gray_spans + skip,
                        ras.render_span_data);
                }

                ras.skip_spans -= ras.num_gray_spans;
                /* ras.render_span( span->y, ras.gray_spans, count ); */
                ras.num_gray_spans = 0;

                span = ras.gray_spans;
            }
            else
                span++;

            /* add a gray span to the current list */
            span->x = x;
            span->len = acount;
            span->y = y;
            span->coverage = (unsigned char)coverage;

            ras.num_gray_spans++;
        }
    }



    static void
        gray_sweep(RAS_ARG)
    {
        int  yindex;

        if (ras.num_cells == 0)
            return;

        for (yindex = 0; yindex < ras.ycount; yindex++)
        {
            PCell   cell = ras.ycells[yindex];
            TCoord  cover = 0;
            TCoord  x = 0;


            for (; cell != NULL; cell = cell->next)
            {
                TArea  area;


                if (cell->x > x && cover != 0)
                    gray_hline(RAS_VAR_ x, yindex, cover * (ONE_PIXEL * 2),
                        cell->x - x);

                cover += cell->cover;
                area = cover * (ONE_PIXEL * 2) - cell->area;

                if (area != 0 && cell->x >= 0)
                    gray_hline(RAS_VAR_ cell->x, yindex, area, 1);

                x = cell->x + 1;
            }

            if (ras.count_ex > x && cover != 0)
                gray_hline(RAS_VAR_ x, yindex, cover * (ONE_PIXEL * 2),
                    ras.count_ex - x);
        }
    }

    PVG_FT_Error PVG_FT_Outline_Check(PVG_FT_Outline* outline)
    {
        if (outline) {
            PVG_FT_Int n_points = outline->n_points;
            PVG_FT_Int n_contours = outline->n_contours;
            PVG_FT_Int end0, end;
            PVG_FT_Int n;

            /* empty glyph? */
            if (n_points == 0 && n_contours == 0) return 0;

            /* check point and contour counts */
            if (n_points <= 0 || n_contours <= 0) goto Bad;

            end0 = end = -1;
            for (n = 0; n < n_contours; n++) {
                end = outline->contours[n];

                /* note that we don't accept empty contours */
                if (end <= end0 || end >= n_points) goto Bad;

                end0 = end;
            }

            if (end != n_points - 1) goto Bad;

            /* XXX: check the tags array */
            return 0;
        }

    Bad:
        return ErrRaster_Invalid_Outline;
    }

    void PVG_FT_Outline_Get_CBox(const PVG_FT_Outline* outline, PVG_FT_BBox* acbox)
    {
        PVG_FT_Pos xMin, yMin, xMax, yMax;

        if (outline && acbox) {
            if (outline->n_points == 0) {
                xMin = 0;
                yMin = 0;
                xMax = 0;
                yMax = 0;
            }
            else {
                PVG_FT_Vector* vec = outline->points;
                PVG_FT_Vector* limit = vec + outline->n_points;

                xMin = xMax = vec->x;
                yMin = yMax = vec->y;
                vec++;

                for (; vec < limit; vec++) {
                    PVG_FT_Pos x, y;

                    x = vec->x;
                    if (x < xMin) xMin = x;
                    if (x > xMax) xMax = x;

                    y = vec->y;
                    if (y < yMin) yMin = y;
                    if (y > yMax) yMax = y;
                }
            }
            acbox->xMin = xMin;
            acbox->xMax = xMax;
            acbox->yMin = yMin;
            acbox->yMax = yMax;
        }
    }

    /*************************************************************************/
    /*                                                                       */
    /*  The following function should only compile in stand_alone mode,      */
    /*  i.e., when building this component without the rest of FreeType.     */
    /*                                                                       */
    /*************************************************************************/

    /*************************************************************************/
    /*                                                                       */
    /* <Function>                                                            */
    /*    PVG_FT_Outline_Decompose                                               */
    /*                                                                       */
    /* <Description>                                                         */
    /*    Walks over an outline's structure to decompose it into individual  */
    /*    segments and Bezier arcs.  This function is also able to emit      */
    /*    `move to' and `close to' operations to indicate the start and end  */
    /*    of new contours in the outline.                                    */
    /*                                                                       */
    /* <Input>                                                               */
    /*    outline        :: A pointer to the source target.                  */
    /*                                                                       */
    /*    user           :: A typeless pointer which is passed to each       */
    /*                      emitter during the decomposition.  It can be     */
    /*                      used to store the state during the               */
    /*                      decomposition.                                   */
    /*                                                                       */
    /* <Return>                                                              */
    /*    Error code.  0 means success.                                      */
    /*                                                                       */
    static
        int  PVG_FT_Outline_Decompose(const PVG_FT_Outline* outline,
            void* user)
    {
#undef SCALED
#define SCALED( x )  (x)

        PVG_FT_Vector   v_last;
        PVG_FT_Vector   v_control;
        PVG_FT_Vector   v_start;

        PVG_FT_Vector* point;
        PVG_FT_Vector* limit;
        char* tags;

        int   n;         /* index of contour in outline     */
        int   first;     /* index of first point in contour */
        int   error;
        char  tag;       /* current point's state           */

        if (!outline)
            return ErrRaster_Invalid_Outline;

        first = 0;

        for (n = 0; n < outline->n_contours; n++)
        {
            int  last;  /* index of last point in contour */


            last = outline->contours[n];
            if (last < 0)
                goto Invalid_Outline;
            limit = outline->points + last;

            v_start = outline->points[first];
            v_start.x = SCALED(v_start.x);
            v_start.y = SCALED(v_start.y);

            v_last = outline->points[last];
            v_last.x = SCALED(v_last.x);
            v_last.y = SCALED(v_last.y);

            v_control = v_start;

            point = outline->points + first;
            tags = outline->tags + first;
            tag = PVG_FT_CURVE_TAG(tags[0]);

            /* A contour cannot start with a cubic control point! */
            if (tag == PVG_FT_CURVE_TAG_CUBIC)
                goto Invalid_Outline;

            /* check first point to determine origin */
            if (tag == PVG_FT_CURVE_TAG_CONIC)
            {
                /* first point is conic control.  Yes, this happens. */
                if (PVG_FT_CURVE_TAG(outline->tags[last]) == PVG_FT_CURVE_TAG_ON)
                {
                    /* start at last point if it is on the curve */
                    v_start = v_last;
                    limit--;
                }
                else
                {
                    /* if both first and last points are conic,         */
                    /* start at their middle and record its position    */
                    /* for closure                                      */
                    v_start.x = (v_start.x + v_last.x) / 2;
                    v_start.y = (v_start.y + v_last.y) / 2;

                    v_last = v_start;
                }
                point--;
                tags--;
            }

            error = gray_move_to(&v_start, (PWorker)user);
            if (error)
                goto Exit;

            while (point < limit)
            {
                point++;
                tags++;

                tag = PVG_FT_CURVE_TAG(tags[0]);
                switch (tag)
                {
                case PVG_FT_CURVE_TAG_ON:  /* emit a single line_to */
                {
                    PVG_FT_Vector  vec;


                    vec.x = SCALED(point->x);
                    vec.y = SCALED(point->y);

                    gray_line_to((PWorker)user, UPSCALE(vec.x), UPSCALE(vec.y));
                    continue;
                }

                case PVG_FT_CURVE_TAG_CONIC:  /* consume conic arcs */
                {
                    v_control.x = SCALED(point->x);
                    v_control.y = SCALED(point->y);

                Do_Conic:
                    if (point < limit)
                    {
                        PVG_FT_Vector  vec;
                        PVG_FT_Vector  v_middle;


                        point++;
                        tags++;
                        tag = PVG_FT_CURVE_TAG(tags[0]);

                        vec.x = SCALED(point->x);
                        vec.y = SCALED(point->y);

                        if (tag == PVG_FT_CURVE_TAG_ON)
                        {
                            gray_render_conic((PWorker)user, &v_control, &vec);
                            continue;
                        }

                        if (tag != PVG_FT_CURVE_TAG_CONIC)
                            goto Invalid_Outline;

                        v_middle.x = (v_control.x + vec.x) / 2;
                        v_middle.y = (v_control.y + vec.y) / 2;

                        gray_render_conic((PWorker)user, &v_control, &v_middle);

                        v_control = vec;
                        goto Do_Conic;
                    }

                    gray_render_conic((PWorker)user, &v_control, &v_start);
                    goto Close;
                }

                default:  /* PVG_FT_CURVE_TAG_CUBIC */
                {
                    PVG_FT_Vector  vec1, vec2;


                    if (point + 1 > limit ||
                        PVG_FT_CURVE_TAG(tags[1]) != PVG_FT_CURVE_TAG_CUBIC)
                        goto Invalid_Outline;

                    point += 2;
                    tags += 2;

                    vec1.x = SCALED(point[-2].x);
                    vec1.y = SCALED(point[-2].y);

                    vec2.x = SCALED(point[-1].x);
                    vec2.y = SCALED(point[-1].y);

                    if (point <= limit)
                    {
                        PVG_FT_Vector  vec;


                        vec.x = SCALED(point->x);
                        vec.y = SCALED(point->y);

                        gray_render_cubic((PWorker)user, &vec1, &vec2, &vec);
                        continue;
                    }

                    gray_render_cubic((PWorker)user, &vec1, &vec2, &v_start);
                    goto Close;
                }
                }
            }

            /* close the contour with a line segment */
            gray_line_to((PWorker)user, UPSCALE(v_start.x), UPSCALE(v_start.y));

        Close:
            first = last + 1;
        }

        return 0;

    Exit:
        return error;

    Invalid_Outline:
        return ErrRaster_Invalid_Outline;
    }

    typedef struct  TBand_
    {
        TPos  min, max;

    } TBand;

    static int
        gray_convert_glyph_inner(RAS_ARG)
    {
        volatile int  error = 0;

        if (pvg_ft_setjmp(ras.jump_buffer) == 0)
        {
            error = PVG_FT_Outline_Decompose(&ras.outline, &ras);
            if (!ras.invalid)
                gray_record_cell(RAS_VAR);
        }
        else
        {
            error = ErrRaster_Memory_Overflow;
        }

        return error;
    }


    static int
        gray_convert_glyph(RAS_ARG)
    {
        TBand            bands[40];
        TBand* volatile  band;
        int volatile     n, num_bands;
        TPos volatile    min, max, max_y;
        PVG_FT_BBox* clip;
        int              skip;

        ras.num_gray_spans = 0;

        /* Set up state in the raster object */
        gray_compute_cbox(RAS_VAR);

        /* clip to target bitmap, exit if nothing to do */
        clip = &ras.clip_box;

        if (ras.max_ex <= clip->xMin || ras.min_ex >= clip->xMax ||
            ras.max_ey <= clip->yMin || ras.min_ey >= clip->yMax)
            return 0;

        ras.clip_flags = ras.clipping = 0;

        if (ras.min_ex < clip->xMin) {
            ras.min_ex = clip->xMin;
            ras.clipping = 1;
        }

        if (ras.min_ey < clip->yMin) {
            ras.min_ey = clip->yMin;
            ras.clipping = 1;
        }

        if (ras.max_ex > clip->xMax) {
            ras.max_ex = clip->xMax;
            ras.clipping = 1;
        }

        if (ras.max_ey > clip->yMax) {
            ras.max_ey = clip->yMax;
            ras.clipping = 1;
        }

        clip->xMin = (ras.min_ex - 1) * ONE_PIXEL;
        clip->yMin = (ras.min_ey - 1) * ONE_PIXEL;
        clip->xMax = (ras.max_ex + 1) * ONE_PIXEL;
        clip->yMax = (ras.max_ey + 1) * ONE_PIXEL;

        ras.count_ex = ras.max_ex - ras.min_ex;
        ras.count_ey = ras.max_ey - ras.min_ey;

        /* set up vertical bands */
        num_bands = (int)((ras.max_ey - ras.min_ey) / ras.band_size);
        if (num_bands == 0)
            num_bands = 1;
        if (num_bands >= 39)
            num_bands = 39;

        ras.band_shoot = 0;

        min = ras.min_ey;
        max_y = ras.max_ey;

        for (n = 0; n < num_bands; n++, min = max)
        {
            max = min + ras.band_size;
            if (n == num_bands - 1 || max > max_y)
                max = max_y;

            bands[0].min = min;
            bands[0].max = max;
            band = bands;

            while (band >= bands)
            {
                TPos  bottom, top, middle;
                int   error;

                {
                    PCell  cells_max;
                    int    yindex;
                    int    cell_start, cell_end, cell_mod;


                    ras.ycells = (PCell*)ras.buffer;
                    ras.ycount = band->max - band->min;

                    cell_start = sizeof(PCell) * ras.ycount;
                    cell_mod = cell_start % sizeof(TCell);
                    if (cell_mod > 0)
                        cell_start += sizeof(TCell) - cell_mod;

                    cell_end = ras.buffer_size;
                    cell_end -= cell_end % sizeof(TCell);

                    cells_max = (PCell)((char*)ras.buffer + cell_end);
                    ras.cells = (PCell)((char*)ras.buffer + cell_start);
                    if (ras.cells >= cells_max)
                        goto ReduceBands;

                    ras.max_cells = (int)(cells_max - ras.cells);
                    if (ras.max_cells < 2)
                        goto ReduceBands;

                    for (yindex = 0; yindex < ras.ycount; yindex++)
                        ras.ycells[yindex] = NULL;
                }

                ras.num_cells = 0;
                ras.invalid = 1;
                ras.min_ey = band->min;
                ras.max_ey = band->max;
                ras.count_ey = band->max - band->min;

                error = gray_convert_glyph_inner(RAS_VAR);

                if (!error)
                {
                    gray_sweep(RAS_VAR);
                    band--;
                    continue;
                }
                else if (error != ErrRaster_Memory_Overflow)
                    return 1;

            ReduceBands:
                /* render pool overflow; we will reduce the render band by half */
                bottom = band->min;
                top = band->max;
                middle = bottom + ((top - bottom) >> 1);

                /* This is too complex for a single scanline; there must */
                /* be some problems.                                     */
                if (middle == bottom)
                {
                    return ErrRaster_OutOfMemory;
                }

                if (bottom - top >= ras.band_size)
                    ras.band_shoot++;

                band[1].min = bottom;
                band[1].max = middle;
                band[0].min = middle;
                band[0].max = top;
                band++;
            }
        }

        if (ras.render_span && ras.num_gray_spans > ras.skip_spans)
        {
            skip = ras.skip_spans > 0 ? ras.skip_spans : 0;
            ras.render_span(ras.num_gray_spans - skip,
                ras.gray_spans + skip,
                ras.render_span_data);
        }

        ras.skip_spans -= ras.num_gray_spans;

        if (ras.band_shoot > 8 && ras.band_size > 16)
            ras.band_size = ras.band_size / 2;

        return 0;
    }


    static int
        gray_raster_render(RAS_ARG_ void* buffer, long buffer_size,
            const PVG_FT_Raster_Params* params)
    {
        const PVG_FT_Outline* outline = (const PVG_FT_Outline*)params->source;
        if (outline == NULL)
            return ErrRaster_Invalid_Outline;

        /* return immediately if the outline is empty */
        if (outline->n_points == 0 || outline->n_contours <= 0)
            return 0;

        if (!outline->contours || !outline->points)
            return ErrRaster_Invalid_Outline;

        if (outline->n_points !=
            outline->contours[outline->n_contours - 1] + 1)
            return ErrRaster_Invalid_Outline;

        /* this version does not support monochrome rendering */
        if (!(params->flags & PVG_FT_RASTER_FLAG_AA))
            return ErrRaster_Invalid_Mode;

        if (!(params->flags & PVG_FT_RASTER_FLAG_DIRECT))
            return ErrRaster_Invalid_Mode;

        /* compute clipping box */
        if (params->flags & PVG_FT_RASTER_FLAG_CLIP)
        {
            ras.clip_box = params->clip_box;
        }
        else
        {
            ras.clip_box.xMin = -(1 << 23);
            ras.clip_box.yMin = -(1 << 23);
            ras.clip_box.xMax = (1 << 23) - 1;
            ras.clip_box.yMax = (1 << 23) - 1;
        }

        gray_init_cells(RAS_VAR_ buffer, buffer_size);

        ras.outline = *outline;
        ras.num_cells = 0;
        ras.invalid = 1;
        ras.band_size = (int)(buffer_size / (long)(sizeof(TCell) * 8));

        ras.render_span = (PVG_FT_Raster_Span_Func)params->gray_spans;
        ras.render_span_data = params->user;

        return gray_convert_glyph(RAS_VAR);
    }

    typedef struct PVG_FT_StrokerRec_* PVG_FT_Stroker;

    typedef enum  PVG_FT_Stroker_LineJoin_
    {
        PVG_FT_STROKER_LINEJOIN_ROUND = 0,
        PVG_FT_STROKER_LINEJOIN_BEVEL = 1,
        PVG_FT_STROKER_LINEJOIN_MITER_VARIABLE = 2,
        PVG_FT_STROKER_LINEJOIN_MITER = PVG_FT_STROKER_LINEJOIN_MITER_VARIABLE,
        PVG_FT_STROKER_LINEJOIN_MITER_FIXED = 3

    } PVG_FT_Stroker_LineJoin;

    typedef enum  PVG_FT_Stroker_LineCap_
    {
        PVG_FT_STROKER_LINECAP_BUTT = 0,
        PVG_FT_STROKER_LINECAP_ROUND,
        PVG_FT_STROKER_LINECAP_SQUARE

    } PVG_FT_Stroker_LineCap;

    typedef enum  PVG_FT_StrokerBorder_
    {
        PVG_FT_STROKER_BORDER_LEFT = 0,
        PVG_FT_STROKER_BORDER_RIGHT

    } PVG_FT_StrokerBorder;

#define PVG_FT_SMALL_CONIC_THRESHOLD (PVG_FT_ANGLE_PI / 6)
#define PVG_FT_SMALL_CUBIC_THRESHOLD (PVG_FT_ANGLE_PI / 8)

#define PVG_FT_EPSILON 2

#define PVG_FT_IS_SMALL(x) ((x) > -PVG_FT_EPSILON && (x) < PVG_FT_EPSILON)

    static PVG_FT_Pos ft_pos_abs(PVG_FT_Pos x)
    {
        return x >= 0 ? x : -x;
    }


    static void ft_conic_split(PVG_FT_Vector* base)
    {
        PVG_FT_Pos a, b;

        base[4].x = base[2].x;
        a = base[0].x + base[1].x;
        b = base[1].x + base[2].x;
        base[3].x = b >> 1;
        base[2].x = (a + b) >> 2;
        base[1].x = a >> 1;

        base[4].y = base[2].y;
        a = base[0].y + base[1].y;
        b = base[1].y + base[2].y;
        base[3].y = b >> 1;
        base[2].y = (a + b) >> 2;
        base[1].y = a >> 1;
    }

    static PVG_FT_Bool ft_conic_is_small_enough(PVG_FT_Vector* base,
        PVG_FT_Angle* angle_in,
        PVG_FT_Angle* angle_out)
    {
        PVG_FT_Vector d1, d2;
        PVG_FT_Angle  theta;
        PVG_FT_Int    close1, close2;

        d1.x = base[1].x - base[2].x;
        d1.y = base[1].y - base[2].y;
        d2.x = base[0].x - base[1].x;
        d2.y = base[0].y - base[1].y;

        close1 = PVG_FT_IS_SMALL(d1.x) && PVG_FT_IS_SMALL(d1.y);
        close2 = PVG_FT_IS_SMALL(d2.x) && PVG_FT_IS_SMALL(d2.y);

        if (close1) {
            if (close2) {
                /* basically a point;                      */
                /* do nothing to retain original direction */
            }
            else {
                *angle_in = *angle_out = PVG_FT_Atan2(d2.x, d2.y);
            }
        }
        else /* !close1 */
        {
            if (close2) {
                *angle_in = *angle_out = PVG_FT_Atan2(d1.x, d1.y);
            }
            else {
                *angle_in = PVG_FT_Atan2(d1.x, d1.y);
                *angle_out = PVG_FT_Atan2(d2.x, d2.y);
            }
        }

        theta = ft_pos_abs(PVG_FT_Angle_Diff(*angle_in, *angle_out));

        return PVG_FT_BOOL(theta < PVG_FT_SMALL_CONIC_THRESHOLD);
    }


    static void ft_cubic_split(PVG_FT_Vector* base)
    {
        PVG_FT_Pos a, b, c;

        base[6].x = base[3].x;
        a = base[0].x + base[1].x;
        b = base[1].x + base[2].x;
        c = base[2].x + base[3].x;
        base[5].x = c >> 1;
        c += b;
        base[4].x = c >> 2;
        base[1].x = a >> 1;
        a += b;
        base[2].x = a >> 2;
        base[3].x = (a + c) >> 3;

        base[6].y = base[3].y;
        a = base[0].y + base[1].y;
        b = base[1].y + base[2].y;
        c = base[2].y + base[3].y;
        base[5].y = c >> 1;
        c += b;
        base[4].y = c >> 2;
        base[1].y = a >> 1;
        a += b;
        base[2].y = a >> 2;
        base[3].y = (a + c) >> 3;
    }

    /* Return the average of `angle1' and `angle2'.            */
    /* This gives correct result even if `angle1' and `angle2' */
    /* have opposite signs.                                    */
    static PVG_FT_Angle ft_angle_mean(PVG_FT_Angle angle1, PVG_FT_Angle angle2)
    {
        return angle1 + PVG_FT_Angle_Diff(angle1, angle2) / 2;
    }

    static PVG_FT_Bool ft_cubic_is_small_enough(PVG_FT_Vector* base,
        PVG_FT_Angle* angle_in,
        PVG_FT_Angle* angle_mid,
        PVG_FT_Angle* angle_out)
    {
        PVG_FT_Vector d1, d2, d3;
        PVG_FT_Angle  theta1, theta2;
        PVG_FT_Int    close1, close2, close3;

        d1.x = base[2].x - base[3].x;
        d1.y = base[2].y - base[3].y;
        d2.x = base[1].x - base[2].x;
        d2.y = base[1].y - base[2].y;
        d3.x = base[0].x - base[1].x;
        d3.y = base[0].y - base[1].y;

        close1 = PVG_FT_IS_SMALL(d1.x) && PVG_FT_IS_SMALL(d1.y);
        close2 = PVG_FT_IS_SMALL(d2.x) && PVG_FT_IS_SMALL(d2.y);
        close3 = PVG_FT_IS_SMALL(d3.x) && PVG_FT_IS_SMALL(d3.y);

        if (close1) {
            if (close2) {
                if (close3) {
                    /* basically a point;                      */
                    /* do nothing to retain original direction */
                }
                else /* !close3 */
                {
                    *angle_in = *angle_mid = *angle_out = PVG_FT_Atan2(d3.x, d3.y);
                }
            }
            else /* !close2 */
            {
                if (close3) {
                    *angle_in = *angle_mid = *angle_out = PVG_FT_Atan2(d2.x, d2.y);
                }
                else /* !close3 */
                {
                    *angle_in = *angle_mid = PVG_FT_Atan2(d2.x, d2.y);
                    *angle_out = PVG_FT_Atan2(d3.x, d3.y);
                }
            }
        }
        else /* !close1 */
        {
            if (close2) {
                if (close3) {
                    *angle_in = *angle_mid = *angle_out = PVG_FT_Atan2(d1.x, d1.y);
                }
                else /* !close3 */
                {
                    *angle_in = PVG_FT_Atan2(d1.x, d1.y);
                    *angle_out = PVG_FT_Atan2(d3.x, d3.y);
                    *angle_mid = ft_angle_mean(*angle_in, *angle_out);
                }
            }
            else /* !close2 */
            {
                if (close3) {
                    *angle_in = PVG_FT_Atan2(d1.x, d1.y);
                    *angle_mid = *angle_out = PVG_FT_Atan2(d2.x, d2.y);
                }
                else /* !close3 */
                {
                    *angle_in = PVG_FT_Atan2(d1.x, d1.y);
                    *angle_mid = PVG_FT_Atan2(d2.x, d2.y);
                    *angle_out = PVG_FT_Atan2(d3.x, d3.y);
                }
            }
        }

        theta1 = ft_pos_abs(PVG_FT_Angle_Diff(*angle_in, *angle_mid));
        theta2 = ft_pos_abs(PVG_FT_Angle_Diff(*angle_mid, *angle_out));

        return PVG_FT_BOOL(theta1 < PVG_FT_SMALL_CUBIC_THRESHOLD &&
            theta2 < PVG_FT_SMALL_CUBIC_THRESHOLD);
    }


    typedef enum PVG_FT_StrokeTags_ {
        PVG_FT_STROKE_TAG_ON = 1,    /* on-curve point  */
        PVG_FT_STROKE_TAG_CUBIC = 2, /* cubic off-point */
        PVG_FT_STROKE_TAG_BEGIN = 4, /* sub-path start  */
        PVG_FT_STROKE_TAG_END = 8    /* sub-path end    */

    } PVG_FT_StrokeTags;

#define PVG_FT_STROKE_TAG_BEGIN_END \
    (PVG_FT_STROKE_TAG_BEGIN | PVG_FT_STROKE_TAG_END)

    typedef struct PVG_FT_StrokeBorderRec_ {
        PVG_FT_UInt    num_points;
        PVG_FT_UInt    max_points;
        PVG_FT_Vector* points;
        PVG_FT_Byte* tags;
        PVG_FT_Bool    movable; /* TRUE for ends of lineto borders */
        PVG_FT_Int     start;   /* index of current sub-path start point */
        PVG_FT_Bool    valid;

    } PVG_FT_StrokeBorderRec, * PVG_FT_StrokeBorder;

    static PVG_FT_Error ft_stroke_border_grow(PVG_FT_StrokeBorder border,
        PVG_FT_UInt         new_points)
    {
        PVG_FT_UInt  old_max = border->max_points;
        PVG_FT_UInt  new_max = border->num_points + new_points;
        PVG_FT_Error error = 0;

        if (new_max > old_max) {
            PVG_FT_UInt cur_max = old_max;

            while (cur_max < new_max) cur_max += (cur_max >> 1) + 16;

            border->points = (PVG_FT_Vector*)realloc(border->points,
                cur_max * sizeof(PVG_FT_Vector));
            border->tags =
                (PVG_FT_Byte*)realloc(border->tags, cur_max * sizeof(PVG_FT_Byte));

            if (!border->points || !border->tags) goto Exit;

            border->max_points = cur_max;
        }

    Exit:
        return error;
    }

    static void ft_stroke_border_close(PVG_FT_StrokeBorder border,
        PVG_FT_Bool         reverse)
    {
        PVG_FT_UInt start = border->start;
        PVG_FT_UInt count = border->num_points;

        assert(border->start >= 0);

        /* don't record empty paths! */
        if (count <= start + 1U)
            border->num_points = start;
        else {
            /* copy the last point to the start of this sub-path, since */
            /* it contains the `adjusted' starting coordinates          */
            border->num_points = --count;
            border->points[start] = border->points[count];
            border->tags[start] = border->tags[count];

            if (reverse) {
                /* reverse the points */
                {
                    PVG_FT_Vector* vec1 = border->points + start + 1;
                    PVG_FT_Vector* vec2 = border->points + count - 1;

                    for (; vec1 < vec2; vec1++, vec2--) {
                        PVG_FT_Vector tmp;

                        tmp = *vec1;
                        *vec1 = *vec2;
                        *vec2 = tmp;
                    }
                }

                /* then the tags */
                {
                    PVG_FT_Byte* tag1 = border->tags + start + 1;
                    PVG_FT_Byte* tag2 = border->tags + count - 1;

                    for (; tag1 < tag2; tag1++, tag2--) {
                        PVG_FT_Byte tmp;

                        tmp = *tag1;
                        *tag1 = *tag2;
                        *tag2 = tmp;
                    }
                }
            }

            border->tags[start] |= PVG_FT_STROKE_TAG_BEGIN;
            border->tags[count - 1] |= PVG_FT_STROKE_TAG_END;
        }

        border->start = -1;
        border->movable = FALSE;
    }

    static PVG_FT_Error ft_stroke_border_lineto(PVG_FT_StrokeBorder border,
        PVG_FT_Vector* to, PVG_FT_Bool movable)
    {
        PVG_FT_Error error = 0;

        assert(border->start >= 0);

        if (border->movable) {
            /* move last point */
            border->points[border->num_points - 1] = *to;
        }
        else {
            /* don't add zero-length lineto, but always add moveto */
            if (border->num_points > (unsigned int)border->start &&
                PVG_FT_IS_SMALL(border->points[border->num_points - 1].x - to->x) &&
                PVG_FT_IS_SMALL(border->points[border->num_points - 1].y - to->y))
                return error;

            /* add one point */
            error = ft_stroke_border_grow(border, 1);
            if (!error) {
                PVG_FT_Vector* vec = border->points + border->num_points;
                PVG_FT_Byte* tag = border->tags + border->num_points;

                vec[0] = *to;
                tag[0] = PVG_FT_STROKE_TAG_ON;

                border->num_points += 1;
            }
        }
        border->movable = movable;
        return error;
    }

    static PVG_FT_Error ft_stroke_border_conicto(PVG_FT_StrokeBorder border,
        PVG_FT_Vector* control,
        PVG_FT_Vector* to)
    {
        PVG_FT_Error error;

        assert(border->start >= 0);

        error = ft_stroke_border_grow(border, 2);
        if (!error) {
            PVG_FT_Vector* vec = border->points + border->num_points;
            PVG_FT_Byte* tag = border->tags + border->num_points;

            vec[0] = *control;
            vec[1] = *to;

            tag[0] = 0;
            tag[1] = PVG_FT_STROKE_TAG_ON;

            border->num_points += 2;
        }

        border->movable = FALSE;

        return error;
    }

    static PVG_FT_Error ft_stroke_border_cubicto(PVG_FT_StrokeBorder border,
        PVG_FT_Vector* control1,
        PVG_FT_Vector* control2,
        PVG_FT_Vector* to)
    {
        PVG_FT_Error error;

        assert(border->start >= 0);

        error = ft_stroke_border_grow(border, 3);
        if (!error) {
            PVG_FT_Vector* vec = border->points + border->num_points;
            PVG_FT_Byte* tag = border->tags + border->num_points;

            vec[0] = *control1;
            vec[1] = *control2;
            vec[2] = *to;

            tag[0] = PVG_FT_STROKE_TAG_CUBIC;
            tag[1] = PVG_FT_STROKE_TAG_CUBIC;
            tag[2] = PVG_FT_STROKE_TAG_ON;

            border->num_points += 3;
        }

        border->movable = FALSE;

        return error;
    }

#define PVG_FT_ARC_CUBIC_ANGLE (PVG_FT_ANGLE_PI / 2)


    static PVG_FT_Error
        ft_stroke_border_arcto(PVG_FT_StrokeBorder  border,
            PVG_FT_Vector* center,
            PVG_FT_Fixed         radius,
            PVG_FT_Angle         angle_start,
            PVG_FT_Angle         angle_diff)
    {
        PVG_FT_Fixed   coef;
        PVG_FT_Vector  a0, a1, a2, a3;
        PVG_FT_Int     i, arcs = 1;
        PVG_FT_Error   error = 0;


        /* number of cubic arcs to draw */
        while (angle_diff > PVG_FT_ARC_CUBIC_ANGLE * arcs ||
            -angle_diff > PVG_FT_ARC_CUBIC_ANGLE * arcs)
            arcs++;

        /* control tangents */
        coef = PVG_FT_Tan(angle_diff / (4 * arcs));
        coef += coef / 3;

        /* compute start and first control point */
        PVG_FT_Vector_From_Polar(&a0, radius, angle_start);
        a1.x = PVG_FT_MulFix(-a0.y, coef);
        a1.y = PVG_FT_MulFix(a0.x, coef);

        a0.x += center->x;
        a0.y += center->y;
        a1.x += a0.x;
        a1.y += a0.y;

        for (i = 1; i <= arcs; i++)
        {
            /* compute end and second control point */
            PVG_FT_Vector_From_Polar(&a3, radius,
                angle_start + i * angle_diff / arcs);
            a2.x = PVG_FT_MulFix(a3.y, coef);
            a2.y = PVG_FT_MulFix(-a3.x, coef);

            a3.x += center->x;
            a3.y += center->y;
            a2.x += a3.x;
            a2.y += a3.y;

            /* add cubic arc */
            error = ft_stroke_border_cubicto(border, &a1, &a2, &a3);
            if (error)
                break;

            /* a0 = a3; */
            a1.x = a3.x - a2.x + a3.x;
            a1.y = a3.y - a2.y + a3.y;
        }

        return error;
    }

    static PVG_FT_Error ft_stroke_border_moveto(PVG_FT_StrokeBorder border,
        PVG_FT_Vector* to)
    {
        /* close current open path if any ? */
        if (border->start >= 0) ft_stroke_border_close(border, FALSE);

        border->start = border->num_points;
        border->movable = FALSE;

        return ft_stroke_border_lineto(border, to, FALSE);
    }

    static void ft_stroke_border_init(PVG_FT_StrokeBorder border)
    {
        border->points = NULL;
        border->tags = NULL;

        border->num_points = 0;
        border->max_points = 0;
        border->start = -1;
        border->valid = FALSE;
    }

    static void ft_stroke_border_reset(PVG_FT_StrokeBorder border)
    {
        border->num_points = 0;
        border->start = -1;
        border->valid = FALSE;
    }

    static void ft_stroke_border_done(PVG_FT_StrokeBorder border)
    {
        free(border->points);
        free(border->tags);

        border->num_points = 0;
        border->max_points = 0;
        border->start = -1;
        border->valid = FALSE;
    }

    static PVG_FT_Error ft_stroke_border_get_counts(PVG_FT_StrokeBorder border,
        PVG_FT_UInt* anum_points,
        PVG_FT_UInt* anum_contours)
    {
        PVG_FT_Error error = 0;
        PVG_FT_UInt  num_points = 0;
        PVG_FT_UInt  num_contours = 0;

        PVG_FT_UInt    count = border->num_points;
        PVG_FT_Vector* point = border->points;
        PVG_FT_Byte* tags = border->tags;
        PVG_FT_Int     in_contour = 0;

        for (; count > 0; count--, num_points++, point++, tags++) {
            if (tags[0] & PVG_FT_STROKE_TAG_BEGIN) {
                if (in_contour != 0) goto Fail;

                in_contour = 1;
            }
            else if (in_contour == 0)
                goto Fail;

            if (tags[0] & PVG_FT_STROKE_TAG_END) {
                in_contour = 0;
                num_contours++;
            }
        }

        if (in_contour != 0) goto Fail;

        border->valid = TRUE;

    Exit:
        *anum_points = num_points;
        *anum_contours = num_contours;
        return error;

    Fail:
        num_points = 0;
        num_contours = 0;
        goto Exit;
    }

    static void ft_stroke_border_export(PVG_FT_StrokeBorder border,
        PVG_FT_Outline* outline)
    {
        /* copy point locations */
        memcpy(outline->points + outline->n_points, border->points,
            border->num_points * sizeof(PVG_FT_Vector));

        /* copy tags */
        {
            PVG_FT_UInt  count = border->num_points;
            PVG_FT_Byte* read = border->tags;
            PVG_FT_Byte* write = (PVG_FT_Byte*)outline->tags + outline->n_points;

            for (; count > 0; count--, read++, write++) {
                if (*read & PVG_FT_STROKE_TAG_ON)
                    *write = PVG_FT_CURVE_TAG_ON;
                else if (*read & PVG_FT_STROKE_TAG_CUBIC)
                    *write = PVG_FT_CURVE_TAG_CUBIC;
                else
                    *write = PVG_FT_CURVE_TAG_CONIC;
            }
        }

        /* copy contours */
        {
            PVG_FT_UInt   count = border->num_points;
            PVG_FT_Byte* tags = border->tags;
            PVG_FT_Int* write = outline->contours + outline->n_contours;
            PVG_FT_Int  idx = (PVG_FT_Int)outline->n_points;

            for (; count > 0; count--, tags++, idx++) {
                if (*tags & PVG_FT_STROKE_TAG_END) {
                    *write++ = idx;
                    outline->n_contours++;
                }
            }
        }

        outline->n_points = (int)(outline->n_points + border->num_points);

        assert(PVG_FT_Outline_Check(outline) == 0);
    }

    /*************************************************************************/
    /*************************************************************************/
    /*****                                                               *****/
    /*****                           STROKER                             *****/
    /*****                                                               *****/
    /*************************************************************************/
    /*************************************************************************/

#define PVG_FT_SIDE_TO_ROTATE(s) (PVG_FT_ANGLE_PI2 - (s)*PVG_FT_ANGLE_PI)

    typedef struct PVG_FT_StrokerRec_ {
        PVG_FT_Angle  angle_in;            /* direction into curr join */
        PVG_FT_Angle  angle_out;           /* direction out of join  */
        PVG_FT_Vector center;              /* current position */
        PVG_FT_Fixed  line_length;         /* length of last lineto */
        PVG_FT_Bool   first_point;         /* is this the start? */
        PVG_FT_Bool   subpath_open;        /* is the subpath open? */
        PVG_FT_Angle  subpath_angle;       /* subpath start direction */
        PVG_FT_Vector subpath_start;       /* subpath start position */
        PVG_FT_Fixed  subpath_line_length; /* subpath start lineto len */
        PVG_FT_Bool   handle_wide_strokes; /* use wide strokes logic? */

        PVG_FT_Stroker_LineCap  line_cap;
        PVG_FT_Stroker_LineJoin line_join;
        PVG_FT_Stroker_LineJoin line_join_saved;
        PVG_FT_Fixed            miter_limit;
        PVG_FT_Fixed            radius;

        PVG_FT_StrokeBorderRec borders[2];
    } PVG_FT_StrokerRec;

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_New(PVG_FT_Stroker* astroker)
    {
        PVG_FT_Error   error = 0; /* assigned in PVG_FT_NEW */
        PVG_FT_Stroker stroker = NULL;

        stroker = (PVG_FT_StrokerRec*)calloc(1, sizeof(PVG_FT_StrokerRec));
        if (stroker) {
            ft_stroke_border_init(&stroker->borders[0]);
            ft_stroke_border_init(&stroker->borders[1]);
        }

        *astroker = stroker;

        return error;
    }

    void PVG_FT_Stroker_Rewind(PVG_FT_Stroker stroker)
    {
        if (stroker) {
            ft_stroke_border_reset(&stroker->borders[0]);
            ft_stroke_border_reset(&stroker->borders[1]);
        }
    }

    /* documentation is in ftstroke.h */

    void PVG_FT_Stroker_Set(PVG_FT_Stroker stroker, PVG_FT_Fixed radius,
        PVG_FT_Stroker_LineCap  line_cap,
        PVG_FT_Stroker_LineJoin line_join,
        PVG_FT_Fixed            miter_limit)
    {
        stroker->radius = radius;
        stroker->line_cap = line_cap;
        stroker->line_join = line_join;
        stroker->miter_limit = miter_limit;

        /* ensure miter limit has sensible value */
        if (stroker->miter_limit < 0x10000) stroker->miter_limit = 0x10000;

        /* save line join style:                                           */
        /* line join style can be temporarily changed when stroking curves */
        stroker->line_join_saved = line_join;

        PVG_FT_Stroker_Rewind(stroker);
    }

    /* documentation is in ftstroke.h */

    void PVG_FT_Stroker_Done(PVG_FT_Stroker stroker)
    {
        if (stroker) {
            ft_stroke_border_done(&stroker->borders[0]);
            ft_stroke_border_done(&stroker->borders[1]);

            free(stroker);
        }
    }

    /* create a circular arc at a corner or cap */
    static PVG_FT_Error ft_stroker_arcto(PVG_FT_Stroker stroker, PVG_FT_Int side)
    {
        PVG_FT_Angle        total, rotate;
        PVG_FT_Fixed        radius = stroker->radius;
        PVG_FT_Error        error = 0;
        PVG_FT_StrokeBorder border = stroker->borders + side;

        rotate = PVG_FT_SIDE_TO_ROTATE(side);

        total = PVG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
        if (total == PVG_FT_ANGLE_PI) total = -rotate * 2;

        error = ft_stroke_border_arcto(border, &stroker->center, radius,
            stroker->angle_in + rotate, total);
        border->movable = FALSE;
        return error;
    }

    /* add a cap at the end of an opened path */
    static PVG_FT_Error
        ft_stroker_cap(PVG_FT_Stroker stroker,
            PVG_FT_Angle angle,
            PVG_FT_Int side)
    {
        PVG_FT_Error error = 0;

        if (stroker->line_cap == PVG_FT_STROKER_LINECAP_ROUND)
        {
            /* add a round cap */
            stroker->angle_in = angle;
            stroker->angle_out = angle + PVG_FT_ANGLE_PI;

            error = ft_stroker_arcto(stroker, side);
        }
        else
        {
            /* add a square or butt cap */
            PVG_FT_Vector        middle, delta;
            PVG_FT_Fixed         radius = stroker->radius;
            PVG_FT_StrokeBorder  border = stroker->borders + side;

            /* compute middle point and first angle point */
            PVG_FT_Vector_From_Polar(&middle, radius, angle);
            delta.x = side ? middle.y : -middle.y;
            delta.y = side ? -middle.x : middle.x;

            if (stroker->line_cap == PVG_FT_STROKER_LINECAP_SQUARE)
            {
                middle.x += stroker->center.x;
                middle.y += stroker->center.y;
            }
            else  /* PVG_FT_STROKER_LINECAP_BUTT */
            {
                middle.x = stroker->center.x;
                middle.y = stroker->center.y;
            }

            delta.x += middle.x;
            delta.y += middle.y;

            error = ft_stroke_border_lineto(border, &delta, FALSE);
            if (error)
                goto Exit;

            /* compute second angle point */
            delta.x = middle.x - delta.x + middle.x;
            delta.y = middle.y - delta.y + middle.y;

            error = ft_stroke_border_lineto(border, &delta, FALSE);
        }

    Exit:
        return error;
    }

    /* process an inside corner, i.e. compute intersection */
    static PVG_FT_Error ft_stroker_inside(PVG_FT_Stroker stroker, PVG_FT_Int side,
        PVG_FT_Fixed line_length)
    {
        PVG_FT_StrokeBorder border = stroker->borders + side;
        PVG_FT_Angle        phi, theta, rotate;
        PVG_FT_Fixed        length;
        PVG_FT_Vector       sigma = { 0, 0 };
        PVG_FT_Vector       delta;
        PVG_FT_Error        error = 0;
        PVG_FT_Bool         intersect; /* use intersection of lines? */

        rotate = PVG_FT_SIDE_TO_ROTATE(side);

        theta = PVG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;

        /* Only intersect borders if between two lineto's and both */
        /* lines are long enough (line_length is zero for curves). */
        if (!border->movable || line_length == 0 ||
            theta > 0x59C000 || theta < -0x59C000)
            intersect = FALSE;
        else {
            /* compute minimum required length of lines */
            PVG_FT_Fixed  min_length;


            PVG_FT_Vector_Unit(&sigma, theta);
            min_length =
                ft_pos_abs(PVG_FT_MulDiv(stroker->radius, sigma.y, sigma.x));

            intersect = PVG_FT_BOOL(min_length &&
                stroker->line_length >= min_length &&
                line_length >= min_length);
        }

        if (!intersect) {
            PVG_FT_Vector_From_Polar(&delta, stroker->radius,
                stroker->angle_out + rotate);
            delta.x += stroker->center.x;
            delta.y += stroker->center.y;

            border->movable = FALSE;
        }
        else {
            /* compute median angle */
            phi = stroker->angle_in + theta + rotate;

            length = PVG_FT_DivFix(stroker->radius, sigma.x);

            PVG_FT_Vector_From_Polar(&delta, length, phi);
            delta.x += stroker->center.x;
            delta.y += stroker->center.y;
        }

        error = ft_stroke_border_lineto(border, &delta, FALSE);

        return error;
    }


    /* process an outside corner, i.e. compute bevel/miter/round */
    static PVG_FT_Error
        ft_stroker_outside(PVG_FT_Stroker  stroker,
            PVG_FT_Int      side,
            PVG_FT_Fixed    line_length)
    {
        PVG_FT_StrokeBorder  border = stroker->borders + side;
        PVG_FT_Error         error;
        PVG_FT_Angle         rotate;


        if (stroker->line_join == PVG_FT_STROKER_LINEJOIN_ROUND)
            error = ft_stroker_arcto(stroker, side);
        else
        {
            /* this is a mitered (pointed) or beveled (truncated) corner */
            PVG_FT_Fixed   radius = stroker->radius;
            PVG_FT_Vector  sigma = { 0, 0 };
            PVG_FT_Angle   theta = 0, phi = 0;
            PVG_FT_Bool    bevel, fixed_bevel;


            rotate = PVG_FT_SIDE_TO_ROTATE(side);

            bevel =
                PVG_FT_BOOL(stroker->line_join == PVG_FT_STROKER_LINEJOIN_BEVEL);

            fixed_bevel =
                PVG_FT_BOOL(stroker->line_join != PVG_FT_STROKER_LINEJOIN_MITER_VARIABLE);

            /* check miter limit first */
            if (!bevel)
            {
                theta = PVG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;

                if (theta == PVG_FT_ANGLE_PI2)
                    theta = -rotate;

                phi = stroker->angle_in + theta + rotate;

                PVG_FT_Vector_From_Polar(&sigma, stroker->miter_limit, theta);

                /* is miter limit exceeded? */
                if (sigma.x < 0x10000L)
                {
                    /* don't create variable bevels for very small deviations; */
                    /* FT_Sin(x) = 0 for x <= 57                               */
                    if (fixed_bevel || ft_pos_abs(theta) > 57)
                        bevel = TRUE;
                }
            }

            if (bevel)  /* this is a bevel (broken angle) */
            {
                if (fixed_bevel)
                {
                    /* the outer corners are simply joined together */
                    PVG_FT_Vector  delta;


                    /* add bevel */
                    PVG_FT_Vector_From_Polar(&delta,
                        radius,
                        stroker->angle_out + rotate);
                    delta.x += stroker->center.x;
                    delta.y += stroker->center.y;

                    border->movable = FALSE;
                    error = ft_stroke_border_lineto(border, &delta, FALSE);
                }
                else /* variable bevel or clipped miter */
                {
                    /* the miter is truncated */
                    PVG_FT_Vector  middle, delta;
                    PVG_FT_Fixed   coef;


                    /* compute middle point and first angle point */
                    PVG_FT_Vector_From_Polar(&middle,
                        PVG_FT_MulFix(radius, stroker->miter_limit),
                        phi);

                    coef = PVG_FT_DivFix(0x10000L - sigma.x, sigma.y);
                    delta.x = PVG_FT_MulFix(middle.y, coef);
                    delta.y = PVG_FT_MulFix(-middle.x, coef);

                    middle.x += stroker->center.x;
                    middle.y += stroker->center.y;
                    delta.x += middle.x;
                    delta.y += middle.y;

                    error = ft_stroke_border_lineto(border, &delta, FALSE);
                    if (error)
                        goto Exit;

                    /* compute second angle point */
                    delta.x = middle.x - delta.x + middle.x;
                    delta.y = middle.y - delta.y + middle.y;

                    error = ft_stroke_border_lineto(border, &delta, FALSE);
                    if (error)
                        goto Exit;

                    /* finally, add an end point; only needed if not lineto */
                    /* (line_length is zero for curves)                     */
                    if (line_length == 0)
                    {
                        PVG_FT_Vector_From_Polar(&delta,
                            radius,
                            stroker->angle_out + rotate);

                        delta.x += stroker->center.x;
                        delta.y += stroker->center.y;

                        error = ft_stroke_border_lineto(border, &delta, FALSE);
                    }
                }
            }
            else /* this is a miter (intersection) */
            {
                PVG_FT_Fixed   length;
                PVG_FT_Vector  delta;


                length = PVG_FT_MulDiv(stroker->radius, stroker->miter_limit, sigma.x);

                PVG_FT_Vector_From_Polar(&delta, length, phi);
                delta.x += stroker->center.x;
                delta.y += stroker->center.y;

                error = ft_stroke_border_lineto(border, &delta, FALSE);
                if (error)
                    goto Exit;

                /* now add an end point; only needed if not lineto */
                /* (line_length is zero for curves)                */
                if (line_length == 0)
                {
                    PVG_FT_Vector_From_Polar(&delta,
                        stroker->radius,
                        stroker->angle_out + rotate);
                    delta.x += stroker->center.x;
                    delta.y += stroker->center.y;

                    error = ft_stroke_border_lineto(border, &delta, FALSE);
                }
            }
        }

    Exit:
        return error;
    }

    static PVG_FT_Error ft_stroker_process_corner(PVG_FT_Stroker stroker,
        PVG_FT_Fixed   line_length)
    {
        PVG_FT_Error error = 0;
        PVG_FT_Angle turn;
        PVG_FT_Int   inside_side;

        turn = PVG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);

        /* no specific corner processing is required if the turn is 0 */
        if (turn == 0) goto Exit;

        /* when we turn to the right, the inside side is 0 */
        inside_side = 0;

        /* otherwise, the inside side is 1 */
        if (turn < 0) inside_side = 1;

        /* process the inside side */
        error = ft_stroker_inside(stroker, inside_side, line_length);
        if (error) goto Exit;

        /* process the outside side */
        error = ft_stroker_outside(stroker, 1 - inside_side, line_length);

    Exit:
        return error;
    }

    /* add two points to the left and right borders corresponding to the */
    /* start of the subpath                                              */
    static PVG_FT_Error ft_stroker_subpath_start(PVG_FT_Stroker stroker,
        PVG_FT_Angle   start_angle,
        PVG_FT_Fixed   line_length)
    {
        PVG_FT_Vector       delta;
        PVG_FT_Vector       point;
        PVG_FT_Error        error;
        PVG_FT_StrokeBorder border;

        PVG_FT_Vector_From_Polar(&delta, stroker->radius,
            start_angle + PVG_FT_ANGLE_PI2);

        point.x = stroker->center.x + delta.x;
        point.y = stroker->center.y + delta.y;

        border = stroker->borders;
        error = ft_stroke_border_moveto(border, &point);
        if (error) goto Exit;

        point.x = stroker->center.x - delta.x;
        point.y = stroker->center.y - delta.y;

        border++;
        error = ft_stroke_border_moveto(border, &point);

        /* save angle, position, and line length for last join */
        /* (line_length is zero for curves)                    */
        stroker->subpath_angle = start_angle;
        stroker->first_point = FALSE;
        stroker->subpath_line_length = line_length;

    Exit:
        return error;
    }

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_LineTo(PVG_FT_Stroker stroker, PVG_FT_Vector* to)
    {
        PVG_FT_Error        error = 0;
        PVG_FT_StrokeBorder border;
        PVG_FT_Vector       delta;
        PVG_FT_Angle        angle;
        PVG_FT_Int          side;
        PVG_FT_Fixed        line_length;

        delta.x = to->x - stroker->center.x;
        delta.y = to->y - stroker->center.y;

        /* a zero-length lineto is a no-op; avoid creating a spurious corner */
        if (delta.x == 0 && delta.y == 0) goto Exit;

        /* compute length of line */
        line_length = PVG_FT_Vector_Length(&delta);

        angle = PVG_FT_Atan2(delta.x, delta.y);
        PVG_FT_Vector_From_Polar(&delta, stroker->radius, angle + PVG_FT_ANGLE_PI2);

        /* process corner if necessary */
        if (stroker->first_point) {
            /* This is the first segment of a subpath.  We need to     */
            /* add a point to each border at their respective starting */
            /* point locations.                                        */
            error = ft_stroker_subpath_start(stroker, angle, line_length);
            if (error) goto Exit;
        }
        else {
            /* process the current corner */
            stroker->angle_out = angle;
            error = ft_stroker_process_corner(stroker, line_length);
            if (error) goto Exit;
        }

        /* now add a line segment to both the `inside' and `outside' paths */
        for (border = stroker->borders, side = 1; side >= 0; side--, border++) {
            PVG_FT_Vector point;

            point.x = to->x + delta.x;
            point.y = to->y + delta.y;

            /* the ends of lineto borders are movable */
            error = ft_stroke_border_lineto(border, &point, TRUE);
            if (error) goto Exit;

            delta.x = -delta.x;
            delta.y = -delta.y;
        }

        stroker->angle_in = angle;
        stroker->center = *to;
        stroker->line_length = line_length;

    Exit:
        return error;
    }

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_ConicTo(PVG_FT_Stroker stroker, PVG_FT_Vector* control,
        PVG_FT_Vector* to)
    {
        PVG_FT_Error   error = 0;
        PVG_FT_Vector  bez_stack[34];
        PVG_FT_Vector* arc;
        PVG_FT_Vector* limit = bez_stack + 30;
        PVG_FT_Bool    first_arc = TRUE;

        /* if all control points are coincident, this is a no-op; */
        /* avoid creating a spurious corner                       */
        if (PVG_FT_IS_SMALL(stroker->center.x - control->x) &&
            PVG_FT_IS_SMALL(stroker->center.y - control->y) &&
            PVG_FT_IS_SMALL(control->x - to->x) &&
            PVG_FT_IS_SMALL(control->y - to->y)) {
            stroker->center = *to;
            goto Exit;
        }

        arc = bez_stack;
        arc[0] = *to;
        arc[1] = *control;
        arc[2] = stroker->center;

        while (arc >= bez_stack) {
            PVG_FT_Angle angle_in, angle_out;

            /* initialize with current direction */
            angle_in = angle_out = stroker->angle_in;

            if (arc < limit &&
                !ft_conic_is_small_enough(arc, &angle_in, &angle_out)) {
                if (stroker->first_point) stroker->angle_in = angle_in;

                ft_conic_split(arc);
                arc += 2;
                continue;
            }

            if (first_arc) {
                first_arc = FALSE;

                /* process corner if necessary */
                if (stroker->first_point)
                    error = ft_stroker_subpath_start(stroker, angle_in, 0);
                else {
                    stroker->angle_out = angle_in;
                    error = ft_stroker_process_corner(stroker, 0);
                }
            }
            else if (ft_pos_abs(PVG_FT_Angle_Diff(stroker->angle_in, angle_in)) >
                PVG_FT_SMALL_CONIC_THRESHOLD / 4) {
                /* if the deviation from one arc to the next is too great, */
                /* add a round corner                                      */
                stroker->center = arc[2];
                stroker->angle_out = angle_in;
                stroker->line_join = PVG_FT_STROKER_LINEJOIN_ROUND;

                error = ft_stroker_process_corner(stroker, 0);

                /* reinstate line join style */
                stroker->line_join = stroker->line_join_saved;
            }

            if (error) goto Exit;

            /* the arc's angle is small enough; we can add it directly to each */
            /* border                                                          */
            {
                PVG_FT_Vector       ctrl, end;
                PVG_FT_Angle        theta, phi, rotate, alpha0 = 0;
                PVG_FT_Fixed        length;
                PVG_FT_StrokeBorder border;
                PVG_FT_Int          side;

                theta = PVG_FT_Angle_Diff(angle_in, angle_out) / 2;
                phi = angle_in + theta;
                length = PVG_FT_DivFix(stroker->radius, PVG_FT_Cos(theta));

                /* compute direction of original arc */
                if (stroker->handle_wide_strokes)
                    alpha0 = PVG_FT_Atan2(arc[0].x - arc[2].x, arc[0].y - arc[2].y);

                for (border = stroker->borders, side = 0; side <= 1;
                    side++, border++) {
                    rotate = PVG_FT_SIDE_TO_ROTATE(side);

                    /* compute control point */
                    PVG_FT_Vector_From_Polar(&ctrl, length, phi + rotate);
                    ctrl.x += arc[1].x;
                    ctrl.y += arc[1].y;

                    /* compute end point */
                    PVG_FT_Vector_From_Polar(&end, stroker->radius,
                        angle_out + rotate);
                    end.x += arc[0].x;
                    end.y += arc[0].y;

                    if (stroker->handle_wide_strokes) {
                        PVG_FT_Vector start;
                        PVG_FT_Angle  alpha1;

                        /* determine whether the border radius is greater than the
                         */
                         /* radius of curvature of the original arc */
                        start = border->points[border->num_points - 1];

                        alpha1 = PVG_FT_Atan2(end.x - start.x, end.y - start.y);

                        /* is the direction of the border arc opposite to */
                        /* that of the original arc? */
                        if (ft_pos_abs(PVG_FT_Angle_Diff(alpha0, alpha1)) >
                            PVG_FT_ANGLE_PI / 2) {
                            PVG_FT_Angle  beta, gamma;
                            PVG_FT_Vector bvec, delta;
                            PVG_FT_Fixed  blen, sinA, sinB, alen;

                            /* use the sine rule to find the intersection point */
                            beta =
                                PVG_FT_Atan2(arc[2].x - start.x, arc[2].y - start.y);
                            gamma = PVG_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);

                            bvec.x = end.x - start.x;
                            bvec.y = end.y - start.y;

                            blen = PVG_FT_Vector_Length(&bvec);

                            sinA = ft_pos_abs(PVG_FT_Sin(alpha1 - gamma));
                            sinB = ft_pos_abs(PVG_FT_Sin(beta - gamma));

                            alen = PVG_FT_MulDiv(blen, sinA, sinB);

                            PVG_FT_Vector_From_Polar(&delta, alen, beta);
                            delta.x += start.x;
                            delta.y += start.y;

                            /* circumnavigate the negative sector backwards */
                            border->movable = FALSE;
                            error = ft_stroke_border_lineto(border, &delta, FALSE);
                            if (error) goto Exit;
                            error = ft_stroke_border_lineto(border, &end, FALSE);
                            if (error) goto Exit;
                            error = ft_stroke_border_conicto(border, &ctrl, &start);
                            if (error) goto Exit;
                            /* and then move to the endpoint */
                            error = ft_stroke_border_lineto(border, &end, FALSE);
                            if (error) goto Exit;

                            continue;
                        }

                        /* else fall through */
                    }

                    /* simply add an arc */
                    error = ft_stroke_border_conicto(border, &ctrl, &end);
                    if (error) goto Exit;
                }
            }

            arc -= 2;

            stroker->angle_in = angle_out;
        }

        stroker->center = *to;
        stroker->line_length = 0;

    Exit:
        return error;
    }

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_CubicTo(PVG_FT_Stroker stroker, PVG_FT_Vector* control1,
        PVG_FT_Vector* control2, PVG_FT_Vector* to)
    {
        PVG_FT_Error   error = 0;
        PVG_FT_Vector  bez_stack[37];
        PVG_FT_Vector* arc;
        PVG_FT_Vector* limit = bez_stack + 32;
        PVG_FT_Bool    first_arc = TRUE;

        /* if all control points are coincident, this is a no-op; */
        /* avoid creating a spurious corner */
        if (PVG_FT_IS_SMALL(stroker->center.x - control1->x) &&
            PVG_FT_IS_SMALL(stroker->center.y - control1->y) &&
            PVG_FT_IS_SMALL(control1->x - control2->x) &&
            PVG_FT_IS_SMALL(control1->y - control2->y) &&
            PVG_FT_IS_SMALL(control2->x - to->x) &&
            PVG_FT_IS_SMALL(control2->y - to->y)) {
            stroker->center = *to;
            goto Exit;
        }

        arc = bez_stack;
        arc[0] = *to;
        arc[1] = *control2;
        arc[2] = *control1;
        arc[3] = stroker->center;

        while (arc >= bez_stack) {
            PVG_FT_Angle angle_in, angle_mid, angle_out;

            /* initialize with current direction */
            angle_in = angle_out = angle_mid = stroker->angle_in;

            if (arc < limit &&
                !ft_cubic_is_small_enough(arc, &angle_in, &angle_mid, &angle_out)) {
                if (stroker->first_point) stroker->angle_in = angle_in;

                ft_cubic_split(arc);
                arc += 3;
                continue;
            }

            if (first_arc) {
                first_arc = FALSE;

                /* process corner if necessary */
                if (stroker->first_point)
                    error = ft_stroker_subpath_start(stroker, angle_in, 0);
                else {
                    stroker->angle_out = angle_in;
                    error = ft_stroker_process_corner(stroker, 0);
                }
            }
            else if (ft_pos_abs(PVG_FT_Angle_Diff(stroker->angle_in, angle_in)) >
                PVG_FT_SMALL_CUBIC_THRESHOLD / 4) {
                /* if the deviation from one arc to the next is too great, */
                /* add a round corner                                      */
                stroker->center = arc[3];
                stroker->angle_out = angle_in;
                stroker->line_join = PVG_FT_STROKER_LINEJOIN_ROUND;

                error = ft_stroker_process_corner(stroker, 0);

                /* reinstate line join style */
                stroker->line_join = stroker->line_join_saved;
            }

            if (error) goto Exit;

            /* the arc's angle is small enough; we can add it directly to each */
            /* border                                                          */
            {
                PVG_FT_Vector       ctrl1, ctrl2, end;
                PVG_FT_Angle        theta1, phi1, theta2, phi2, rotate, alpha0 = 0;
                PVG_FT_Fixed        length1, length2;
                PVG_FT_StrokeBorder border;
                PVG_FT_Int          side;

                theta1 = PVG_FT_Angle_Diff(angle_in, angle_mid) / 2;
                theta2 = PVG_FT_Angle_Diff(angle_mid, angle_out) / 2;
                phi1 = ft_angle_mean(angle_in, angle_mid);
                phi2 = ft_angle_mean(angle_mid, angle_out);
                length1 = PVG_FT_DivFix(stroker->radius, PVG_FT_Cos(theta1));
                length2 = PVG_FT_DivFix(stroker->radius, PVG_FT_Cos(theta2));

                /* compute direction of original arc */
                if (stroker->handle_wide_strokes)
                    alpha0 = PVG_FT_Atan2(arc[0].x - arc[3].x, arc[0].y - arc[3].y);

                for (border = stroker->borders, side = 0; side <= 1;
                    side++, border++) {
                    rotate = PVG_FT_SIDE_TO_ROTATE(side);

                    /* compute control points */
                    PVG_FT_Vector_From_Polar(&ctrl1, length1, phi1 + rotate);
                    ctrl1.x += arc[2].x;
                    ctrl1.y += arc[2].y;

                    PVG_FT_Vector_From_Polar(&ctrl2, length2, phi2 + rotate);
                    ctrl2.x += arc[1].x;
                    ctrl2.y += arc[1].y;

                    /* compute end point */
                    PVG_FT_Vector_From_Polar(&end, stroker->radius,
                        angle_out + rotate);
                    end.x += arc[0].x;
                    end.y += arc[0].y;

                    if (stroker->handle_wide_strokes) {
                        PVG_FT_Vector start;
                        PVG_FT_Angle  alpha1;

                        /* determine whether the border radius is greater than the
                         */
                         /* radius of curvature of the original arc */
                        start = border->points[border->num_points - 1];

                        alpha1 = PVG_FT_Atan2(end.x - start.x, end.y - start.y);

                        /* is the direction of the border arc opposite to */
                        /* that of the original arc? */
                        if (ft_pos_abs(PVG_FT_Angle_Diff(alpha0, alpha1)) >
                            PVG_FT_ANGLE_PI / 2) {
                            PVG_FT_Angle  beta, gamma;
                            PVG_FT_Vector bvec, delta;
                            PVG_FT_Fixed  blen, sinA, sinB, alen;

                            /* use the sine rule to find the intersection point */
                            beta =
                                PVG_FT_Atan2(arc[3].x - start.x, arc[3].y - start.y);
                            gamma = PVG_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);

                            bvec.x = end.x - start.x;
                            bvec.y = end.y - start.y;

                            blen = PVG_FT_Vector_Length(&bvec);

                            sinA = ft_pos_abs(PVG_FT_Sin(alpha1 - gamma));
                            sinB = ft_pos_abs(PVG_FT_Sin(beta - gamma));

                            alen = PVG_FT_MulDiv(blen, sinA, sinB);

                            PVG_FT_Vector_From_Polar(&delta, alen, beta);
                            delta.x += start.x;
                            delta.y += start.y;

                            /* circumnavigate the negative sector backwards */
                            border->movable = FALSE;
                            error = ft_stroke_border_lineto(border, &delta, FALSE);
                            if (error) goto Exit;
                            error = ft_stroke_border_lineto(border, &end, FALSE);
                            if (error) goto Exit;
                            error = ft_stroke_border_cubicto(border, &ctrl2, &ctrl1,
                                &start);
                            if (error) goto Exit;
                            /* and then move to the endpoint */
                            error = ft_stroke_border_lineto(border, &end, FALSE);
                            if (error) goto Exit;

                            continue;
                        }

                        /* else fall through */
                    }

                    /* simply add an arc */
                    error = ft_stroke_border_cubicto(border, &ctrl1, &ctrl2, &end);
                    if (error) goto Exit;
                }
            }

            arc -= 3;

            stroker->angle_in = angle_out;
        }

        stroker->center = *to;
        stroker->line_length = 0;

    Exit:
        return error;
    }

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_BeginSubPath(PVG_FT_Stroker stroker, PVG_FT_Vector* to,
        PVG_FT_Bool open)
    {
        /* We cannot process the first point, because there is not enough      */
        /* information regarding its corner/cap.  The latter will be processed */
        /* in the `PVG_FT_Stroker_EndSubPath' routine.                             */
        /*                                                                     */
        stroker->first_point = TRUE;
        stroker->center = *to;
        stroker->subpath_open = open;

        /* Determine if we need to check whether the border radius is greater */
        /* than the radius of curvature of a curve, to handle this case       */
        /* specially.  This is only required if bevel joins or butt caps may  */
        /* be created, because round & miter joins and round & square caps    */
        /* cover the negative sector created with wide strokes.               */
        stroker->handle_wide_strokes =
            PVG_FT_BOOL(stroker->line_join != PVG_FT_STROKER_LINEJOIN_ROUND ||
                (stroker->subpath_open &&
                    stroker->line_cap == PVG_FT_STROKER_LINECAP_BUTT));

        /* record the subpath start point for each border */
        stroker->subpath_start = *to;

        stroker->angle_in = 0;

        return 0;
    }

    static PVG_FT_Error ft_stroker_add_reverse_left(PVG_FT_Stroker stroker,
        PVG_FT_Bool    open)
    {
        PVG_FT_StrokeBorder right = stroker->borders + 0;
        PVG_FT_StrokeBorder left = stroker->borders + 1;
        PVG_FT_Int          new_points;
        PVG_FT_Error        error = 0;

        assert(left->start >= 0);

        new_points = left->num_points - left->start;
        if (new_points > 0) {
            error = ft_stroke_border_grow(right, (PVG_FT_UInt)new_points);
            if (error) goto Exit;

            {
                PVG_FT_Vector* dst_point = right->points + right->num_points;
                PVG_FT_Byte* dst_tag = right->tags + right->num_points;
                PVG_FT_Vector* src_point = left->points + left->num_points - 1;
                PVG_FT_Byte* src_tag = left->tags + left->num_points - 1;

                while (src_point >= left->points + left->start) {
                    *dst_point = *src_point;
                    *dst_tag = *src_tag;

                    if (open)
                        dst_tag[0] &= ~PVG_FT_STROKE_TAG_BEGIN_END;
                    else {
                        PVG_FT_Byte ttag =
                            (PVG_FT_Byte)(dst_tag[0] & PVG_FT_STROKE_TAG_BEGIN_END);

                        /* switch begin/end tags if necessary */
                        if (ttag == PVG_FT_STROKE_TAG_BEGIN ||
                            ttag == PVG_FT_STROKE_TAG_END)
                            dst_tag[0] ^= PVG_FT_STROKE_TAG_BEGIN_END;
                    }

                    src_point--;
                    src_tag--;
                    dst_point++;
                    dst_tag++;
                }
            }

            left->num_points = left->start;
            right->num_points += new_points;

            right->movable = FALSE;
            left->movable = FALSE;
        }

    Exit:
        return error;
    }

    /* documentation is in ftstroke.h */

    /* there's a lot of magic in this function! */
    PVG_FT_Error PVG_FT_Stroker_EndSubPath(PVG_FT_Stroker stroker)
    {
        PVG_FT_Error error = 0;

        if (stroker->subpath_open) {
            PVG_FT_StrokeBorder right = stroker->borders;

            /* All right, this is an opened path, we need to add a cap between */
            /* right & left, add the reverse of left, then add a final cap     */
            /* between left & right.                                           */
            error = ft_stroker_cap(stroker, stroker->angle_in, 0);
            if (error) goto Exit;

            /* add reversed points from `left' to `right' */
            error = ft_stroker_add_reverse_left(stroker, TRUE);
            if (error) goto Exit;

            /* now add the final cap */
            stroker->center = stroker->subpath_start;
            error =
                ft_stroker_cap(stroker, stroker->subpath_angle + PVG_FT_ANGLE_PI, 0);
            if (error) goto Exit;

            /* Now end the right subpath accordingly.  The left one is */
            /* rewind and doesn't need further processing.             */
            ft_stroke_border_close(right, FALSE);
        }
        else {
            PVG_FT_Angle turn;
            PVG_FT_Int   inside_side;

            /* close the path if needed */
            if (stroker->center.x != stroker->subpath_start.x ||
                stroker->center.y != stroker->subpath_start.y) {
                error = PVG_FT_Stroker_LineTo(stroker, &stroker->subpath_start);
                if (error) goto Exit;
            }

            /* process the corner */
            stroker->angle_out = stroker->subpath_angle;
            turn = PVG_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);

            /* no specific corner processing is required if the turn is 0 */
            if (turn != 0) {
                /* when we turn to the right, the inside side is 0 */
                inside_side = 0;

                /* otherwise, the inside side is 1 */
                if (turn < 0) inside_side = 1;

                error = ft_stroker_inside(stroker, inside_side,
                    stroker->subpath_line_length);
                if (error) goto Exit;

                /* process the outside side */
                error = ft_stroker_outside(stroker, 1 - inside_side,
                    stroker->subpath_line_length);
                if (error) goto Exit;
            }

            /* then end our two subpaths */
            ft_stroke_border_close(stroker->borders + 0, FALSE);
            ft_stroke_border_close(stroker->borders + 1, TRUE);
        }

    Exit:
        return error;
    }

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_GetBorderCounts(PVG_FT_Stroker       stroker,
        PVG_FT_StrokerBorder border,
        PVG_FT_UInt* anum_points,
        PVG_FT_UInt* anum_contours)
    {
        PVG_FT_UInt  num_points = 0, num_contours = 0;
        PVG_FT_Error error;

        if (!stroker || border > 1) {
            error = -1;  // PVG_FT_THROW( Invalid_Argument );
            goto Exit;
        }

        error = ft_stroke_border_get_counts(stroker->borders + border, &num_points,
            &num_contours);
    Exit:
        if (anum_points) *anum_points = num_points;

        if (anum_contours) *anum_contours = num_contours;

        return error;
    }

    /* documentation is in ftstroke.h */

    PVG_FT_Error PVG_FT_Stroker_GetCounts(PVG_FT_Stroker stroker,
        PVG_FT_UInt* anum_points,
        PVG_FT_UInt* anum_contours)
    {
        PVG_FT_UInt  count1, count2, num_points = 0;
        PVG_FT_UInt  count3, count4, num_contours = 0;
        PVG_FT_Error error;

        error = ft_stroke_border_get_counts(stroker->borders + 0, &count1, &count2);
        if (error) goto Exit;

        error = ft_stroke_border_get_counts(stroker->borders + 1, &count3, &count4);
        if (error) goto Exit;

        num_points = count1 + count3;
        num_contours = count2 + count4;

    Exit:
        *anum_points = num_points;
        *anum_contours = num_contours;
        return error;
    }

    /* documentation is in ftstroke.h */

    void PVG_FT_Stroker_ExportBorder(PVG_FT_Stroker       stroker,
        PVG_FT_StrokerBorder border,
        PVG_FT_Outline* outline)
    {
        if (border == PVG_FT_STROKER_BORDER_LEFT ||
            border == PVG_FT_STROKER_BORDER_RIGHT) {
            PVG_FT_StrokeBorder sborder = &stroker->borders[border];

            if (sborder->valid) ft_stroke_border_export(sborder, outline);
        }
    }

    /* documentation is in ftstroke.h */

    void PVG_FT_Stroker_Export(PVG_FT_Stroker stroker, PVG_FT_Outline* outline)
    {
        PVG_FT_Stroker_ExportBorder(stroker, PVG_FT_STROKER_BORDER_LEFT, outline);
        PVG_FT_Stroker_ExportBorder(stroker, PVG_FT_STROKER_BORDER_RIGHT, outline);
    }

    /* documentation is in ftstroke.h */

    /*
     *  The following is very similar to PVG_FT_Outline_Decompose, except
     *  that we do support opened paths, and do not scale the outline.
     */
    PVG_FT_Error PVG_FT_Stroker_ParseOutline(PVG_FT_Stroker        stroker,
        const PVG_FT_Outline* outline)
    {
        PVG_FT_Vector v_last;
        PVG_FT_Vector v_control;
        PVG_FT_Vector v_start;

        PVG_FT_Vector* point;
        PVG_FT_Vector* limit;
        char* tags;

        PVG_FT_Error error;

        PVG_FT_Int  n;     /* index of contour in outline     */
        PVG_FT_UInt first; /* index of first point in contour */
        PVG_FT_Int  tag;   /* current point's state           */

        if (!outline || !stroker) return -1;  // PVG_FT_THROW( Invalid_Argument );

        PVG_FT_Stroker_Rewind(stroker);

        first = 0;

        for (n = 0; n < outline->n_contours; n++) {
            PVG_FT_UInt last; /* index of last point in contour */

            last = outline->contours[n];
            limit = outline->points + last;

            /* skip empty points; we don't stroke these */
            if (last <= first) {
                first = last + 1;
                continue;
            }

            v_start = outline->points[first];
            v_last = outline->points[last];

            v_control = v_start;

            point = outline->points + first;
            tags = outline->tags + first;
            tag = PVG_FT_CURVE_TAG(tags[0]);

            /* A contour cannot start with a cubic control point! */
            if (tag == PVG_FT_CURVE_TAG_CUBIC) goto Invalid_Outline;

            /* check first point to determine origin */
            if (tag == PVG_FT_CURVE_TAG_CONIC) {
                /* First point is conic control.  Yes, this happens. */
                if (PVG_FT_CURVE_TAG(outline->tags[last]) == PVG_FT_CURVE_TAG_ON) {
                    /* start at last point if it is on the curve */
                    v_start = v_last;
                    limit--;
                }
                else {
                    /* if both first and last points are conic, */
                    /* start at their middle                    */
                    v_start.x = (v_start.x + v_last.x) / 2;
                    v_start.y = (v_start.y + v_last.y) / 2;
                }
                point--;
                tags--;
            }

            error = PVG_FT_Stroker_BeginSubPath(stroker, &v_start, outline->contours_flag[n]);
            if (error) goto Exit;

            while (point < limit) {
                point++;
                tags++;

                tag = PVG_FT_CURVE_TAG(tags[0]);
                switch (tag) {
                case PVG_FT_CURVE_TAG_ON: /* emit a single line_to */
                {
                    PVG_FT_Vector vec;

                    vec.x = point->x;
                    vec.y = point->y;

                    error = PVG_FT_Stroker_LineTo(stroker, &vec);
                    if (error) goto Exit;
                    continue;
                }

                case PVG_FT_CURVE_TAG_CONIC: /* consume conic arcs */
                    v_control.x = point->x;
                    v_control.y = point->y;

                Do_Conic:
                    if (point < limit) {
                        PVG_FT_Vector vec;
                        PVG_FT_Vector v_middle;

                        point++;
                        tags++;
                        tag = PVG_FT_CURVE_TAG(tags[0]);

                        vec = point[0];

                        if (tag == PVG_FT_CURVE_TAG_ON) {
                            error =
                                PVG_FT_Stroker_ConicTo(stroker, &v_control, &vec);
                            if (error) goto Exit;
                            continue;
                        }

                        if (tag != PVG_FT_CURVE_TAG_CONIC) goto Invalid_Outline;

                        v_middle.x = (v_control.x + vec.x) / 2;
                        v_middle.y = (v_control.y + vec.y) / 2;

                        error =
                            PVG_FT_Stroker_ConicTo(stroker, &v_control, &v_middle);
                        if (error) goto Exit;

                        v_control = vec;
                        goto Do_Conic;
                    }

                    error = PVG_FT_Stroker_ConicTo(stroker, &v_control, &v_start);
                    goto Close;

                default: /* PVG_FT_CURVE_TAG_CUBIC */
                {
                    PVG_FT_Vector vec1, vec2;

                    if (point + 1 > limit ||
                        PVG_FT_CURVE_TAG(tags[1]) != PVG_FT_CURVE_TAG_CUBIC)
                        goto Invalid_Outline;

                    point += 2;
                    tags += 2;

                    vec1 = point[-2];
                    vec2 = point[-1];

                    if (point <= limit) {
                        PVG_FT_Vector vec;

                        vec = point[0];

                        error = PVG_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &vec);
                        if (error) goto Exit;
                        continue;
                    }

                    error = PVG_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &v_start);
                    goto Close;
                }
                }
            }

        Close:
            if (error) goto Exit;

            if (stroker->first_point) {
                stroker->subpath_open = TRUE;
                error = ft_stroker_subpath_start(stroker, 0, 0);
                if (error) goto Exit;
            }

            error = PVG_FT_Stroker_EndSubPath(stroker);
            if (error) goto Exit;

            first = last + 1;
        }

        return 0;

    Exit:
        return error;

    Invalid_Outline:
        return -2;  // PVG_FT_THROW( Invalid_Outline );
    }


    void PVG_FT_Raster_Render(const PVG_FT_Raster_Params* params)
    {
        char stack[PVG_FT_MINIMUM_POOL_SIZE];
        size_t length = PVG_FT_MINIMUM_POOL_SIZE;

        TWorker worker;
        worker.skip_spans = 0;
        int rendered_spans = 0;
        int error = gray_raster_render(&worker, stack, (long)length, params);
        while (error == ErrRaster_OutOfMemory) {
            if (worker.skip_spans < 0)
                rendered_spans += -worker.skip_spans;
            worker.skip_spans = rendered_spans;
            length *= 2;
            void* heap = malloc(length);
            error = gray_raster_render(&worker, heap, (long)length, params);
            free(heap);
        }
    }

    static void ft_outline_destroy(PVG_FT_Outline* outline)
    {
        free(outline);
    }

#define FT_COORD(x) (PVG_FT_Pos)(roundf(x * 64))
    static void ft_outline_move_to(PVG_FT_Outline* ft, float x, float y)
    {
        ft->points[ft->n_points].x = FT_COORD(x);
        ft->points[ft->n_points].y = FT_COORD(y);
        ft->tags[ft->n_points] = PVG_FT_CURVE_TAG_ON;
        if (ft->n_points) {
            ft->contours[ft->n_contours] = ft->n_points - 1;
            ft->n_contours++;
        }

        ft->contours_flag[ft->n_contours] = 1;
        ft->n_points++;
    }

#define ALIGN_SIZE(size) (((size) + 7ul) & ~7ul)
    static PVG_FT_Outline* ft_outline_create(int points, int contours)
    {
        size_t points_size = ALIGN_SIZE((points + contours) * sizeof(PVG_FT_Vector));
        size_t tags_size = ALIGN_SIZE((points + contours) * sizeof(char));
        size_t contours_size = ALIGN_SIZE(contours * sizeof(int));
        size_t contours_flag_size = ALIGN_SIZE(contours * sizeof(char));
        PVG_FT_Outline* outline = (PVG_FT_Outline*)malloc(points_size + tags_size + contours_size + contours_flag_size + sizeof(PVG_FT_Outline));

        PVG_FT_Byte* outline_data = (PVG_FT_Byte*)(outline + 1);
        outline->points = (PVG_FT_Vector*)(outline_data);
        outline->tags = (char*)(outline_data + points_size);
        outline->contours = (int*)(outline_data + points_size + tags_size);
        outline->contours_flag = (char*)(outline_data + points_size + tags_size + contours_size);
        outline->n_points = 0;
        outline->n_contours = 0;
        outline->flags = 0x0;
        return outline;
    }

    static void ft_outline_end(PVG_FT_Outline* ft)
    {
        if (ft->n_points) {
            ft->contours[ft->n_contours] = ft->n_points - 1;
            ft->n_contours++;
        }
    }

    static void ft_outline_close(PVG_FT_Outline* ft)
    {
        ft->contours_flag[ft->n_contours] = 0;
        int index = ft->n_contours ? ft->contours[ft->n_contours - 1] + 1 : 0;
        if (index == ft->n_points)
            return;
        ft->points[ft->n_points].x = ft->points[index].x;
        ft->points[ft->n_points].y = ft->points[index].y;
        ft->tags[ft->n_points] = PVG_FT_CURVE_TAG_ON;
        ft->n_points++;
    }

    static void ft_outline_line_to(PVG_FT_Outline* ft, float x, float y)
    {
        ft->points[ft->n_points].x = FT_COORD(x);
        ft->points[ft->n_points].y = FT_COORD(y);
        ft->tags[ft->n_points] = PVG_FT_CURVE_TAG_ON;
        ft->n_points++;
    }


    static void ft_outline_cubic_to(PVG_FT_Outline* ft, float x1, float y1, float x2, float y2, float x3, float y3)
    {
        ft->points[ft->n_points].x = FT_COORD(x1);
        ft->points[ft->n_points].y = FT_COORD(y1);
        ft->tags[ft->n_points] = PVG_FT_CURVE_TAG_CUBIC;
        ft->n_points++;

        ft->points[ft->n_points].x = FT_COORD(x2);
        ft->points[ft->n_points].y = FT_COORD(y2);
        ft->tags[ft->n_points] = PVG_FT_CURVE_TAG_CUBIC;
        ft->n_points++;

        ft->points[ft->n_points].x = FT_COORD(x3);
        ft->points[ft->n_points].y = FT_COORD(y3);
        ft->tags[ft->n_points] = PVG_FT_CURVE_TAG_ON;
        ft->n_points++;
    }

    static PVG_FT_Outline* ft_outline_convert(const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_stroke_data_t* stroke_data);

    static PVG_FT_Outline* ft_outline_convert_dash(const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_stroke_dash_t* stroke_dash)
    {
        if (stroke_dash->array.size == 0)
            return ft_outline_convert(path, matrix, NULL);
        plutovg_path_t* dashed = plutovg_path_clone_dashed(path, stroke_dash->offset, stroke_dash->array.data, stroke_dash->array.size);
        PVG_FT_Outline* outline = ft_outline_convert(dashed, matrix, NULL);
        plutovg_path_destroy(dashed);
        return outline;
    }

    static PVG_FT_Outline* ft_outline_convert_stroke(const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_stroke_data_t* stroke_data)
    {
        double scale_x = sqrt(matrix->a * matrix->a + matrix->b * matrix->b);
        double scale_y = sqrt(matrix->c * matrix->c + matrix->d * matrix->d);

        double scale = hypot(scale_x, scale_y) / PLUTOVG_SQRT2;
        double width = stroke_data->style.width * scale;

        PVG_FT_Fixed ftWidth = (PVG_FT_Fixed)(width * 0.5 * (1 << 6));
        PVG_FT_Fixed ftMiterLimit = (PVG_FT_Fixed)(stroke_data->style.miter_limit * (1 << 16));

        PVG_FT_Stroker_LineCap ftCap;
        switch (stroke_data->style.cap) {
        case PLUTOVG_LINE_CAP_SQUARE:
            ftCap = PVG_FT_STROKER_LINECAP_SQUARE;
            break;
        case PLUTOVG_LINE_CAP_ROUND:
            ftCap = PVG_FT_STROKER_LINECAP_ROUND;
            break;
        default:
            ftCap = PVG_FT_STROKER_LINECAP_BUTT;
            break;
        }

        PVG_FT_Stroker_LineJoin ftJoin;
        switch (stroke_data->style.join) {
        case PLUTOVG_LINE_JOIN_BEVEL:
            ftJoin = PVG_FT_STROKER_LINEJOIN_BEVEL;
            break;
        case PLUTOVG_LINE_JOIN_ROUND:
            ftJoin = PVG_FT_STROKER_LINEJOIN_ROUND;
            break;
        default:
            ftJoin = PVG_FT_STROKER_LINEJOIN_MITER_FIXED;
            break;
        }

        PVG_FT_Stroker stroker;
        PVG_FT_Stroker_New(&stroker);
        PVG_FT_Stroker_Set(stroker, ftWidth, ftCap, ftJoin, ftMiterLimit);

        PVG_FT_Outline* outline = ft_outline_convert_dash(path, matrix, &stroke_data->dash);
        PVG_FT_Stroker_ParseOutline(stroker, outline);

        PVG_FT_UInt points;
        PVG_FT_UInt contours;
        PVG_FT_Stroker_GetCounts(stroker, &points, &contours);

        PVG_FT_Outline* stroke_outline = ft_outline_create(points, contours);
        PVG_FT_Stroker_Export(stroker, stroke_outline);

        PVG_FT_Stroker_Done(stroker);
        ft_outline_destroy(outline);
        return stroke_outline;
    }

    static PVG_FT_Outline* ft_outline_convert(const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_stroke_data_t* stroke_data)
    {
        if (stroke_data) {
            return ft_outline_convert_stroke(path, matrix, stroke_data);
        }

        plutovg_path_iterator_t it;
        plutovg_path_iterator_init(&it, path);

        plutovg_point_t points[3];
        PVG_FT_Outline* outline = ft_outline_create(path->num_points, path->num_contours);
        while (plutovg_path_iterator_has_next(&it)) {
            switch (plutovg_path_iterator_next(&it, points)) {
            case PLUTOVG_PATH_COMMAND_MOVE_TO:
                plutovg_matrix_map_points(matrix, points, points, 1);
                ft_outline_move_to(outline, points[0].x, points[0].y);
                break;
            case PLUTOVG_PATH_COMMAND_LINE_TO:
                plutovg_matrix_map_points(matrix, points, points, 1);
                ft_outline_line_to(outline, points[0].x, points[0].y);
                break;
            case PLUTOVG_PATH_COMMAND_CUBIC_TO:
                plutovg_matrix_map_points(matrix, points, points, 3);
                ft_outline_cubic_to(outline, points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);
                break;
            case PLUTOVG_PATH_COMMAND_CLOSE:
                ft_outline_close(outline);
                break;
            }
        }

        ft_outline_end(outline);
        return outline;
    }

    static void spans_generation_callback(int count, const PVG_FT_Span* spans, void* user)
    {
        plutovg_span_buffer_t* span_buffer = (plutovg_span_buffer_t*)(user);
        plutovg_array_append_data(span_buffer->spans, spans, count);
    }

    void plutovg_rasterize(plutovg_span_buffer_t* span_buffer, const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_rect_t* clip_rect, const plutovg_stroke_data_t* stroke_data, plutovg_fill_rule_t winding)
    {
        PVG_FT_Outline* outline = ft_outline_convert(path, matrix, stroke_data);
        if (stroke_data) {
            outline->flags = PVG_FT_OUTLINE_NONE;
        }
        else {
            switch (winding) {
            case PLUTOVG_FILL_RULE_EVEN_ODD:
                outline->flags = PVG_FT_OUTLINE_EVEN_ODD_FILL;
                break;
            default:
                outline->flags = PVG_FT_OUTLINE_NONE;
                break;
            }
        }

        PVG_FT_Raster_Params params;
        params.flags = PVG_FT_RASTER_FLAG_DIRECT | PVG_FT_RASTER_FLAG_AA;
        params.gray_spans = spans_generation_callback;
        params.user = span_buffer;
        params.source = outline;
        if (clip_rect) {
            params.flags |= PVG_FT_RASTER_FLAG_CLIP;
            params.clip_box.xMin = (PVG_FT_Pos)clip_rect->x;
            params.clip_box.yMin = (PVG_FT_Pos)clip_rect->y;
            params.clip_box.xMax = (PVG_FT_Pos)(clip_rect->x + clip_rect->w);
            params.clip_box.yMax = (PVG_FT_Pos)(clip_rect->y + clip_rect->h);
        }

        plutovg_span_buffer_reset(span_buffer);
        PVG_FT_Raster_Render(&params);
        ft_outline_destroy(outline);
    }

};