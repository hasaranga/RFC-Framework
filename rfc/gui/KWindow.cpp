
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

#include "KWindow.h"
#include "KGUIProc.h"
#include "KTimer.h"
#include "KIDGenerator.h"

KWindow::KWindow() noexcept : KComponent(true)
{
	compText.assignStaticText(TXT_WITH_LEN("KWindow"));

	compPX = 0;
	compPY = 0;
	compLWidth = 400;
	compLHeight = 200;

	compVisible = false;
	enableDPIUnawareMode = false;
	resizingForDPIChange = false;
	prevDPIContext = 0;
	dpiAwarenessContextChanged = false;
	compDwStyle = WS_POPUP | WS_CLIPCHILDREN;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
	windowIcon = nullptr;
	largeIconHandle = 0;
	smallIconHandle = 0;

	closeOperation = KCloseOperation::DestroyAndExit;
}

void KWindow::applyDPIUnawareModeToThread() noexcept
{
	if ((KApplication::dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY) && enableDPIUnawareMode)
	{
		if (KDPIUtility::pSetThreadDpiAwarenessContext)
		{
			prevDPIContext = KDPIUtility::pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
			dpiAwarenessContextChanged = true;
		}
	}
}

void KWindow::restoreDPIModeOfThread() noexcept
{
	if (dpiAwarenessContextChanged)
		KDPIUtility::pSetThreadDpiAwarenessContext(prevDPIContext);
}

bool KWindow::create(bool requireInitialMessages) noexcept
{
	if (enableDPIUnawareMode)
		applyDPIUnawareModeToThread();

	// captured BEFORE createComponentFor96DPI (see below) - compLWidth/compLHeight themselves
	// can get silently corrupted by that call when requireInitialMessages is true: that wires up
	// the HWND<->component association (via a CBT hook) before CreateWindowExW even returns,
	// which means an early, synchronous WM_SIZE (sent by CreateWindowExW as a normal part of
	// creating the window, while it's still at its unscaled "for96DPI" physical size) reaches
	// this class's own WM_SIZE handler further down in this file, which recomputes
	// compLWidth/compLHeight from the window's CURRENT (still unscaled) physical rect using the
	// monitor's ACTUAL (already-correct) DPI - e.g. toLogical(640, 120) = 512 instead of the
	// intended 640, at 125%. callers using requireInitialMessages=false never hit this: the
	// association is only wired up AFTER CreateWindowExW returns, so that same early WM_SIZE
	// falls through to DefWindowProcW instead and never touches compLWidth/compLHeight at all.
	// using these captured originals (not compLWidth/compLHeight directly, which may already be
	// corrupted by the time we get here) for the SetWindowPos fixup below sidesteps the
	// corruption entirely - the fixup's OWN resize then sends a SECOND WM_SIZE, this time with
	// the window truly at its scaled physical size, which correctly (re)computes
	// compLWidth/compLHeight back to their intended values.
	const Logical originalCompLWidth = compLWidth;
	const Logical originalCompLHeight = compLHeight;

	if (::RegisterClassExW(&wc))
	{
		isRegistered = true;
		KGUIProc::createComponentFor96DPI(this, requireInitialMessages, compPX, compPY);
		if (compHWND)
		{
			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode))
			{
				// if the default monitor is scaled then we won't get dpi change message.
				// so we do manual positioning/scaling for the window.
				// if the SetWindowPos causes window to position over scaled second monitor, then os will send dpi change message.
				const int dpi_1 = getDPI();
				::SetWindowPos(compHWND,
					NULL,
					compPX,
					compPY,
					KDPIUtility::toPhysical(originalCompLWidth, dpi_1),
					KDPIUtility::toPhysical(originalCompLHeight, dpi_1),
					SWP_NOZORDER | SWP_NOACTIVATE);
			}

			::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);

			for (int i = 0; i < componentList.size(); i++)
			{
				KComponent* comp = componentList[i];
				comp->setParentHWND(compHWND);
				comp->create(requireInitialMessages);
			}

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode))
			{
				const int dpi_2 = KDPIUtility::getWindowDPI(compHWND);
				if (dpi_2 != USER_DEFAULT_SCREEN_DPI)
				{
					for (int i = 0; i < componentList.size(); i++)
					{
						componentList[i]->setDPI(dpi_2);
					}
				}
			}
			if (enableDPIUnawareMode)
				restoreDPIModeOfThread();

			afterCreated();
			return true;
		}
	}
	if (enableDPIUnawareMode)
		restoreDPIModeOfThread();
	return false;
}

