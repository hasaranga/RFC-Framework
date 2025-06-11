
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

#include "KDPIUtility.h"
#include "KApplication.h"

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