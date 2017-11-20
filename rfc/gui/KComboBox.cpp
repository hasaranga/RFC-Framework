
/*
	RFC - KComboBox.cpp
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

#include "../rfc.h"
#include "KComboBox.h"


KComboBox::KComboBox(bool sort)
{
	listener = 0;
	selectedItemIndex = -1;

	compClassName = STATIC_TXT("COMBOBOX");

	this->SetSize(100, 100);
	this->SetPosition(0, 0);

	this->SetStyle(WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP);

	if(sort)
		this->SetStyle(compDwStyle | CBS_SORT);

	this->SetExStyle(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);

	stringList = new KPointerList<KString*>;
}

void KComboBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KComboBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::RemoveItem(const KString& text)
{
	int itemIndex = this->GetItemIndex(text);
	if(itemIndex>-1)
		this->RemoveItem(itemIndex);
}

int KComboBox::GetItemIndex(const KString& text)
{
	int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if(stringList->GetPointer(i)->EqualsIgnoreCase(text))
				return i;
		}
	}
	return -1;
}

int KComboBox::GetItemCount()
{
	return stringList->GetSize();
}

int KComboBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
		return -1;
	}else
	{
		return -1;
	}	
}

KString KComboBox::GetSelectedItem()
{
	int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

void KComboBox::ClearList()
{
	stringList->DeleteAll(true);
	if(compHWND)
	{
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
	}
}

void KComboBox::SelectItem(int index)
{
	selectedItemIndex = index;
	if(compHWND)
	{
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
	}
}

bool KComboBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		this->OnItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KComboBox::CreateComponent(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register COMBOBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(selectedItemIndex > -1)
			::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KComboBox::SetListener(KComboBoxListener *listener)
{
	this->listener = listener;
}

void KComboBox::OnItemSelect()
{
	if(listener)
		listener->OnComboBoxItemSelect(this);
}

KComboBox::~KComboBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}


