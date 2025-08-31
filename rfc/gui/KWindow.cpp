
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

#include "KWindow.h"
#include "KGUIProc.h"
#include "KTimer.h"
#include "KIDGenerator.h"

KWindow::KWindow() : KComponent(true)
{
	compText.assignStaticText(TXT_WITH_LEN("KWindow"));

	compWidth = 400;
	compHeight = 200;

	compVisible = false;
	enableDPIUnawareMode = false;
	prevDPIContext = 0;
	dpiAwarenessContextChanged = false;
	compDwStyle = WS_POPUP | WS_CLIPCHILDREN;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
	windowIcon = nullptr;
	largeIconHandle = 0;
	smallIconHandle = 0;

	closeOperation = KCloseOperation::DestroyAndExit;
}

void KWindow::applyDPIUnawareModeToThread()
{
	if ((KApplication::dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY) && KApplication::dpiAwareAPICalled && enableDPIUnawareMode)
	{
		if (KDPIUtility::pSetThreadDpiAwarenessContext)
		{
			prevDPIContext = KDPIUtility::pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
			dpiAwarenessContextChanged = true;
		}
	}
}

void KWindow::restoreDPIModeOfThread()
{
	if (dpiAwarenessContextChanged)
		KDPIUtility::pSetThreadDpiAwarenessContext(prevDPIContext);
}

bool KWindow::create(bool requireInitialMessages)
{
	if (enableDPIUnawareMode)
		this->applyDPIUnawareModeToThread();

	bool retVal = KComponent::create(requireInitialMessages);

	if (enableDPIUnawareMode)
		this->restoreDPIModeOfThread();

	if(retVal && (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		int dpi = KDPIUtility::getWindowDPI(compHWND);
		if (dpi != USER_DEFAULT_SCREEN_DPI)
			this->setDPI(dpi);
	}

	return retVal;
}

void KWindow::flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::updateWindowIconForNewDPI()
{
	if (windowIcon == nullptr)
		return;

	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);

	// 32x32 for large icon
	// 16x16 for small icon

	if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		largeIconHandle = windowIcon->getScaledIcon(KDPIUtility::scaleToNewDPI(32, compDPI));
		smallIconHandle = windowIcon->getScaledIcon(KDPIUtility::scaleToNewDPI(16, compDPI));
	}
	else
	{
		largeIconHandle = windowIcon->getScaledIcon(32);
		smallIconHandle = windowIcon->getScaledIcon(16);
	}

	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)largeIconHandle);
	::SetClassLongPtrW(compHWND, GCLP_HICONSM, (LONG_PTR)smallIconHandle);
}

void KWindow::setIcon(KIcon* icon)
{
	windowIcon = icon;

	if (compHWND)
		this->updateWindowIconForNewDPI();
}

void KWindow::setCloseOperation(KCloseOperation closeOperation)
{
	this->closeOperation = closeOperation;
}

void KWindow::setEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

void KWindow::onClose()
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

void KWindow::onDestroy()
{
	for (int i = 0; i < componentList.size(); i++)
	{
		componentList[i]->onParentDestroy();
	}

	if (closeOperation == KCloseOperation::DestroyAndExit)
		::PostQuitMessage(0);
}

void KWindow::postCustomMessage(WPARAM msgID, LPARAM param)
{
	::PostMessageW(compHWND, RFC_CUSTOM_MESSAGE, msgID, param);
}

void KWindow::onCustomMessage(WPARAM msgID, LPARAM param)
{

}

