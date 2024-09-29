
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

#include "KWindow.h"
#include <Windowsx.h> // GET_X_LPARAM

class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void OnClose();

	virtual void OnDestroy();

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

// enables client area dragging
template <class T>
class KDraggable : public T
{
protected:
	bool enableClientAreaDragging;
	bool windowDraging;
	short clientAreaDraggingX;
	short clientAreaDraggingY;

	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

			windowDraging = true;
			::SetCapture(T::compHWND);
		}

		return 0;
	}

	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam)
	{
		if (windowDraging)
		{
			POINT pos;
			::GetCursorPos(&pos);

			this->SetPosition(pos.x - clientAreaDraggingX, pos.y - clientAreaDraggingY);
		}

		return 0;
	}

	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		if (windowDraging)
		{
			::ReleaseCapture();
			windowDraging = false;
		}

		return 0;
	}

public:
	KDraggable()
	{
		enableClientAreaDragging = true;
	}

	virtual void SetEnableClientAreaDrag(bool enable)
	{
		enableClientAreaDragging = enable;
	}

	virtual ~KDraggable() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_LBUTTONDOWN, OnLButtonDown)
		ON_KMSG(WM_MOUSEMOVE, OnMouseMove)
		ON_KMSG(WM_LBUTTONUP, OnLButtonUp)
	END_KMSG_HANDLER(T)
};

// provides flicker free double buffered drawing method
template <class T>
class KDrawable : public T
{
protected:

	// override this method in subclass and draw your stuff
	virtual void OnPaint(HDC hDCMem, RECT* rect, const int width, const int height)
	{
		::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		::FrameRect(hDCMem, rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	}

	virtual LRESULT OnWMPaint(WPARAM wParam, LPARAM lParam)
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

		this->OnPaint(hDCMem, &rect, width, height);

		::BitBlt(hdc, 0, 0, width, height, hDCMem, 0, 0, SRCCOPY);

		::DeleteDC(hDCMem);
		::DeleteObject(memBMP);

		::EndPaint(T::compHWND, &ps);

		return 0;
	}

	virtual LRESULT OnEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return 1; // avoids flickering
	}

public:
	KDrawable(){}

	virtual ~KDrawable() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, OnWMPaint)
		ON_KMSG(WM_ERASEBKGND, OnEraseBackground)
	END_KMSG_HANDLER(T)
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
