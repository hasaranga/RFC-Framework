
/*
	RFC - KComponent.cpp
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


#include "KComponent.h"
#include "KIDGenerator.h"
#include "KGUIProc.h"

KComponent::KComponent(bool generateWindowClassDetails)
{
	isRegistered = false;

	KIDGenerator *idGenerator = KIDGenerator::getInstance();
	compCtlID = idGenerator->generateControlID();

	compHWND = 0;
	compParentHWND = 0;
	compDwStyle = 0;
	compDwExStyle = 0;
	cursor = 0;
	compX = 0;
	compY = 0;
	compWidth = CW_USEDEFAULT;
	compHeight = CW_USEDEFAULT;
	compDPI = USER_DEFAULT_SCREEN_DPI;
	compVisible = true;
	compEnabled = true;

	if (generateWindowClassDetails)
	{
		idGenerator->generateClassName(compClassName);
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = 0;
		wc.lpszMenuName = 0;
		wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIconSm = 0;
		wc.style = 0;
		wc.hInstance = KApplication::hInstance;
		wc.lpszClassName = compClassName;

		wc.lpfnWndProc = KGUIProc::windowProc;
	}

	compFont = KFont::getDefaultFont();
}

KComponent::operator HWND()const
{
	return compHWND;
}

void KComponent::onHotPlug()
{

}

void KComponent::hotPlugInto(HWND component, bool fetchInfo)
{
	compHWND = component;

	if (fetchInfo)
	{
		wchar_t *clsName = (wchar_t*)::malloc(256 * sizeof(wchar_t));  // assume 256 is enough
		clsName[0] = 0;
		::GetClassNameW(compHWND, clsName, 256);
		compClassName = KString(clsName, KStringBehaviour::FREE_ON_DESTROY);

		::GetClassInfoExW(KApplication::hInstance, compClassName, &wc);

		compCtlID = (UINT)::GetWindowLongPtrW(compHWND, GWL_ID);

		RECT rect;
		::GetWindowRect(compHWND, &rect);
		compWidth = rect.right - rect.left;
		compHeight = rect.bottom - rect.top;
		compX = rect.left;
		compY = rect.top;

		compVisible = (::IsWindowVisible(compHWND) ? true : false);
		compEnabled = (::IsWindowEnabled(compHWND) ? true : false);

		compDwStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_STYLE);
		compDwExStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_EXSTYLE);

		compParentHWND = ::GetParent(compHWND);

		wchar_t *buff = (wchar_t*)::malloc(256 * sizeof(wchar_t)); // assume 256 is enough
		buff[0] = 0;
		::GetWindowTextW(compHWND, buff, 256);
		compText = KString(buff, KStringBehaviour::FREE_ON_DESTROY);
	}

	KGUIProc::attachRFCPropertiesToHWND(compHWND, (KComponent*)this);	

	this->onHotPlug();
}

UINT KComponent::getControlID()
{
	return compCtlID;
}

void KComponent::setMouseCursor(KCursor *cursor)
{
	this->cursor = cursor;
	if(compHWND)
		::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->getHandle());
}

KString KComponent::getComponentClassName()
{
	return compClassName;
}

bool KComponent::create(bool requireInitialMessages)
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered = true;

	KGUIProc::createComponent(this, requireInitialMessages);

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);
		::ShowWindow(compHWND, compVisible ? SW_SHOW : SW_HIDE);

		if(cursor)
			::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->getHandle());

		return true;
	}
	return false;
}

void KComponent::onParentDestroy() {}

void KComponent::destroy()
{
	if (compHWND)
	{
		::DestroyWindow(compHWND);
		compHWND = 0;
	}
}

LRESULT KComponent::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::atomOldProc));
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)KGUIProc::windowProc) // it's a subclassed common control or hot-plugged dialog! RFCOldProc property of subclassed control|dialog is not KGUIProc::windowProc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam); // custom control or window
}

bool KComponent::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	return false;
}

void KComponent::setFont(KFont *compFont)
{
	this->compFont = compFont;
	if(compHWND)
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0));
}

KFont* KComponent::getFont()
{
	return compFont;
}

KString KComponent::getText()
{
	return compText;
}

void KComponent::setText(const KString& compText)
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, this->compText);
}

void KComponent::setHWND(HWND compHWND)
{
	this->compHWND = compHWND;
}

HWND KComponent::getHWND()
{
	return compHWND;
}

void KComponent::setParentHWND(HWND compParentHWND)
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::getParentHWND()
{
	return compParentHWND;
}

DWORD KComponent::getStyle()
{
	return compDwStyle;
}

void KComponent::setStyle(DWORD compStyle)
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::getExStyle()
{
	return compDwExStyle;
}

void KComponent::setExStyle(DWORD compDwExStyle)
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

int KComponent::getDPI()
{
	return compDPI;
}

int KComponent::scaleToCurrentDPI(int valueFor96DPI)
{
	return MulDiv(valueFor96DPI, compDPI, USER_DEFAULT_SCREEN_DPI);
}

int KComponent::getX()
{
	return compX; 
}

int KComponent::getY()
{
	return compY;
}

int KComponent::getWidth()
{
	return compWidth;
}

int KComponent::getHeight()
{
	return compHeight;
}

void KComponent::setDPI(int newDPI)
{
	if (newDPI == compDPI)
		return;

	const int oldDPI = compDPI;
	compDPI = newDPI;

	if (compDwStyle & WS_CHILD) // do not change position and font size of top level windows.
	{
		this->compX = ::MulDiv(compX, newDPI, oldDPI);
		this->compY = ::MulDiv(compY, newDPI, oldDPI);

		if (!compFont->isDefaultFont())
			compFont->setDPI(newDPI);
	}

	this->compWidth = ::MulDiv(compWidth, newDPI, oldDPI);
	this->compHeight = ::MulDiv(compHeight, newDPI, oldDPI);

	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compX, compY, compWidth, compHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
		if((!compFont->isDefaultFont()) && (compDwStyle & WS_CHILD))
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0));
	}
}

void KComponent::setSize(int compWidth, int compHeight)
{
	this->compWidth = compWidth;
	this->compHeight = compHeight;

	if(compHWND)
		::SetWindowPos(compHWND, 0, 0, 0, compWidth, compHeight, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::setPosition(int compX, int compY)
{
	this->compX = compX;
	this->compY = compY;

	if(compHWND)
		::SetWindowPos(compHWND, 0, compX, compY, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::placeToRightOf(KComponent& target, int spacing)
{
	setPosition(target.getX() + target.getWidth() + spacing, target.getY());
}

void KComponent::placeToLeftOf(KComponent& target, int spacing)
{
	setPosition(target.getX() - getWidth() - spacing, target.getY());
}

void KComponent::placeBelow(KComponent& target, int spacing)
{
	setPosition(target.getX(), target.getY() + target.getHeight() + spacing);
}

void KComponent::placeAbove(KComponent& target, int spacing)
{
	setPosition(target.getX(), target.getY() - getHeight() - spacing);
}

void KComponent::alignTopWith(KComponent& target)
{
	setPosition(getX(), target.getY());
}

void KComponent::alignBottomWith(KComponent& target)
{
	setPosition(getX(), target.getY() + target.getHeight() - getHeight());
}

void KComponent::alignLeftWith(KComponent& target)
{
	setPosition(target.getX(), getY());
}

void KComponent::alignRightWith(KComponent& target)
{
	setPosition(target.getX() + target.getWidth() - getWidth(), getY());
}

void KComponent::alignCenterHorizontallyWith(KComponent& target)
{
	int x = target.getX() + (target.getWidth() - getWidth()) / 2;
	setPosition(x, getY());
}

void KComponent::alignCenterVerticallyWith(KComponent& target)
{
	int y = target.getY() + (target.getHeight() - getHeight()) / 2;
	setPosition(getX(), y);
}

void KComponent::setVisible(bool state)
{
	compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::isVisible()
{
	if (compHWND)
	{
		compVisible = (::IsWindowVisible(compHWND) == TRUE);
		return compVisible;
	}

	return false;
}

bool KComponent::isEnabled()
{
	if (compHWND)
		compEnabled = (::IsWindowEnabled(compHWND) == TRUE);

	return compEnabled;
}

void KComponent::setEnabled(bool state)
{
	compEnabled = state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::bringToFront()
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::setKeyboardFocus()
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::repaint()
{
	if (compHWND)
	{
		::InvalidateRect(compHWND, NULL, TRUE);
		::UpdateWindow(compHWND); // instant update
	}
}

KComponent::~KComponent()
{
	if(isRegistered)
		::UnregisterClassW(compClassName, KApplication::hInstance);
}