
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

#include "KMenuItem.h"
#include "KIDGenerator.h"

KMenuItem::KMenuItem()
{
	hMenu = 0;
	param = nullptr;
	intParam = -1;
	enabled = true;
	checked = false;
	itemID = KIDGenerator::getInstance()->generateMenuItemID(this);
}

void KMenuItem::addToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
	mii.cch = itemText.length();
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);

}

void KMenuItem::setParam(void* param)
{
	this->param = param;
}

void KMenuItem::setIntParam(int intParam)
{
	this->intParam = intParam;
}

int KMenuItem::getIntParam()
{
	return intParam;
}

void* KMenuItem::getParam()
{
	return param;
}

bool KMenuItem::isChecked()
{
	return checked;
}

void KMenuItem::setCheckedState(bool state)
{
	checked = state;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool KMenuItem::isEnabled()
{
	return enabled; 
}

void KMenuItem::setEnabled(bool state)
{
	enabled = state;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void KMenuItem::setText(const KString& text)
{
	itemText = text;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

KString KMenuItem::getText()
{
	return itemText;
}

UINT KMenuItem::getItemID()
{
	return itemID;
}

HMENU KMenuItem::getMenuHandle()
{
	return hMenu;
}

void KMenuItem::_onPress()
{
	if(onPress)
		onPress(this);
}

KMenuItem::~KMenuItem()
{
}

