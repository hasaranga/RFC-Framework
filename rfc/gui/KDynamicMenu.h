
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

#pragma once

#include "../core/CoreModule.h"

// uses the same item id range after removeAll method called.
// no need to allocate anything when adding item.
// you need to override KWindow::onDynamicMenuItemPress to handle press event.
// pressed index can be calculated by substracting initialItemID from itemID at the KWindow::onDynamicMenuItemPress.
class KDynamicMenu
{
protected:
	HMENU hMenu;
	int initialItemID;
	int itemCount;

public:
	KDynamicMenu(int initialItemID) noexcept
	{
		this->initialItemID = initialItemID;
		itemCount = 0;
		hMenu = ::CreatePopupMenu();
	}

	// returns item id
	UINT addMenuItem(const KString& text, bool isChecked = false, bool isDisabled = false) noexcept
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)text;
		mii.cch = text.length();
		mii.fState = (isDisabled ? MFS_DISABLED : MFS_ENABLED) | (isChecked ? MFS_CHECKED : MFS_UNCHECKED);
		mii.wID = initialItemID + itemCount;
		mii.dwItemData = NULL;

		::InsertMenuItemW(hMenu, mii.wID, FALSE, &mii);
		itemCount++;

		return mii.wID;
	}

	void setItemCheckedByID(UINT itemID, bool isChecked)
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = isChecked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}

	void setItemCheckedByIndex(int index, bool isChecked)
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = isChecked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, index, TRUE, &mii);
	}

	bool isItemChecked(UINT itemID) noexcept
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;

		if (::GetMenuItemInfoW(hMenu, itemID, FALSE, &mii))
		{
			return (mii.fState & MFS_CHECKED) != 0;
		}

		return false;
	}

	void addSeperator() noexcept
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;

		::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
	}

	HMENU getMenuHandle() noexcept { return hMenu; }

	void removeAll() noexcept
	{
		int count = ::GetMenuItemCount(hMenu);
		for (int i = count - 1; i >= 0; i--) {
			::DeleteMenu(hMenu, i, MF_BYPOSITION);
		}

		itemCount = 0;
	}

	// set bringWindowToForeground when showing popup menu for notify icon(systray).
	// does not return until the menu close.
	void popUpMenu(HWND window, bool bringWindowToForeground = false) noexcept
	{
		if (bringWindowToForeground)
			::SetForegroundWindow(window);

		POINT p;
		::GetCursorPos(&p);
		::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window, NULL);
	}

	~KDynamicMenu() noexcept { ::DestroyMenu(hMenu); }

private:
	RFC_LEAK_DETECTOR(KDynamicMenu)
};

