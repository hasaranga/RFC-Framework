
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
#include <type_traits> // std::is_base_of

// shows zooming rectangle effect when window visibility change.
// might be slow on devices where hardware acceleration not present.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KZoomRectEffect : public T
{
protected:
	class ZoomRectOverlay : public KComponent
	{
	protected:
		HPEN greyPen, whitePen;

		void updateSizePos(const RECT& startRect, const RECT& endRect)
		{
			// Calculate bounding rectangle that covers the entire animation area
			RECT boundingRect;
			boundingRect.left = min(startRect.left, endRect.left);
			boundingRect.top = min(startRect.top, endRect.top);
			boundingRect.right = max(startRect.right, endRect.right);
			boundingRect.bottom = max(startRect.bottom, endRect.bottom);

			// Add some padding for the pen width and border
			int padding = 15; // Increased padding to accommodate border
			boundingRect.left -= padding;
			boundingRect.top -= padding;
			boundingRect.right += padding;
			boundingRect.bottom += padding;

			const int width = boundingRect.right - boundingRect.left;
			const int height = boundingRect.bottom - boundingRect.top;

			setSize(width, height);
			setPosition(boundingRect.left, boundingRect.top);
		}

		void clearOverlay()
		{
			HDC hdc = ::GetDC(compHWND);

			// Clear the window first (make it transparent)
			RECT clientRect;
			::GetClientRect(compHWND, &clientRect);
			::FillRect(hdc, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			::ReleaseDC(compHWND, hdc);
		}

		void drawRectangleOnOverlay(const RECT& rect)
		{
			HDC hdc = ::GetDC(compHWND);

			// Clear the window first (make it transparent)
			RECT clientRect;
			::GetClientRect(compHWND, &clientRect);
			::FillRect(hdc, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			// Define border width
			const int borderWidth = 1;

			// Create grey border rectangle (outer)
			RECT greyRect = rect;
			greyRect.left -= borderWidth;
			greyRect.top -= borderWidth;
			greyRect.right += borderWidth;
			greyRect.bottom += borderWidth;

			// Set transparent brush so we only draw the outlines
			HBRUSH oldBrush = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));

			// Draw grey border (outer rectangle)
			HPEN oldPen = (HPEN)::SelectObject(hdc, greyPen);
			::Rectangle(hdc, greyRect.left, greyRect.top, greyRect.right, greyRect.bottom);

			// Draw white rectangle (inner)
			::SelectObject(hdc, whitePen);
			::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

			// Cleanup
			::SelectObject(hdc, oldPen);
			::SelectObject(hdc, oldBrush);
			::ReleaseDC(compHWND, hdc);
		}

		LRESULT onWMPaint(WPARAM wParam, LPARAM lParam)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(compHWND, &ps);
			// Don't paint anything - keep transparent
			EndPaint(compHWND, &ps);
			return 0;
		}

	public:
		ZoomRectOverlay() : KComponent(true)
		{
			compText.assignStaticText(TXT_WITH_LEN("ZoomRectOverlay"));
			compCtlID = 0; // control id is zero for top level window

			compWidth = 400;
			compHeight = 200;

			compVisible = false;
			compDwStyle = WS_POPUP;
			compDwExStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
			wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);

			greyPen = ::CreatePen(PS_SOLID, 1, RGB(10, 10, 10)); // near black
			whitePen = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); // White pen
		}

		~ZoomRectOverlay()
		{
			::DeleteObject(greyPen);
			::DeleteObject(whitePen);
		}

		bool create(bool requireInitialMessages) override
		{
			if (KComponent::create(requireInitialMessages))
			{
				::SetLayeredWindowAttributes(compHWND, RGB(0, 0, 0), 0, LWA_COLORKEY); // using black as key color
				return true;
			}
			return false;
		}

		void showZoomInEffect(RECT endRect, int steps = 10, int delayMs = 18)
		{
			if (compHWND == NULL)
				return;

			// Calculate center of the end rectangle
			int centerX = (endRect.left + endRect.right) / 2;
			int centerY = (endRect.top + endRect.bottom) / 2;

			// Calculate dimensions of end rectangle
			int endWidth = endRect.right - endRect.left;
			int endHeight = endRect.bottom - endRect.top;

			// Create start rectangle as small version (e.g., 1/4 size) centered at same position
			float startScale = 0.25; // 1/4 of the final size
			int startWidth = (int)(endWidth * startScale);
			int startHeight = (int)(endHeight * startScale);

			RECT startRect;
			startRect.left = centerX - startWidth / 2;
			startRect.top = centerY - startHeight / 2;
			startRect.right = centerX + startWidth / 2;
			startRect.bottom = centerY + startHeight / 2;

			updateSizePos(startRect, endRect);
			clearOverlay(); // clear old stuff before showing!
			setVisible(true);

			// Get overlay window position to adjust rectangle coordinates
			RECT overlayRect;
			::GetWindowRect(compHWND, &overlayRect);

			for (int i = 0; i <= steps; i++)
			{
				float progress = (float)i / steps;

				// Calculate interpolated rectangle
				RECT currentRect;
				currentRect.left = startRect.left + (int)((endRect.left - startRect.left) * progress);
				currentRect.top = startRect.top + (int)((endRect.top - startRect.top) * progress);
				currentRect.right = startRect.right + (int)((endRect.right - startRect.right) * progress);
				currentRect.bottom = startRect.bottom + (int)((endRect.bottom - startRect.bottom) * progress);

				// Convert to overlay window coordinates
				currentRect.left -= overlayRect.left;
				currentRect.top -= overlayRect.top;
				currentRect.right -= overlayRect.left;
				currentRect.bottom -= overlayRect.top;

				// Draw the rectangle
				drawRectangleOnOverlay(currentRect);
				::Sleep(delayMs);
			}

			setVisible(false);
		}

		void showZoomOutEffect(RECT startRect, int steps = 10, int delayMs = 18)
		{
			if (compHWND == NULL)
				return;

			// Calculate center of the start rectangle
			int centerX = (startRect.left + startRect.right) / 2;
			int centerY = (startRect.top + startRect.bottom) / 2;

			// Calculate dimensions of start rectangle
			int startWidth = startRect.right - startRect.left;
			int startHeight = startRect.bottom - startRect.top;

			// Create end rectangle as small version (e.g., 1/4 size) centered at same position
			float endScale = 0.25; // 1/4 of the original size
			int endWidth = (int)(startWidth * endScale);
			int endHeight = (int)(startHeight * endScale);

			RECT endRect;
			endRect.left = centerX - endWidth / 2;
			endRect.top = centerY - endHeight / 2;
			endRect.right = centerX + endWidth / 2;
			endRect.bottom = centerY + endHeight / 2;

			updateSizePos(startRect, endRect);
			clearOverlay(); // clear old stuff before showing!
			setVisible(true);

			// Get overlay window position to adjust rectangle coordinates
			RECT overlayRect;
			::GetWindowRect(compHWND, &overlayRect);

			for (int i = 0; i <= steps; i++)
			{
				float progress = (float)i / steps;

				// Calculate interpolated rectangle (reverse of zoom in)
				RECT currentRect;
				currentRect.left = startRect.left + (int)((endRect.left - startRect.left) * progress);
				currentRect.top = startRect.top + (int)((endRect.top - startRect.top) * progress);
				currentRect.right = startRect.right + (int)((endRect.right - startRect.right) * progress);
				currentRect.bottom = startRect.bottom + (int)((endRect.bottom - startRect.bottom) * progress);

				// Convert to overlay window coordinates
				currentRect.left -= overlayRect.left;
				currentRect.top -= overlayRect.top;
				currentRect.right -= overlayRect.left;
				currentRect.bottom -= overlayRect.top;

				// Draw the rectangle
				drawRectangleOnOverlay(currentRect);

				Sleep(delayMs);
			}

			setVisible(false);
		}

		BEGIN_KMSG_HANDLER
			ON_KMSG(WM_PAINT, onWMPaint)
		END_KMSG_HANDLER
	};

	ZoomRectOverlay zoomRectOverlay;
	int _zoomRectSteps = 10, _zoomRectDelay = 18;

