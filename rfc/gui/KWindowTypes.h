
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

#include "KWindow.h"
#include <Windowsx.h> // GET_X_LPARAM
#include <type_traits> // std::is_base_of

class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog() noexcept;

	virtual void onClose() noexcept;

	virtual void onDestroy() noexcept;

	virtual ~KHotPluggedDialog() noexcept;
};

class KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow() noexcept;

	virtual ~KOverlappedWindow() noexcept;
};

class KFrame : public KWindow
{
public:
	KFrame() noexcept;

	virtual ~KFrame() noexcept;
};

class KDialog : public KWindow
{
public:
	KDialog() noexcept;

	virtual ~KDialog() noexcept;
};

class KToolWindow : public KWindow
{
public:
	KToolWindow() noexcept;

	virtual ~KToolWindow() noexcept;
};

// enables client area dragging. window should not have a title bar.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KDraggable : public T
{
protected:
	bool enableClientAreaDragging;
	bool windowDragging;
	Physical clientAreaDraggingX; // physical value
	Physical clientAreaDraggingY; // physical value

	virtual LRESULT onLButtonDown(WPARAM wParam, LPARAM lParam) noexcept
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

			windowDragging = true;
			::SetCapture(T::compHWND);
		}

		return 0;
	}

	virtual LRESULT onMouseMove(WPARAM wParam, LPARAM lParam) noexcept
	{
		if (windowDragging)
		{
			POINT pos;
			::GetCursorPos(&pos);
			const int dpi = KDPIUtility::getWindowDPI(this->compHWND);

			this->setPositionPhysical(pos.x - clientAreaDraggingX,
				pos.y - clientAreaDraggingY);
		}

		return 0;
	}

	virtual LRESULT onLButtonUp(WPARAM wParam, LPARAM lParam) noexcept
	{
		if (windowDragging)
		{
			::ReleaseCapture();
			windowDragging = false;
		}

		return 0;
	}

public:
	template<typename... Args>
	KDraggable(Args&&... args) noexcept : T(std::forward<Args>(args)...)
	{
		enableClientAreaDragging = true;
		windowDragging = false;
		clientAreaDraggingX = 0;
		clientAreaDraggingY = 0;
	}

	virtual void setEnableClientAreaDrag(bool enable) noexcept
	{
		enableClientAreaDragging = enable;
	}

	virtual ~KDraggable() noexcept {}

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
	// width & height are physical values
	virtual void onPaint(HDC hDCMem, const RECT& clientRect, const Physical width, const Physical height) noexcept
	{
		::FillRect(hDCMem, &clientRect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		::FrameRect(hDCMem, &clientRect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	}

	virtual LRESULT onWMPaint(WPARAM wParam, LPARAM lParam) noexcept
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

		this->onPaint(hDCMem, rect, width, height);

		::BitBlt(hdc, 0, 0, width, height, hDCMem, 0, 0, SRCCOPY);

		::DeleteDC(hDCMem);
		::DeleteObject(memBMP);

		::EndPaint(T::compHWND, &ps);

		return 0;
	}

	virtual LRESULT onEraseBackground(WPARAM wParam, LPARAM lParam) noexcept
	{
		return 1; // avoids flickering
	}

public:
	template<typename... Args>
	KDrawable(Args&&... args)noexcept : T(std::forward<Args>(args)...){}

	virtual ~KDrawable() noexcept {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, onWMPaint)
		ON_KMSG(WM_ERASEBKGND, onEraseBackground)
	END_KMSG_HANDLER
};

// adds onCloseEvent to KWindow.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KWithOnCloseEvent : public T
{
public:
	std::function<void()> onCloseEvent;

	template<typename... Args>
	KWithOnCloseEvent(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnCloseEvent() noexcept = default;

	virtual void onClose() noexcept override
	{
		if (onCloseEvent)
			onCloseEvent();

		__super::onClose();
	}
};

// adds onDestroyEvent to KWindow.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KWithOnDestroyEvent : public T
{
public:
	std::function<void()> onDestroyEvent;

	template<typename... Args>
	KWithOnDestroyEvent(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnDestroyEvent() noexcept = default;

	virtual void onDestroy() noexcept override
	{
		if (onDestroyEvent)
			onDestroyEvent();

		__super::onDestroy();
	}
};

// adds onCustomMsgEvent to KWindow.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KWithOnCustomMsgEvent : public T
{
public:
	std::function<void(WPARAM msgID, LPARAM param)> onCustomMsgEvent;

	template<typename... Args>
	KWithOnCustomMsgEvent(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnCustomMsgEvent() noexcept = default;

	virtual void onCustomMessage(WPARAM msgID, LPARAM param) noexcept override
	{
		if (onCustomMsgEvent)
			onCustomMsgEvent(msgID, param);
	}
};

// adds onWindowProcEvent to KWindow.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KWithOnWindowProcEvent : public T
{
public:
	// returns true if msg handled.
	std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& result)> onWindowProcEvent;

	template<typename... Args>
	KWithOnWindowProcEvent(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnWindowProcEvent() noexcept = default;

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override
	{
		if (onWindowProcEvent)
		{
			LRESULT result = 0;
			if (onWindowProcEvent(hwnd, msg, wParam, lParam, result))
				return result;
		}

		return T::windowProc(hwnd, msg, wParam, lParam);
	}
};

class KWidget : public KDrawable<KDraggable<KWindow>>
{
public:
	KWidget() noexcept
	{
		compDwStyle = WS_POPUP;
		compDwExStyle = WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT;
		compLWidth = 128;
		compLHeight = 128;
	}

	virtual ~KWidget() noexcept {}
};

class KChildControl : public KComponent
{
public:
	KChildControl() noexcept : KComponent(true) { compDwStyle = WS_CHILD; }
	virtual ~KChildControl() noexcept {}
};

// removes the titlebar from Window.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KRemoveTitleBar : public T
{
public:
	template<typename... Args>
	KRemoveTitleBar(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KRemoveTitleBar() noexcept = default;

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override
	{
		if (msg == WM_NCCALCSIZE)
			return 0; // Removes non-client area (caption, borders)

		return T::windowProc(hwnd, msg, wParam, lParam);
	}

	virtual bool create(bool requireInitialMessages = false) noexcept override
	{
		return __super::create(true); // to catch WM_NCCALCSIZE
	}
};