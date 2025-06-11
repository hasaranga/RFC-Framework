
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

#include "KComboBox.h"
#include "KGUIProc.h"

KComboBox::KComboBox(bool sort) : KComponent(false)
{
	selectedItemIndex = -1;

	compClassName.assignStaticText(TXT_WITH_LEN("COMBOBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;

	if(sort)
		compDwStyle = compDwStyle | CBS_SORT;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

void KComboBox::addItem(const KString& text)
{
	stringList.add(text);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)text);
}

void KComboBox::removeItem(int index)
{
	stringList.remove(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::removeItem(const KString& text)
{
	const int itemIndex = getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

int KComboBox::getItemIndex(const KString& text)
{
	return stringList.getIndex(text);
}

int KComboBox::getItemCount()
{
	return stringList.size();
}

int KComboBox::getSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
	}
	return -1;		
}

KString KComboBox::getSelectedItem()
{
	const int itemIndex = getSelectedItemIndex();
	if(itemIndex > -1)
		return stringList.get(itemIndex);

	return KString();
}

void KComboBox::clearList()
{
	stringList.removeAll();
	if(compHWND)
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
}

void KComboBox::selectItem(int index)
{
	selectedItemIndex = index;
	if(compHWND)
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
}

bool KComboBox::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		_onItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

bool KComboBox::create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::createComponent(this, requireInitialMessages); // we dont need to register COMBOBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->getFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList.size();
		if(listSize)
		{
			for (int i = 0; i < listSize; i++)
			{
				::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)stringList.get(i));
			}
		}

		if(selectedItemIndex > -1)
			::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KComboBox::_onItemSelect()
{
	if(onItemSelect)
		onItemSelect(this);
}

KComboBox::~KComboBox() {}


