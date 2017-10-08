
/*
    RFC - KComponent.cpp
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


#include "KComponent.h"
#include "../rfc.h"


KComponent::KComponent()
{
	isRegistered = false;

	compClassName = KPlatformUtil::GetInstance()->GenerateClassName();

	compHWND = 0;
	compParentHWND = 0;
	compText = KString();
	compDwStyle = 0;
	compDwExStyle = 0;
	cursor = 0;
	compX = CW_USEDEFAULT;
	compY = CW_USEDEFAULT;
	compWidth = CW_USEDEFAULT;
	compHeight = CW_USEDEFAULT;
	compVisible = true;
	compEnabled = true;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = 0;
	wc.lpszMenuName = 0;
	wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIconSm = 0;
	wc.style = 0;
	wc.hInstance = KPlatformUtil::GetInstance()->GetAppHInstance();
	wc.lpszClassName = (const wchar_t*)compClassName;

	wc.lpfnWndProc = ::GlobalWnd_Proc;

	compFont = KFont::GetDefaultFont();
}

void KComponent::OnHotPlug()
{

	RECT rect;
	::GetWindowRect(compHWND, &rect);
	compWidth = rect.right - rect.left;
	compHeight = rect.bottom - rect.top;
	compX = rect.left;
	compY = rect.top;

	compVisible = ::IsWindowVisible(compHWND) ? true : false;
	compEnabled = ::IsWindowEnabled(compHWND) ? true : false;

	compDwStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_STYLE);
	compDwExStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_EXSTYLE);

	compParentHWND = ::GetParent(compHWND);

	wchar_t *buff = (wchar_t*)::malloc(256 * sizeof(wchar_t));
	buff[0] = 0;
	::GetWindowTextW(compHWND, buff, 256);
	compText = buff;

	free(buff);
}

void KComponent::HotPlugInto(HWND component)
{
	static const wchar_t* RFCPropText_Object = L"RFC";
	static const wchar_t* RFCPropText_OldProc = L"RFCOldProc";
	static const wchar_t* RFCPropText_ClsName = L"RFCClsName";

	compHWND = component;
	::SetPropW(compHWND, RFCPropText_Object, (HANDLE)this);
	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(compHWND, GWLP_WNDPROC);
	::SetPropW(compHWND, RFCPropText_OldProc, (HANDLE)lpfnOldWndProc);

	wchar_t *clsName = (wchar_t*)::malloc(256 * sizeof(wchar_t));
	clsName[0] = 0;
	::GetClassNameW(compHWND, clsName, 256);
	compClassName = clsName;
	::free(clsName);

	::GetClassInfoExW(KPlatformUtil::GetInstance()->GetAppHInstance(), compClassName, &wc);

	::SetPropW(compHWND, RFCPropText_ClsName, (HANDLE)(const wchar_t*)compClassName);

	::SetWindowLongPtrW(compHWND, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...

	this->OnHotPlug();
}

void KComponent::SetMouseCursor(KCursor *cursor)
{
	this->cursor = cursor;
	if(compHWND)
		::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());
}

KString KComponent::GetComponentClassName()
{
	return compClassName;
}

bool KComponent::CreateComponent()
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered=true;

	::CreateRFCComponent(this);

	if(compHWND)
	{
		::EnableWindow(compHWND, compEnabled);

		if(this->IsVisible())
			this->SetVisible(true);

		if(cursor)
			::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());

		return true;
	}
	return false;
}

LRESULT KComponent::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static const wchar_t* RFCPropText_OldProc = L"RFCOldProc";

	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, RFCPropText_OldProc);
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)::GlobalWnd_Proc) // it's subclassed control or dialog! RFCOldProc of subclassed dialog is not GlobalDlg_Proc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam);
}

void KComponent::SetFont(KFont *compFont)
{
	this->compFont = compFont;
	if(compHWND)
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0));
}

KFont* KComponent::GetFont()
{
	return compFont;
}

KString KComponent::GetText()
{
	return compText;
}

void KComponent::SetText(const KString& compText)
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, (const wchar_t*)this->compText);
}

void KComponent::SetHWND(HWND compHWND)
{
	this->compHWND = compHWND;
}

HWND KComponent::GetHWND()
{
	return compHWND;
}

void KComponent::SetParentHWND(HWND compParentHWND)
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::GetParentHWND()
{
	return compParentHWND;
}

DWORD KComponent::GetStyle()
{
	return compDwStyle;
}

void KComponent::SetStyle(DWORD compStyle)
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::GetExStyle()
{
	return compDwExStyle;
}

void KComponent::SetExStyle(DWORD compDwExStyle)
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

int KComponent::GetX()
{
	return compX; 
}

int KComponent::GetY()
{
	return compY;
}

int KComponent::GetWidth()
{
	return compWidth;
}

int KComponent::GetHeight()
{
	return compHeight;
}

void KComponent::SetSize(int compWidth, int compHeight)
{
	this->compWidth = compWidth;
	this->compHeight = compHeight;

	if(compHWND)
		::SetWindowPos(compHWND, 0, 0, 0, compWidth, compHeight, SWP_NOMOVE | SWP_NOREPOSITION);
}

void KComponent::SetPosition(int compX, int compY)
{
	this->compX = compX;
	this->compY = compY;

	if(compHWND)
		::SetWindowPos(compHWND, 0, compX, compY, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION);
}

void KComponent::SetVisible(bool state)
{
	this->compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::IsVisible()
{
	return compHWND ? this->compVisible : false;
}

bool KComponent::IsEnabled()
{
	return compEnabled;
}

void KComponent::SetEnabled(bool state)
{
	compEnabled=state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::BringToFront()
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::SetKeyboardFocus()
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::Repaint()
{
	if(compHWND)
		::InvalidateRect(compHWND, NULL, TRUE);
}

KComponent::~KComponent()
{
	if(isRegistered)
		::UnregisterClassW((const wchar_t*)compClassName, KPlatformUtil::GetInstance()->GetAppHInstance());
}