public:
	template<typename... Args>
	KZoomRectEffect(Args&&... args) : T(std::forward<Args>(args)...) {}
	virtual ~KZoomRectEffect() {}

	void setupZoomRect(int steps, int delayMs)
	{
		_zoomRectSteps = steps;
		_zoomRectDelay = delayMs;
	}

	virtual bool create(bool requireInitialMessages = false) override
	{
		if (__super::create(requireInitialMessages))
		{
			zoomRectOverlay.setParentHWND(this->compHWND);
			zoomRectOverlay.create(false);
			return true;
		}
		return false;
	}

	virtual void onDestroy() override
	{
		zoomRectOverlay.destroy();
		__super::onDestroy();
	}

	virtual void setVisible(bool state) override
	{
		// if window minimized or not created, then we don't show the effect!
		if ((this->compHWND == NULL) || ::IsIconic(this->compHWND))
		{
			__super::setVisible(state);
		}
		else if(state != this->compVisible)
		{
			RECT wndRect;
			::GetWindowRect(this->compHWND, &wndRect);

			if (state)
			{
				zoomRectOverlay.showZoomInEffect(wndRect, _zoomRectSteps, _zoomRectDelay);
				__super::setVisible(true);
			}
			else
			{
				__super::setVisible(false);
				zoomRectOverlay.showZoomOutEffect(wndRect, _zoomRectSteps, _zoomRectDelay);
			}
		}
	}
};
