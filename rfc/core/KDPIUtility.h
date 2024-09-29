
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
private: 
    static float GetMonitorScalingRatio(HMONITOR monitor);
public:		
	static KGetDpiForMonitor pGetDpiForMonitor;
	static KSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext;
	static KSetProcessDpiAwareness pSetProcessDpiAwareness;
	static KSetProcessDPIAware pSetProcessDPIAware;
	static KSetThreadDpiAwarenessContext pSetThreadDpiAwarenessContext;

	static void InitDPIFunctions();

	static WORD GetWindowDPI(HWND hWnd);

	static void MakeProcessDPIAware(KDPIAwareness dpiAwareness);

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
			monitorScale = KDPIUtility::GetScaleForMonitor(hmon);
    */
    static float GetScaleForMonitor(HMONITOR monitor);

    // scale given 96dpi value according to window current dpi.
    static int ScaleToWindowDPI(int valueFor96DPI, HWND window);

    // scale given 96dpi value according to new dpi.
    static int ScaleToNewDPI(int valueFor96DPI, int newDPI);
};

