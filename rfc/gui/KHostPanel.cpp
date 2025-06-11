
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


#include "KHostPanel.h"
#include "KGUIProc.h"
#include "KTimer.h"
#include "KIDGenerator.h"

KHostPanel::KHostPanel() : KComponent(true)
{
	compDwStyle = WS_CHILD | WS_CLIPCHILDREN;
	compDwExStyle = compDwExStyle | WS_EX_CONTROLPARENT; // non-top-level windows that contain child controls/dialogs must have the WS_EX_CONTROLPARENT style.
	// otherwise you will get infinite loop when calling IsDialogMessage.

	compText.assignStaticText(TXT_WITH_LEN("KHostPanel"));
	enableDPIUnawareMode = false;
	componentList = nullptr;
}

void KHostPanel::setComponentList(KPointerList<KComponent*, 24, false>* componentList)
{
	this->componentList = componentList;
}

void KHostPanel::setEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

bool KHostPanel::addComponent(KComponent* component, bool requireInitialMessages)
{
	if (component)
	{
		if ((compHWND != 0) && (componentList != nullptr))
		{
			component->setParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
				component->setDPI(compDPI);

			componentList->add(component);

			return component->create(requireInitialMessages);
		}
	}
	return false;
}

void KHostPanel::removeComponent(KComponent* component)
{
	if (componentList == nullptr)
		return;

	int index = componentList->getIndex(component);
	if (index != -1)
	{
		componentList->remove(index);
		component->destroy();
	}
}

bool KHostPanel::addContainer(KHostPanel* container, bool requireInitialMessages)
{
	if (container)
	{
		container->setComponentList(componentList);
		container->setEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->addComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

LRESULT KHostPanel::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
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
				KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd, 
					((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::atomComponent));

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
			if (timer)
			{
				timer->_onTimer();
				break;
			}
		}
		return KComponent::windowProc(hwnd, msg, wParam, lParam);

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
		{
			if ((HIWORD(wParam) == 0) && (lParam == 0)) // its menu item! unfortunately windows does not send menu handle with clicked event!
			{
				KMenuItem* menuItem = KIDGenerator::getInstance()->getMenuItemByID(LOWORD(wParam));
				if (menuItem)
				{
					menuItem->_onPress();
					break;
				}
			}
			else if (lParam)// send to appropriate component
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

		default:
			return KComponent::windowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

KHostPanel::~KHostPanel() {}