
// ========== RFC Generator v1.0 ==========


#include "rfc.h"


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