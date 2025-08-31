
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

#include "KWindow.h"
#include <Windowsx.h> // GET_X_LPARAM
#include <type_traits> // std::is_base_of

class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void onClose();

	virtual void onDestroy();

	virtual ~KHotPluggedDialog();
};

class KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow();

	virtual ~KOverlappedWindow();
};

class KFrame : public KWindow
{
public:
	KFrame();

	virtual ~KFrame();
};

class KDialog : public KWindow
{
public:
	KDialog();

	virtual ~KDialog();
};

class KToolWindow : public KWindow
{
public:
	KToolWindow();

	virtual ~KToolWindow();
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
	short clientAreaDraggingX;
	short clientAreaDraggingY;

	virtual LRESULT onLButtonDown(WPARAM wParam, LPARAM lParam)
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

	virtual LRESULT onMouseMove(WPARAM wParam, LPARAM lParam)
	{
		if (windowDragging)
		{
			POINT pos;
			::GetCursorPos(&pos);

			this->setPosition(pos.x - clientAreaDraggingX, pos.y - clientAreaDraggingY);
		}

		return 0;
	}

	virtual LRESULT onLButtonUp(WPARAM wParam, LPARAM lParam)
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
	KDraggable(Args&&... args) : T(std::forward<Args>(args)...)
	{
		enableClientAreaDragging = true;
		windowDragging = false;
		clientAreaDraggingX = 0;
		clientAreaDraggingY = 0;
	}

	virtual void setEnableClientAreaDrag(bool enable)
	{
		enableClientAreaDragging = enable;
	}

	virtual ~KDraggable() {}

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
	virtual void onPaint(HDC hDCMem, RECT* rect, const int width, const int height)
	{
		::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		::FrameRect(hDCMem, rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	}

	virtual LRESULT onWMPaint(WPARAM wParam, LPARAM lParam)
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

		this->onPaint(hDCMem, &rect, width, height);

		::BitBlt(hdc, 0, 0, width, height, hDCMem, 0, 0, SRCCOPY);

		::DeleteDC(hDCMem);
		::DeleteObject(memBMP);

		::EndPaint(T::compHWND, &ps);

		return 0;
	}

	virtual LRESULT onEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return 1; // avoids flickering
	}

public:
	template<typename... Args>
	KDrawable(Args&&... args) : T(std::forward<Args>(args)...) {}

	virtual ~KDrawable() {}

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
	KWithOnCloseEvent(Args&&... args) : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnCloseEvent() = default;

	virtual void onClose() override
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
	KWithOnDestroyEvent(Args&&... args) : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnDestroyEvent() = default;

	virtual void onDestroy() override
	{
		if (onDestroyEvent)
			onDestroyEvent();

		__super::onDestroy();
	}
};

class KWidget : public KDrawable<KDraggable<KWindow>>
{
public:
	KWidget()
	{
		compDwStyle = WS_POPUP;
		compDwExStyle = WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT;
		compWidth = 128;
		compHeight = 128;
	}

	virtual ~KWidget() {}
};

class KChildControl : public KComponent
{
public:
	KChildControl() : KComponent(true) { compDwStyle = WS_CHILD; }
	virtual ~KChildControl() {}
};

