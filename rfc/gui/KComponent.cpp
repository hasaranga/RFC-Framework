
/*
	RFC - KComponent.cpp
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


#include "KComponent.h"
#include "KIDGenerator.h"
#include "KGUIProc.h"

KComponent::KComponent(bool generateWindowClassDetails) noexcept
{
	isRegistered = false;
	shouldRegisterClassName = generateWindowClassDetails;

	KIDGenerator *idGenerator = KIDGenerator::getInstance();
	compCtlID = idGenerator->generateControlID();

	compHWND = 0;
	compParentHWND = 0;
	compDwStyle = 0;
	compDwExStyle = 0;
	compLX = 0;
	compLY = 0;
	compLWidth = CW_USEDEFAULT;
	compLHeight = CW_USEDEFAULT;
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
}

KComponent::operator HWND()const noexcept
{
	return compHWND;
}

void KComponent::onHotPlug() noexcept {}

void KComponent::hotPlugInto(HWND component, bool fetchInfo) noexcept
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
		const int compDPI = KDPIUtility::getWindowDPI(compHWND);

		RECT physicalRect;
		::GetWindowRect(compHWND, &physicalRect);

		compLWidth = KDPIUtility::toLogical(physicalRect.right - physicalRect.left, compDPI);
		compLHeight = KDPIUtility::toLogical(physicalRect.bottom - physicalRect.top, compDPI);
		compLX = KDPIUtility::toLogical(physicalRect.left, compDPI);
		compLY = KDPIUtility::toLogical(physicalRect.top, compDPI);

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

UINT KComponent::getControlID() noexcept
{
	return compCtlID;
}

KString KComponent::getComponentClassName() noexcept
{
	return compClassName;
}

void KComponent::afterCreated() noexcept
{
	if (compVisible)
		::ShowWindow(compHWND, SW_SHOW);
}

bool KComponent::create(bool requireInitialMessages) noexcept
{
	// if this is a child component, then compParentHWND must be valid.
	if (compDwStyle & WS_CHILD)
	{
		K_ASSERT(compParentHWND != NULL, "compParentHWND is NULL");
	}

	if (shouldRegisterClassName)
	{
		if (!::RegisterClassExW(&wc))
			return false;

		isRegistered = true;
	}

	KGUIProc::createComponentFor96DPI(this, requireInitialMessages, getX(), getY());

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFontRef.getFontHandle(), 
			MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);

		afterCreated();
		return true;
	}
	return false;
}

void KComponent::onParentDestroy() noexcept {}

void KComponent::destroy() noexcept
{
	if (compHWND)
	{
		::DestroyWindow(compHWND);
		compHWND = 0;
	}

	if (isRegistered)
	{
		::UnregisterClassW(compClassName, KApplication::hInstance);
		isRegistered = false;
	}
}

LRESULT KComponent::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::atomOldProc));
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)KGUIProc::windowProc) // it's a subclassed common control or hot-plugged dialog! RFCOldProc property of subclassed control|dialog is not KGUIProc::windowProc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam); // custom control or window
}

bool KComponent::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) noexcept
{
	return false;
}

void KComponent::setFontType(const KFontType& fontType) noexcept
{
	if (compHWND)
	{
		compFontRef.update(fontType, KDPIUtility::getWindowDPI(compHWND));
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFontRef.getFontHandle(), MAKELPARAM(true, 0));
	}
	else
	{
		compFontRef.update(fontType, USER_DEFAULT_SCREEN_DPI);
	}
}

KFontType KComponent::getFontType() noexcept
{
	return compFontRef.getFontType();
}

KString KComponent::getText() noexcept
{
	return compText;
}

void KComponent::setText(const KString& compText) noexcept
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, this->compText);
}

void KComponent::setHWND(HWND compHWND) noexcept
{
	this->compHWND = compHWND;
}

HWND KComponent::getHWND() noexcept
{
	return compHWND;
}

void KComponent::setParentHWND(HWND compParentHWND) noexcept
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::getParentHWND() noexcept
{
	return compParentHWND;
}

DWORD KComponent::getStyle() noexcept
{
	return compDwStyle;
}

void KComponent::setStyle(DWORD compStyle) noexcept
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::getExStyle() noexcept
{
	return compDwExStyle;
}

void KComponent::setExStyle(DWORD compDwExStyle) noexcept
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

Logical KComponent::getX() noexcept
{
	return compLX; 
}

Logical KComponent::getY() noexcept
{
	return compLY;
}

Logical KComponent::getWidth() noexcept
{
	return compLWidth;
}

Logical KComponent::getHeight() noexcept
{
	return compLHeight;
}

void KComponent::setDPI(int newDPI) noexcept
{
	compFontRef.update(newDPI);

	if (compHWND)
	{
		const Physical physicalX = KDPIUtility::toPhysical(compLX, newDPI);
		const Physical physicalY = KDPIUtility::toPhysical(compLY, newDPI);
		const Physical physicalWidth = KDPIUtility::toPhysical(compLWidth, newDPI);
		const Physical physicalHeight = KDPIUtility::toPhysical(compLHeight, newDPI);

		::SetWindowPos(compHWND, 0, physicalX, physicalY,
			physicalWidth, physicalHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);

		// do not set font of top level windows.
		if(compDwStyle & WS_CHILD)
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFontRef.getFontHandle(), MAKELPARAM(true, 0));
	}
}

void KComponent::setSize(Logical width, Logical height) noexcept
{
	compLWidth = width;
	compLHeight = height;

	if (compHWND)
	{
		const int dpi = KDPIUtility::getWindowDPI(compHWND);
		const Physical physicalWidth = KDPIUtility::toPhysical(width, dpi);
		const Physical physicalHeight = KDPIUtility::toPhysical(height, dpi);

		::SetWindowPos(compHWND, 0, 0, 0, 
			physicalWidth, physicalHeight, SWP_NOMOVE | SWP_NOREPOSITION |
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void KComponent::setSizePhysical(Physical width, Physical height) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	const int dpi = getDPI();
	compLWidth = KDPIUtility::toLogical(width, dpi);
	compLHeight = KDPIUtility::toLogical(height, dpi);

	::SetWindowPos(compHWND, 0, 0, 0,
		width, height, SWP_NOMOVE | SWP_NOREPOSITION |
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::setPosition(Logical x, Logical y) noexcept
{
	compLX = x;
	compLY = y;

	if (compHWND)
	{
		const int dpi = KDPIUtility::getWindowDPI(compHWND);
		const Physical physicalX = KDPIUtility::toPhysical(x, dpi);
		const Physical physicalY = KDPIUtility::toPhysical(y, dpi);

		::SetWindowPos(compHWND, 0, physicalX, physicalY, 0, 0, SWP_NOSIZE |
			SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void KComponent::placeToRightOf(KComponent& target, Logical spacing) noexcept
{
	setPosition(target.getX() + target.getWidth() + spacing, target.getY());
}

void KComponent::placeToLeftOf(KComponent& target, Logical spacing) noexcept
{
	setPosition((target.getX() - getWidth()) - spacing, target.getY());
}

void KComponent::placeBelow(KComponent& target, Logical spacing) noexcept
{
	setPosition(target.getX(), target.getY() + target.getHeight() + spacing);
}

void KComponent::placeAbove(KComponent& target, Logical spacing) noexcept
{
	setPosition(target.getX(), (target.getY() - getHeight()) - spacing);
}

void KComponent::alignTopWith(KComponent& target) noexcept
{
	setPosition(getX(), target.getY());
}

void KComponent::alignBottomWith(KComponent& target) noexcept
{
	setPosition(getX(), (target.getY() + target.getHeight()) - getHeight());
}

void KComponent::alignLeftWith(KComponent& target) noexcept
{
	setPosition(target.getX(), getY());
}

void KComponent::alignRightWith(KComponent& target) noexcept
{
	setPosition(target.getX() + target.getWidth() - getWidth(), getY());
}

void KComponent::alignCenterHorizontallyWith(KComponent& target) noexcept
{
	const int x = target.getX() + (target.getWidth() - getWidth()) / 2;
	setPosition(x, getY());
}

void KComponent::alignCenterVerticallyWith(KComponent& target) noexcept
{
	const int y = target.getY() + (target.getHeight() - getHeight()) / 2;
	setPosition(getX(), y);
}

void KComponent::setVisible(bool state) noexcept
{
	compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::isVisible() noexcept
{
	if (compHWND)
	{
		compVisible = (::IsWindowVisible(compHWND) == TRUE);
		return compVisible;
	}

	return false;
}

bool KComponent::isEnabled() noexcept
{
	if (compHWND)
		compEnabled = (::IsWindowEnabled(compHWND) == TRUE);

	return compEnabled;
}

void KComponent::setEnabled(bool state) noexcept
{
	compEnabled = state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::bringToFront() noexcept
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::setKeyboardFocus() noexcept
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::repaint() noexcept
{
	if (compHWND)
	{
		::InvalidateRect(compHWND, NULL, TRUE);
		::UpdateWindow(compHWND); // instant update
	}
}

KComponent::~KComponent() noexcept
{
	if (isRegistered)
	{
		::UnregisterClassW(compClassName, KApplication::hInstance);
		isRegistered = false;
	}
}