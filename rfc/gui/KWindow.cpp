
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

#include "KWindow.h"
#include "KGUIProc.h"
#include "KTimer.h"
#include "KIDGenerator.h"

KWindow::KWindow() : KComponent(true), componentList(50, false)
{
	compText.AssignStaticText(TXT_WITH_LEN("KWindow"));

	compWidth = 400;
	compHeight = 200;

	compVisible = false;
	enableDPIUnawareMode = false;
	compDwStyle = WS_POPUP;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
	dpiChangeListener = NULL;
}

bool KWindow::Create(bool requireInitialMessages)
{
	DPI_AWARENESS_CONTEXT prevContext = 0;
	bool dpiAwarenessContextChanged = false;

	if ((KApplication::dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY) && KApplication::dpiAwareAPICalled && enableDPIUnawareMode)
	{
		if (KDPIUtility::pSetThreadDpiAwarenessContext)
		{
			prevContext = KDPIUtility::pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
			dpiAwarenessContextChanged = true;
		}
	}

	bool retVal = KComponent::Create(requireInitialMessages);

	if(dpiAwarenessContextChanged)
		KDPIUtility::pSetThreadDpiAwarenessContext(prevContext);

	if(retVal && (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		int dpi = KDPIUtility::GetWindowDPI(compHWND);
		if (dpi != USER_DEFAULT_SCREEN_DPI)
			this->SetDPI(dpi);
	}

	return retVal;
}

void KWindow::Flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::SetIcon(KIcon *icon)
{
	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)icon->GetHandle());
}

void KWindow::SetDPIChangeListener(KDPIChangeListener* dpiChangeListener)
{
	this->dpiChangeListener = dpiChangeListener;
}

void KWindow::SetEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

void KWindow::OnClose()
{
	this->Destroy();
}

void KWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

void KWindow::PostCustomMessage(WPARAM msgID, LPARAM param)
{
	::PostMessageW(compHWND, RFC_CUSTOM_MESSAGE, msgID, param);
}

void KWindow::OnCustomMessage(WPARAM msgID, LPARAM param)
{

}

void KWindow::CenterScreen()
{
	this->SetPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

bool KWindow::AddComponent(KComponent *component, bool requireInitialMessages)
{
	if(component)
	{
		if(compHWND)
		{		
			component->SetParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled )
				component->SetDPI(compDPI);

			componentList.AddPointer(component);

			return component->Create(requireInitialMessages);
		}
	}
	return false;
}

void KWindow::RemoveComponent(KComponent* component)
{
	int index = componentList.GetID(component);
	if (index != -1)
	{
		componentList.RemovePointer(index);
		component->Destroy();
	}
}

bool KWindow::SetClientAreaSize(int width, int height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		wndRect.right = wndRect.left + width;
		wndRect.bottom = wndRect.top + height;

		::AdjustWindowRect(&wndRect, compDwStyle, ::GetMenu(compHWND) == NULL ? FALSE : TRUE);
		this->SetSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

		return true;
	}
	return false;
}

bool KWindow::IsOffScreen(int posX, int posY)
{
	POINT point;
	point.x = posX;
	point.y = posY;
	return (::MonitorFromPoint(point, MONITOR_DEFAULTTONULL) == NULL);
}

bool KWindow::GetClientAreaSize(int *width, int *height)
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

void KWindow::OnMoved()
{

}

void KWindow::OnResized()
{

}

LRESULT KWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent *component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::AtomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				KComponent *component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, MAKEINTATOM(KGUIProc::AtomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

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
				KComponent *component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::AtomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}				
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent *component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::AtomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}			
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				KComponent *component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::AtomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
			{
				KTimer *timer = KIDGenerator::GetInstance()->GetTimerByID((UINT)wParam);
				if(timer)
				{
					timer->OnTimer();
					break;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compWidth = rect.right - rect.left;
				this->compHeight = rect.bottom - rect.top;

				this->OnResized();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->OnMoved();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_DPICHANGED:
			{
				if ((KApplication::dpiAwareness == KDPIAwareness::UNAWARE_MODE) || enableDPIUnawareMode || (!KApplication::dpiAwareAPICalled))
					return KComponent::WindowProc(hwnd, msg, wParam, lParam);

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

				::InvalidateRect(compHWND, NULL, TRUE);

				for (int i = 0; i < componentList.GetSize(); i++)
				{
					componentList[i]->SetDPI(compDPI);
				}

				if (dpiChangeListener)
					dpiChangeListener->OnDPIChange(compHWND, compDPI);

				return 0;
			}

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == 0) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					KMenuItem *menuItem = KIDGenerator::GetInstance()->GetMenuItemByID(LOWORD(wParam));
					if(menuItem)
					{
						menuItem->OnPress();
						break;
					}
				}
				else if(lParam)// send to appropriate component
				{
					KComponent *component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::AtomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
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
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_ACTIVATE: // save last focused item when inactive
			if (wParam != WA_INACTIVE)
				return KComponent::WindowProc(hwnd, msg, wParam, lParam);
			this->lastFocusedChild = ::GetFocus();
			break;

		case WM_SETFOCUS:
			if (this->lastFocusedChild) // set focus to last item
				::SetFocus(this->lastFocusedChild);
			else // set focus to first child
				::SetFocus(::GetNextDlgTabItem(this->compHWND, NULL, FALSE));
			break;

		case WM_CLOSE:
			this->OnClose();
			break;

		case WM_DESTROY:
			this->OnDestroy();
			break;

		case RFC_CUSTOM_MESSAGE:
			this->OnCustomMessage(wParam, lParam);
			break;

		default:
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow()
{

}