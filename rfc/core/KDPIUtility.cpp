
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

#include "KDPIUtility.h"
#include "KApplication.h"

KGetDpiForMonitor KDPIUtility::pGetDpiForMonitor = nullptr;
KSetProcessDpiAwarenessContext KDPIUtility::pSetProcessDpiAwarenessContext = nullptr;
KSetProcessDpiAwareness KDPIUtility::pSetProcessDpiAwareness = nullptr;
KSetProcessDPIAware KDPIUtility::pSetProcessDPIAware = nullptr;
KSetThreadDpiAwarenessContext KDPIUtility::pSetThreadDpiAwarenessContext = nullptr;
KAdjustWindowRectExForDpi KDPIUtility::pAdjustWindowRectExForDpi = nullptr;
KGetDpiForWindow KDPIUtility::pGetDpiForWindow = nullptr;

void KDPIUtility::initDPIFunctions() noexcept
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
		KDPIUtility::pGetDpiForWindow =
			reinterpret_cast<KGetDpiForWindow>
			(::GetProcAddress(hUser32, "GetDpiForWindow")); // win10 version 1607

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
WORD KDPIUtility::getWindowDPI(HWND hWnd) noexcept
{
	if (KDPIUtility::pGetDpiForWindow != nullptr)
		return KDPIUtility::pGetDpiForWindow(hWnd);

	if (KDPIUtility::pGetDpiForMonitor != nullptr)
	{
		HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
		UINT uiDpiX, uiDpiY;
		HRESULT hr = KDPIUtility::pGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &uiDpiX, &uiDpiY);

		if (SUCCEEDED(hr))
			return static_cast<WORD>(uiDpiX);
	}

	// for win8 & win7
	HDC hScreenDC = ::GetDC(NULL);
	int iDpiX = ::GetDeviceCaps(hScreenDC, LOGPIXELSX);
	::ReleaseDC(NULL, hScreenDC);

	return static_cast<WORD>(iDpiX);
}

int KDPIUtility::getDPIOfPoint(const POINT& point) noexcept
{
	UINT dpiX, dpiY;
	if (KDPIUtility::pGetDpiForMonitor != nullptr)
	{
		HMONITOR hMonitor = ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);		
		HRESULT hr = KDPIUtility::pGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

		if (SUCCEEDED(hr))
			return (int)dpiX;
	}

	// for win8 & win7
	HDC hdc = ::GetDC(NULL);
	dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
	::ReleaseDC(NULL, hdc);

	return dpiX;
}

BOOL KDPIUtility::adjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) noexcept
{
	if (KDPIUtility::pAdjustWindowRectExForDpi)
		return pAdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);

	return ::AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

void KDPIUtility::makeProcessDPIAware(KDPIAwareness dpiAwareness) noexcept
{
	if (dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY)
	{
		if (KDPIUtility::pSetProcessDpiAwarenessContext)
		{
			KDPIUtility::pSetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		}
	}
	else if (dpiAwareness == KDPIAwareness::STANDARD_MODE)
	{
		if (KDPIUtility::pSetProcessDpiAwarenessContext)
		{
			// intentionally requests the same _V2 context as MIXEDMODE_ONLY to get the
			// best available feature set; mixed-mode windows are simply never enabled
			// for this mode, since KWindow.cpp only exercises them when
			// KApplication::dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY.
			KDPIUtility::pSetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		}
		else if (KDPIUtility::pSetProcessDpiAwareness)
		{
			KDPIUtility::pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		}
		else if (KDPIUtility::pSetProcessDPIAware)
		{
			KDPIUtility::pSetProcessDPIAware();
		}
	}
}

// https://stackoverflow.com/questions/70976583/get-real-screen-resolution-using-win32-api
float KDPIUtility::getMonitorScalingRatio(HMONITOR monitor) noexcept
{
	MONITORINFOEXW info = {};
	info.cbSize = sizeof(MONITORINFOEXW);
	::GetMonitorInfoW(monitor, &info);
	DEVMODEW devmode = {};
	devmode.dmSize = sizeof(DEVMODEW);
	::EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

	if (devmode.dmPelsWidth == 0) // GetMonitorInfoW/EnumDisplaySettingsW failed (e.g. monitor unplugged mid-call)
		return 1.0f;

	return (info.rcMonitor.right - info.rcMonitor.left) / static_cast<float>(devmode.dmPelsWidth);
}

float KDPIUtility::getScaleForMonitor(HMONITOR monitor) noexcept
{
	return (float)(::GetDpiForSystem() / 96.0 / getMonitorScalingRatio(monitor));
}