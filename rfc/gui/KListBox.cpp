
/*
	RFC - KListBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

#include "../rfc.h"
#include "KListBox.h"

KListBox::KListBox(bool multipleSelection, bool sort, bool vscroll) : KComponent(false)
{
	this->multipleSelection = multipleSelection;
	listener = 0;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	compClassName.AssignStaticText(TXT_WITH_LEN("LISTBOX"));

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

	stringList = new KPointerList<KString*>(100);
}

void KListBox::SetListener(KListBoxListener *listener)
{
	this->listener = listener;
}

void KListBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KListBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
}

void KListBox::RemoveItem(const KString& text)
{
	const int itemIndex = this->GetItemIndex(text);
	if(itemIndex > -1)
		this->RemoveItem(itemIndex);
}

int KListBox::GetItemIndex(const KString& text)
{
	const int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if (stringList->GetPointer(i)->Compare(text))
				return i;
		}
	}
	return -1;
}

int KListBox::GetItemCount()
{
	return stringList->GetSize();
}

int KListBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

KString KListBox::GetSelectedItem()
{
	const int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

int KListBox::GetSelectedItems(int* itemArray, int itemCountInArray)
{
	if(compHWND)
	{	 
		const int items = (int)::SendMessageW(compHWND, LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);
		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void KListBox::ClearList()
{
	stringList->DeleteAll(true);

	if(compHWND)
		::SendMessageW(compHWND, LB_RESETCONTENT, 0, 0);
}

void KListBox::SelectItem(int index)
{
	selectedItemIndex = index;

	if(compHWND)
		::SendMessageW(compHWND, LB_SETCURSEL, index, 0);
}

void KListBox::SelectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(compHWND)
			::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool KListBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_COMMAND) 
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) // listbox sel change!
		{
			this->OnItemSelect();
			*result = 0;
			return true;
		}
		else if (HIWORD(wParam) == LBN_DBLCLK) // listbox double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KListBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register LISTBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
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

void KListBox::OnItemSelect()
{
	if(listener)
		listener->OnListBoxItemSelect(this);
}

void KListBox::OnItemDoubleClick()
{
	if(listener)
		listener->OnListBoxItemDoubleClick(this);
}

KListBox::~KListBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}