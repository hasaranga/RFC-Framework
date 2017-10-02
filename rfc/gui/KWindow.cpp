
/*
    RFC - KWindow.cpp
    Copyright (C) 2013-2017 CrownSoft
  
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

#include"KWindow.h"
#include"../rfc.h"

KWindow::KWindow()
{
	this->SetText(L"KWindow");
	this->SetSize(400, 200);
	this->SetVisible(false);
	this->SetStyle(WS_POPUP);
	this->SetExStyle(WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT);
	wc.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
}

void KWindow::Flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::SetIcon(KIcon *icon)
{
	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)icon->GetHandle());
}

void KWindow::Destroy()
{
	::DestroyWindow(compHWND);
}


void KWindow::OnClose()
{
	this->Destroy();
}

void KWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

void KWindow::CenterScreen()
{
	this->SetPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

bool KWindow::AddComponent(KComponent *component)
{
	if(component)
	{
		if(compHWND)
		{
			component->SetParentHWND(compHWND);
			return component->CreateComponent();
		}
	}
	return false;
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
	static const wchar_t* RFCPropText_Object=L"RFC";
	static const wchar_t* RFCPropText_ClsName=L"RFCClsName";

	switch(msg)
	{
		case WM_COMMAND:
			{
				if(HIWORD(wParam)==BN_CLICKED) // button, checkbox, radio button or menu clicked event!
				{
					if(lParam)
					{
						KButton *btn=(KButton*)::GetPropW((HWND)lParam, RFCPropText_Object);
						if(btn) // button, checkbox or radio button!
						{
							btn->OnPress();
							break;
						}
					}else // its menu item! unfortunately windows does not send menu handle with clicked event!
					{
						KMenuItem *menuItem=KPlatformUtil::GetInstance()->GetMenuItemByID(LOWORD(wParam));
						if(menuItem)
						{
							menuItem->OnPress();
							break;
						}
					}
				}else if(HIWORD(wParam)==LBN_SELCHANGE) // listbox sel change! (this msg also pops for combo)
				{
					wchar_t* clsName=(wchar_t*)::GetPropW((HWND)lParam, RFCPropText_ClsName);
					if(::_wcsicmp(clsName,L"COMBOBOX")!=0) // ignore combobox (use _wcsicmp instead of wcscmp, coz combo cls name might be ComboBox or COMBOBOX)
					{
						KListBox *listBox=(KListBox*)::GetPropW((HWND)lParam, RFCPropText_Object);
						if(listBox)
						{
							listBox->OnItemSelect();
							break;
						}
					}
				}else if(HIWORD(wParam)==CBN_SELENDOK) // combobox sel change!
				{
					KComboBox *comboBox=(KComboBox*)::GetPropW((HWND)lParam, RFCPropText_Object);
					if(comboBox)
					{
						comboBox->OnItemSelect();
						break;
					}
				}
			}
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);

		case WM_HSCROLL:
		case WM_VSCROLL:
			{
				int nScrollCode = (int)LOWORD(wParam);

				if( (TB_THUMBTRACK==nScrollCode)||(TB_LINEDOWN==nScrollCode) || (TB_LINEUP==nScrollCode) || (TB_BOTTOM==nScrollCode) || (TB_TOP==nScrollCode) || (TB_PAGEUP==nScrollCode) || (TB_PAGEDOWN==nScrollCode) ) // its trackbar!
				{
					KTrackBar *trackBar=(KTrackBar*)::GetPropW((HWND)lParam, RFCPropText_Object);
					if(trackBar)
					{
						trackBar->OnChange();
						break;
					}
				}
			}
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);

		case WM_TIMER:
			{
				KTimer *timer=KPlatformUtil::GetInstance()->GetTimerByID((UINT)wParam);
				if(timer)
				{
					timer->OnTimer();
					break;
				}
			}
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->OnMoved();
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

		case WM_CLOSE:
			OnClose();
			break;

		case WM_DESTROY:
			OnDestroy();
			break;

		default:
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow()
{

}