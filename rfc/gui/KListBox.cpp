
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

#include "KListBox.h"
#include "KGUIProc.h"
#include <windowsx.h>

KListBox::KListBox(bool multipleSelection, bool sort, bool vscroll) : KComponent(false)
{
	this->multipleSelection = multipleSelection;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	compClassName.assignStaticText(TXT_WITH_LEN("LISTBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = LBS_NOTIFY | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;
	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	if(multipleSelection)
		compDwStyle = compDwStyle | LBS_MULTIPLESEL;
	if(sort)
		compDwStyle = compDwStyle | LBS_SORT;
	if(vscroll)
		compDwStyle = compDwStyle | WS_VSCROLL;
}

void KListBox::addItem(const KString& text)
{
	stringList.add(text);

	if(compHWND)
		::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)text);
}

void KListBox::removeItem(int index)
{
	if(stringList.remove(index))
	{
		if (compHWND)
			::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
	}
}

void KListBox::removeItem(const KString& text)
{
	const int itemIndex = getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

void KListBox::updateItem(int index, const KString& text)
{
	if (stringList.set(index, text))
	{
		if (compHWND)
		{
			::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
			::SendMessageW(compHWND, LB_INSERTSTRING, index, (LPARAM)(const wchar_t*)text);
		}
	}
}

int KListBox::getItemIndex(const KString& text)
{
	return stringList.getIndex(text);
}

int KListBox::getItemCount()
{
	return stringList.size();
}

int KListBox::getSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

KString KListBox::getSelectedItem()
{
	const int itemIndex = getSelectedItemIndex();
	if(itemIndex > -1)
		return stringList.get(itemIndex);

	return KString();
}

int KListBox::getSelectedItems(int* itemArray, int itemCountInArray)
{
	if(compHWND)
	{	 
		const int items = (int)::SendMessageW(compHWND, 
			LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);

		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void KListBox::clearList()
{
	stringList.removeAll();

	if(compHWND)
		::SendMessageW(compHWND, LB_RESETCONTENT, 0, 0);
}

void KListBox::selectItem(int index)
{
	selectedItemIndex = index;

	if(compHWND)
		::SendMessageW(compHWND, LB_SETCURSEL, index, 0);
}

void KListBox::selectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(compHWND)
			::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool KListBox::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (msg == WM_COMMAND) 
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) // listbox sel change!
		{
			_onItemSelect();
			*result = 0;
			return true;
		}
		else if (HIWORD(wParam) == LBN_DBLCLK) // listbox double click
		{
			_onItemDoubleClick();
			*result = 0;
			return true;
		}
	}
	else if (msg == WM_CONTEXTMENU)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		// Convert screen coordinates to client coordinates for the ListBox
		::ScreenToClient(compHWND, &pt);

		// Determine which item is at the clicked position
		DWORD index = (DWORD)::SendMessageW(compHWND, LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));

		// HIWORD is 0 if the click is on a valid item
		if (HIWORD(index) == 0)
		{
			DWORD itemIndex = LOWORD(index);
			::SendMessageW(compHWND, LB_SETCURSEL, itemIndex, 0); // select it

			_onItemRightClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KListBox::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register LISTBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList.size();
		if(listSize)
		{
			for (int i = 0; i < listSize; i++)
			{
				::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)stringList.get(i));
			}
		}

		if(!multipleSelection) // single selction!
		{
			if(selectedItemIndex > -1)
				::SendMessageW(compHWND, LB_SETCURSEL, selectedItemIndex, 0);
		}else
		{
			if(selectedItemIndex>-1)
				::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(selectedItemIndex, selectedItemEnd));
		}

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KListBox::_onItemSelect()
{
	if(onItemSelect)
		onItemSelect(this);
}

void KListBox::_onItemDoubleClick()
{
	if(onItemDoubleClick)
		onItemDoubleClick(this);
}

void KListBox::_onItemRightClick()
{
	if (onItemRightClick)
		onItemRightClick(this);
}

KListBox::~KListBox() {}