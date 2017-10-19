
/*
	RFC - KGridView.cpp
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

#include "KGridView.h"
#include "../rfc.h"

KGridView::KGridView(bool sortItems)
{
	itemCount = 0;
	colCount = 0;
	listener = 0;
	compClassName = WC_LISTVIEWW;

	this->SetPosition(0, 0);
	this->SetSize(300, 200);
	this->SetStyle(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL);
	this->SetExStyle(WS_EX_WINDOWEDGE);

	if (sortItems)
		compDwStyle |= LVS_SORTASCENDING;
}

KGridView::~KGridView(){}

void KGridView::SetListener(KGridViewListener *listener)
{
	this->listener = listener;
}

KGridViewListener* KGridView::GetListener()
{
	return listener;
}

void KGridView::InsertRecord(KString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = itemCount;

	int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvi = { 0 };
		lvi.iSubItem = i;
		lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvi);
	}

	itemCount++;
}

void KGridView::InsertRecordTo(int rowIndex, KString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = rowIndex;

	int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvi= { 0 };
		lvi.iSubItem = i;
		lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvi);
	}

	itemCount++;
}

KString KGridView::GetRecordAt(int rowIndex, int columnIndex)
{
	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(compHWND, LVM_GETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
}

int KGridView::GetSelectedRow()
{
	return ListView_GetNextItem(compHWND, -1, LVNI_SELECTED);
}

void KGridView::RemoveRecordAt(int rowIndex)
{
	if (ListView_DeleteItem(compHWND, rowIndex))
		itemCount--;
}

void KGridView::RemoveAll()
{
	ListView_DeleteAllItems(compHWND);
	itemCount = 0;
}

void KGridView::UpdateRecordAt(int rowIndex, int columnIndex, const KString& text)
{
	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.pszText = (wchar_t*)(const wchar_t*)text;

	::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void KGridView::SetColumnWidth(int columnIndex, int columnWidth)
{
	ListView_SetColumnWidth(compHWND, columnIndex, columnWidth);
}

int KGridView::GetColumnWidth(int columnIndex)
{
	return ListView_GetColumnWidth(compHWND, columnIndex);
}

void KGridView::CreateColumn(const KString& text, int columnWidth)
{
	LVCOLUMN lvc = { 0 };

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = columnWidth;
	lvc.pszText = (wchar_t*)(const wchar_t*)text;
	lvc.iSubItem = colCount;

	::SendMessageW(compHWND, LVM_INSERTCOLUMNW, (WPARAM)colCount, (LPARAM)&lvc);

	colCount++;
}

bool KGridView::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				this->OnItemSelect();
				*result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)lParam)->code == NM_RCLICK) // List view item right click
		{
			this->OnItemRightClick();
			*result = 0;
			return true;
		}
		else if (((LPNMHDR)lParam)->code == NM_DBLCLK) // List view item double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KGridView::CreateComponent(bool subClassWindowProc)
{
	if (!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register WC_LISTVIEWW class!

	if (compHWND)
	{
		ListView_SetExtendedListViewStyle(compHWND, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KGridView::OnItemSelect()
{
	if (listener)
		listener->OnGridViewItemSelect(this);
}

void KGridView::OnItemRightClick()
{
	if (listener)
		listener->OnGridViewItemRightClick(this);
}

void KGridView::OnItemDoubleClick()
{
	if (listener)
		listener->OnGridViewItemDoubleClick(this);
}

