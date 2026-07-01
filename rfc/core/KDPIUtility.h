
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

#pragma once

#include <windows.h>
#include <shellscalingapi.h>
#include <cmath>

typedef HRESULT(WINAPI* KGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
typedef BOOL(WINAPI* KSetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
typedef HRESULT(STDAPICALLTYPE* KSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
typedef BOOL(WINAPI* KSetProcessDPIAware)(VOID);
typedef DPI_AWARENESS_CONTEXT (WINAPI* KSetThreadDpiAwarenessContext) (DPI_AWARENESS_CONTEXT dpiContext);
typedef BOOL(WINAPI* KAdjustWindowRectExForDpi)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
typedef UINT(WINAPI* KGetDpiForWindow)(HWND hwnd);

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

using Logical = int; // we use int, so evrything should be aligned to pixel grid always.
using Physical = int;

struct LogicalPoint
{
    Logical x;
    Logical y;
};

struct PhysicalPoint
{
    Physical x;
    Physical y;
};

struct LogicalEdges
{
    Logical left;
    Logical top;
    Logical right;
    Logical bottom;

    LogicalEdges() noexcept : left(0), top(0), right(0), bottom(0) {}

    LogicalEdges(Logical left, Logical top, Logical right, Logical bottom) noexcept : left(left), top(top),
        right(right), bottom(bottom) {
    }

    LogicalEdges(const RECT& rect) noexcept : left(rect.left), top(rect.top),
        right(rect.right), bottom(rect.bottom) {
    }

    RECT toRECT() const noexcept
    {
        RECT rect = { left, top, right, bottom };
        return rect;
    }

    static inline bool intersect(LogicalEdges& out, const LogicalEdges& a, const LogicalEdges& b)
    {
        RECT intersect;
        const RECT aRect = a.toRECT();
        const RECT bRect = b.toRECT();

        const BOOL retVal = ::IntersectRect(&intersect, &aRect, &bRect);

        out.left = intersect.left;
        out.top = intersect.top;
        out.right = intersect.right;
        out.bottom = intersect.bottom;

        return retVal == TRUE;
    }

    LogicalEdges expand(Logical value = 1) const noexcept
    {
        LogicalEdges rect(left - value, top - value, right + value, bottom + value);
        if (rect.left < 0)
            rect.left = 0;
        if (rect.top < 0)
            rect.top = 0;

        return rect;
    }
};

struct LogicalRect
{
    Logical x;
    Logical y;
    Logical width;
    Logical height;

    LogicalRect() noexcept : x(0), y(0), width(0), height(0) {}

    LogicalRect(Logical x, Logical y, Logical width, Logical height) noexcept : x(x), y(y),
        width(width), height(height) {
    }

    LogicalRect(const LogicalEdges& edges) noexcept :
        x(edges.left), y(edges.top),
        width(edges.right - edges.left),
        height(edges.bottom - edges.top) {
    }

    LogicalEdges toLogicalEdges() const noexcept
    {
        return { x, y, x + width, y + height };
    }

    LogicalRect expand(Logical value = 1) const noexcept
    {
        LogicalRect rect(x - value, y - value, width + value, height + value);
        if (rect.x < 0)
            rect.x = 0;
        if (rect.y < 0)
            rect.y = 0;

        return rect;
    }
};

class KDPIUtility
{
private: 
    static float getMonitorScalingRatio(HMONITOR monitor) noexcept;
public:		
	static KGetDpiForMonitor pGetDpiForMonitor;
	static KSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext;
	static KSetProcessDpiAwareness pSetProcessDpiAwareness;
	static KSetProcessDPIAware pSetProcessDPIAware;
	static KSetThreadDpiAwarenessContext pSetThreadDpiAwarenessContext;
    static KAdjustWindowRectExForDpi pAdjustWindowRectExForDpi;
    static KGetDpiForWindow pGetDpiForWindow;

	static void initDPIFunctions() noexcept;

    // returns dpi of monitor which our window is in. returns 96 if application is not dpi aware.
	static WORD getWindowDPI(HWND hWnd) noexcept;

    // automatically fall back to AdjustWindowRectEx when lower than win10
    static BOOL adjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) noexcept;

	static void makeProcessDPIAware(KDPIAwareness dpiAwareness) noexcept;

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
    static float getScaleForMonitor(HMONITOR monitor) noexcept;

    // point is physical
    static int getDPIOfPoint(const POINT& point) noexcept;

    static inline Physical toPhysical(Logical value, int dpi) noexcept
    {
        if (dpi == USER_DEFAULT_SCREEN_DPI)
            return value;
        return ::MulDiv(value, dpi, USER_DEFAULT_SCREEN_DPI);
    }

    static inline RECT toPhysicalRECT(const LogicalEdges& edges, int dpi) noexcept
    {
        return { KDPIUtility::toPhysical(edges.left, dpi),
            KDPIUtility::toPhysical(edges.top, dpi),
            KDPIUtility::toPhysical(edges.right, dpi),
            KDPIUtility::toPhysical(edges.bottom, dpi) };
    }

    static inline Logical toLogical(Physical value, int dpi) noexcept
    {
        if (dpi == USER_DEFAULT_SCREEN_DPI)
            return value;
        return ::MulDiv(value, USER_DEFAULT_SCREEN_DPI, dpi);
    }
};