void KWindow::flash() noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	::FlashWindow(compHWND, TRUE);
}

void KWindow::updateWindowIconForNewDPI() noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	if (windowIcon == nullptr)
		return;

	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);

	// 32x32 for large icon
	// 16x16 for small icon

	if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode))
	{
		const int dpi = KDPIUtility::getWindowDPI(compHWND);
		largeIconHandle = windowIcon->getScaledIcon(KDPIUtility::toPhysical(32, dpi));
		smallIconHandle = windowIcon->getScaledIcon(KDPIUtility::toPhysical(16, dpi));
	}
	else
	{
		largeIconHandle = windowIcon->getScaledIcon(32);
		smallIconHandle = windowIcon->getScaledIcon(16);
	}

	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)largeIconHandle);
	::SetClassLongPtrW(compHWND, GCLP_HICONSM, (LONG_PTR)smallIconHandle);
}

void KWindow::setIcon(KIcon* icon) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	windowIcon = icon;
	this->updateWindowIconForNewDPI();
}

void KWindow::setCloseOperation(KCloseOperation closeOperation) noexcept
{
	this->closeOperation = closeOperation;
}

void KWindow::setEnableDPIUnawareMode(bool enable) noexcept
{
	enableDPIUnawareMode = enable;
}

void KWindow::onClose() noexcept
{
	if (closeOperation == KCloseOperation::DestroyAndExit)
	{
		this->setVisible(false);
		this->destroy();
	}
	else if (closeOperation == KCloseOperation::Hide)
	{
		this->setVisible(false);
	}
}

void KWindow::onDestroy() noexcept
{
	for (int i = 0; i < componentList.size(); i++)
	{
		componentList[i]->onParentDestroy();
	}

	if (closeOperation == KCloseOperation::DestroyAndExit)
		::PostQuitMessage(0);
}

void KWindow::onDynamicMenuItemPress(UINT itemID) noexcept {}

void KWindow::postCloseMessage() noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	::PostMessageW(compHWND, WM_CLOSE, 0, 0);
}

void KWindow::postCustomMessage(WPARAM msgID, LPARAM param) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	::PostMessageW(compHWND, RFC_CUSTOM_MESSAGE, msgID, param);
}

void KWindow::onCustomMessage(WPARAM msgID, LPARAM param) noexcept
{

}