void KWindow::centerScreen()
{
	this->setPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

void KWindow::centerOnSameMonitor(HWND window)
{
	if (window)
	{
		HMONITOR hmon = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

		if (hmon != NULL)
		{
			MONITORINFO monitorInfo;
			::ZeroMemory(&monitorInfo, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if (::GetMonitorInfoW(hmon, &monitorInfo))
			{
				const int posX = monitorInfo.rcMonitor.left + (((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) - compWidth) / 2);
				const int posY = monitorInfo.rcMonitor.top + (((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) - compHeight) / 2);
				this->setPosition(posX, posY);

				return;
			}
		}
	}

	this->centerScreen();
}

void KWindow::getNormalSize(int* width, int* height)
{
	if (compHWND)
	{
		WINDOWPLACEMENT wndPlacement{ 0 };
		wndPlacement.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement(compHWND, &wndPlacement);

		*width = wndPlacement.rcNormalPosition.right - wndPlacement.rcNormalPosition.left;
		*height = wndPlacement.rcNormalPosition.bottom - wndPlacement.rcNormalPosition.top;
	}
	else
	{
		*width = compWidth;
		*height = compHeight;
	}
}

bool KWindow::addComponent(KComponent* component, bool requireInitialMessages)
{
	if(component)
	{
		if(compHWND)
		{		
			component->setParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled )
				component->setDPI(compDPI);

			componentList.add(component);

			if (enableDPIUnawareMode)
				this->applyDPIUnawareModeToThread();

			bool retVal = component->create(requireInitialMessages);

			if (enableDPIUnawareMode)
				this->restoreDPIModeOfThread();

			return retVal;
		}
	}
	return false;
}

bool KWindow::addComponent(KComponent& component, bool requireInitialMessages)
{
	return addComponent(&component, requireInitialMessages);
}

void KWindow::removeComponent(KComponent* component)
{
	int index = componentList.getIndex(component);
	if (index != -1)
	{
		componentList.remove(index);
		component->destroy();
	}
}

bool KWindow::addContainer(KHostPanel* container, bool requireInitialMessages)
{
	if (container)
	{
		container->setComponentList(&componentList);
		container->setEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->addComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

bool KWindow::setClientAreaSize(int width, int height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		wndRect.right = wndRect.left + width;
		wndRect.bottom = wndRect.top + height;

		KDPIUtility::adjustWindowRectExForDpi(&wndRect, compDwStyle,
			::GetMenu(compHWND) == NULL ? FALSE : TRUE, compDwExStyle, compDPI);

		this->setSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

		return true;
	}
	return false;
}

bool KWindow::isOffScreen(int posX, int posY)
{
	POINT point;
	point.x = posX;
	point.y = posY;
	return (::MonitorFromPoint(point, MONITOR_DEFAULTTONULL) == NULL);
}

bool KWindow::getClientAreaSize(int* width, int* height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		if (width)
			*width = wndRect.right - wndRect.left;

		if (height)
			*height = wndRect.bottom - wndRect.top;

		return true;
	}
	return false;
}

void KWindow::onMoved()
{

}

void KWindow::onResized()
{

}

LRESULT KWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::atomComponent));
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
				KComponent* component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, MAKEINTATOM(KGUIProc::atomComponent));
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
		case WM_VSCROLL:
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
					KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::atomComponent));
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
				KComponent* component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::atomComponent));
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

				this->compWidth = rect.right - rect.left;
				this->compHeight = rect.bottom - rect.top;

				this->onResized();
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->onMoved();
			}
			return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_DPICHANGED:
			{
				if ((KApplication::dpiAwareness == KDPIAwareness::UNAWARE_MODE) || enableDPIUnawareMode || (!KApplication::dpiAwareAPICalled))
					return KComponent::windowProc(hwnd, msg, wParam, lParam);

				this->compDPI = HIWORD(wParam);
				RECT* const prcNewWindow = (RECT*)lParam;

				this->compX = prcNewWindow->left;
				this->compY = prcNewWindow->top;
				this->compWidth = prcNewWindow->right - prcNewWindow->left;
				this->compHeight = prcNewWindow->bottom - prcNewWindow->top;

				::SetWindowPos(compHWND,
					NULL,
					this->compX,
					this->compY,
					this->compWidth,
					this->compHeight,
					SWP_NOZORDER | SWP_NOACTIVATE);

				this->updateWindowIconForNewDPI();

				::InvalidateRect(compHWND, NULL, TRUE);

				for (int i = 0; i < componentList.size(); i++)
				{
					componentList[i]->setDPI(compDPI);
				}

				if (onDPIChange)
					onDPIChange(this);

				return 0;
			}

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == 0) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					KMenuItem* menuItem = KIDGenerator::getInstance()->getMenuItemByID(LOWORD(wParam));
					if(menuItem)
					{
						menuItem->_onPress();
						break;
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


KWindow::~KWindow()
{
	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);
}