
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
#include <type_traits> // Logical's integral-only constructor/operators (compile-time only)

#include "KAssert.h"   // toLogicalExact's round-trip check (debug builds only)

typedef HRESULT(WINAPI* KGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
typedef BOOL(WINAPI* KSetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
typedef HRESULT(STDAPICALLTYPE* KSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
typedef BOOL(WINAPI* KSetProcessDPIAware)(VOID);
typedef DPI_AWARENESS_CONTEXT (WINAPI* KSetThreadDpiAwarenessContext) (DPI_AWARENESS_CONTEXT dpiContext);
typedef BOOL(WINAPI* KAdjustWindowRectExForDpi)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
typedef UINT(WINAPI* KGetDpiForWindow)(HWND hwnd);

/*
    How this app deals with monitors that use dpi scaling (125%, 150%, ...).

    Pass one of these to KDPIUtility::makeProcessDPIAware() once at startup, before creating
    any window. Windows does not let a process change its choice afterwards.

    MIXEDMODE_ONLY: each window scales its own contents, but a single window can opt out and
                    let the OS stretch it instead. Needs win10 or higher - on an older OS the
                    app ends up not dpi aware at all, like UNAWARE_MODE.
    STANDARD_MODE:  every window scales its own contents. Works on win7 or higher. Use this
                    one unless you specifically need a window that opts out.
    UNAWARE_MODE:   the OS stretches every window. Nothing in the app has to know about dpi,
                    but text and images look blurry on a scaled monitor.
*/

enum class KDPIAwareness
{
	MIXEDMODE_ONLY, // win10 only, app is not dpi aware on other os
	STANDARD_MODE, // win7 or higher
	UNAWARE_MODE
};

// ---------------------------------------------------------------------------------------
// Logical: a length in dpi-independent pixels.
//
// This is the unit you use for every position, size, spacing and radius you hand to this
// framework. Write the value as it should look at 100% scaling (96 dpi) and let
// KDPIUtility::toPhysical() below turn it into real screen pixels for whatever monitor the
// window is on. Physical (further down) is the other unit: real device pixels, which is what
// Win32 and the mouse speak.
//
// In normal code you just write whole numbers. A plain int means PIXELS - Logical(420) is
// 420px - so setPosition(420, 92) or BORDER = 1 reads exactly as it looks.
//
// Internally the value is a whole number of 1/64 pixels, so it can also carry a fraction of
// a pixel. That is needed because a fraction of a logical pixel is a whole REAL pixel once
// the monitor is scaled: at 125% scaling, logical 102.4 is physical 128, and with whole
// logical pixels only there is no way to name that pixel at all. The fraction never appears
// unless you ask for it - through fromPixels(), fromRaw(), or a division that does not come
// out even.
//
// Why a fixed number of 1/64ths instead of a float: adding and subtracting stays exact, so
// two expressions that are equal on paper give bit-identical results. Two floats can differ
// by a tiny amount, and a tiny difference right on a rounding boundary turns into a whole
// pixel on screen.
//
// Deliberately NOT provided:
//   - Logical * Logical and Logical / Logical. A length times a length is an area, and a
//     length divided by a length is a plain count, not a length - use countOf() for that.
//   - anything that mixes Logical and float. Ratio math (scrollbar thumb sizes, slider and
//     knob positions) has to convert on purpose with (float) and fromPixels(). Those are
//     exactly the places where a unit mistake would never be noticed.
//
// Debugging: the debugger shows the raw field, so 420px reads as 26880. Add a .natvis entry
// to see pixels instead:
//   <Type Name="Logical"><DisplayString>{raw/64.0}px</DisplayString></Type>
// ---------------------------------------------------------------------------------------
struct Logical
{
    static constexpr int FRACTION = 64; // raw units per logical pixel

    int raw;

    constexpr Logical() noexcept : raw(0) {}

    // implicit on purpose: it is what lets you write a plain number and have it mean pixels.
    //
    // Integer types only, and that is not decoration. A plain Logical(int) sitting next to
    // the deleted float/double constructors below would make a cast like (Logical)someLONG
    // ambiguous, because LONG converts to int and to float equally well and neither wins.
    // Written as a template it matches LONG, short, unsigned and so on exactly, so it wins,
    // while a float still lands on the deleted overload and gets a clear error message.
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr Logical(T pixels) noexcept : raw((int)pixels * FRACTION) {}

    // builds a Logical straight from the internal 1/64-pixel count. Only for code that
    // already has a raw value in hand - a conversion result, or another Logical's raw field.
    // Anywhere else, use whole pixels or fromPixels().
    static constexpr Logical fromRaw(int rawValue) noexcept
    {
        Logical l;
        l.raw = rawValue;
        return l;
    }

    // builds a Logical from a pixel value that may have a fraction: fromPixels(2.5f) is two
    // and a half logical pixels. Use it whenever a calculation produced a float and the
    // result is a LENGTH - that conversion is meant to be visible, not automatic.
    // Rounds half away from zero, like every other rounding in this file.
    static constexpr Logical fromPixels(float pixels) noexcept
    {
        return fromRaw((int)(pixels * FRACTION + (pixels >= 0.0f ? 0.5f : -0.5f)));
    }

    // gives PIXELS back (not the internal 1/64 count). Explicit, so it can never happen by
    // accident - handing a length to plain float math is where unit mistakes hide. Use it at
    // ratio sites (thumb sizes, slider and knob positions), then turn the answer back into a
    // length with fromPixels().
    explicit constexpr operator float() const noexcept { return (float)raw / FRACTION; }

    // drops any fraction, rounding toward zero. Use it where something really does need a
    // whole number of logical pixels - a loop count, or a division that must not carry a
    // fraction: someLength.toWholePixels() / 2.
    constexpr int toWholePixels() const noexcept { return raw / FRACTION; }

    // how many `unit` lengths fit inside this one, ignoring the remainder - for example how
    // many 20px rows fit in the visible height. This is why Logical / Logical is not an
    // operator: dividing a length by a length gives a plain count, not a length, and writing
    // countOf() keeps that visible instead of hiding the change of meaning behind a `/`.
    constexpr int countOf(Logical unit) const noexcept { return raw / unit.raw; }

    constexpr Logical operator+(Logical o) const noexcept { return fromRaw(raw + o.raw); }
    constexpr Logical operator-(Logical o) const noexcept { return fromRaw(raw - o.raw); }
    constexpr Logical operator-() const noexcept { return fromRaw(-raw); }
    // scaling by a COUNT. integral-constrained for the same reason as the constructor.
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr Logical operator*(T n) const noexcept { return fromRaw(raw * (int)n); }

    // EXACT: the result keeps its fraction. Logical(7) / 2 is 3.5px, not 3px.
    //
    // That is the whole point of a fractional unit. The value is turned into real pixels only
    // once, at draw time, so a centred item lands on the correct screen pixel instead of
    // sitting half a logical pixel toward the top-left. Centring a 21px label inside a 50px
    // parent at 200% scaling: exact division puts 29 real pixels on each side, whole-pixel
    // division puts 28 on one side and 30 on the other.
    //
    // Nothing to gain at 100% scaling, where half a pixel cannot be shown anyway - there the
    // bias simply moves to the other side.
    //
    // If a call site really does need a whole-pixel answer, it has to say so right there:
    // toWholePixels() / n, or countOf() when dividing by another length.
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr Logical operator/(T n) const noexcept
    {
        return fromRaw(raw / (int)n);
    }

    Logical& operator+=(Logical o) noexcept { raw += o.raw; return *this; }
    Logical& operator-=(Logical o) noexcept { raw -= o.raw; return *this; }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Logical& operator*=(T n) noexcept { raw *= (int)n; return *this; }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Logical& operator/=(T n) noexcept { *this = *this / (int)n; return *this; }

    // Floats are rejected on purpose. Without these, a float would quietly become an int at
    // every operation above: multiplying a length by a 0.5f speed would truncate that speed
    // to 0, and the compiler would say nothing louder than a warning. A float here is never a
    // length - it is a ratio, an angle, or a measured extent - so turning one into a Logical
    // is always something you write down: fromPixels() to round, Logical((int)x) to truncate.
    Logical(float) = delete;
    Logical(double) = delete;
    Logical operator*(float) const = delete;
    Logical operator*(double) const = delete;
    Logical operator/(float) const = delete;
    Logical operator/(double) const = delete;
    Logical& operator*=(float) = delete;
    Logical& operator/=(float) = delete;

    constexpr bool operator==(Logical o) const noexcept { return raw == o.raw; }
    constexpr bool operator!=(Logical o) const noexcept { return raw != o.raw; }
    constexpr bool operator<(Logical o) const noexcept { return raw < o.raw; }
    constexpr bool operator<=(Logical o) const noexcept { return raw <= o.raw; }
    constexpr bool operator>(Logical o) const noexcept { return raw > o.raw; }
    constexpr bool operator>=(Logical o) const noexcept { return raw >= o.raw; }
};

// the same operations with the plain number on the LEFT: 200 + EXTRA_WIDTH, 10 + getHeight().
// A member operator cannot cover these, because the left operand of a member operator is never
// converted for you. Deliberately not written as operator+(Logical, Logical), which would then
// also be considered for ordinary (int, int) expressions elsewhere in the program.
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr Logical operator*(T n, Logical v) noexcept { return Logical::fromRaw(v.raw * (int)n); }

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr Logical operator+(T pixels, Logical v) noexcept { return Logical(pixels) + v; }

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr Logical operator-(T pixels, Logical v) noexcept { return Logical(pixels) - v; }

// same reason as the deleted overloads inside the struct: a float on the left must not slip
// into the integer form.
Logical operator*(float, Logical) = delete;
Logical operator*(double, Logical) = delete;
Logical operator+(float, Logical) = delete;
Logical operator+(double, Logical) = delete;
Logical operator-(float, Logical) = delete;
Logical operator-(double, Logical) = delete;

// a length in real device pixels - what Win32, the mouse and the screen itself use. Whole
// pixels only: there is nothing finer on a monitor. Convert with the two functions at the
// bottom of this file, and keep values in Logical everywhere else.
using Physical = int;

// a position in logical pixels.
struct LogicalPoint
{
    Logical x;
    Logical y;
};

// a position in real device pixels - for example a cursor position from ::GetCursorPos.
struct PhysicalPoint
{
    Physical x;
    Physical y;
};

// a rectangle described by its four EDGES, in logical pixels. LogicalRect below describes the
// same thing as position + size; each converts to the other, so use whichever makes the code
// read better - edges for clipping and intersecting, rect for placing and sizing.
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

    // reads the RECT as whole logical PIXELS. Careful: this is NOT the inverse of toRawRECT()
    // below, which writes out raw 1/64 units. The two deal in different units on purpose.
    LogicalEdges(const RECT& rect) noexcept : left((int)rect.left), top((int)rect.top),
        right((int)rect.right), bottom((int)rect.bottom) {
    }

    // packs the four edges into a RECT as RAW 1/64 units, so RECT helpers like ::IntersectRect
    // can be used on them (see intersect() below). This is NOT a pixel rect - never pass it to
    // a Win32 call that draws, measures or positions anything. Use KDPIUtility::toPhysicalRECT
    // for that. It was called toRECT() in older versions; the name changed so that every call
    // site had to stop and decide whether it wanted raw units or real pixels.
    RECT toRawRECT() const noexcept
    {
        RECT rect = { left.raw, top.raw, right.raw, bottom.raw };
        return rect;
    }

    static inline bool intersect(LogicalEdges& out, const LogicalEdges& a, const LogicalEdges& b)
    {
        RECT intersect;
        const RECT aRect = a.toRawRECT();
        const RECT bRect = b.toRawRECT();

        const BOOL retVal = ::IntersectRect(&intersect, &aRect, &bRect);

        // fromRaw, not the plain int constructor: the RECT holds raw units, and reading them
        // as pixels here would silently multiply the answer by 64.
        out.left = Logical::fromRaw(intersect.left);
        out.top = Logical::fromRaw(intersect.top);
        out.right = Logical::fromRaw(intersect.right);
        out.bottom = Logical::fromRaw(intersect.bottom);

        return retVal == TRUE;
    }

    // grows the rectangle by value on all four sides, then clamps the left and top edges so
    // they never go negative. The right and bottom edges are not pulled in to compensate, so
    // the result always covers at least the original area. Handy for widening an area that has
    // to be repainted. LogicalRect::expand below does the same thing to a position+size rect.
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

// a rectangle described as a position plus a size, in logical pixels - the same rectangle
// LogicalEdges above describes by its four edges. Convert either way with the constructor
// that takes LogicalEdges, or with toLogicalEdges().
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

    // grows the rectangle by value on all four sides, then clamps x and y so they never go
    // negative. Same growth as LogicalEdges::expand above, which is why the size grows by
    // TWICE value: the position moves back by value on one side and the far edge moves out by
    // value on the other. Handy for widening an area that has to be repainted.
    //
    // Note the clamp does not shrink the size, so a rectangle already touching 0 comes back
    // reaching value further right or down than asked. That is deliberate - this exists to
    // cover MORE than the original area, never less.
    LogicalRect expand(Logical value = 1) const noexcept
    {
        LogicalRect rect(x - value, y - value, width + (value * 2), height + (value * 2));
        if (rect.x < 0)
            rect.x = 0;
        if (rect.y < 0)
            rect.y = 0;

        return rect;
    }
};

// dpi helpers: set the process dpi mode at startup, ask a window or a monitor for its dpi,
// and convert between Logical and Physical lengths. Everything here is static - there is
// nothing to create.
class KDPIUtility
{
private:
    static float getMonitorScalingRatio(HMONITOR monitor) noexcept;
public:
    // Win32 dpi functions that do not exist on every supported OS, looked up at runtime by
    // initDPIFunctions() below. A pointer stays NULL when the running OS does not have that
    // function, so always test one before calling it - the methods below already do.
	static KGetDpiForMonitor pGetDpiForMonitor;
	static KSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext;
	static KSetProcessDpiAwareness pSetProcessDpiAwareness;
	static KSetProcessDPIAware pSetProcessDPIAware;
	static KSetThreadDpiAwarenessContext pSetThreadDpiAwarenessContext;
    static KAdjustWindowRectExForDpi pAdjustWindowRectExForDpi;
    static KGetDpiForWindow pGetDpiForWindow;

    // fills in the function pointers above. Called for you during framework startup; you only
    // need it if you use this class on its own.
	static void initDPIFunctions() noexcept;

    // dpi of the monitor the given window is on. Returns 96 (100% scaling) when the app is not
    // dpi aware, because that is all Windows will tell it. This is the dpi to pass to
    // toPhysical() and the two toLogical functions.
	static WORD getWindowDPI(HWND hWnd) noexcept;

    // works out the window size needed to get a client area of the given size, for a window
    // with the given styles, at the given dpi. Falls back to the plain AdjustWindowRectEx
    // below win10, which is only correct at 96 dpi.
    static BOOL adjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) noexcept;

    // picks the process dpi mode (see KDPIAwareness above) using the best API the running OS
    // has. Call once at startup, before creating any window; Windows will not let it change
    // afterwards.
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

    // logical pixels -> real device pixels, for a monitor running at the given dpi. This is
    // THE conversion boundary: keep a value in Logical through all the intermediate maths and
    // convert once, as late as possible, right before it is drawn or handed to Win32. Convert
    // early and every later addition works on an already-rounded number, which is how edges
    // that should touch end up one pixel apart. Rounds half away from zero.
    //
    // Deliberately not ::MulDiv: that is a kernel32 export, so it would cost a DLL call for
    // every coordinate, and it cannot skip the work at 96 dpi anyway - the raw 1/64 units
    // still have to be divided out. This inline version is a little faster at every dpi.
    static inline Physical toPhysical(Logical value, int dpi) noexcept
    {
        const long long num = (long long)value.raw * dpi;
        const long long den = (long long)USER_DEFAULT_SCREEN_DPI * Logical::FRACTION;
        return (Physical)(num >= 0 ? (num + den / 2) / den : (num - den / 2) / den);
    }

    // the same conversion for all four edges at once, giving a RECT in real pixels that is
    // safe to pass to Win32. Use this instead of LogicalEdges::toRawRECT() whenever the RECT
    // is going to leave the framework.
    static inline RECT toPhysicalRECT(const LogicalEdges& edges, int dpi) noexcept
    {
        return { KDPIUtility::toPhysical(edges.left, dpi),
            KDPIUtility::toPhysical(edges.top, dpi),
            KDPIUtility::toPhysical(edges.right, dpi),
            KDPIUtility::toPhysical(edges.bottom, dpi) };
    }

    // real device pixels -> logical pixels, rounded to a WHOLE logical pixel.
    //
    // USE THIS FOR A SIZE OR AN EXTENT - a width read back from ::GetClientRect, a measured
    // text width, anything that is going to be stored in a plain int anyway. Do NOT use it for
    // a CURSOR POSITION: use toLogicalExact() below for that. The rest of this comment is why
    // that distinction matters.
    //
    // The answer here is MANY-TO-ONE. Above 100% scaling the screen grid is finer than a
    // whole-pixel logical grid, so several real pixels share one logical answer. At 125%,
    // physical 127 and 128 both come back as logical 102, and nothing downstream can tell
    // those two apart any more. An item whose left edge is logical 102 starts painting at
    // physical 128, so a click on 127 - one real pixel OUTSIDE it - also reads as 102 and
    // counts as a hit. The opposite mistake happens too: an edge placed exactly on physical
    // 128 is logical 102.4, so a click on 128 reads as 102, compares as less than 102.4, and
    // counts as a miss on the very pixel that was painted. One item can show both errors, on
    // opposite edges. It gets worse as scaling goes up, not better - at 200% EVERY logical
    // value is shared by two columns of real pixels.
    //
    // None of that matters for a size: an int-sized answer is all the caller can store.
    //
    // THE LONG NAME IS THE POINT. This used to be called toLogical, which reads like the
    // obvious opposite of toPhysical, so anyone converting a cursor position reached for it by
    // name and inherited the error above. Spelling out WholePixel means the coarse behaviour
    // has to be chosen on purpose - nobody types "WholePixel" for a mouse position without
    // stopping to think.
    //
    // UPDATING OLD CODE: code written against an older version of this header calls toLogical
    // and will not compile, which is intended. This function is that old one, unchanged, with
    // the same result for every input - so a size or extent conversion only needs the new
    // name. A cursor conversion should move to toLogicalExact() instead, because renaming it
    // would just carry the old error forward. There is deliberately no toLogical left behind
    // as a forwarder: it would still let a wrong mouse conversion compile, and that is exactly
    // the thing worth breaking the build over.
    static inline Logical toLogicalWholePixel(Physical value, int dpi) noexcept
    {
        if (dpi == USER_DEFAULT_SCREEN_DPI)
            return Logical(value);
        return Logical(::MulDiv(value, USER_DEFAULT_SCREEN_DPI, dpi));
    }

    // real device pixels -> logical pixels, keeping the fraction.
    //
    // USE THIS FOR A CURSOR POSITION, and for anything that has to line up with one exact
    // pixel on the screen. The answer is the logical value that converts back to precisely the
    // pixel you asked about, which is what toLogicalWholePixel() above cannot always give you:
    // with whole logical pixels, at 125% scaling one real pixel in five has no logical value
    // at all, so it can be neither named nor hit. With 1/64ths there are about 51 usable values
    // per real pixel, so the pixel you want is always reachable.
    //
    // Because the fraction survives, positions that come from the mouse are fractional too.
    // Drag distances, caret positions and scroll offsets therefore follow the cursor exactly
    // instead of stepping a whole logical pixel at a time.
    static inline Logical toLogicalExact(Physical value, int dpi) noexcept
    {
        const long long num = (long long)value * USER_DEFAULT_SCREEN_DPI * Logical::FRACTION;
        const Logical result = Logical::fromRaw(
            (int)(num >= 0 ? (num + dpi / 2) / dpi : (num - dpi / 2) / dpi));

        // the guarantee this function exists to provide: feeding the answer back into
        // toPhysical() must give the exact pixel that was asked for. That holds for every dpi
        // up to 96*64, since there is always at least one usable logical value per real pixel
        // (about 51 of them at 125%), so this can only fire if one of the two conversions is
        // edited wrongly. If it ever does, every value converted here is landing on a different
        // pixel than the caller asked for. Debug builds only - costs nothing in release.
        K_ASSERT(KDPIUtility::toPhysical(result, dpi) == value,
            "toLogicalExact: round-trip failed - toPhysical(result) != the requested pixel");

        return result;
    }
};

