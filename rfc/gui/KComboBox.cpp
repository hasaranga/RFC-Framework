
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

#include "KComboBox.h"
#include "KGUIProc.h"

KComboBox::KComboBox(bool sort) noexcept : KComponent(false)
{
	selectedItemIndex = -1;

	compClassName.assignStaticText(TXT_WITH_LEN("COMBOBOX"));

	compLWidth = 100;
	compLHeight = 100;

	compDwStyle = WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;

	if(sort)
		compDwStyle = compDwStyle | CBS_SORT;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

void KComboBox::addItem(const KString& text) noexcept
{
	stringList.add(text);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)text);
}

void KComboBox::removeItem(int index) noexcept
{
	stringList.remove(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::removeItem(const KString& text) noexcept
{
	const int itemIndex = getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

int KComboBox::getItemIndex(const KString& text) noexcept
{
	return stringList.getIndex(text);
}

int KComboBox::getItemCount() noexcept
{
	return stringList.size();
}

int KComboBox::getSelectedItemIndex() noexcept
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
	}
	return -1;		
}

KString KComboBox::getSelectedItem() noexcept
{
	const int itemIndex = getSelectedItemIndex();
	if(itemIndex > -1)
		return stringList.get(itemIndex);

	return KString();
}

void KComboBox::clearList() noexcept
{
	stringList.removeAll();
	if(compHWND)
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
}

void KComboBox::selectItem(int index) noexcept
{
	selectedItemIndex = index;
	if(compHWND)
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
}

bool KComboBox::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) noexcept
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		_onItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

void KComboBox::afterCreated() noexcept
{
	const int listSize = stringList.size();
	if (listSize)
	{
		for (int i = 0; i < listSize; i++)
		{
			::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)stringList.get(i));
		}
	}

	if (selectedItemIndex > -1)
		::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

	__super::afterCreated();
}

void KComboBox::_onItemSelect() noexcept
{
	if(onItemSelect)
		onItemSelect(this);
}

KComboBox::~KComboBox() noexcept {}


