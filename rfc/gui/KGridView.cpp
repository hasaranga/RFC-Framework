
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

#include "KGridView.h"
#include "KGUIProc.h"
#include <commctrl.h>

KGridView::KGridView(bool sortItems) noexcept : KComponent(false)
{
	itemCount = 0;
	colCount = 0;

	compClassName.assignStaticText(TXT_WITH_LEN("SysListView32"));

	compLWidth = 300;
	compLHeight = 200;

	compDwStyle = WS_CHILD | WS_TABSTOP | WS_BORDER | 
		LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	compDwExStyle = WS_EX_WINDOWEDGE;

	if (sortItems)
		compDwStyle |= LVS_SORTASCENDING;
}

KGridView::~KGridView() noexcept {}

void KGridView::insertRecord(KString** columnsData) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	LVITEMW lvi = {};
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = itemCount;

	const int row = (int)::SendMessageW(compHWND, 
		LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem = {};
		lvItem.iSubItem = i;
		lvItem.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
			(WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

void KGridView::insertRecordTo(int rowIndex, KString **columnsData) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	LVITEMW lvi = {};
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = rowIndex;

	const int row = (int)::SendMessageW(compHWND, 
		LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem= {};
		lvItem.iSubItem = i;
		lvItem.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
			(WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

KString KGridView::getRecordAt(int rowIndex, int columnIndex) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");

	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = {};
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(compHWND, LVM_GETITEMTEXTW, 
		(WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return KString(buffer, KStringBehaviour::FREE_ON_DESTROY);
}

int KGridView::getSelectedRow() noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	return ListView_GetNextItem(compHWND, -1, LVNI_SELECTED);
}

void KGridView::removeRecordAt(int rowIndex) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	if (ListView_DeleteItem(compHWND, rowIndex))
		--itemCount;
}

void KGridView::removeAll() noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	ListView_DeleteAllItems(compHWND);
	itemCount = 0;
}

void KGridView::updateRecordAt(int rowIndex, int columnIndex, const KString& text) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	LV_ITEMW lvi = {};
	lvi.iSubItem = columnIndex;
	lvi.pszText = (wchar_t*)(const wchar_t*)text;

	::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
		(WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void KGridView::setColumnWidth(int columnIndex, Logical columnWidth) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	const int dpi = KDPIUtility::getWindowDPI(compHWND);
	ListView_SetColumnWidth(compHWND, columnIndex, KDPIUtility::toPhysical(columnWidth, dpi));
}

Logical KGridView::getColumnWidth(int columnIndex) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	const int dpi = KDPIUtility::getWindowDPI(compHWND);
	return KDPIUtility::toLogical(ListView_GetColumnWidth(compHWND, columnIndex), dpi);
}

void KGridView::createColumn(const KString& text, Logical columnWidth) noexcept
{
	K_ASSERT(compHWND != NULL, "compHWND is NULL");
	const int dpi = KDPIUtility::getWindowDPI(compHWND);

	LVCOLUMNW lvc = {};

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = KDPIUtility::toPhysical(columnWidth, dpi);
	lvc.pszText = (wchar_t*)(const wchar_t*)text;
	lvc.iSubItem = colCount;

	::SendMessageW(compHWND, LVM_INSERTCOLUMNW, 
		(WPARAM)colCount, (LPARAM)&lvc);

	++colCount;
}

bool KGridView::eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) noexcept
{
	if (msg == WM_NOTIFY)
	{
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				_onItemSelect();
				*result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)lParam)->code == NM_RCLICK) // List view item right click
		{
			_onItemRightClick();
			*result = 0;
			return true;
		}
		else if (((LPNMHDR)lParam)->code == NM_DBLCLK) // List view item double click
		{
			_onItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::eventProc(msg, wParam, lParam, result);
}

void KGridView::afterCreated() noexcept
{
	ListView_SetExtendedListViewStyle(compHWND,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	__super::afterCreated();
}

void KGridView::_onItemSelect() noexcept
{
	if (onItemSelect)
		onItemSelect(this);
}

void KGridView::_onItemRightClick() noexcept
{
	if (onItemRightClick)
		onItemRightClick(this);
}

void KGridView::_onItemDoubleClick() noexcept
{
	if (onItemRightClick)
		onItemRightClick(this);
}