void KWindow::centerScreen() noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	const int dpi = KDPIUtility::getWindowDPI(compHWND);
	const Physical physicalWidth = KDPIUtility::toPhysical(compLWidth, dpi);
	const Physical physicalHeight = KDPIUtility::toPhysical(compLHeight, dpi);
	compPX = (::GetSystemMetrics(SM_CXSCREEN) - physicalWidth) / 2;
	compPY = (::GetSystemMetrics(SM_CYSCREEN) - physicalHeight) / 2;

	::SetWindowPos(compHWND, 0, compPX, compPY, 0, 0, SWP_NOSIZE |
		SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KWindow::centerOnSameMonitor(HWND window) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	K_ASSERT(window != NULL, "window handle is NULL");

	HMONITOR hmon = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

	if (hmon != NULL)
	{
		MONITORINFO monitorInfo;
		::ZeroMemory(&monitorInfo, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);

		if (::GetMonitorInfoW(hmon, &monitorInfo))
		{
			const int dpi = KDPIUtility::getWindowDPI(compHWND);
			const Physical physicalWidth = KDPIUtility::toPhysical(compLWidth, dpi);
			const Physical physicalHeight = KDPIUtility::toPhysical(compLHeight, dpi);

			compPX = monitorInfo.rcMonitor.left +
				(((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) - physicalWidth) / 2);
			compPY = monitorInfo.rcMonitor.top +
				(((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) - physicalHeight) / 2);

			::SetWindowPos(compHWND, 0, compPX, compPY, 0, 0, SWP_NOSIZE |
				SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
			return;
		}
	}
	this->centerScreen();
}

void KWindow::getNormalSize(Logical& width, Logical& height) noexcept
{
	if (compHWND)
	{
		WINDOWPLACEMENT wndPlacement{ 0 };
		wndPlacement.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement(compHWND, &wndPlacement);

		const int dpi = KDPIUtility::getWindowDPI(compHWND);

		width = KDPIUtility::toLogical(wndPlacement.rcNormalPosition.right - wndPlacement.rcNormalPosition.left, dpi);
		height = KDPIUtility::toLogical(wndPlacement.rcNormalPosition.bottom - wndPlacement.rcNormalPosition.top, dpi);
	}
	else
	{
		width = compLWidth;
		height = compLHeight;
	}
}

bool KWindow::isMinimized() noexcept
{
	if (compHWND)
		return ::IsIconic(compHWND) == TRUE;

	return false;
}

void KWindow::getPositionPhysical(Physical& x, Physical& y) noexcept
{
	if (compHWND)
	{
		WINDOWPLACEMENT wndPlacement{ 0 };
		wndPlacement.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement(compHWND, &wndPlacement);

		x = wndPlacement.rcNormalPosition.left;
		y = wndPlacement.rcNormalPosition.top;
	}
	else
	{
		x = compPX;
		y = compPY;
	}
}

void KWindow::setPositionPhysical(Physical x, Physical y) noexcept
{
	compPX = x;
	compPY = y;
	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compPX, compPY, 0, 0, SWP_NOSIZE |
			SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

bool KWindow::addComponent(KComponent* component, bool requireInitialMessages) noexcept
{
	K_ASSERT(component != nullptr, "component is null");

	if(compHWND)
	{		
		component->setParentHWND(compHWND);
		const int dpi = KDPIUtility::getWindowDPI(compHWND);

		componentList.add(component);

		if (enableDPIUnawareMode)
			this->applyDPIUnawareModeToThread();

		bool retVal = component->create(requireInitialMessages);

		if (retVal && dpi != USER_DEFAULT_SCREEN_DPI && KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE && !enableDPIUnawareMode)
			component->setDPI(dpi);

		if (enableDPIUnawareMode)
			this->restoreDPIModeOfThread();

		return retVal;
	}
	else
	{
		componentList.add(component);
		return true;
	}	
}

bool KWindow::addComponent(KComponent& component, bool requireInitialMessages) noexcept
{
	return addComponent(&component, requireInitialMessages);
}

void KWindow::removeComponent(KComponent* component) noexcept
{
	int index = componentList.getIndex(component);
	if (index != -1)
	{
		componentList.remove(index);
		component->destroy();
	}
}

bool KWindow::addContainer(KHostPanel* container, bool requireInitialMessages) noexcept
{
	if (container)
	{
		container->setComponentList(&componentList);
		container->setEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->addComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

void KWindow::setClientAreaSize(Logical width, Logical height) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	RECT wndRect;
	::GetClientRect(compHWND, &wndRect);

	const int dpi = KDPIUtility::getWindowDPI(compHWND);
	wndRect.right = wndRect.left + KDPIUtility::toPhysical(width, dpi);
	wndRect.bottom = wndRect.top + KDPIUtility::toPhysical(height, dpi);

	KDPIUtility::adjustWindowRectExForDpi(&wndRect, compDwStyle,
		::GetMenu(compHWND) == NULL ? FALSE : TRUE, compDwExStyle, dpi);

	setSize(KDPIUtility::toLogical(wndRect.right - wndRect.left, dpi),
		KDPIUtility::toLogical(wndRect.bottom - wndRect.top, dpi));
}

void KWindow::setClientAreaSizePhysical(Physical width, Physical height) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	const int dpi = getDPI();

	RECT wndRect;
	::GetClientRect(compHWND, &wndRect);

	wndRect.right = wndRect.left + width;
	wndRect.bottom = wndRect.top + height;

	KDPIUtility::adjustWindowRectExForDpi(&wndRect, compDwStyle,
		::GetMenu(compHWND) == NULL ? FALSE : TRUE, compDwExStyle, dpi);

	setSizePhysical(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
}

bool KWindow::isOffScreen(Physical x, Physical y) noexcept
{
	POINT point;
	point.x = x;
	point.y = y;
	return (::MonitorFromPoint(point, MONITOR_DEFAULTTONULL) == NULL);
}

void KWindow::getClientAreaSize(Logical& width, Logical& height) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	RECT wndRect;
	::GetClientRect(compHWND, &wndRect);
	const int dpi = KDPIUtility::getWindowDPI(compHWND);

	width = KDPIUtility::toLogical(wndRect.right - wndRect.left, dpi);
	height = KDPIUtility::toLogical(wndRect.bottom - wndRect.top, dpi);
}

void KWindow::onMoved() noexcept {}

void KWindow::onResized(bool resizingForDPIChange) noexcept {}

LRESULT KWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, 
						MAKEINTATOM(KGUIProc::atomComponent));

					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->eventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				KComponent* component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, 
					MAKEINTATOM(KGUIProc::atomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL: // trackbar
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::atomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}				
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), 
						MAKEINTATOM(KGUIProc::atomComponent));

					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->eventProc(msg, wParam, lParam, &result))
							return result;
					}
				}			
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				KComponent* component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, 
					MAKEINTATOM(KGUIProc::atomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
			{
				KTimer* timer = KIDGenerator::getInstance()->getTimerByID((UINT)wParam);
				if(timer)
				{
					timer->_onTimer();
					break;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);
				const int dpi = KDPIUtility::getWindowDPI(compHWND);

				compLWidth = KDPIUtility::toLogical(rect.right - rect.left, dpi);
				compLHeight = KDPIUtility::toLogical(rect.bottom - rect.top, dpi);

				onResized(resizingForDPIChange);
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);
				compPX = rect.left;
				compPY = rect.top;

				onMoved();
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_DPICHANGED:
			{
				if ((KApplication::dpiAwareness == KDPIAwareness::UNAWARE_MODE) || enableDPIUnawareMode)
					return KComponent::windowProc(hwnd, msg, wParam, lParam);

				const int newDPI = HIWORD(wParam);
				compFontRef.update(newDPI); // just update!

				RECT* const prcNewWindow = (RECT*)lParam;

				// we must use the os provided position according to ms docs!
				compPX = prcNewWindow->left;
				compPY = prcNewWindow->top;

				// ignore OS suggestion for width & height, we use our calculated value.
				const Physical physicalNewWidth = KDPIUtility::toPhysical(compLWidth, newDPI);
				const Physical physicalNewHeight = KDPIUtility::toPhysical(compLHeight, newDPI);

				resizingForDPIChange = true;
				::SetWindowPos(compHWND,
					NULL,
					compPX,
					compPY,
					physicalNewWidth,
					physicalNewHeight,
					SWP_NOZORDER | SWP_NOACTIVATE);
				resizingForDPIChange = false;

				this->updateWindowIconForNewDPI();
				::InvalidateRect(compHWND, NULL, TRUE);

				for (int i = 0; i < componentList.size(); i++)
				{
					componentList[i]->setDPI(newDPI);
				}

				if (onDPIChange)
					onDPIChange(this, newDPI);

				return 0;
			}

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == 0) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					UINT itemID = LOWORD(wParam);
					if (itemID >= KIDGenerator::DynamicMenuItemStart)
					{
						onDynamicMenuItemPress(itemID);
						break;
					}
					else
					{
						KMenuItem* menuItem = KIDGenerator::getInstance()->getMenuItemByID(itemID);
						if (menuItem)
						{
							menuItem->_onPress();
							break;
						}
					}
				}
				else if(lParam)// send to appropriate component
				{
					KComponent* component = (KComponent*)::GetPropW((HWND)lParam, 
						MAKEINTATOM(KGUIProc::atomComponent));

					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->eventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
				else if (LOWORD(wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate enter key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0); 

					return 0;
				}
				else if (LOWORD(wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate esc key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0); 

					return 0;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_CONTEXTMENU:
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)wParam, MAKEINTATOM(KGUIProc::atomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->eventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_ACTIVATE: // save last focused item when inactive
			if (wParam != WA_INACTIVE)
				return KComponent::windowProc(hwnd, msg, wParam, lParam);
			this->lastFocusedChild = ::GetFocus();
			break;

		case WM_SETFOCUS:
			if (this->lastFocusedChild) // set focus to last item
			{
				::SetFocus(this->lastFocusedChild);
			}
			else // set focus to first child
			{
				// if hCtl is NULL, GetNextDlgTabItem returns first control of the window.
				HWND hFirstControl = ::GetNextDlgTabItem(this->compHWND, NULL, FALSE);
				if (hFirstControl)
				{
					if (::GetWindowLongPtrW(hFirstControl, GWL_STYLE) & WS_TABSTOP)
						::SetFocus(hFirstControl);
				}
			}
			break;

		case WM_CLOSE:
			this->onClose();
			break;

		case WM_DESTROY:
			// os automatically destroy child controls after WM_DESTROY.
			this->onDestroy();
			break;

		case RFC_CUSTOM_MESSAGE:
			this->onCustomMessage(wParam, lParam);
			break;

		default:
			return KComponent::windowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow() noexcept
{
	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);
}